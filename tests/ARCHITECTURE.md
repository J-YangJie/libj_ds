# libj_ds permanent test architecture

Status: architecture baseline for the 2.0.0 release and later 3.0.0/4.0.0
work. The system is implemented for every libj_ds container. Deque was the
first module, not the shape of the system itself; this document remains the
design reference for new modules.

## 1. Goals and non-goals

Goals:

- Give every container in `libj_ds` the same permanent test layers: build/API
  smoke, unit semantics, shared contracts, randomized models, differential
  checks against the C++ standard library where meaningful, fault injection,
  long stress, and an optional benchmark.
- Keep one reusable core: runner, assertions, cleanup stack, deterministic RNG,
  bounded trace, test allocator, tracked values, death-test helper.
- Let a new container join by adding one directory and a small adapter file.
  The core, contracts, models, and Makefile must not need container-specific
  edits.
- Produce reproducible failures: seed, operation trace, configuration, and the
  exact suite/case name are printed on every failure.
- Keep the test system independent of the production implementation. Models
  are written from scratch; contracts use only public APIs.
- Support the permanent configurations: default, `JDSC_DEBUG`, aligned
  allocation, and 32-bit when the toolchain provides it.

Non-goals:

- Proving the absence of undefined behaviour. Valgrind is the memory checker;
  ASan and UBSan are deliberately not part of this system.
- Replacing demos. Demos remain usage examples, not oracles.
- Running performance measurements as a correctness gate.
- Testing undefined operations with valid-looking inputs. Invalid iterators,
  ownership violations, and empty-container `pop_front/back/top` are only
  exercised through death tests when the library documents an abort.

## 2. Layers

| Layer | Name | Default gate | Contents |
|---|---|---|---|
| L0 | Build/API smoke | yes | public headers compile; all test binaries, stress, and bench compile |
| L1 | Unit semantics | yes | fixed-input tests per container: lifecycle, element types, boundaries, iterators, strings/SSO, custom ops, adapters |
| L2 | Contracts and fault injection | yes | shared sequence/associative/adapter contracts; exhaustive fail-at-N allocation sweeps; debug death tests |
| L3 | Models and differential | yes (short) | independent C models and optional `std::` differential, seeded and traced |
| L4 | Stress | release/nightly | longer random runs, periodic invariant checks, Valgrind |
| L5 | Bench | no | throughput smoke, informational only |

Suites named `stress` and `bench` are excluded by default. `make stress`, or
`--suite stress`, selects them explicitly. All other suites run by default.

## 3. Directory layout

```text
tests/
  ARCHITECTURE.md            this file
  README.md                  commands, failure diagnosis, conventions
  DEQUE_AUDIT.md             2.0.0 deque audit report
  VECTOR_AUDIT.md            2.0.0 vector audit report
  LIST_AUDIT.md              2.0.0 list audit report
  TEST_REPORT.md             real results for the current release
  Makefile                   one build front end; auto-discovers containers
  .gitignore                 build/
  core/
    test.h                   runner, assertions, cleanup, trace, RNG, iters
    test_main.c              CLI parsing and main
    test_case.c              registry, filtering, case execution, leak check
    test_assert.c            failure formatting and longjmp
    test_alloc.c/.h          strong _p_* overrides, fail-at-N, live counters
    test_rng.c/.h            splitmix64/xorshift-style deterministic PRNG
    test_trace.c/.h          bounded ring of recent operations
    test_death.c/.h          fork/wait death helper (debug builds)
    test_value.c/.h          tracked owned value and counters
    test_ops.c/.h            ops tables for tracked, string, SSO fixtures
    self_test.c              negative tests for the test infrastructure itself
  contracts/
    sequence_desc.h          type-erased int32 sequence descriptor
    sequence_str_desc.h      type-erased owned-string descriptor (differential)
    sequence_contract.c      shared sequence semantics
    sequence_model.c         independent array model and random driver
    assoc_desc.h             ordered/hashed associative descriptor
    assoc_contract.c         shared associative semantics
    assoc_model.c            sorted-array/hash-map models and drivers
    assoc_alloc.c/.h         shared fail-at-N associative insert sweeps
    adaptor_desc.h           stack/queue/priority-queue descriptor
    adaptor_contract.c       shared adapter semantics
    adaptor_model.c          independent adapter models and drivers
  containers/
    deque/
      container.mk           metadata consumed by tests/Makefile
      SEMANTICS.md           documented semantics and STL differences
      desc.c/.h              int32 and string descriptors over public vtable
      unit.c                 L1 deque-specific tests
      contract.c             registers generic contracts with the descriptors
      model.c                L3 model suites
      alloc.c                L2 fail-at-N scenarios
      death.c                L2/debug death tests
      value.c/.h             deque-specific fixtures (SSO, custom ops)
      diff.cpp               L3 std::deque differential (optional)
      stress.c               L4
      bench.c                L5
    vector/ list/            same convention as deque, including diff/
    stack/ queue/ priority_queue/
      desc, unit, contract, model, SEMANTICS.md
    hashmap/ map/ multimap/ set/ multiset/
      desc, unit, contract, model, alloc, stress, SEMANTICS.md
  build/<config>/            objects, archives, binaries (generated)
  build/logs/<config>/       run logs (generated)
```

A container directory is a module. Every `.c` file in it is compiled into the
same test binary. Every `.cpp` file is compiled as C++ and the binary is linked
with the C++ driver. `container.mk` supplies only metadata; there is no central
list of containers.

## 4. Build model

### 4.1 Production archive per configuration

Each configuration builds every production source into
`build/<config>/libprod.a`. The list is the complete container library, not a
deque-only subset:

```text
operations/ds_ops_string.c
associative_container/associative_container_ops.c
sequence_container/operations/ds_ops_sso.c
sequence_container/sequence_container_ops.c
stack/stack.c
queue/queue.c
list/list.c
vector/vector.c
deque/deque.c
priority_queue/priority_queue.c
priority_queue/priority_queue_ops.c
hashmap/hashmap.c
hashmap/hashmap_ops.c
map/map.c
multimap/multimap.c
set/set.c
multiset/multiset.c
linux/rbtree.c
sort/sort.c
```

This keeps the archive stable when a new container is tested. A production
build failure is an L0 failure for the whole project, which is desirable.

The test binaries link, in this order:

1. test objects (container module, core, contracts),
2. the strong test allocator object, always as a standalone object,
3. `libprod.a`.

`test_alloc.o` must precede the archive so its strong `_p_malloc`,
`_p_calloc`, `_p_realloc`, `_p_free`, `_p_aligned_malloc`,
`_p_aligned_realloc`, and `_p_aligned_free` override the weak definitions in
`include/_memory.h`. The allocator self-test fails if injection does not work,
so a link-order mistake cannot pass silently.

### 4.2 Configurations

| Config | Flags | Purpose |
|---|---|---|
| `default` | `-std=gnu99 -O2 -Wall -Werror -DJDSC_MEMPOOL -DJDSC_ALIGNMEM=0` | release mirror |
| `debug` | default plus `-DJDSC_DEBUG -O0 -g` | assertions and death tests |
| `align` | default but `-DJDSC_ALIGNMEM=1` | aligned allocation paths |
| `32bit` | default plus `-m32` | ILP32 sizes and SSO layout |

`make matrix` runs available configurations. If `-m32` cannot link, the target
prints `SKIP: 32-bit toolchain unavailable` and records the skip; it never
pretends the configuration passed. Valgrind runs against `default` by default;
`debug` may be run manually but fork-based death cases are not part of the
Valgrind target.

### 4.3 Targets

```text
make test            build and run L0-L3 short suites for every container
make check           test + public header smoke + demo build + stress/bench build
make test-debug      CONFIG=debug, includes death tests
make test-align      CONFIG=align
make test-32         CONFIG=32bit when the toolchain is available
make test-valgrind   default binaries under Valgrind, 0 errors and 0 leaks
make test-diff       only suites named `diff`
make test-model      only suites named `model`
make test-alloc      only suites named `alloc`
make stress          only suites named `stress`, fixed default seed
make stress-long     random seed, prints it
make stress-valgrind stress under Valgrind
make bench           suite `bench`, never a gate
make matrix          default + debug + align (+ 32-bit when available)
make clean           remove tests/build
```

Root `Makefile` additions are thin: `demos: $(DEMO_BINS)`, `test`, and
`check` delegate to `tests/Makefile`. No test logic is duplicated there.

### 4.4 `container.mk` convention

Variables are namespaced by the container directory name so that all metadata
files can be included by one Makefile:

```make
# containers/deque/container.mk
deque_PUBLIC_HEADERS := include/deque/deque.h
deque_CFLAGS         := -DTEST_MODEL_ITERS=3000
deque_LDFLAGS        :=
deque_HAS_DIFF       := 1
deque_MODEL_ITERS    := 3000
deque_STRESS_ITERS   := 200000
```

Only `*_PUBLIC_HEADERS` and optional flags are required. Sources are
discovered by file name, so adding `diff.cpp`, `stress.c`, or `bench.c` needs
no metadata change.

## 5. Core runtime

### 5.1 Runner and CLI

`core/test.h` is C99 and C++11 compatible and wraps declarations in
`extern "C"` when compiled as C++.

```c
TEST_CASE(unit, push_pop_boundaries) { ... }
```

The macro registers the case through an ELF constructor. A case has a suite
token and a name token; both use lowercase snake_case. The runner sorts cases
by `suite.case`, detects duplicate names, and runs them independently.

CLI:

```text
--suite NAME[,NAME]   run only these suites (repeatable)
--filter TEXT         run cases whose suite.case contains TEXT
--seed N              override the build default seed
--iters N             override the default model/stress iteration count
--trace-last N        number of trace rows printed on failure (default 32)
--list                list cases and exit
--verbose             print each case, not only failures
--help
```

A failure uses `setjmp/longjmp` to stop the case. The runner then:

1. prints `[ FAIL ] suite.case`, location, message, expected/actual,
2. prints the recent trace,
3. runs the case cleanup stack in LIFO order with failure injection disabled,
4. compares tracked live blocks with the pre-case baseline and fails on a
   container leak,
5. continues with the next case.

The process exits non-zero if any case failed. A `SKIP` result is printed with
its reason and does not fail the run.

### 5.2 Assertions

```c
TEST_ASSERT_TRUE(cond)
TEST_ASSERT_FALSE(cond)
TEST_ASSERT_EQ(expected, actual)       /* integer/pointer-value compare */
TEST_ASSERT_NE(a, b)
TEST_ASSERT_NULL(ptr)
TEST_ASSERT_NOT_NULL(ptr)
TEST_ASSERT_PTR_EQ(expected, actual)
TEST_ASSERT_STR_EQ(expected, actual)
TEST_ASSERT_OK(expr)                   /* bool-returning operation */
TEST_FAIL(fmt, ...)
TEST_SKIP(fmt, ...)
```

Integer values are printed as `long long` for diagnostics; pointer values use
`(const void*)` only for printf matching. No arbitrary casts, no `PRI*`
macros. Failure messages are sentence-case English and quote identifiers with
backticks.

### 5.3 Cleanup stack

```c
void test_cleanup_push(void (*fn)(void*), void* arg);
void test_cleanup_pop(bool run);
```

Every resource created by a case is registered immediately after creation.
Cleanup functions call public delete/destroy functions and must not assert;
they run after a failure and after success. The runner checks allocation
baselines after cleanup, so a missing cleanup is reported as a leak.

### 5.4 Deterministic RNG and trace

- `test_rng_seed`, `test_rng_u64`, `test_rng_u32`, `test_rng_bounded`,
  `test_rng_range`, `test_rng_percent`.
- The runner initializes the RNG per case from
  `mix(global_seed, fnv1a("suite.case"))`, so filtering does not change a
  case's seed. The effective seed is printed in the case header.
- `test_trace("push_back", index, value, 0)` writes to a fixed ring (256
  entries). No allocation, no `snprintf` in hot loops. The last N entries are
  printed on failure.
- Randomized suites record every operation. Long stress records operations
  too; the ring is enough to reproduce the immediate context.

### 5.5 Test allocator

Strong overrides provide exact accounting:

- `test_alloc_reset_stats`, `test_alloc_calls`, `test_alloc_live_blocks`,
  `test_alloc_live_bytes`, `test_alloc_peak_blocks`.
- `test_alloc_count_begin` / `test_alloc_count_end` count allocation attempts.
- `test_alloc_fail_at(n)`: the n-th allocation after arming fails and returns
  NULL; later allocations succeed. `n == 0` disables injection.
- `test_alloc_disarm`: disable injection and preserve counters.
- `_p_free` and `_p_aligned_free` accept `void**` and set `*ptr = NULL`.
- `_p_realloc` preserves the old block on failure. `_p_calloc` checks
  multiplication overflow. Size zero and NULL operands follow libc.
- Blocks carry a magic header and tail canary, checked on realloc/free.
  Corruption aborts with a diagnostic rather than silently continuing.
- Alignment semantics mirror `include/_memory.h`; the allocator self-test
  covers both default and over-aligned requests regardless of the current
  `JDSC_ALIGNMEM` setting of the production code.

Accounting is single-threaded. Tests are single-threaded; a container using
threads would need a separate allocator instance and is out of scope.

### 5.6 Generic fail-at-N sweep

`core` provides:

```c
typedef struct test_alloc_scenario {
    const char* name;
    void* (*setup)(void);
    bool (*operate)(void* state);
    void (*verify)(void* state, bool succeeded);
    void (*teardown)(void* state);
} test_alloc_scenario_t;

void test_alloc_sweep(const test_alloc_scenario_t* scenario);
```

The sweep:

1. runs setup/teardown once to verify a clean baseline,
2. runs setup, arms counting, runs operate/teardown once to learn the maximum
   number of allocation attempts,
3. for N = 1 through max + 1, runs setup, arms `fail_at(N)`, runs operate,
   disarms, verifies the resulting state (both the failed and the succeeded
   case are legal), tears down, and checks that live counters returned to the
   baseline.

The operation must not leave the container corrupted, and teardown must always
succeed. `verify` uses normal assertions; a failure longjmps to the runner,
whose cleanup stack destroys the current scenario state.

## 6. Container adapter protocol

Descriptors are type-erased function tables implemented by each container
module. They contain only public-API calls. Generic contracts and model
drivers are written against these tables and never include container headers.

### 6.1 Sequence descriptor

Canonical element type: `int32_t`.

```c
typedef bool (*test_seq_visit_fn)(void* ctx, int32_t value);

typedef struct test_seq_desc {
    const char* name;
    unsigned caps;                 /* TEST_SEQ_* bits */
    void* (*create)(void);
    void (*destroy)(void* self);
    size_t (*size)(const void* self);
    bool (*get)(const void* self, size_t index, int32_t* out);
    bool (*foreach)(const void* self, bool reverse,
                    test_seq_visit_fn fn, void* ctx);
    bool (*push_back)(void* self, int32_t value);
    bool (*push_front)(void* self, int32_t value);
    void (*pop_back)(void* self);
    void (*pop_front)(void* self);
    bool (*insert)(void* self, size_t index, int32_t value,
                   size_t* out_index);
    bool (*insert_n)(void* self, size_t index, size_t n, int32_t value,
                     size_t* out_index);
    bool (*insert_range_values)(void* self, size_t index,
                                const int32_t* values, size_t n,
                                size_t* out_index);
    bool (*erase)(void* self, size_t index, size_t* out_next);
    bool (*erase_range)(void* self, size_t first, size_t last,
                        size_t* out_next);
    bool (*find)(const void* self, int32_t value, size_t* out_index);
    size_t (*count)(const void* self, int32_t value);
    size_t (*remove)(void* self, int32_t value);
    size_t (*remove_if)(void* self, bool (*pred)(int32_t, void*), void* ctx);
    bool (*assign)(void* self, size_t n, int32_t value);
    bool (*assign_range_values)(void* self, const int32_t* values, size_t n);
    bool (*resize)(void* self, size_t n, int32_t value);
    bool (*reserve)(void* self, size_t n);
    void (*shrink_to_fit)(void* self);
    size_t (*clear)(void* self);
    void (*swap)(void* self, void* other);
} test_seq_desc_t;
```

Capabilities:

```text
TEST_SEQ_PUSH_FRONT
TEST_SEQ_POP_FRONT
TEST_SEQ_RESERVE
TEST_SEQ_SHRINK
TEST_SEQ_ASSIGN_RANGE
TEST_SEQ_INSERT_RANGE
TEST_SEQ_REMOVE_IF
TEST_SEQ_SWAP
```

A callback for an absent capability is NULL. `insert_range_values` and
`assign_range_values` construct a temporary source container of the same type,
copy the values into it, call the public range API, and destroy it. This keeps
the generic driver independent of concrete iterator structs. Range aliasing
from the destination container is undefined by the library and is not tested
through this path.

Out-of-range `get` returns false. Empty `pop_*` is never called by generic
drivers. Returned-index callbacks convert the returned iterator to an index
inside the adapter; the contract also verifies the element content at that
index. `out_next` for `erase_range(first, last)` is the index of the old
element at `last`, or `size` after deletion; for an empty range it is `first`.

### 6.2 Owned-string descriptor

Used for `std::deque<std::string>`-style differential tests and for SSO
containers that own string storage.

```c
typedef bool (*test_seq_str_visit_fn)(void* ctx, const char* value);

typedef struct test_seq_str_desc {
    const char* name;
    void* (*create)(void);
    void (*destroy)(void* self);
    size_t (*size)(const void* self);
    bool (*get)(const void* self, size_t index, char* buf, size_t cap);
    bool (*foreach)(const void* self, bool reverse,
                    test_seq_str_visit_fn fn, void* ctx);
    bool (*push_back)(void* self, const char* value);
    bool (*push_front)(void* self, const char* value);
    bool (*insert)(void* self, size_t index, const char* value,
                   size_t* out_index);
    bool (*erase)(void* self, size_t index, size_t* out_next);
    size_t (*clear)(void* self);
} test_seq_str_desc_t;
```

`get` copies into the caller buffer and returns false if the buffer is too
small. Values are non-empty because the library's string `valid_data`
callbacks reject empty strings. The descriptor is optional: a container
without owned strings does not export one.

### 6.3 Associative descriptor

Canonical key and value type: `int32_t`. Sets report `value == key`.

```c
typedef bool (*test_assoc_visit_fn)(void* ctx, int32_t key, int32_t value);

typedef struct test_assoc_desc {
    const char* name;
    unsigned caps;             /* TEST_ASSOC_* bits */
    void* (*create)(void);
    void (*destroy)(void* self);
    size_t (*size)(const void* self);
    size_t (*count)(const void* self, int32_t key);
    bool (*insert)(void* self, int32_t key, int32_t value,
                   bool* out_inserted);
    bool (*insert_replace)(void* self, int32_t key, int32_t value);
    bool (*erase_key)(void* self, int32_t key, int32_t* out_next_key,
                      bool* out_next_found);
    size_t (*remove)(void* self, int32_t key);
    bool (*find)(const void* self, int32_t key, int32_t* out_value);
    bool (*lower_bound)(const void* self, int32_t key,
                        int32_t* out_key, int32_t* out_value);
    bool (*upper_bound)(const void* self, int32_t key,
                        int32_t* out_key, int32_t* out_value);
    bool (*foreach)(const void* self, bool reverse,
                    test_assoc_visit_fn fn, void* ctx);
    size_t (*clear)(void* self);
    void (*swap)(void* self, void* other);
} test_assoc_desc_t;
```

Capabilities:

```text
TEST_ASSOC_ORDERED        traversal is sorted by key
TEST_ASSOC_MULTI          duplicate keys are allowed
TEST_ASSOC_HAS_VALUE      map/hashmap; set uses value == key
TEST_ASSOC_INSERT_REPLACE map/hashmap insert-or-assign
TEST_ASSOC_BOUNDS         ordered containers with lower/upper_bound
TEST_ASSOC_HASHED         bucket-based; order is unspecified
```

`insert` returns false only for an operational failure (for example
allocation); `out_inserted` reports whether a new element was added, so a
unique-container duplicate is a successful call with `out_inserted == false`.
`erase_key` erases one element with that key and reports the iterator's
successor as a key. `find` returns true if a matching element exists.
`lower_bound`/`upper_bound` return false when no such element exists.

Equal-key ordering inside multimap/multiset is not asserted until the
implementation documents a guarantee. Contracts compare keys in order and
treat equal keys as an unordered group.

### 6.4 Adapter descriptor

```c
typedef enum test_adaptor_kind {
    TEST_ADAPTOR_STACK,
    TEST_ADAPTOR_QUEUE,
    TEST_ADAPTOR_PRIORITY_QUEUE
} test_adaptor_kind_t;

typedef struct test_adaptor_desc {
    const char* name;
    test_adaptor_kind_t kind;
    void* (*create)(void);
    void (*destroy)(void* self);
    size_t (*size)(const void* self);
    bool (*empty)(const void* self);
    bool (*push)(void* self, int32_t value);
    void (*pop)(void* self);
    bool (*peek)(const void* self, int32_t* out);       /* top/front */
    bool (*peek_back)(const void* self, int32_t* out);  /* queue only */
    size_t (*clear)(void* self);
    void (*swap)(void* self, void* other);
} test_adaptor_desc_t;
```

Each backend variant is a separate descriptor. For example the stack module
exports descriptors for deque, vector, and list backends. The same contract
and model driver then run against each backend without container-specific
test logic.

## 7. Shared contracts and models

### 7.1 Sequence contract

For every sequence descriptor:

- lifecycle: empty size; `clear` returns 0 and keeps the container usable;
- push/pop front and back where supported, including repeated cycles;
- `get`, forward traversal, and reverse traversal agree with each other and
  with `size`;
- `insert`, `insert_n(0/1/many)`, and `insert_range` return the first inserted
  position and preserve surrounding order;
- `erase`, `erase_range(empty/partial/entire)` return the correct successor;
- `assign`, `assign_range`, `resize`, and `remove` update content and size;
- `find`/`count`/`remove_if` match a reference count;
- `clear` returns the number of removed elements;
- `swap` exchanges contents and leaves both containers usable;
- failed operations never leave a value that cannot be destroyed.

### 7.2 Sequence model

`contracts/sequence_model.c` implements an independent dynamic array with
`memmove`; it does not share code with any production container. A weighted
operation generator drives the descriptor and the model together:

- boundaries are preferred: empty, size 0/1, both ends, bucket/block edges for
  containers with blocks, and random middle positions;
- `insert_n` occasionally uses a large n to cross internal boundaries;
- values come from a small range so duplicates and removal are frequent;
- every step verifies size, full forward and reverse content, and sampled
  `get`/`find`/`count`;
- the trace records operation, index, n, and value.

Model iterations default to `TEST_MODEL_ITERS` or `--iters`. Stress uses the
same model driver with a larger iteration budget and periodic rather than
per-step full scans.

### 7.3 Associative contract and models

Contract checks for each descriptor:

- empty behavior: `size == 0`, `count == 0`, `find` false, `remove` 0;
- unique insert twice: second call succeeds with `out_inserted == false`, size
  unchanged;
- multi insert twice: both inserted, count 2, size 2;
- `insert_replace`: replaces an existing value or inserts a new key;
- `find`, `count`, and, for ordered containers, `lower_bound`/`upper_bound`;
- ordered traversal is strictly increasing by key; reverse is decreasing;
- hashed traversal contains exactly the model entries (order ignored);
- `erase_key` returns the correct successor iterator, including the last
  element (no successor);
- `remove` removes all equivalent keys and returns the count;
- `clear` returns the number of removed elements;
- no associative container exposes `swap`, so its descriptor slot is `NULL`
  and the contract does not test it.

Models:

- ordered: a sorted array of `(key, value)` entries; unique or multi mode;
- hashed: an independent chained hash table with a simple deterministic hash;
- set descriptors use `value == key`;
- verification of ordered containers checks full key/value order; hashed
  containers compare sorted key/value multisets.

### 7.4 Adapter contract and models

Common checks: empty state, push/pop order, `size`, `clear` count, `swap`;
`peek` is never called on an empty adapter.

Models:

- stack: array with LIFO;
- queue: ring buffer with FIFO and a back peek;
- priority queue: independent linear-max array (not a heap copied from the
  implementation), so it is a meaningful oracle for the library heap;
- each backend variant is driven separately.

## 8. Value fixtures and element types

Generic descriptors use `int32_t` so contracts stay readable and comparable
across containers. Type-specific behavior is covered by unit and model tests
in each container module, using shared fixtures from `core/test_value.c` and
`core/test_ops.c`:

- native integers of all widths and pointer-width types;
- `float`/`double`, including NaN/inf comparisons through the library's numeric
  `__eq`;
- `char*` string ops;
- `ds_sso_t` SSO values: local capacity 15, boundary lengths 15/16, long
  strings, copying across internal storage, sort with SSO;
- tracked owned struct: magic, unique id, heap member; `copy_data` allocates a
  new owned copy, `free_data` releases it, counters record copy/free/move;
  tests assert independent copies, exact call counts, and zero live objects
  after delete;
- over-aligned struct: alignment assertions are active only in the `align`
  configuration; the default configuration records the known limitation
  instead of asserting a false guarantee (see `DEQUE_AUDIT.md`).

The tracked fixture is node-mode (stored by value in the container slot, with
an owned pointer member) because that exercises the documented deep-copy
contract without relying on pointer-identity equality.

## 9. Memory checking, failure injection, and death tests

- Valgrind is the only sanitizer. The release gate uses:

  ```sh
  valgrind --leak-check=full --show-leak-kinds=all \
           --errors-for-leak-kinds=all --error-exitcode=99 ...
  ```

- Each case checks allocator live counters after cleanup. Each long-running
  suite also checks counters at the end.
- Fail-at-N sweeps exist for construction, every allocation point in
  push/pop bucket growth, map growth, insert/insert_n/insert_range,
  assign/assign_range/resize, shrink, and copy/move callback failures.
- Death tests fork a child, run one operation expected to abort under
  `JDSC_DEBUG`, and require `SIGABRT`. They are registered only when
  `JDSC_DEBUG` is defined. They are excluded from Valgrind runs.
- Release-mode behavior for checked APIs is tested through return values, not
  through signals. Undefined operations are not given expected results.

## 10. Iterator and invalidation policy

Each container module has `SEMANTICS.md` recording:

- which operations invalidate which iterators, as documented, implemented, and
  legally testable;
- element-address stability guarantees;
- return-value differences from the STL (for example, the library's
  `remove` deletes matching elements and returns their count, while
  `std::remove` only partitions);
- checked versus unchecked API behavior and out-of-range tokens.

Rules followed by all tests:

- never use an iterator after an operation that invalidates it;
- reacquire iterators after insertion/erasure and after container-wide
  operations;
- test returned iterators immediately, and separately test element-address
  stability with addresses captured before the operation when the container
  documents it;
- differential bridges map library-specific tokens to explicit indexes or
  "not found" values; they never pretend the library matches STL where it does
  not;
- undefined aliasing (range operations whose source is the destination
  container) is documented as undefined and not exercised as a valid test.

## 11. Adding a container: checklist

1. Create `tests/containers/<name>/`.
2. Add `container.mk` with `name_PUBLIC_HEADERS`; add CFLAGS/LDFLAGS only if
   needed.
3. Implement the applicable descriptor(s) in `desc.c`/`desc.h` using only the
   public vtable. No production internals in generic code.
4. Add `contract.c` that calls the shared registration functions with the
   descriptors:
   `test_sequence_contract_register(&desc)`,
   `test_assoc_contract_register(&desc)`, or
   `test_adaptor_contract_register(&desc)`.
5. Add `model.c` that calls the matching shared model driver.
6. Add `unit.c` for container-specific semantics, element types, SSO/custom
   ops, and any API not covered by a shared contract.
7. Add `alloc.c` scenarios and register them; add `death.c` if the container
   has documented debug assertions.
8. Optionally add `diff.cpp` using the type-erased descriptors (not the public
   libj_ds headers), plus `stress.c` and `bench.c`.
9. Write `SEMANTICS.md` and list actual tests in `README.md`/`TEST_REPORT.md`.
10. Run `make test`, `make test-debug`, `make test-align`, `make test-valgrind`,
    and the container's stress target; record real results.

No core, contract, or Makefile edit is needed when these conventions are
followed. If a new container needs a genuinely new capability, extend the
descriptor with a new capability bit and a NULL callback, then implement the
contract branch once for all containers.

## 12. C++ differential without public C++ headers

The public libj_ds headers are C headers and do not have `extern "C"` guards.
C++ tests therefore include only test-owned headers: the descriptor headers
and `core/test.h`, both C++-safe. A `diff.cpp` file:

- includes `<deque>` (or the relevant STL container) and `sequence_desc.h`;
- obtains the C descriptor through an `extern "C"` declaration exported by
  the container module;
- drives the descriptor and the STL container with the same seeded operation
  stream;
- compares size, full content, traversal order, and mapped return values.

This avoids per-container bridge boilerplate and keeps C++ tests from
depending on libj_ds header hygiene. A separate L0 target still compiles each
public header in C99/C11 with `-Werror`; a C++ syntax-only target records the
known non-`extern "C"` limitation without turning it into a false failure.

## 13. Style and reporting

- New files carry the project GPLv2 header and `Copyright (C) 2026`.
- Four-space indent, snake_case functions and variables, libj_ds-like macro
  naming for public test APIs.
- Comments are concise English; multi-line comments use the ` *  ` style.
  No Chinese comments in test code or docs.
- Error/failure messages are sentence-case, identifier names in backticks.
- No `PRI*` macros, no arbitrary casts; casts are limited to printf argument
  matching.
- Every run prints configuration, seed, suite, case counts, and elapsed time.
  Raw logs live under `tests/build/logs/<config>/`; `TEST_REPORT.md` cites
  real commands and results, including skips and uncovered items.

## 14. Release gate for this system

For the 2.0.0 all-container release:

- audit gate: `DEQUE_AUDIT.md`, `VECTOR_AUDIT.md`, and `LIST_AUDIT.md` are
  complete, based on the current production code, and contain no unverified
  claims; the user decides from these three reports whether 2.0.0 ships as-is
  or needs production changes first;
- `make check`, `make test`, `make test-debug`, `make test-align`, and
  `make test-32` when available pass;
- the tests cover every container: deque, vector, list, stack, queue,
  priority_queue, map, set, multimap, multiset, hashmap;
- `make stress` with a fixed seed passes;
- `make stress-valgrind` reports 0 errors and 0 bytes in use at exit;
- benchmark results are recorded but never block release.

The same targets apply to future containers as they join the system; the
matrix and reporting format remain unchanged. A production defect exposed by
the tests is reported with a minimal reproduction; the production code is not
changed without the user's approval.
