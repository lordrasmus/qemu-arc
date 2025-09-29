#ifndef QEMU_ARC_CPU_QOM_H
#define QEMU_ARC_CPU_QOM_H

#include "hw/core/cpu.h"

#define TYPE_ARC_CPU "arc-cpu"

#define ARC_CPU_TYPE_SUFFIX "-" TYPE_ARC_CPU
#define ARC_CPU_TYPE_NAME(model) model ARC_CPU_TYPE_SUFFIX

#define TYPE_ARC_CPU_HS4X ARC_CPU_TYPE_NAME("em")
#define TYPE_ARC_CPU_HS4X ARC_CPU_TYPE_NAME("hs4x")

/*
 * This macro emits these declarations:
 *
 *     typedef struct ArchCPU ArcCPU;
 *     typedef struct ArchCPU ArchCPU;
 *     typedef struct ArcCPUClass ArcCPUClass;
 *     ...
 */

OBJECT_DECLARE_CPU_TYPE(ArcCPU, ArcCPUClass, ARC_CPU)

#endif
