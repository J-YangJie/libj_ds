# Stack semantics

This note records the public stack behavior that the permanent tests rely on.
`stack` is an adaptor: it owns no storage and forwards every operation to a
`vector`, `deque`, or `list` base container.

## Mapping to the base container

| Stack operation | Base operation |
|---|---|
| `push` | `push_back` |
| `top` | `back` |
| `pop` | `pop_back` |
| `size`, `clear`, `swap` | same-named base operation |
| `__cstack_empty` | same-named base operation |

Consequently the top of the stack is the last element of the base container,
and iteration order of the base (when used directly) is bottom to top.

## Result and error conventions

| Operation | libj_ds |
|---|---|
| `size` | `stack_size_t`; `-1` for a null stack |
| `__cstack_empty` | diagnostic name only; requires a non-null stack |
| `push` | `bool`; false for a null stack, a value rejected by the ops table, or a failed base allocation |
| `top` on a null stack | `.u8 == NULL` |
| `top` on an empty stack | forwards to the checked base `back`; assertion failure in `JDSC_DEBUG`, undefined otherwise |
| `__cstack_top` | unchecked variant; requires a non-empty stack |
| `pop` on a null stack | no-op |
| `pop` on an empty stack | forwards to the base pop; assertion failure in `JDSC_DEBUG`, undefined otherwise |
| `clear` | number of removed elements; `-1` for a null stack |
| `swap` | `void`; no-op unless both stacks are non-null, distinct, and use the same base id |

`std::stack` has no `size` error channel and swaps any two stacks of the same
type at compile time. In libj_ds the base id is a runtime property: stacks over
different base containers can be constructed with the same element type, and
`swap` silently does nothing for them. For the same base id, debug builds
assert that the ops tables and element layouts match; release builds do not
re-check them.

## Elements, strings, and failure behavior

- Element dtypes, SSO/chars handling, `copy_data`/`free_data`, alignment, and
  the `_COMPLATE` constructors are inherited from the base container.
- A stack performs no element copies of its own; copying, moving, and freeing
  happen exactly as in the base container.
- `push` on a base allocation failure returns false and leaves the stack
  usable, with the same content it had before the call.
- Construction aborts through `JDSC_ASSERT_NEW` when the base pointer is null;
  a failed base construction is also reported through `JDSC_ASSERT_NEW` and
  yields a null stack in release builds.
- `swap` exchanges only the base pointer. Element addresses and iterators
  behave as for the base container after a swap.

## Test coverage

- Descriptors: `adaptor.stack.vector`, `adaptor.stack.deque`,
  `adaptor.stack.list`.
- Shared adaptor contract cases (lifecycle, push/pop order, clear count,
  swap) and the random adaptor model run for all three backends.
- Unit cases add float/double/int64 dtypes, SSO and owning-string elements,
  a tracked custom ops fixture, swap for each backend, and null-stack guards.
