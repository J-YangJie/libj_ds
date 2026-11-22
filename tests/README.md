# libj_ds permanent tests

The test system validates semantics, contracts, randomized models, allocation
failure handling, debug assertions, and long stress runs. It is separate from
`demo/` and from the historical `stress/` programs: demos remain usage examples,
and the old stress logs are not modified.

## Quick start

Run from the repository root:

```sh
make test
```

or from this directory:

```sh
make test
```

`make test` builds `tests/build/default/libj_ds_tests` and runs every suite
except `stress` and `bench`. A failure returns a non-zero status and prints the
file, line, expected/actual values, and the most recent operation trace.

## Targets

| Target | Meaning |
|---|---|
| `make test` | default configuration, all non-stress/bench suites |
| `make check` | public header smoke plus the default/debug/align/32-bit matrix |
| `make test-debug` | `JDSC_DEBUG` build; adds death tests |
| `make test-align` | `JDSC_ALIGNMEM=1` build |
| `make test-32` | `-m32` build; prints `SKIP` when unavailable |
| `make test-valgrind` | default suites under Valgrind |
| `make test-diff` | sampled differential suite only |
| `make test-model` | randomized model suite only |
| `make test-alloc` | allocation failure sweep only |
| `make stress` | stress suite, `STRESS_ITERS` (default 200000) |
| `make stress-long` | stress suite, `STRESS_LONG_ITERS` (default 5000000) |
| `make stress-valgrind` | stress suite under Valgrind |
| `make bench` | benchmark suite, informational only |
| `make headers` | compile each public header as C and C++ syntax-only |
| `make matrix` | `test`, `test-debug`, `test-align`, `test-32` |
| `make clean` | remove `tests/build/` |

Useful variables:

```sh
make test TEST_SEED=0x1234
make stress STRESS_ITERS=2000000 TEST_SEED=0x1234
make test-32 CC=gcc-12 CXX=g++-12
```

Logs are written to `tests/build/logs/<config>/<target>.log`. Build objects and
archives stay under `tests/build/<config>/` and never overwrite the root
`libj_ds.so` or `libj_ds.a`.

## Layout

```text
tests/
  core/            runner, assertions, cleanup, RNG, trace, allocator, tracked
  contracts/       type-erased descriptors and shared models
  containers/      one directory per container
  build/           ignored build output and logs
```

A container directory contains some subset of:

```text
container.mk       public header declaration for the smoke target
desc.c/.h          descriptor adapters over the public vtable
unit.c             fixed-input unit and boundary tests
contract.c         shared contract registration
model.c            randomized model registration
alloc.c            fail-at-N allocation scenarios
death.c            debug-only abort tests
diff.cpp           optional std:: differential
stress.c           long random runs
bench.c            optional benchmark
value.c/.h         invariant checkers and fixtures
SEMANTICS.md       documented behavior and STL differences
```

`tests/Makefile` discovers `containers/*/container.mk`, all `.c` and `.cpp`
sources in those directories, and the shared `core/` and `contracts/` sources.
Adding a container does not require editing a container list.

## Container coverage

| Modules | Layers |
|---|---|
| `deque`, `vector`, `list` | unit, sequence contract/model, allocation sweeps, debug death tests, `std::` differential, stress, bench, semantics |
| `stack`, `queue`, `priority_queue` | unit, adapter contract/model over every backend, stress, semantics |
| `map`, `set`, `multimap`, `multiset`, `hashmap` | unit, associative contract/model, allocation sweeps, stress, semantics |

Associative allocation sweeps cover construction, `insert`, and, for
`map`/`hashmap`, `insert_replace` growth. A failed insert must leave all
earlier entries intact and the container usable. `remove_if` is covered for the
four ordered associative containers; `hashmap` has no `remove_if` and no
ordered bounds. Adapter allocation is owned by their sequence backends, whose
sweeps cover it.

## Test design

- The generic code knows containers only through descriptors. It never includes
  a container's public header, so the same contract and model compile for every
  sequence container.
- Contracts check shared semantics with fixed inputs. Models keep an independent
  array (or other reference structure) and compare after every operation.
- The test allocator overrides the weak `_p_*` definitions with strong
  definitions. The allocator object is linked before the production archive, so
  accounting, fail-at-N injection, and leak checks apply to production code.
- Each case uses a deterministic seed derived from the case name and the global
  seed. Re-run with `TEST_SEED` to reproduce.
- Allocation sweeps run the scenario once without injection, then once for every
  allocation position. Content expectations are checked only where the
  operation promises rollback.
- The invariant checker reads internal layout and is itself tested with
  deliberately corrupted snapshots.

## Configuration notes

- The vector construction failure sweep is release-only because debug builds
  abort in `JDSC_ASSERT_NEW` on its construction allocation failure. The
  associative construction sweeps run in every configuration, and debug death
  tests cover documented assertion failures instead.
- The C++ differential includes only test-owned descriptors and standard
  library headers. Public C headers are smoke-tested as C; the C++ smoke build
  permits sign-compare warnings from C headers.
- ASan and UBSan are intentionally not used. Valgrind is the memory checker.
- `bench` results are not a correctness gate.

## Failure diagnosis

1. Read the `[ FAIL ]` line, source location, expected/actual values, and trace.
2. Re-run the single case with the printed seed:
   ```sh
   tests/build/default/libj_ds_tests --suite <suite> --filter <name> \
       --seed <seed> --verbose
   ```
3. For memory defects, run the same filter under Valgrind:
   ```sh
   valgrind --leak-check=full --error-exitcode=99 \
       tests/build/default/libj_ds_tests --suite <suite> --filter <name>
   ```
4. Do not change production code to make a test pass without first confirming
   the documented semantics. Record production defects in the matching audit
   report.
