# multimap semantics

This module tests the public `multimap` vtable (`include/multimap/multimap.h`)
through fixed unit cases, the shared associative contract, a randomized
sorted-array model, and fail-at-N allocation sweeps.

## Representation

- An ordered red-black tree (`linux/rbtree.c`); each entry is one heap node
  holding a `multimap_key_t` and a `multimap_value_t`.
- Duplicate keys are allowed. Default ordering is ascending through the key
  `__lt` ops; `MULTIMAP_NEW_KV_GT(...)` builds a descending multimap.
- Equal keys form one contiguous group in traversal. The current
  implementation visits equal keys in insertion order, but only the grouping
  and multiplicity are treated as contract; shared tests do not assert the
  order inside a group.
- `begin()`/`end()` are tree iterators. `end()` is a tagged sentinel, not
  `NULL`; a null iterator (`.d == NULL`) only denotes a failed `insert` or a
  checked query on a null container. Compare iterators with `it_eq`/`it_ne`.
- `it_key()` and `it_value()` expose token fields; typed accessors convert the
  configured key/value type.
- Iterators are node-stable: inserting other keys does not invalidate them;
  erasing an entry invalidates only iterators to that entry; `clear`
  invalidates all iterators. There is no public `swap`.

## Public API differences from `std::multimap`

- `insert(key, value)` always inserts and returns an iterator to the new
  entry. There is no `insert_replace` and no range insert.
- `erase(it)` returns the in-order successor, or `end()` when the last entry
  was erased. `erase(end())` and `erase` on a null iterator return a null
  iterator.
- `find` returns some entry with the key; `lower_bound` (`>= key`) and
  `upper_bound` (`> key`) return the first and one-past-last position of the
  equal-key group, or `end()` when absent.
- `count` returns the number of entries with the key and -1 for a null
  multimap.
- `remove(key)` removes every entry with the key and returns the removed
  count; a null multimap returns -1. `remove_if(cond)` calls
  `cond(key, value)` and returns the removed count, or -1 for a null multimap
  or callback.
- `clear` returns the removed count and leaves the multimap usable; a null
  multimap returns -1.
- `size` returns -1 for a null multimap. `find`, bounds, and `insert` on a
  null multimap return a null iterator. `MULTIMAP_DELETE(NULL)` is safe.
- There is no `at`, `swap`, `capacity`, or `reserve`; `max_size` is not
  exposed on the vtable.

Documentation note: the vtable comment on `insert` says a matching key returns
`NULL`. That comment is inaccurate for multimap; the implementation always
inserts, and the tests assert multimap behavior.

## Element types

- Integer, pointer-width, and floating-point keys and values are stored
  directly as tokens. `float`/`double` keys use the numeric key ops and
  compare with `__lt`; NaN inputs are outside the ordered contract.
- `string` keys use copied C strings (`ds_skey("...")`); `it_skey()` returns
  the stored `char*`. The multimap owns the copies and frees them on erase,
  clear, and delete.
- Custom ops compose through `MULTIMAP_NEW_KV_OPS`; `copy_key`/`copy_value`
  must be paired with `free_key`/`free_value`.

## Coverage

- `unit.c`: duplicate keys and counts, traversal and value sums, bounds, erase
  successor inside a duplicate group, remove and clear, descending `_GT`,
  string keys, and null guards.
- `contract.c`: lifecycle, multi insert, bounds, traversal, erase successor.
- `model.c`: 2000 randomized operations against an independent sorted array.
- `alloc.c`: fail-at-N insert growth. A failed insert leaves the previously
  inserted entries intact and the multimap usable.
