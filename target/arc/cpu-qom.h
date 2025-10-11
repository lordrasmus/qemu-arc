#ifndef QEMU_ARC_CPU_QOM_H
#define QEMU_ARC_CPU_QOM_H

#include "hw/core/cpu.h"

#define TYPE_ARC_CPU "arc-cpu"

#define ARC_CPU_TYPE_SUFFIX "-" TYPE_ARC_CPU
#define ARC_CPU_TYPE_NAME(model) model ARC_CPU_TYPE_SUFFIX

#define TYPE_ARC_CPU_HS4X ARC_CPU_TYPE_NAME("hs4x")

/*
 * OBJECT_DECLARE_CPU_TYPE emits these declarations:
 *
 *     typedef struct ArchCPU ArcCPU;
 *     typedef struct ArchCPU ArchCPU;
 *     typedef struct ArcCPUClass ArcCPUClass;
 *     ...
 *     G_DEFINE_AUTOPTR_CLEANUP_FUNC(ArcCPU, object_unref);
 *     static inline G_GNUC_UNUSED ArcCPU *ARC_CPU(const void *obj)
 *         { return OBJECT_CHECK(ArcCPU, obj, TYPE_ARC_CPU); }
 *    static inline G_GNUC_UNUSED ArcCPUClass *ARC_CPU_GET_CLASS(const void *obj)
 *        { return OBJECT_GET_CLASS(ArcCPUClass, obj, TYPE_ARC_CPU); }
 *    static inline G_GNUC_UNUSED ArcCPUClass *ARC_CPU_CLASS(const void *klass)
 *        { return OBJECT_CLASS_CHECK(ArcCPUClass, klass, TYPE_ARC_CPU); }
 */

 /*

 */

OBJECT_DECLARE_CPU_TYPE(ArcCPU, ArcCPUClass, ARC_CPU)

#endif
