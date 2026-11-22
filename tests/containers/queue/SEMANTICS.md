# Queue semantics

This note records the public queue behavior that the permanent tests rely on.
`queue` is an adaptor: it owns no storage and forwards every operation to a
`deque` or `list` base container. A `vector` base is not provided because
removing from the front of a vector is not a queue operation.

## Mapping to the base container

| Queue operation | Base operation |
|---|---|
| `push` | `push_back` |
| `front` | `front` |
| `back` | `back` |
| `pop` | `pop_front` |
| `size`, `clear`, `swap` | same-named base operation |
| `__cqueue_empty` | same-named base operation |

The front of the queue is the oldest element and the back is the newest one.

## Result and error conventions

| Operation | libj_ds |
|---|---|
| `size` | `queue_size_t`; `-1` for a null queue |
| `__cqueue_empty` | diagnostic name only; requires a non-null queue |
| `push` | `bool`; false for a null queue, a value rejected by the ops table, or a failed base allocation |
| `front`/`back` on a null queue | `.u8 == NULL` |
| `front`/`back` on an empty queue | forward to the checked base accessor; assertion failure in `JDSC_DEBUG`, undefined otherwise |
| `__cqueue_front`/`__cqueue_back` | unchecked variants; require a non-empty queue |
| `pop` on a null queue | no-op |
| `pop` on an empty queue | forwards to the base pop; assertion failure in `JDSC_DEBUG`, undefined otherwise |
| `clear` | number of removed elements; `-1` for a null queue |
| `swap` | `void`; no-op unless both queues are non-null, distinct, and use the same base id |

`std::queue` compares types at compile time and allows `swap` for any matching
type. In libj_ds the base id is checked at run time; queues over different base
containers silently do not swap. For the same base id, debug builds assert that
the ops tables and element layouts match; release builds do not re-check them.

## Elements, strings, and failure behavior

- Element dtypes, SSO/chars handling, `copy_data`/`free_data`, alignment, and
  the `_COMPLATE` constructors are inherited from the base container.
- A queue performs no element copies of its own; copying, moving, and freeing
  happen exactly as in the base container.
- `push` on a base allocation failure returns false and leaves the queue
  usable, with the same content it had before the call.
- Construction aborts through `JDSC_ASSERT_NEW` when the base pointer is null;
  a failed base construction is also reported through `JDSC_ASSERT_NEW` and
  yields a null queue in release builds.
- `swap` exchanges only the base pointer. Element addresses and iterators
  behave as for the base container after a swap.

## Test coverage

- Descriptors: `adaptor.queue.deque`, `adaptor.queue.list`.
- Shared adaptor contract cases (lifecycle, push/pop order, clear count,
  swap, queue-only back access) and the random adaptor model run for both
  backends.
- Unit cases add float/double/int64 dtypes, SSO and owning-string elements,
  a tracked custom ops fixture, swap for each backend, and null-queue guards.
