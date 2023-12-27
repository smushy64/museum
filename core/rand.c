// * Description:  Random number generator implementation.
// * Author:       Alicia Amarilla (smushyaa@gmail.com)
// * File Created: September 07, 2023
#include "core/rand.h"
#include "shared/constants.h"

#include "core/time.h"

struct InternalRandStateLCG {
    i32 seed, current;
};

struct InternalRandStateXOR {
    u32 seed, current;
};

#define RAND_LCG_CONSTANT_A (166463)
#define RAND_LCG_CONSTANT_B (235)
#define RAND_LCG_CONSTANT_M (21446837)

#define RAND_XOR_CONSTANT_0 (13)
#define RAND_XOR_CONSTANT_1 (17)
#define RAND_XOR_CONSTANT_2 (5)

struct InternalRandStateLCG GLOBAL_LCG_STATE =
    { 346737457, 346737457 };
struct InternalRandStateXOR GLOBAL_XOR_STATE =
    { 745743634, 745743634 };

CORE_API i32 rand_generate_seed(void) {
    TimeRecord record = time_record();
    u32 result =
        (record.second + record.minute + record.hour + record.day) * record.year;
    return *(i32*)&result;
}
CORE_API void rand_reset_global_state(void) {
    i32 seed = rand_generate_seed();
    RandState generic_state = rand_init_state( seed );
    GLOBAL_LCG_STATE = reinterpret_cast( struct InternalRandStateLCG, &generic_state );
    seed = rand_generate_seed();
    generic_state = rand_init_state( seed );
    GLOBAL_XOR_STATE = reinterpret_cast( struct InternalRandStateXOR, &generic_state );
}

CORE_API i32 rand_lcg_i32_state( RandState* state_ ) {
    struct InternalRandStateLCG* state =
        (struct InternalRandStateLCG*)state_;

    state->current = (
        RAND_LCG_CONSTANT_A * state->current +
        RAND_LCG_CONSTANT_B
    ) % RAND_LCG_CONSTANT_M;
    return state->current;
}
CORE_API u32 rand_lcg_u32_state( RandState* state ) {
    i32 next = rand_lcg_i32_state( state );
    return reinterpret_cast( u32, &next );
}
CORE_API f32 rand_lcg_f32_state( RandState* state ) {
    i32 next = rand_lcg_i32_state( state );
    return (f32)next / 1000.0f;
}
CORE_API f32 rand_lcg_f32_01_state( RandState* state ) {
    u32 next = rand_lcg_i32_state( state );
    return ((f32)(next) / (f32)(U32_MAX));
}
CORE_API f32 rand_lcg_f32_11_state( RandState* state ) {
    f32 next = rand_lcg_f32_01_state( state );
    return (next - 0.5f) * 2.0f;
}

CORE_API u32 rand_xor_u32_state( RandState* state_ ) {
    struct InternalRandStateXOR* state =
        (struct InternalRandStateXOR*)state_;
    assert(state->seed);
    u32 x = state->current;
    x ^= x << RAND_XOR_CONSTANT_0;
    x ^= x >> RAND_XOR_CONSTANT_1;
    x ^= x << RAND_XOR_CONSTANT_2;

    state->current = x;
    return state->current;
}
CORE_API i32 rand_xor_i32_state( RandState* state ) {
    u32 next = rand_xor_u32_state( state );
    return (i32)next * ( next % 2 ? -1 : 1 );
}
CORE_API f32 rand_xor_f32_state( RandState* state ) {
    i32 next = rand_xor_i32_state( state );
    return (f32)next / 1000.0f;
}
CORE_API f32 rand_xor_f32_01_state( RandState* state ) {
    u32 next = rand_xor_u32_state( state );
    return ((f32)(next) / (f32)(U32_MAX));
}
CORE_API f32 rand_xor_f32_11_state( RandState* state ) {
    f32 next = rand_xor_f32_01_state( state );
    return (next - 0.5f) * 2.0f;
}

CORE_API u32 rand_lcg_u32(void) {
    return rand_lcg_u32_state( (RandState*)&GLOBAL_LCG_STATE );
}
CORE_API i32 rand_lcg_i32(void) {
    return rand_lcg_i32_state( (RandState*)&GLOBAL_LCG_STATE );
}
CORE_API f32 rand_lcg_f32(void) {
    return rand_lcg_f32_state( (RandState*)&GLOBAL_LCG_STATE );
}
CORE_API f32 rand_lcg_f32_01(void) {
    return rand_lcg_f32_01_state( (RandState*)&GLOBAL_LCG_STATE );
}
CORE_API f32 rand_lcg_f32_11(void) {
    return rand_lcg_f32_11_state( (RandState*)&GLOBAL_LCG_STATE );
}

CORE_API u32 rand_xor_u32(void) {
    return rand_xor_u32_state( (RandState*)&GLOBAL_XOR_STATE );
}
CORE_API i32 rand_xor_i32(void) {
    return rand_xor_i32_state( (RandState*)&GLOBAL_XOR_STATE );
}
CORE_API f32 rand_xor_f32(void) {
    return rand_xor_f32_state( (RandState*)&GLOBAL_XOR_STATE );
}
CORE_API f32 rand_xor_f32_01(void) {
    return rand_xor_f32_01_state( (RandState*)&GLOBAL_XOR_STATE );
}
CORE_API f32 rand_xor_f32_11(void) {
    return rand_xor_f32_11_state( (RandState*)&GLOBAL_XOR_STATE );
}

