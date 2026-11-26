# Priority queue semantics

This note records the public priority-queue behavior that the permanent tests
rely on. `priority_queue` is an adaptor over a `vector` or `deque` base
container; it maintains a binary heap in the base storage.

## Ordering

- The default constructors (`PRIORITY_QUEUE_NEW_T`, `PRIORITY_QUEUE_NEW_SSO_T`,
  `PRIORITY_QUEUE_NEW_CHAR_T`) use `operator<` on the element type. The heap
  root is the largest element, so `top` returns the maximum.
- The `_GT` constructors install a `>` comparator, so `top` returns the
  smallest element. This matches `std::priority_queue<T, Container,
  std::greater<T>>`.
- Struct and union elements need a `priority_queue_ops_t` table with
  `__lt`; a custom `__move` is optional. Native integer, pointer-width, float,
  and double dtypes use the built-in comparison and pass a null ops table.
- Equal keys compare false in both directions; their relative order is
  unspecified. The tests only require that `top` is one of the maxima (or
  minima for `_GT`).
- The tests do not use NaN keys; the built-in comparison follows C `<`, whose
  result is false for unordered operands.

## Result and error conventions

| Operation | libj_ds |
|---|---|
| `size` | `priority_queue_size_t`; `-1` for a null priority queue |
| `__cpqueue_empty` | diagnostic name only; requires a non-null priority queue |
| `push` | `bool`; false for a null queue, a value rejected by the ops table, an allocation failure in the base, or an unsupported element step |
| `top` on a null or empty priority queue | `.u8 == NULL` |
| `__cpqueue_top` | unchecked variant; requires a non-empty priority queue |
| `pop` on a null or empty priority queue | no-op |
| `clear` | number of removed elements; `-1` for a null priority queue |
| `swap` | `void`; no-op unless both queues are non-null, distinct, and use the same base id |

`push` inserts through the base container and then sifts the new element up.
For every supported dtype and step the sift cannot fail after a successful
insert. On a base allocation failure the base keeps its previous content and
`push` returns false.

`swap` exchanges only the base pointer. Debug builds assert that the ops
tables and element layouts match; release builds do not re-check them.

## Base-container differences

- `vector` and `deque` both provide the heap storage; the heap formulas and
  element access differ, but the observable order is identical.
- SSO elements use a dedicated fast path on both backends. On 64-bit builds
  `ds_sso_t` has a power-of-two size and the deque path uses the same
  bucket-shift arithmetic as numeric elements; on 32-bit builds the deque path
  uses the generic non-power-of-two fallback.
- Owning `char*` and `ds_sso_t` elements are copied on push and freed on pop,
  clear, and delete through the adaptor ops table.
- Custom struct ops are forwarded to the base container as well, so its
  `copy_data`, `free_data`, and `__move` callbacks must be compatible with the
  priority-queue table layout.

## Test coverage

- Descriptors: `adaptor.pq.vector.max`, `adaptor.pq.vector.min`,
  `adaptor.pq.deque.max`, `adaptor.pq.deque.min`.
- The shared adaptor contract sorts the pushed values independently and checks
  ascending or descending extraction; the random adaptor model performs the
  same check against a linear-scan maximum/minimum model.
- Unit cases cover signed and unsigned 8/16/32/64-bit widths, float, double,
  SSO and owning strings, a tracked custom ops fixture on both backends,
  duplicates, clear counts, swap for every descriptor, and null guards.
