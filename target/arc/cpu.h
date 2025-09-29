#ifndef ARC_CPU_H
#define ARC_CPU_H

#include "hw/core/cpu.h"
#include "cpu-qom.h"
#include "exec/cpu-common.h"
#include "exec/cpu-defs.h"
#include "qemu/cpu-float.h"
#include "exec/cpu-interrupt.h"
#include "hw/registerfields.h"

#ifdef CONFIG_USER_ONLY
#error "ARC does not support user mode yet"
#endif

FIELD(STATUS32, H,   0, 1); /* Halt flag */
FIELD(STATUS32, E ,  1, 4); /* Interrupt priority */
FIELD(STATUS32, AE,  5, 1); /* Is in exception state? */
FIELD(STATUS32, DE,  6, 1); /* Delayed branch is pending? */
FIELD(STATUS32, U,   7, 1); /* User mode */
FIELD(STATUS32, V,   8, 1); /* Overflow status flag */
FIELD(STATUS32, C,   9, 1); /* Carry status flag */
FIELD(STATUS32, N,  10, 1); /* Negative status flag*/
FIELD(STATUS32, Z,  11, 1); /* Zero status flag */
FIELD(STATUS32, L,  12, 1); /* ZOL disable */
FIELD(STATUS32, DZ, 13, 1); /* EV_DivZero exception enable */
FIELD(STATUS32, SC, 14, 1); /* Enable stack checking */
FIELD(STATUS32, ES, 15, 1); /* EI_S table instruction pending */
FIELD(STATUS32, RB, 16, 3); /* Select a register bank */
FIELD(STATUS32, AD, 19, 1); /* Disable alignment checking */
FIELD(STATUS32, US, 20, 1); /* User sleep mode enable */
FIELD(STATUS32, IE, 21, 1); /* Enable interrupts */

typedef struct CPUArchState CPUArcState;

struct CPUArchState {
    /* Core register set */
    target_ulong gpr[32];               /* r00-r31 */
    target_ulong gpr_apex[26];          /* r32-r57 */
    target_ulong reg_accl;              /* r58 */
    target_ulong reg_acch;              /* r59 */
    target_ulong reg_lp_count;          /* r60 */
                                        /* r61 - reserved */
    target_ulong reg_long_immediate;    /* r62 */
    target_ulong reg_pcl;               /* r63 */

    /* AUX registers. Current architectural state */
    target_ulong aux_lp_start;          /* 0x002 */
    target_ulong aux_lp_end;            /* 0x003 */
    target_ulong aux_identity;          /* 0x004 */
    target_ulong aux_pc;                /* 0x006 */
    target_ulong aux_status32;          /* 0x00A */
    target_ulong aux_status32_p0;       /* 0x00B */
    target_ulong aux_user_sp;           /* 0x00D */
    target_ulong aux_bta;               /* 0x412 */
    target_ulong aux_int_vector_base;   /* 0x025 */

    /* AUX registers. Saved exception and interrupt state */
    target_ulong aux_eret;              /* 0x400 */
    target_ulong aux_erbta;             /* 0x401 */
    target_ulong aux_erstatus;          /* 0x402 */
    target_ulong aux_ecr;               /* 0x403 */
    target_ulong aux_efa;               /* 0x404 */

    /* AUX registers. Build Configuration Registers (BCRs) */
    target_ulong aux_bcr_ver;           /* 0x060 */
    target_ulong aux_bta_link_build;    /* 0x063 */
    target_ulong aux_vecbase_ac_build;  /* 0x068 */
    target_ulong aux_rf_build;          /* 0x06E */
    target_ulong aux_mpy_build;         /* 0x07B */
    target_ulong aux_swap_build;        /* 0x07C */
    target_ulong aux_norm_build;        /* 0x07D */
    target_ulong aux_minmax_build;      /* 0x07E */
    target_ulong aux_barrel_build;      /* 0x07F */
    target_ulong aux_isa_config;        /* 0x0C1 */
    target_ulong aux_irq_build;         /* 0x0F3 */
    target_ulong aux_isa_profile;       /* 0x0F8 */
    target_ulong aux_micro_arch_build;  /* 0x0F9 */

}

struct ArchCPU {
    CPUState parent_obj;

    CPUArcState env;
};

typedef struct ArcCPUConfig {
    bool halt_on_reset;
    bool intvbase_preset;
    uint8_t number_of_interrupts;
    uint8_t external_interrupts;
    uint8_t number_of_level;
    bool isa_code_density;
    bool isa_bitscan;
    bool isa_swap;
    bool isa_shift;
    bool isa_bitfield;
    bool isa_os_opt;
    bool isa_atomic;
    bool isa_div_rem;
    bool isa_ll64;
    uint8_t isa_mpy;
};

struct ArcCPUClass {
    CPUClass parent_class;

    DeviceRealize parent_realize;
    ResettablePhases parent_phases;

    ArcCPUConfig *config;
};

#endif
