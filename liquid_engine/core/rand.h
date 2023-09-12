#if !defined(LD_RAND_H)
#define LD_RAND_H
// * Description:  Random number generators.
// * Author:       Alicia Amarilla (smushyaa@gmail.com)
// * File Created: September 07, 2023
#include "defines.h"

/// State for random functions.
typedef struct RandState {
    i32 seed, current;
} RandState;

/// Initialize random function state.
header_only RandState rand_init_state( i32 seed ) {
    RandState result;
    result.seed    = seed;
    result.current = seed;
    return result;
}

/// Random 32-bit unsigned integer.
/// Uses LCG algorithm.
LD_API u32 rand_lcg_u32_state( RandState* state );
/// Random 32-bit integer.
/// Uses LCG algorithm.
LD_API i32 rand_lcg_i32_state( RandState* state );
/// Random 32-bit float.
/// Uses LCG algorithm.
LD_API f32 rand_lcg_f32_state( RandState* state );
/// Random 32-bit float in range 0-1.
/// Uses LCG algorithm.
LD_API f32 rand_lcg_f32_01_state( RandState* state );
/// Random 32-bit float in range -1 - 1.
/// Uses LCG algorithm.
LD_API f32 rand_lcg_f32_11_state( RandState* state );

/// Random 32-bit unsigned integer.
/// Uses XOR Shift algorithm.
LD_API u32 rand_xor_u32_state( RandState* state );
/// Random 32-bit integer.
/// Uses XOR Shift algorithm.
LD_API i32 rand_xor_i32_state( RandState* state );
/// Random 32-bit float.
/// Uses XOR Shift algorithm.
LD_API f32 rand_xor_f32_state( RandState* state );
/// Random 32-bit float in range 0-1.
/// Uses XOR Shift algorithm.
LD_API f32 rand_xor_f32_01_state( RandState* state );
/// Random 32-bit float in range -1 - 1.
/// Uses XOR Shift algorithm.
LD_API f32 rand_xor_f32_11_state( RandState* state );

/// Random 32-bit unsigned integer.
/// Uses LCG algorithm with global state.
LD_API u32 rand_lcg_u32(void);
/// Random 32-bit integer.
/// Uses LCG algorithm with global state.
LD_API i32 rand_lcg_i32(void);
/// Random 32-bit float.
/// Uses LCG algorithm with global state.
LD_API f32 rand_lcg_f32(void);
/// Random 32-bit float in range 0-1.
/// Uses LCG algorithm with global state.
LD_API f32 rand_lcg_f32_01(void);
/// Random 32-bit float in range -1 - 1.
/// Uses LCG algorithm with global state.
LD_API f32 rand_lcg_f32_11(void);

/// Random 32-bit unsigned integer.
/// Uses XOR Shift algorithm with global state.
LD_API u32 rand_xor_u32(void);
/// Random 32-bit integer.
/// Uses XOR Shift algorithm with global state.
LD_API i32 rand_xor_i32(void);
/// Random 32-bit float.
/// Uses XOR Shift algorithm with global state.
LD_API f32 rand_xor_f32(void);
/// Random 32-bit float in range 0-1.
/// Uses XOR Shift algorithm with global state.
LD_API f32 rand_xor_f32_01(void);
/// Random 32-bit float in range -1 - 1.
/// Uses XOR Shift algorithm with global state.
LD_API f32 rand_xor_f32_11(void);

#endif // header guard

