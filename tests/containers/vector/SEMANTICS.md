# vector semantics

This module tests the public `vector` vtable (`include/vector/vector.h`) through
fixed cases, shared sequence contracts, randomized models, a C++ differential,
allocation-failure sweeps, debug death tests, and stress runs.

## Representation

- Elements are contiguous, `step` bytes apart, with `end` one past the last
  element.
- `rend` points at a one-element guard slot before `begin`, so a valid buffer
  allocation is `(capacity + 1) * step` bytes. Element pointers never point at
  the guard.
- `end_of_storage` is `begin + capacity`, and `capacity` is derived from it.
- An empty vector has `begin == end == end_of_storage`. A default-constructed
  vector has capacity zero and still owns the guard allocation.
- `capacity_init` values 1 through 3 are promoted to 4. The value 0 keeps the
  initial capacity at zero. Other positive values are used as given.
- Growth from `push_back`, `insert`, `insert_n`, `insert_range`, `assign`,
  `assign_range`, and `resize` is `size + max(size, n)`, capped by `max_size`.
  Tests only require `capacity >= size` and content preservation, not the exact
  growth factor.

## Public API differences from `std::vector`

- There is no public `empty`; tests use the public `__empty` wrapper.
- There is no `push_front` or `pop_front`. The equivalent front operations are
  `insert(begin, value)` and `erase(begin)`.
- `at(index)` returns a union whose `u8` member is `NULL` for an out-of-range
  index instead of throwing. `it(index)` likewise returns a null iterator when
  `index >= size`; `index == size` must use `end()`.
- `insert_n(..., n == 0, ...)` returns `pos` unchanged. `insert_range` with an
  empty source range returns `pos`. `assign_range` with an empty range clears
  the vector.
- `erase` returns an iterator to the successor, or `end()`. `erase_range`
  returns the iterator that follows the removed range.
- `remove` and `remove_if` remove matching elements and return the removed
  count, unlike `std::remove` plus `erase`.
- `find` returns `end()` when the value is absent. Searching a struct/union
  type without `ops->__eq` asserts in debug builds and silently compares raw
  bytes in release builds.
- `shrink_to_fit` is best effort: allocation failure leaves the capacity
  unchanged and is not reported.
- `reserve(n)` with `n < 0`, `n > max_size`, or a failed allocation returns
  false and leaves the vector unchanged.
- `swap` asserts in debug builds when `ops`, dtype layout, `step`, or `align`
  differ. `swap` with itself is a no-op.

## Iterator and pointer invalidation

- `push_back` without reallocation preserves all element addresses.
- Any operation that reallocates invalidates all iterators, pointers, and
  references. This includes growth-triggering insert/assign/resize, `reserve`
  above the current capacity, and a successful `shrink_to_fit`.
- `insert` and `erase` without reallocation keep the buffer, but they shift
  elements: iterators to shifted or erased positions are invalid.
- `clear` keeps the buffer and capacity; iterators become invalid because the
  logical range is empty.
- `swap` exchanges the buffers, so iterators remain valid but now address the
  other vector's elements.

## Self-reference

`push_back`, `insert`, `insert_n`, growing `resize`, and single-value `assign`
may take a value token that points at an element of the same vector, including
an inline SSO element, a struct slot, or a boxed 8-byte token on 32-bit. The
implementation records the token's byte offset before the buffer changes and
recomputes it after a reallocation or the tail shift, so the copied value is
never read from freed or overwritten storage.

- `unit.vector_self_reference_strings` covers inline and heap-backed SSO
  values with front, middle, and own-position inserts, `insert_n`, `push_back`,
  and growing `resize`, with and without reallocation.
- `unit.vector_self_reference_tracked` covers a struct type with owned memory
  and a `__move` callback across a reallocating insert and `insert_n`.
- `unit.vector_self_reference_w8_32bit` covers 32-bit boxed 8-byte tokens.

`insert_range` and `assign_range` remain undefined when the source range
overlaps the target vector: the range pointers are not adjusted for
reallocation or shifting.

## Element types and operations

- Native numeric element types use the shared sequence ops and compare values
  with equality; `float` and `double` treat `-0.0` as equal to `0.0`.
- Owned strings use the SSO representation (`ds_sso_t`). Strings of up to 15
  bytes are inline (`p == buf`); longer strings own a heap buffer. Reallocation
  and sorting fix inline pointers.
- `VECTOR_NEW_CHAR` copies each C string into a heap buffer. The vector owns
  those copies; the caller's buffer is only read during the operation.
- Struct and union types require `ops->copy_data`; when `copy_data` is set,
  `ops->free_data` is also required. The tests cover a tracked type whose copy
  and free callbacks count live objects.
- `VECTOR_NEW_COMPLATE(ops, dtype, type, capacity_init)` validates `step`,
  `align`, dtype width, `capacity_init`, and SSO/ops consistency. In debug
  builds invalid arguments abort in `JDSC_ASSERT_NEW`; in release builds the
  constructor returns `NULL`.

## Failure behavior

- `push_back`, `insert`, `insert_n`, `insert_range`, `assign`, and
  `assign_range` report allocation failure with false or a null iterator.
  Allocator sweeps verify that the original contents remain intact and the
  vector stays usable.
- `resize` growing is implemented as `insert_n` at `end()` and gives the same
  rollback behavior.
- `shrink_to_fit` has no failure report; the allocation sweep verifies that
  either the new capacity is applied or the old contents remain valid.
- `VECTOR_DELETE` clears the vector, releases the guard buffer, and sets the
  caller's pointer to `NULL`.

## Alignment

`JDSC_ALIGNMEM=0` maps `p_aligned_*` to plain allocation, so over-aligned
element types are not guaranteed to receive their requested alignment.
`JDSC_ALIGNMEM=1` honors `align` for the buffer; the align test uses a 64-byte
aligned 64-byte element type and checks every element address.
