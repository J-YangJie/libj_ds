# multiset semantics

This module tests the public `multiset` vtable (`include/multiset/multiset.h`)
through fixed unit cases, the shared associative contract, a randomized
sorted-array model, and fail-at-N allocation sweeps.

## Representation

- An ordered red-black tree (`linux/rbtree.c`); each element is one heap node
  holding a `multiset_key_t`. The stored value is the key itself, so shared
  associative tests use `value == key`.
- Duplicate keys are allowed. Default ordering is ascending through the key
  `__lt` ops; `MULTISET_NEW_T_GT(...)` builds a descending multiset.
- Equal keys form one contiguous group in traversal. The current
  implementation visits equal keys in insertion order, but only the grouping
  and multiplicity are treated as contract; shared tests do not assert the
  order inside a group.
- `begin()`/`end()` are tree iterators. `end()` is a tagged sentinel, not
  `NULL`; a null iterator (`.d == NULL`) denotes a failed `insert` or a
  checked query on a null container. Compare iterators with `it_eq`/`it_ne`.
- `it_data(it)` yields the element token. `it_sdata(it)` is the string
  accessor: for a string multiset it returns the stored `char*`.
- Iterators are node-stable: inserting other keys does not invalidate them;
  erasing an element invalidates only iterators to that element; `clear`
  invalidates all iterators. There is no public `swap`.

## Public API differences from `std::multiset`

- `insert(key)` always inserts and returns an iterator to the new element.
  There is no `insert_replace` and no range insert.
- `erase(it)` returns the in-order successor, or `end()` when the last element
  was erased. `erase(end())` and `erase` on a null iterator return a null
  iterator.
- `find` returns some element equal to the key; `lower_bound` (`>= key`) and
  `upper_bound` (`> key`) return the first and one-past-last position of the
  equal-key group, or `end()` when absent.
- `count` returns the number of equal elements and -1 for a null multiset.
- `remove(key)` removes every element equal to the key and returns the removed
  count; a null multiset returns -1. `remove_if(cond)` calls `cond(key)` and
  returns the removed count, or -1 for a null multiset or callback.
- `clear` returns the removed count and leaves the multiset usable; a null
  multiset returns -1.
- `size` returns -1 for a null multiset. `find`, bounds, and `insert` on a
  null multiset return a null iterator. `MULTISET_DELETE(NULL)` is safe.
- There is no `at`, `swap`, `capacity`, or `reserve`; `max_size` is not
  exposed on the vtable.

Documentation note: the vtable comment on `insert` says a matching key returns
`NULL`. That comment is inaccurate for multiset; the implementation always
inserts, and the tests assert multiset behavior.

## Element types

- Integer, pointer-width, and floating-point keys are stored directly as
  tokens. `float`/`double` keys use the numeric key ops and compare with
  `__lt`; NaN inputs are outside the ordered contract.
- `string` keys use copied C strings (`MULTISET_NEW_T(string)`,
  `ds_skey("...")`); `it_sdata()` returns the stored `char*`. The multiset
  owns the copies and frees them on erase, clear, and delete.
- Custom key ops compose through `MULTISET_NEW_OPS`; `copy_key` must be paired
  with `free_key`.

## Coverage

- `unit.c`: duplicate keys and counts, traversal, bounds, erase successor
  inside a duplicate group, remove and clear, descending `_GT`, string keys,
  and null guards.
- `contract.c`: lifecycle, multi insert, bounds, traversal, erase successor.
- `model.c`: 2000 randomized operations against an independent sorted array.
- `alloc.c`: fail-at-N insert growth. A failed insert leaves the previously
  inserted elements intact and the multiset usable.
