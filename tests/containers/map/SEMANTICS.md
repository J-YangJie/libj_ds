# map semantics

This module tests the public `map` vtable (`include/map/map.h`) through fixed
unit cases, the shared associative contract, a randomized sorted-array model,
and fail-at-N allocation sweeps.

## Representation

- An ordered red-black tree (`linux/rbtree.c`); each entry is one heap node
  holding a `map_key_t` and a `map_value_t`.
- Keys are unique. Default ordering is ascending through the key `__lt` ops;
  `MAP_NEW_KV_GT(...)` builds a descending map with a greater-than comparator.
- `begin()`/`end()` are tree iterators. `end()` is a tagged sentinel, not
  `NULL`; a null iterator (`.d == NULL`) only denotes a failed `insert` or a
  checked query on a null container. Compare iterators with `it_eq`/`it_ne`.
- `it_key()` and `it_value()` expose token fields; typed accessors such as
  `it_s32key()`, `it_s64value()`, `it_skey()`, and `it_lfvalue()` convert the
  token for the configured key/value type.
- Iterators are node-stable: inserting other keys does not invalidate them;
  erasing an entry invalidates only iterators to that entry; `clear`
  invalidates all iterators. There is no public `swap`.

## Public API differences from `std::map`

- `insert(key, value)` on a duplicate key is a successful call that returns a
  null iterator; it does not replace the value. `insert_replace(key, value)`
  is the equivalent of `a[key] = value`: it replaces an existing value or
  inserts a new entry.
- `erase(it)` returns the in-order successor, or `end()` when the last entry
  was erased. `erase(end())` and `erase` on a null iterator return a null
  iterator.
- `find`, `lower_bound` (`>= key`), and `upper_bound` (`> key`) return `end()`
  when no entry matches.
- `count` returns 0 or 1 for a non-null map and -1 for a null map.
- `remove(key)` removes all matching entries (at most one) and returns the
  removed count; a null map returns -1. `remove_if(cond)` calls
  `cond(key, value)` and returns the removed count, or -1 for a null map or
  callback.
- `clear` returns the removed count and leaves the map usable; a null map
  returns -1.
- `size` returns -1 for a null map. `find`, bounds, `insert`, and
  `insert_replace` on a null map return a null iterator. `MAP_DELETE(NULL)` is
  safe.
- There is no `at`, `operator[]`, `swap`, `capacity`, or `reserve`; `max_size`
  is not exposed on the vtable.

## Element types

- Integer, pointer-width, and floating-point keys and values are stored
  directly as tokens. `float`/`double` keys use the numeric key ops and compare
  with `__lt`; NaN inputs are outside the ordered contract.
- `string` keys use copied C strings (`ds_skey("...")` plus map key ops);
  `it_skey()` returns the stored `char*`. The map owns the copies and frees
  them on erase, clear, and delete.
- Custom ops compose through `MAP_NEW_KV_OPS`; `copy_key`/`copy_value` must be
  paired with `free_key`/`free_value`.

## Coverage

- `unit.c`: ordering and typed traversal, duplicate insert, insert_replace,
  bounds, remove and clear, descending `_GT`, string keys, typed values, and
  null guards.
- `contract.c`: lifecycle, unique insert, bounds, traversal, erase successor.
- `model.c`: 2000 randomized operations against an independent sorted array.
- `alloc.c`: fail-at-N insert and insert_replace growth. A failed insert leaves
  the previously inserted entries intact and the map usable.
