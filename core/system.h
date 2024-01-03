#if !defined(LD_CORE_SYSTEM_H)
#define LD_CORE_SYSTEM_H
/**
 * Description:  Query system info.
 * Author:       Alicia Amarilla (smushyaa@gmail.com)
 * File Created: January 02, 2024
*/
#include "shared/defines.h"

/// Size of cpu name buffer.
#define SYSTEM_INFO_CPU_NAME_CAPACITY (255)

/// Feature flags.
typedef u16 CPUFeatureFlags;
#define CPU_FEATURE_SSE     (1 << 0)
#define CPU_FEATURE_SSE2    (1 << 1)
#define CPU_FEATURE_SSE3    (1 << 2)
#define CPU_FEATURE_SSSE3   (1 << 3)
#define CPU_FEATURE_SSE4_1  (1 << 4)
#define CPU_FEATURE_SSE4_2  (1 << 5)

#define CPU_FEATURE_SSE_MASK \
    ( CPU_FEATURE_SSE |\
        CPU_FEATURE_SSE2 |\
        CPU_FEATURE_SSE3 |\
        CPU_FEATURE_SSSE3 |\
        CPU_FEATURE_SSE4_1 |\
        CPU_FEATURE_SSE4_2 )

#define CPU_FEATURE_AVX     (1 << 6)
#define CPU_FEATURE_AVX2    (1 << 7)

#define CPU_FEATURE_AVX_MASK \
    ( CPU_FEATURE_AVX | CPU_FEATURE_AVX2 )

#define CPU_FEATURE_AVX_512 (1 << 8)

/// System Information.
typedef struct SystemInfo {
    char  cpu_name[SYSTEM_INFO_CPU_NAME_CAPACITY];
    usize total_memory;
    usize page_size;
    u16   cpu_count;
    CPUFeatureFlags feature_flags;
} SystemInfo;

/// Query information about the current system.
CORE_API void system_info_query( SystemInfo* out_info );

/// Check if x86 cpu has SSE instructions (1,2,3,SSSE3,4.1,4.2)
/// Returns bitfield with missing instructions set to 1.
/// Returns zero if cpu has SSE instructions.
header_only CPUFeatureFlags system_info_feature_check_x86_sse( SystemInfo* info ) {
    CPUFeatureFlags inverse              = ~info->feature_flags;
    CPUFeatureFlags missing_instructions = inverse & CPU_FEATURE_SSE_MASK;

    return missing_instructions;
}
/// Check if x86 cpu has AVX instructions (1,2)
/// Returns bitfield with missing instructions set to 1.
/// Returns zero if cpu has AVX instructions.
header_only CPUFeatureFlags system_info_feature_check_x86_avx( SystemInfo* info ) {
    CPUFeatureFlags inverse              = ~info->feature_flags;
    CPUFeatureFlags missing_instructions = inverse & CPU_FEATURE_AVX_MASK;

    return missing_instructions;
}

#endif /* header guard */
