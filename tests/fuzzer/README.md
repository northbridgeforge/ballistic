This fuzzer is responsible for the correctness of Ballistic's tier 1 compiler. It translates arm instructions via
Ballistic and Unicorn Engine's and compare the results.

# Usage

The fuzzer currently supports Linux only.
Building Ballistic in debug mode also builds the fuzzer.
Then you can run the fuzzer with the following command assuming you are `./build/debug/`

```
ctest -R fuzzer --verbose
<!-- snip -->
14: [INFO] [main] [generate_seeds.c:133] Wrote 1592 seeds from 3856 instructions to seeds.bin.
14: 
14: [INFO] [main] [main.c:21] Running Test...
14: [INFO] [main] [main.c:51] Loaded 1592 seeds from seeds.bin.
14: [INFO] [bal_fuzzer_ipc_spawn] [bal_fuzzer_ipc.c:96] Spawned worker '/home/apollo/Development/Ballistic/build/debug/bin/linux/bal_fuzzer_worker_ballistic' with pid 79140 (in fd 5, out fd 6).
14: [INFO] [bal_fuzzer_ipc_spawn] [bal_fuzzer_ipc.c:96] Spawned worker '/home/apollo/Development/Ballistic/build/debug/bin/linux/bal_fuzzer_worker_unicorn' with pid 79141 (in fd 7, out fd 8).
14: [WARN] [main] [main.c:169] Unicorn worker reported execution error for seed 0x91800000 and returned status code 1, skipping...
14: [WARN] [main] [main.c:169] Unicorn worker reported execution error for seed 0x91800000 and returned status code 1, skipping...
14: [WARN] [main] [main.c:169] Unicorn worker reported execution error for seed 0x91800001 and returned status code 1, skipping...
14: [WARN] [main] [main.c:169] Unicorn worker reported execution error for seed 0x9180000F and returned status code 1, skipping...
14: [WARN] [main] [main.c:169] Unicorn worker reported execution error for seed 0x9180001E and returned status code 1, skipping...
14: [WARN] [main] [main.c:169] Unicorn worker reported execution error for seed 0x9180001F and returned status code 1, skipping...
14: [WARN] [main] [main.c:169] Unicorn worker reported execution error for seed 0x91800000 and returned status code 1, skipping...
14: [WARN] [main] [main.c:169] Unicorn worker reported execution error for seed 0x91800020 and returned status code 1, skipping...
14: [WARN] [main] [main.c:169] Unicorn worker reported execution error for seed 0x918001E0 and returned status code 1, skipping...
14: [WARN] [main] [main.c:169] Unicorn worker reported execution error for seed 0x918003C0 and returned status code 1, skipping...
14: [WARN] [main] [main.c:169] Unicorn worker reported execution error for seed 0x918003E0 and returned status code 1, skipping...
14: [WARN] [main] [main.c:169] Unicorn worker reported execution error for seed 0x91800000 and returned status code 1, skipping...
14: [WARN] [main] [main.c:169] Unicorn worker reported execution error for seed 0x91800000 and returned status code 1, skipping...
14: [WARN] [main] [main.c:169] Unicorn worker reported execution error for seed 0x919F0000 and returned status code 1, skipping...
14: [WARN] [main] [main.c:169] Unicorn worker reported execution error for seed 0x91BF0000 and returned status code 1, skipping...
14: [INFO] [main] [main.c:251] Fuzz complete: 1592 seeds | 182 passed | 0 failed | 1410 seeds skipped | 0 errors.
```

The unicorn warnings mean that unicorn does not support that specific instruction and can be safely ignored.
All that matters is `0 failed`, which proves that Ballistic translates all of its supported instructions correctly.
The fuzzer must report `0 failed` **AT ALL TIMES**.
`1410 seeds skipped` means that Ballistic doesn't support that specific instruction and skipped it.

## Debugging Specific Instructions

If Ballistic is translating an instruction incorrectly, you can pass the instruction directly to the ballistic worker
to debug the engine's state via the following command, assuming you are in `./build/debug/linux/bin`.

```
./bal_fuzzer_worker_ballistic <hex instruction>
```

## Deterministic CPU-State Matrix

The fuzzer normally executes each instruction once with a zero-initialized
CPU state. Pass `--matrix` after the seed filename to test each instruction
against 16 deterministic initial CPU states:

    ./bal_fuzzer ./bal_fuzzer_worker_ballistic ./bal_fuzzer_worker_unicorn seeds.bin --matrix

State 0 preserves the original all-zero baseline. States 1-15 initialize
X0-X29 with rotating boundary-value patterns, set the stack pointer to
0x100000, and vary NZCV flags. Across all 16 states, every NZCV combination
is represented.

X30 (LR) is reserved by the worker execution harness. Both workers
overwrite it with instruction_count * 4 (4 for these single-instruction
cases), so matrix mode does not vary X30.

Matrix mode requires the Unicorn SP snapshot correction from PR #98.
Without that correction, nonzero-SP states produce false mismatches
because Unicorn captures XZR rather than SP.

The matrix is intended for curated register-only instruction corpora.
Arbitrary memory-access and control-flow instructions may require suitable
guest memory mappings or other execution setup. Matrix mode does not
provide those mappings.

On mismatches, the fuzzer reports the instruction, seed index, state index,
and divergent CPU fields. The seed corpus and deterministic state index
allow the initial state to be reconstructed.

As with normal fuzzer operation, skipped instructions are not validated.
