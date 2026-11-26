# list semantics

This module tests the public `list` vtable (`include/list/list.h`) through fixed
cases, shared sequence contracts, randomized models, a C++ differential,
allocation-failure sweeps, debug death tests, stress runs, and benchmarks.

## Representation

- Every element lives in its own heap node:
  `struct list_head node; uint8_t data[]`, where `data` holds `step` bytes.
- The list owns an embedded sentinel `head`. Links are circular: `head.next` is
  the first node and `head.prev` is the last node; an empty list has
  `head.next == head.prev == &head`.
- `begin()` points at the first node's data slot. `end()` points at the
  sentinel's data slot inside the `list_t` object, so a valid end iterator is
  not `NULL`. A null iterator (`.d == NULL`) is a separate value returned by
  checked queries when no element exists.
- Reverse iteration uses the mirrored `rnext`/`rprev` views of the same links
  (`rnext` aliases `prev`, `rprev` aliases `next`). `rbegin()` is the last node,
  `rend()` is the sentinel, and `rnext()` walks toward the front.
- `size` is maintained by every mutating operation. The sentinel address is
  stable for the lifetime of the `list_t`; node addresses never change while a
  node stays in the list.
- With `JDSC_ALIGNMEM=1`, over-aligned element types receive a per-node padding
  prefix (`pad`) so that `node->data` satisfies `align`.

## Public API differences from `std::list`

- There is no public `empty`; tests use the public `__empty` wrapper.
- There is no `reserve`, `capacity`, or `shrink_to_fit`; capacity is unbounded
  except for `max_size()`, which returns `SSIZE_MAX / step - 1`.
- `at(index)` returns a union whose `u8` member is `NULL` for an out-of-range
  index instead of throwing. `it(index)` returns a null iterator when
  `index >= size`; advancing to the end must use `end()`, because `it(size)` is
  null, not the sentinel.
- `front()`, `back()`, `pop_back()`, and `pop_front()` require a non-empty
  list. Debug builds abort; release builds are out of contract and may read or
  free the sentinel.
- `insert(pos, value)`, `insert_n(pos, n, value)`, and
  `insert_range(pos, first, last)` insert before `pos`. They return an iterator
  to the first inserted element. `n == 0` and an empty source range return
  `pos` unchanged.
- `erase(pos)` returns the successor iterator, `end()` for the last element,
  or `NULL` for `end()` and null arguments. `erase_range(first, last)` removes
  `[first, last)` and returns `last`; `erase_range(end(), end())` returns
  `end()`. A `first` before `last` in the same list is required.
- `find` returns `end()` when the value is absent, not a null iterator.
  `count`, `remove`, and `unique` return counts, with `-1` for null arguments or
  failed `valid_data`.
- `remove` and `remove_if` remove all matches and return the removed count,
  unlike `std::list::remove` and `std::list::remove_if`, which return `void`.
  `unique` and `unique_if` only compare adjacent elements and return the removed
  count.
- `assign(n, value)` replaces the content with `n` copies; `n < 0` returns
  false and `n == 0` clears. `assign_range(first, last)` with an empty range
  clears the list. Source ranges from the target list itself are undefined.
- `resize(n, value)` appends copies when growing and erases the tail when
  shrinking; `n < 0` returns false.
- `splice(pos, other)` moves the entire chain of `other` before `pos` and
  leaves `other` empty. A self splice or an empty `other` is a no-op.
- `splice_range(pos, other, first, last)` moves `[first, last)` of `other`.
  Self splice is supported when `pos` is outside the moved range. In debug
  builds an unreachable `last` or a `pos` inside the range aborts; release
  builds silently leave both lists unchanged.
- `merge(other, lt)` requires both lists to be ordered by `lt`. It is stable,
  moves nodes, and leaves `other` empty. A null comparator is a documented
  extension: it appends the whole chain of `other` after the last element of
  the target without comparing. Merging a list with itself is a no-op.
- `sort(lt)` is a stable in-place merge sort with 64 bins. It only relinks
  nodes, so element addresses and existing element iterators stay valid. A null
  comparator is a no-op, although the header says it must not be null.
- `swap` requires compatible `ops`, dtype, `step`, `align`, and `pad`. Debug
  builds abort on mismatch; release builds are undefined.
- `clear` returns the removed count and leaves the list usable. `swap` with
  itself and null-argument calls are no-ops.

## Compatibility and checking

- `swap`, `splice`, `splice_range`, and `merge` compare element layouts only
  through a debug assertion (`JDSC_ASSERT`). Release builds do not validate
  them, so using incompatible lists is undefined and can read past node slots.
- Public checked entry points reject null pointers, null iterators, empty-list
  edge cases, and out-of-range indexes. Checked functions do not verify that an
  iterator belongs to the target list; inserting, erasing, or replacing through
  a stranger iterator is undefined.
- The `__`-prefixed `it`, `at`, and `replace_index` entries are unchecked by
  design and require `index < size`.
- Searching a struct/union type without `ops->__eq` asserts in debug builds and
  falls back to raw slot comparison in release builds. For widths other than
  1/2/4/8 that fallback compares nothing and finds no match.

## Iterator and node invalidation

- `push_back`, `push_front`, `insert`, `insert_n`, `insert_range`, `splice`,
  `splice_range`, `merge`, `sort`, and `reverse` relink nodes without moving
  element storage. Iterators and pointers to elements stay valid, and they
  follow their nodes into the target list after a splice, merge, or swap.
- `erase` and `pop_back`/`pop_front` invalidate only the removed element's
  iterator and pointers. Other iterators stay valid.
- `resize` shrinking invalidates iterators to removed tail elements; growing
  keeps existing ones.
- `clear` invalidates every element iterator. `assign` and `assign_range`
  replace the entire chain, so every element iterator becomes invalid.
- `end()` and `rend()` of a list object remain valid for the life of that
  object. `swap` exchanges chains and sizes but not the embedded sentinels, so
  each end iterator still belongs to its original `list_t`.
- Links are circular: `next(end())` wraps to `begin()` and
  `prev(begin())` wraps to `end()`. This is an implementation detail outside
  the `std::list` iterator contract; tests confine iteration to
  `[begin, end)` and `[rbegin, rend)`.

## Element types and operations

- Native numeric element types use the shared sequence ops. `float` and
  `double` treat `-0.0` as equal to `0.0`.
- Owned strings use the SSO representation (`ds_sso_t`). Strings of up to 15
  bytes are inline (`p == buf`); longer strings own a heap buffer. Splice,
  merge, sort, and reverse only relink nodes, so inline string pointers remain
  valid. Copying a range from another SSO list deep-copies the strings; the
  source list keeps ownership of its own values.
- `LIST_NEW_CHAR` copies each C string into a heap buffer. The list owns those
  copies; the caller's buffer is only read during the operation.
- Struct and union types require `ops->copy_data`; when `copy_data` is set,
  `ops->free_data` is also required. `replace_it` writes the replacement into a
  per-list swap buffer before freeing the old value, so a failed copy leaves the
  element unchanged.
- `LIST_NEW_COMPLATE(ops, dtype, type)` validates `step`, `align`, dtype width,
  `copy_data`/`free_data` pairing, and SSO/ops consistency. In debug builds
  invalid arguments abort in `JDSC_ASSERT_NEW`; in release builds the
  constructor returns `NULL`. Construction also allocates the `step`-byte swap
  buffer.

## Failure behavior

- `push_back`, `push_front`, `insert`, `insert_n`, `insert_range`, `assign`,
  `assign_range`, and growing `resize` report allocation failure with false or
  a null iterator. `insert_n` and `insert_range` roll back partially inserted
  nodes; `assign` and `assign_range` build the replacement chain before erasing
  the old prefix, so the original content stays intact on failure. Allocator
  sweeps verify contents, invariants, and continued usability.
- `replace_it` returns false when `valid_data` rejects the new value or when the
  value cannot be written to the swap buffer; the old value is preserved.
- `LIST_DELETE` clears the list, releases the swap buffer and the list object,
  and sets the caller's pointer to `NULL`; null pointers are ignored.

## Alignment

`JDSC_ALIGNMEM=0` maps `p_aligned_*` to plain allocation, so over-aligned
element types are not guaranteed to receive their requested alignment.
`JDSC_ALIGNMEM=1` honors `align` for every node; the align test uses a 64-byte
aligned element type and checks every element address.
