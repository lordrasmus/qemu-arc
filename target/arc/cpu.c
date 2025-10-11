
#include "qemu/osdep.h"
#include "qemu/log.h"
#include "qapi/error.h"
#include "cpu.h"
#include "qemu/module.h"
#include "hw/qdev-properties.h"
#include "accel/tcg/cpu-ldst.h"
#include "exec/gdbstub.h"
#include "exec/translation-block.h"
#include "fpu/softfloat-helpers.h"
#include "accel/tcg/cpu-ops.h"
#include "tcg/tcg.h"

static Property arc_cpu_properties[] = {
    /* TODO: Define properties. */
    DEFINE_PROP_END_OF_LIST(),
};

static void arc_cpu_realizefn(DeviceState *dev, Error **errp)
{
    CPUState *cs = CPU(dev);
    ArcCPUClass *arccc = ARC_CPU_GET_CLASS(dev);
    Error *local_err = NULL;

    cpu_exec_realizefn(cs, &local_err);
    if (local_err != NULL) {
        error_propagate(errp, local_err);
        return;
    }

    qemu_init_vcpu(cs);
    cpu_reset(cs);

    arccc->parent_realize(dev, errp);
}

static void arc_mmu_init(CPUArcState *env)
{
    /* TODO: Implement MMU */
}

static void arc_cpu_reset_hold(Object *obj, ResetType type)
{
    CPUState *s = CPU(obj);
    ArcCPU *cpu = ARC_CPU(s);
    ArcCPUClass *arccc = ARC_CPU_GET_CLASS(cpu);

    if (arccc->parent_phases.hold) {
        arccc->parent_phases.hold(obj, type);
    }

    memset(&cpu->env, 0, offsetof(CPUArcState, end_reset_fields));

#ifndef CONFIG_USER_ONLY
    arc_mmu_init(&cpu->env);
#endif

    /* TODO: Reset everything. */
}

static void arc_cpu_class_init(ObjectClass *oc, void *data)
{
    DeviceClass *dc = DEVICE_CLASS(oc);
    CPUClass *cc = CPU_CLASS(oc);
    ArcCPUClass *arccc = ARC_CPU_CLASS(oc);
    ResettableClass *rc = RESETTABLE_CLASS(oc);

    device_class_set_parent_realize(dc, arc_cpu_realizefn, &arccc->parent_realize);
    resettable_class_set_parent_phases(rc, NULL, arc_cpu_reset_hold, NULL,
                                       &arccc->parent_reset);

    /* TODO: Initialize GDB assets. */

    device_class_set_props(dc, arc_cpu_properties);
}

static void arc_cpu_init(Object *obj)
{
}

static void arc_cpu_init_hs4x(Object *obj)
{
}

static const TypeInfo arc_cpu_type_infos[] = {
    {
        .name = TYPE_ARC_CPU,
        .parent = TYPE_CPU,
        .instance_size = sizeof(ArcCPU),
        .instance_align = __alignof__(ArcCPU),
        .instance_init = arc_cpu_init,
        .abstract = true,
        .class_size = sizeof(ArcCPUClass),
        .class_init = arc_cpu_class_init
    },
    {
        .name = TYPE_ARC_CPU_HS4X,
        .parent = TYPE_ARC_CPU,
        .instance_init = arc_cpu_init_hs4x,
    }
};

DEFINE_TYPES(arc_cpu_type_infos)
