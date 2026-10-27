#include "bal_fuzzer_state_matrix.h"
#include <string.h>

void
bal_fuzzer_state_matrix_initialize(bal_fuzzer_cpu_snapshot_t *state, const size_t state_index)
{
    static const uint64_t values[] = {
        0ULL,
        1ULL,
        0xFFFFFFFFFFFFFFFFULL,
        0x7FFFFFFFFFFFFFFFULL,
        0x8000000000000000ULL,
        0x00000000FFFFFFFFULL,
        0x0000000100000000ULL,
        0x5555555555555555ULL
    };

    (void)memset(state, 0, sizeof(*state));

    // State zero preserves the original zero-initialized CPU state.
    if (state_index == 0U)
    {
        return;
    }

    state->x[31] = 0x100000ULL;

    for (size_t reg = 0U; reg < 30U; ++reg)
    {
        state->x[reg] = values[(state_index - 1U + reg) %
                               (sizeof(values) / sizeof(values[0]))];
    }

    state->flag_c = (uint8_t)(state_index & 1U);
    state->flag_z = (uint8_t)((state_index >> 1U) & 1U);
    state->flag_n = (uint8_t)((state_index >> 2U) & 1U);
    state->flag_v = (uint8_t)((state_index >> 3U) & 1U);
}
