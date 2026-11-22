# hashmap semantics

This module tests the public `hashmap` vtable (`include/hashmap/hashmap.h`)
through fixed unit cases, the shared associative contract, a randomized model,
and fail-at-N allocation sweeps.

## Representation

- A bucket array of `hashmap_node_t` heads. Each bucket is a singly linked
  chain or, when a chain reaches `TREEIFY_THRESHOLD` (8) and the bucket count is
  at least `MIN_TREEIFY_CAPACITY` (64), a red-black tree. Rehashing untreeifies
  buckets down to `UNTREEIFY_THRESHOLD` (6).
- Bucket storage is lazy: a new hashmap has `bucket_count() == 0` and no bucket
  array. The first insert allocates `bucket_count_init` buckets, rounded up to
  a power of two. The default initial capacity is 16 and the default maximum is
  `1 << 30`.
- The default load factor is 0.75. When `size > bucket_count * load_factor`
  and the bucket count is below the maximum, rehashing doubles the bucket
  count.
- A failed bucket-array allocation leaves the hashmap unchanged and fails the
  insert. A failed growth reallocation does not fail the insert: the entry is
  inserted into the existing buckets and a later insert retries growth.
- `begin()`/`end()` are bucket-order iterators. `end()` is a tagged sentinel,
  not `NULL`; a null iterator (`.d == NULL`) only denotes a duplicate insert or
  a checked query on a null container. Compare iterators with `it_eq`/`it_ne`.
- Traversal order is unspecified and may change after rehashing. Forward and
  reverse traversal visit the same entries; tests compare content, not order.
- `it_key()`/`it_value()` expose token fields; typed accessors convert the
  configured key/value type. `it_hash()` exposes the stored hash.

## Public API differences from `std::unordered_map`

- `insert(key, value)` on a duplicate key is a successful call that returns a
  null iterator; it does not replace the value. `insert_replace(key, value)` is
  the equivalent of `a[key] = value`.
- `erase(it)` returns the successor in traversal order, or `end()` when the
  last entry was erased. `erase(end())` and `erase` on a null iterator return a
  null iterator.
- `find` returns `end()` when no entry matches. There is no `lower_bound`,
  `upper_bound`, or ordered iteration.
- `count` returns 0 or 1 for a non-null hashmap and -1 for a null hashmap.
- `remove(key)` removes the matching entry and returns the removed count; a
  null hashmap returns -1. `clear` returns the removed count, leaves the
  bucket array allocated, and resets `bucket_valid_count()` to 0; a null
  hashmap returns -1.
- `size`, `bucket_count`, and `bucket_valid_count` return -1 for a null
  hashmap. `find`, `insert`, and `insert_replace` on a null hashmap return a
  null iterator. `HASHMAP_DELETE(NULL)` is safe.
- There is no `at`, `operator[]`, `swap`, `reserve`, or `max_load_factor`; the
  load factor and bucket limits are constructor parameters only.
- Constructor arguments are normalized: non-positive `bucket_count_init`
  becomes 16, non-positive `bucket_count_max` becomes `1 << 30`, an out-of-range
  load factor becomes 0.75, and bucket counts are rounded up to powers of two.

## Element types

- Integer, pointer-width, and floating-point keys and values are stored
  directly as tokens. Numeric keys hash from the token value; `float`/`double`
  keys use numeric hash and equality ops. When no ops are supplied, the key
  token is used directly as the hash.
- `string` keys use copied C strings (`ds_skey("...")`); `it_skey()` returns
  the stored `char*`. The hashmap owns the copies and frees them on erase,
  clear, and delete.
- Custom ops compose through `HASHMAP_NEW_KV_OPS`; `copy_key`/`copy_value` must
  be paired with `free_key`/`free_value`, and `__hash` supplies the bucket
  hash.

## Coverage

- `unit.c`: lazy bucket allocation, duplicate insert, insert_replace, forward
  and reverse content sums, remove and clear, bucket growth over 200 keys,
  `bucket_valid_count` bounds, string keys, typed values, and null guards.
- `contract.c`: lifecycle, unique insert, traversal as a content multiset, and
  erase-successor consistency.
- `model.c`: 2000 randomized operations against an independent chained hash
  table; hashed traversal is compared as sorted key/value multisets.
- `alloc.c`: fail-at-N insert and insert_replace growth. A failed insert leaves
  the previously inserted entries intact and the hashmap usable.
