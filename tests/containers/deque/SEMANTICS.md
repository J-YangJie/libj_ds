# Deque semantics and STL differences

This note records the public deque behavior that the permanent tests rely on.
It documents libj_ds choices; it does not claim that the library mirrors
`std::deque` where it deliberately does not.

## Result and error conventions

| Operation | libj_ds | `std::deque` |
|---|---|---|
| `size` | `deque_size_t`; `-1` for a null container | `size_type` |
| `max_size` | per-container element limit | per-container limit |
| `empty` | only the diagnostic name `__cdeque_empty` | `empty()` |
| `at` | `deque_value_t`; `.u8 == NULL` when the index is out of range | throws `std::out_of_range` |
| `it(index)` | null iterator when `index >= size`; use `end()` for the past-the-end position | valid iterator for `begin() + size()` |
| `front`/`back` on empty | assertion failure in `JDSC_DEBUG`; undefined otherwise | undefined |
| `pop_front`/`pop_back` on empty | assertion failure in `JDSC_DEBUG`; undefined otherwise | undefined |
| `insert_n(pos, 0, ...)` | returns `pos`, no modification | returns `pos` |
| `insert_range(pos, empty)` | returns `pos`, no modification | returns `pos` |
| `erase(pos)` | iterator to the successor, or `end()` | iterator to the successor |
| `erase_range(first, last)` | iterator at the old `last`, or `end()` | iterator `last` |
| `assign(n, value)` | `bool`, false on allocation failure | `void` |
| `assign_range(first, last)` | `bool`; an empty source clears the deque | `void` |
| `insert_range`/`assign_range` aliasing | source and destination must not overlap; not checked | undefined for overlapping ranges from the same container |
| `resize(n, value)` | `bool`, false on allocation failure | `void` |
| `clear` | returns the number of removed elements | returns `void` |
| `shrink_to_fit` | non-binding request; no capacity API is exposed | non-binding request; `capacity()` remains observable |
| `swap` | `void`; incompatible element layouts assert in debug | swaps any two deques with the same allocator rules |

`reserve` and `capacity` do not exist. `max_size` is the only capacity-related
query.

## Value search and removal

- `find(value)` returns an iterator to the first match, `end()` when no element
  matches, and a null iterator for invalid arguments (including values rejected
  by `ops->valid_data`).
- `count(value)` returns the number of matches.
- `remove(value)` erases every match and returns the number removed. This is a
  remove-and-erase operation, unlike `std::remove`, which only partitions.
- `remove_if(pred)` also erases and returns the number removed.
- Struct and union element types need `ops->__eq` for search and removal. In
  release builds a missing comparator is not diagnosed by the library, so the
  tests always install one for such types.

## Strings and SSO

- `DEQUE_NEW_SSO()` stores `ds_sso_t` elements with an inline buffer of
  `__DS_SSO_LOCAL_CAP` (15) characters plus the terminator.
- For strings of at most 15 characters, `ds_sso_t::p` points at `ds_sso_t::buf`.
  Longer strings own a heap buffer. The tests treat this canonical pointer as
  part of the invariants, including after sort and shrink operations.
- `DEQUE_NEW_CHAR()` stores owning `char*` elements through the string ops
  table. Element values are copied in and freed on erase, clear, and delete.
- Passing `NULL` or an empty string to the character/SSO ops is rejected by
  `valid_data`; the corresponding push/insert returns false.

## Single-value self-reference

- `insert(pos, value)` and `insert_n(pos, n, value)` accept a value whose
  address belongs to the same deque for token-address element types: STRUCT and
  UNION slot addresses, inline SSO tokens, and boxed 8-byte numerics on 32-bit.
- A middle insert stages one deep copy in the internal swap slot before the
  shift, so a source on the moved side cannot be relocated or overwritten before
  it is read. The inserted run is still `n` copies of the value; `n - 1` come
  from the staged copy and the final slot receives the staged copy by move.
- Begin/end inserts, `push_front`/`push_back`, `assign(n, value)`, and growing
  `resize` do not shift existing elements, so they read the source directly.
- `insert_range` and `assign_range` still do not support same-container
  overlapping ranges; see the table above.

## Element types and comparators

- Native integer and pointer-width dtypes are constructed with
  `DEQUE_NEW_T(type)`.
- Float and double compare numerically inside the library, so `-0.0` matches
  `0.0`. The tests avoid NaN as a search key.
- Struct/union elements require `copy_data` and `free_data`; the constructor
  rejects `copy_data` without `free_data`. A custom `__move` is optional and
  must deep-move when supplied.
- `DEQUE_NEW_COMPLATE(ops, SEQ_CNTR_DTYPE_STRUCT, type)` is used for custom
  struct fixtures; `SEQ_CNTR_DTYPE_SSO` alone does not identify SSO storage
  because custom structs share the same dtype bit. The invariant checker uses
  the internal `sso` flag instead.

## Iterators and element addresses

- Deque iterators carry a bucket pointer and step; they are not raw element
  pointers. `find` and `insert` return iterator structs.
- The tests observe element addresses only for operations whose implementation
  does not move elements: pushes and pops at either end keep the address of an
  untouched middle element stable. This is an observed property, not a broader
  invalidation promise; the tests re-acquire iterators after middle
  insert/erase, `assign`, `resize`, `clear`, and `swap`.
- `erase`/`erase_range` return iterators valid for the container state they
  leave behind. They are not used after a later mutation.

## Failure behavior

- Operations that grow or compact storage return false and leave a usable
  container when allocation fails. The allocation sweep verifies invariants and
  a follow-up push/pop for every injected failure position.
- `insert_n`, `assign`, and `resize` roll back to the original content on
  failure. `shrink_to_fit` leaves content unchanged on failure. A
  self-referencing middle insert also frees its staged copy and reverses the
  pushed slots and shift before returning failure.
- Construction allocation failures are reported through `JDSC_ASSERT_NEW`. In
  debug builds that macro aborts, so the constructor-failure sweep runs only in
  release configurations; the abort itself is covered by death tests where the
  public API documents an assertion.
- `clear` never fails and always leaves an empty, reusable deque.

## Differential testing

The C++ differential drives only the test-owned descriptor tables, never
libj_ds headers. `std::deque<int32_t>` is compared for push/pop at both ends,
middle insert/erase, assign, resize, remove-all, clear, and swap.
`std::deque<std::string>` is compared for push/pop, insert/erase, clear, and
swap with strings on both sides of the 15-character SSO boundary. Differences
listed above are handled by the descriptor, not by changing comparisons to
match non-STL behavior.
