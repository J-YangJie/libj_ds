# set semantics

This module tests the public `set` vtable (`include/set/set.h`) through fixed
unit cases, the shared associative contract, a randomized sorted-array model,
and fail-at-N allocation sweeps.

## Representation

- An ordered red-black tree (`linux/rbtree.c`); each element is one heap node
  holding a `set_key_t`. The stored value is the key itself, so shared
  associative tests use `value == key`.
- Elements are unique. Default ordering is ascending through the key `__lt`
  ops; `SET_NEW_T_GT(...)` builds a descending set with a greater-than
  comparator.
- `begin()`/`end()` are tree iterators. `end()` is a tagged sentinel, not
  `NULL`; a null iterator (`.d == NULL`) denotes a failed `insert` or a
  checked query on a null container. Compare iterators with `it_eq`/`it_ne`.
- `it_data(it)` yields the element token. `it_sdata(it)` is the string
  accessor: for a string set it returns the stored `char*`.
- Iterators are node-stable: inserting other keys does not invalidate them;
  erasing an element invalidates only iterators to that element; `clear`
  invalidates all iterators. There is no public `swap`.

## Public API differences from `std::set`

- `insert(key)` on a duplicate key is a successful call that returns a null
  iterator. There is no `insert_replace`.
- `erase(it)` returns the in-order successor, or `end()` when the last element
  was erased. `erase(end())` and `erase` on a null iterator return a null
  iterator.
- `find`, `lower_bound` (`>= key`), and `upper_bound` (`> key`) return `end()`
  when no element matches.
- `count` returns 0 or 1 for a non-null set and -1 for a null set.
- `remove(key)` removes the matching element and returns the removed count; a
  null set returns -1. `remove_if(cond)` calls `cond(key)` and returns the
  removed count, or -1 for a null set or callback.
- `clear` returns the removed count and leaves the set usable; a null set
  returns -1.
- `size` returns -1 for a null set. `find`, bounds, and `insert` on a null set
  return a null iterator. `SET_DELETE(NULL)` is safe.
- There is no `at`, `swap`, `capacity`, or `reserve`; `max_size` is not exposed
  on the vtable.

## Element types

- Integer, pointer-width, and floating-point keys are stored directly as
  tokens. `float`/`double` keys use the numeric key ops and compare with
  `__lt`; NaN inputs are outside the ordered contract.
- `string` keys use copied C strings (`SET_NEW_T(string)`, `ds_skey("...")`);
  `it_sdata()` returns the stored `char*`. The set owns the copies and frees
  them on erase, clear, and delete.
- Custom key ops compose through `SET_NEW_OPS`; `copy_key` must be paired with
  `free_key`.

## Coverage

- `unit.c`: ordering and traversal, duplicate insert, find and bounds, erase
  successor, remove and clear, descending `_GT`, string keys, and null guards.
- `contract.c`: lifecycle, unique insert, bounds, traversal, erase successor.
- `model.c`: 2000 randomized operations against an independent sorted array.
- `alloc.c`: fail-at-N insert growth. A failed insert leaves the previously
  inserted elements intact and the set usable.
