/*
 * Copyright (c) 2011, Max Filippov, Open Source and Linux Lab.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the Open Source and Linux Lab nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "cpu-qom.h"
#include "qemu/osdep.h"
#include "qapi/error.h"
#include "system/reset.h"
#include "system/system.h"
#include "hw/boards.h"
#include "hw/loader.h"
#include "elf.h"
#include "system/memory.h"
#include "qemu/error-report.h"

static uint64_t translate_phys_addr(void *opaque, uint64_t addr)
{
    XtensaCPU *cpu = opaque;

    return cpu_get_phys_page_debug(CPU(cpu), addr);
}

static void sim_reset(void *opaque)
{
    XtensaCPU *cpu = opaque;

    cpu_reset(CPU(cpu));
}

XtensaCPU *xtensa_sim_common_init(MachineState *machine)
{
    XtensaCPU *cpu = NULL;
    CPUXtensaState *env = NULL;
    ram_addr_t ram_size = machine->ram_size;
    int n;

    for (n = 0; n < machine->smp.cpus; n++) {
        cpu = XTENSA_CPU(cpu_create(machine->cpu_type));
        env = &cpu->env;

        env->sregs[PRID] = n;
        qemu_register_reset(sim_reset, cpu);
        /* Need MMU initialized prior to ELF loading,
         * so that ELF gets loaded into virtual addresses
         */
        sim_reset(cpu);
    }

    if (env) {
        XtensaMemory sysram = env->config->sysram;

        sysram.location[0].size = ram_size;
        xtensa_create_memory_regions(&env->config->instrom, "xtensa.instrom",
                                     get_system_memory());
        xtensa_create_memory_regions(&env->config->instram, "xtensa.instram",
                                     get_system_memory());
        xtensa_create_memory_regions(&env->config->datarom, "xtensa.datarom",
                                     get_system_memory());
        xtensa_create_memory_regions(&env->config->dataram, "xtensa.dataram",
                                     get_system_memory());
        xtensa_create_memory_regions(&env->config->sysrom, "xtensa.sysrom",
                                     get_system_memory());
        xtensa_create_memory_regions(&sysram, "xtensa.sysram",
                                     get_system_memory());
    }
    if (serial_hd(0)) {
        xtensa_sim_open_console(serial_hd(0));
    }
    return cpu;
}

void xtensa_sim_load_kernel(XtensaCPU *cpu, MachineState *machine)
{
    const char *kernel_filename = machine->kernel_filename;

    if (kernel_filename) {
        uint64_t elf_entry;
        int success = load_elf(kernel_filename, NULL, translate_phys_addr, cpu,
                               &elf_entry, NULL, NULL, NULL,
                               TARGET_BIG_ENDIAN ? ELFDATA2MSB : ELFDATA2LSB,
                               EM_XTENSA, 0, 0);

        if (success > 0) {
            cpu->env.pc = elf_entry;
        }
    }
}

static void xtensa_sim_init(MachineState *machine)
{
    XtensaCPU *cpu = xtensa_sim_common_init(machine);

    xtensa_sim_load_kernel(cpu, machine);
}

static void arc_sim_init(MachineState *machine)
{
    static struct arc_boot_info boot_info;
    unsigned int smp_cpus = machine->smp.cpus;
    ram_addr_t ram_base = 0;
    ram_addr_t ram_size = machine->ram_size;
    ArcCPU *cpu = NULL;
    MemoryRegion *ram, *system_io;
    int n;

    boot_info.ram_start = ram_base;
    boot_info.ram_size = ram_size;
    boot_info.kernel_filename = machine->kernel_filename;

    for (n = 0; n < smp_cpus; n++) {
        cpu = ARC_CPU(object_new(machine->cpu_type));
        if (cpu == NULL) {
            fprintf(stderr, "Unable to find CPU definition!\n");
            exit(1);
        }

        /* Set the initial CPU properties. */
        object_property_set_uint(OBJECT(cpu), "freq_hz", 1000000, &error_fatal);
        object_property_set_bool(OBJECT(cpu), "rtc-opt", true, &error_fatal);
        object_property_set_bool(OBJECT(cpu), "realized", true, &error_fatal);

        /* Initialize internal devices. */
        cpu_arc_pic_init(cpu);
        cpu_arc_clock_init(cpu);

        qemu_register_reset(arc_cpu_reset, cpu);
    }

    ram = g_new(MemoryRegion, 1);
    memory_region_init_ram(ram, NULL, "arc.ram", ram_size, &error_fatal);
    memory_region_add_subregion(get_system_memory(), ram_base, ram);

    system_io = g_new(MemoryRegion, 1);
    memory_region_init_io(system_io, NULL, &arc_io_ops, NULL, "arc.io",
                           1024);
    memory_region_add_subregion(get_system_memory(), 0xf0000000, system_io);

    if (semihosting_enabled()) {
        if (serial_hd(0)) {
            arc_sim_open_console(serial_hd(0));
        }
    } else {
        serial_mm_init(get_system_memory(), 0x90000000, 2, cpu->env.irq[20],
                       115200, serial_hd(0), DEVICE_NATIVE_ENDIAN);
    }

    arc_load_kernel(cpu, &boot_info);
}

static void arc_sim_machine_init(MachineClass *mc)
{
    mc->desc = "ARC simulation";
    mc->is_default = true;
    mc->init = arc_sim_init;
    mc->max_cpus = 1;
    mc->no_serial = 1;
    mc->default_cpu_type = TYPE_ARC_CPU_HS4x;
}

DEFINE_MACHINE("arc-sim", arc_sim_machine_init)
