#include "bal_fuzzer_state_matrix.h"
#include "unity.h"
#include <string.h>

void
setUp(void)
{
}

void
tearDown(void)
{
}

static void
test_StateMatrix_ZeroBaseline(void)
{
    bal_fuzzer_cpu_snapshot_t state;
    bal_fuzzer_cpu_snapshot_t expected;

    (void)memset(&state, 0xA5, sizeof(state));
    (void)memset(&expected, 0, sizeof(expected));

    bal_fuzzer_state_matrix_initialize(&state, 0U);

    TEST_ASSERT_EQUAL_MEMORY(&expected, &state, sizeof(state));
}

static void
test_StateMatrix_RegisterValuesAndFlags(void)
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

    uint16_t seen_flags = 0U;

    for (size_t i = 0U; i < BAL_FUZZER_STATE_MATRIX_SIZE; ++i)
    {
        bal_fuzzer_cpu_snapshot_t state;
        (void)memset(&state, 0xA5, sizeof(state));

        bal_fuzzer_state_matrix_initialize(&state, i);

        const uint8_t c = (uint8_t)(i & 1U);
        const uint8_t z = (uint8_t)((i >> 1U) & 1U);
        const uint8_t n = (uint8_t)((i >> 2U) & 1U);
        const uint8_t v = (uint8_t)((i >> 3U) & 1U);

        TEST_ASSERT_EQUAL_UINT8(c, state.flag_c);
        TEST_ASSERT_EQUAL_UINT8(z, state.flag_z);
        TEST_ASSERT_EQUAL_UINT8(n, state.flag_n);
        TEST_ASSERT_EQUAL_UINT8(v, state.flag_v);

        seen_flags |= (uint16_t)(1U << i);

        if (i == 0U)
        {
            TEST_ASSERT_EQUAL_HEX64(0U, state.x[31]);
            continue;
        }

        TEST_ASSERT_EQUAL_HEX64(0x100000ULL, state.x[31]);
        TEST_ASSERT_EQUAL_HEX64(0U, state.x[30]);

        for (size_t reg = 0U; reg < 30U; ++reg)
        {
            const size_t value_index = (i - 1U + reg) % 8U;

            TEST_ASSERT_EQUAL_HEX64(
                values[value_index], state.x[reg]);
        }
    }

    TEST_ASSERT_EQUAL_HEX16(0xFFFFU, seen_flags);
}

static void
test_StateMatrix_Repeatability(void)
{
    for (size_t i = 0U; i < BAL_FUZZER_STATE_MATRIX_SIZE; ++i)
    {
        bal_fuzzer_cpu_snapshot_t first;
        bal_fuzzer_cpu_snapshot_t second;

        (void)memset(&first, 0xA5, sizeof(first));
        (void)memset(&second, 0x5A, sizeof(second));

        bal_fuzzer_state_matrix_initialize(&first, i);
        bal_fuzzer_state_matrix_initialize(&second, i);

        TEST_ASSERT_EQUAL_MEMORY(&first, &second, sizeof(first));
    }
}

int
main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_StateMatrix_ZeroBaseline);
    RUN_TEST(test_StateMatrix_RegisterValuesAndFlags);
    RUN_TEST(test_StateMatrix_Repeatability);
    return UNITY_END();
}
