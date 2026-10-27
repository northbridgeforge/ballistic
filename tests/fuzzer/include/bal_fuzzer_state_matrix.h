#ifndef BALLISTIC_BAL_FUZZER_STATE_MATRIX_H
#define BALLISTIC_BAL_FUZZER_STATE_MATRIX_H

#include "bal_fuzzer_protocol.h"
#include <stddef.h>

#define BAL_FUZZER_STATE_MATRIX_SIZE 16U

void
bal_fuzzer_state_matrix_initialize(bal_fuzzer_cpu_snapshot_t *state,
                                   size_t state_index);

#endif // BALLISTIC_BAL_FUZZER_STATE_MATRIX_H
