/*
  Macro Define Sort
  Copyright (C) 2021  YangJie <yangjie98765@yeah.net>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License along
  with this program; if not, write to the Free Software Foundation, Inc.,
  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/

#ifndef __J_SORT_MACRO_H
#define __J_SORT_MACRO_H

#include <stddef.h> /* ptrdiff_t */
#include <stdbool.h>
#include <_compiler.h>

HEADER_STATIC_INLINE
ptrdiff_t __algo_sort_lg(ptrdiff_t n)
{
    ptrdiff_t k;

    for (k = 0; n > 1; n >>= 1)
        ++k;
    return k;
}

#define __I_SORT_THRESHOLD 16

#ifndef __I_SORT_USE_NO_SENTINEL
#define __I_SORT_USE_NO_SENTINEL 0
#endif /* __I_SORT_USE_NO_SENTINEL */

#if __I_SORT_USE_NO_SENTINEL
#define __I_SORT_FINAL_INSERT(name, __first, __last) \
    __algo_sort_##name##_insert_no_sentinel(__first, __last)
#else
#define __I_SORT_FINAL_INSERT(name, __first, __last) \
    __algo_sort_##name##_final_insert(__first, __last)
#endif

/* Three default macros provided for external use */
#define __i_sort_default_move(_dest, _src)  (*(_dest) = *(_src))
#define __i_sort_default_less(_a, _b)       ((_a) < (_b))
#define __i_sort_default_greater(_a, _b)    ((_a) > (_b))

#define __i_sort_define(name, type, __less, __move, iter, __add, __next, __prev, __dist, __eq, __lt, __get) \
                                                                                                    \
/* algo.h:iter_swap */                                                                              \
HEADER_STATIC_INLINE                                                                                \
void __algo_sort_##name##_iter_swap(iter __a, iter __b)                                             \
{                                                                                                   \
    type __tmp;                                                                                     \
                                                                                                    \
    __move(&__tmp, &__get(type, __a));                                                              \
    __move(&__get(type, __a), &__get(type, __b));                                                   \
    __move(&__get(type, __b), &__tmp);                                                              \
}                                                                                                   \
                                                                                                    \
HEADER_STATIC_INLINE                                                                                \
bool __algo_sort_##name##_sorted(iter __first, iter __last)                                         \
{                                                                                                   \
    iter __cur, __nxt;                                                                              \
                                                                                                    \
    if (__eq(__first, __last))                                                                      \
        return true;                                                                                \
                                                                                                    \
    for (__cur = __first, __nxt = __next(__first); !__eq(__nxt, __last);                            \
         __cur = __nxt, __nxt = __next(__nxt))                                                      \
        if (__less(__get(type, __nxt), __get(type, __cur)))                                         \
            return false;                                                                           \
    return true;                                                                                    \
}                                                                                                   \
                                                                                                    \
/* algo.h:1799 */                                                                                   \
HEADER_STATIC_INLINE                                                                                \
void __algo_sort_##name##_insert_linear(iter __last)                                                \
{                                                                                                   \
    type __value;                                                                                   \
    iter __prev_pos = __prev(__last);                                                               \
                                                                                                    \
    __move(&__value, &__get(type, __last));                                                         \
                                                                                                    \
    while (__less(__value, __get(type, __prev_pos))) {                                              \
        __move(&__get(type, __last), &__get(type, __prev_pos));                                     \
        __last = __prev_pos;                                                                        \
        __prev_pos = __prev(__prev_pos);                                                            \
    }                                                                                               \
    __move(&__get(type, __last), &__value);                                                         \
}                                                                                                   \
                                                                                                    \
/* algo.h:1819 */                                                                                   \
HEADER_STATIC_INLINE                                                                                \
void __algo_sort_##name##_insert(iter __first, iter __last)                                         \
{                                                                                                   \
    iter __pos;                                                                                     \
                                                                                                    \
    if (__eq(__first, __last))                                                                      \
        return ;                                                                                    \
                                                                                                    \
    for (__pos = __next(__first); !__eq(__pos, __last); __pos = __next(__pos)) {                    \
        if (__less(__get(type, __pos), __get(type, __first))) {                                     \
            /* _GLIBCXX_MOVE_BACKWARD3(_first, _i, _i + 1) */                                       \
            type __value;                                                                           \
            iter __cur = __pos;                                                                     \
            iter __prev_pos;                                                                        \
                                                                                                    \
            __move(&__value, &__get(type, __pos));                                                  \
            while (!__eq(__cur, __first)) {                                                         \
                __prev_pos = __prev(__cur);                                                         \
                __move(&__get(type, __cur), &__get(type, __prev_pos));                              \
                __cur = __prev_pos;                                                                 \
            }                                                                                       \
            __move(&__get(type, __first), &__value);                                                \
        } else {                                                                                    \
            __algo_sort_##name##_insert_linear(__pos);                                              \
        }                                                                                           \
    }                                                                                               \
}                                                                                                   \
                                                                                                    \
HEADER_STATIC_INLINE                                                                                \
void __algo_sort_##name##_insert_no_sentinel(iter __first, iter __last)                             \
{                                                                                                   \
    iter __pos;                                                                                     \
                                                                                                    \
    if (__eq(__first, __last))                                                                      \
        return ;                                                                                    \
                                                                                                    \
    for (__pos = __next(__first); !__eq(__pos, __last); __pos = __next(__pos)) {                    \
        type __value;                                                                               \
        iter __cur = __pos;                                                                         \
        iter __prev_pos;                                                                            \
                                                                                                    \
        __move(&__value, &__get(type, __cur));                                                      \
        while (!__eq(__cur, __first)) {                                                             \
            __prev_pos = __prev(__cur);                                                             \
            if (!__less(__value, __get(type, __prev_pos)))                                          \
                break;                                                                              \
            __move(&__get(type, __cur), &__get(type, __prev_pos));                                  \
            __cur = __prev_pos;                                                                     \
        }                                                                                           \
        __move(&__get(type, __cur), &__value);                                                      \
    }                                                                                               \
}                                                                                                   \
                                                                                                    \
/* algo.h:1843 */                                                                                   \
HEADER_STATIC_INLINE                                                                                \
void __algo_sort_##name##_insert_unguarded(iter __first, iter __last)                               \
{                                                                                                   \
    iter __pos;                                                                                     \
                                                                                                    \
    for (__pos = __first; !__eq(__pos, __last); __pos = __next(__pos))                              \
        __algo_sort_##name##_insert_linear(__pos);                                                  \
}                                                                                                   \
                                                                                                    \
/* algo.h:1861 */                                                                                   \
HEADER_STATIC_INLINE                                                                                \
void __algo_sort_##name##_final_insert(iter __first, iter __last)                                   \
{                                                                                                   \
    if (__dist(__first, __last) > __I_SORT_THRESHOLD) {                                             \
        __algo_sort_##name##_insert(__first, __add(__first, __I_SORT_THRESHOLD));                   \
        __algo_sort_##name##_insert_unguarded(__add(__first, __I_SORT_THRESHOLD), __last);          \
    } else {                                                                                        \
        __algo_sort_##name##_insert(__first, __last);                                               \
    }                                                                                               \
}                                                                                                   \
                                                                                                    \
/* algo.h:79 */                                                                                     \
HEADER_STATIC_INLINE                                                                                \
void __algo_sort_##name##_move_median_to_first(iter __result, iter __left,                          \
                                               iter __mid, iter __right)                            \
{                                                                                                   \
    if (__less(__get(type, __left), __get(type, __mid))) {                                          \
        if (__less(__get(type, __mid), __get(type, __right)))                                       \
            __algo_sort_##name##_iter_swap(__result, __mid);                                        \
        else if (__less(__get(type, __left), __get(type, __right)))                                 \
            __algo_sort_##name##_iter_swap(__result, __right);                                      \
        else                                                                                        \
            __algo_sort_##name##_iter_swap(__result, __left);                                       \
    } else if (__less(__get(type, __left), __get(type, __right)))                                   \
        __algo_sort_##name##_iter_swap(__result, __left);                                           \
    else if (__less(__get(type, __mid), __get(type, __right)))                                      \
        __algo_sort_##name##_iter_swap(__result, __right);                                          \
    else                                                                                            \
        __algo_sort_##name##_iter_swap(__result, __mid);                                            \
}                                                                                                   \
                                                                                                    \
/* algo.h:1878 */                                                                                   \
HEADER_STATIC_INLINE                                                                                \
iter __algo_sort_##name##_unguarded_partition(iter __first, iter __last, iter __pivot)              \
{                                                                                                   \
    for (;;) {                                                                                      \
        while (__less(__get(type, __first), __get(type, __pivot)))                                  \
            __first = __next(__first);                                                              \
        __last = __prev(__last);                                                                    \
        while (__less(__get(type, __pivot), __get(type, __last)))                                   \
            __last = __prev(__last);                                                                \
        if (!(__lt(__first, __last)))                                                               \
            return __first;                                                                         \
        __algo_sort_##name##_iter_swap(__first, __last);                                            \
        __first = __next(__first);                                                                  \
    }                                                                                               \
}                                                                                                   \
                                                                                                    \
/* algo.h:1900 */                                                                                   \
HEADER_STATIC_INLINE                                                                                \
iter __algo_sort_##name##_unguarded_partition_pivot(iter __first, iter __last)                      \
{                                                                                                   \
    iter __mid = __add(__first, __dist(__first, __last) / 2);                                       \
                                                                                                    \
    __algo_sort_##name##_move_median_to_first(__first, __next(__first), __mid, __prev(__last));     \
    return __algo_sort_##name##_unguarded_partition(__next(__first), __last, __first);              \
}                                                                                                   \
                                                                                                    \
/* heap.h:134 */                                                                                    \
HEADER_STATIC_INLINE                                                                                \
void __algo_sort_##name##_heap_push(iter __first, ptrdiff_t __hole, ptrdiff_t __top,                \
                                    type* __value)                                                  \
{                                                                                                   \
    ptrdiff_t __parent = (__hole - 1) / 2;                                                          \
                                                                                                    \
    /* Bottom-up: sift `__value` upward to find its proper position */                              \
    while (__hole > __top && __less(__get(type, __add(__first, __parent)), *__value)) {             \
        __move(&__get(type, __add(__first, __hole)), &__get(type, __add(__first, __parent)));       \
        __hole = __parent;                                                                          \
        __parent = (__hole - 1) / 2;                                                                \
    }                                                                                               \
    __move(&__get(type, __add(__first, __hole)), __value);                                          \
}                                                                                                   \
                                                                                                    \
/* heap.h:223 */                                                                                    \
HEADER_STATIC_INLINE                                                                                \
void __algo_sort_##name##_heap_heapify(iter __first, ptrdiff_t __hole, ptrdiff_t __len,             \
                                       type* __value)                                               \
{                                                                                                   \
    const ptrdiff_t __top = __hole;                                                                 \
    ptrdiff_t __child = __hole;                                                                     \
                                                                                                    \
    /* Top-down: heapify                                                                            \
     * std heap: only handle the left/right children here; where `__value`                          \
     * belongs is left to `heap_push`, which performs the reverse (upward) pass */                  \
    while (__child < (__len - 1) / 2) {                                                             \
        __child = 2 * (__child + 1);                                                                \
        /* `__child` is right now, left > right */                                                  \
        if (__less(__get(type, __add(__first, __child)),                                            \
                   __get(type, __add(__first, __child - 1))))                                       \
            __child--;                                                                              \
        /* The max/min `__child` move to parent, the first parent is `__value` */                   \
        __move(&__get(type, __add(__first, __hole)), &__get(type, __add(__first, __child)));        \
        __hole = __child;                                                                           \
    }                                                                                               \
                                                                                                    \
    /* After while top-down heapify, single child(only left child) */                               \
    if (0 == (__len & 1) && __child == (__len - 2) / 2) {                                           \
        __child = 2 * (__child + 1);                                                                \
        __move(&__get(type, __add(__first, __hole)), &__get(type, __add(__first, __child - 1)));    \
        __hole = __child - 1;                                                                       \
    }                                                                                               \
    __algo_sort_##name##_heap_push(__first, __hole, __top, __value);                                \
}                                                                                                   \
                                                                                                    \
/* heap.h:253 */                                                                                    \
HEADER_STATIC_INLINE                                                                                \
void __algo_sort_##name##_heap_pop(iter __first, iter __last, iter __result)                        \
{                                                                                                   \
    type __value;                                                                                   \
                                                                                                    \
    /* Selection sort: move the extreme element to the end, shrink __len, and repeat */             \
    __move(&__value, &__get(type, __result));                                                       \
    __move(&__get(type, __result), &__get(type, __first));                                          \
    __algo_sort_##name##_heap_heapify(__first, 0, __dist(__first, __last), &__value);               \
}                                                                                                   \
                                                                                                    \
/* heap.h:339 */                                                                                    \
HEADER_STATIC_INLINE                                                                                \
void __algo_sort_##name##_heap_build(iter __first, iter __last)                                     \
{                                                                                                   \
    ptrdiff_t __len = __dist(__first, __last), __parent;                                            \
                                                                                                    \
    if (__len < 2)                                                                                  \
        return ;                                                                                    \
                                                                                                    \
    for (__parent = (__len - 2) / 2; ; --__parent) {                                                \
        type __value;                                                                               \
                                                                                                    \
        /* Free up the parent slot beforehand by moving the child up */                             \
        __move(&__value, &__get(type, __add(__first, __parent)));                                   \
        __algo_sort_##name##_heap_heapify(__first, __parent, __len, &__value);                      \
        if (0 == __parent)                                                                          \
            return ;                                                                                \
    }                                                                                               \
}                                                                                                   \
                                                                                                    \
/* heap.h:418 */                                                                                    \
HEADER_STATIC_INLINE                                                                                \
void __algo_sort_##name##_heap_loop(iter __first, iter __last)                                      \
{                                                                                                   \
    /* Selection sort: move the extreme element to the end, shrink __len, and repeat */             \
    while (__dist(__first, __last) > 1) {                                                           \
        __last = __prev(__last);                                                                    \
        __algo_sort_##name##_heap_pop(__first, __last, __last);                                     \
    }                                                                                               \
}                                                                                                   \
                                                                                                    \
/* algo.h:1912 */                                                                                   \
HEADER_STATIC_INLINE                                                                                \
void __algo_sort_##name##_heap(iter __first, iter __last)                                           \
{                                                                                                   \
    __algo_sort_##name##_heap_build(__first, __last);                                               \
    __algo_sort_##name##_heap_loop(__first, __last);                                                \
}                                                                                                   \
                                                                                                    \
/* algo.h:1925 */                                                                                   \
HEADER_STATIC_INLINE                                                                                \
void __algo_sort_##name##_quick_loop(iter __first, iter __last, ptrdiff_t __depth)                  \
{                                                                                                   \
    while (__dist(__first, __last) > __I_SORT_THRESHOLD) {                                          \
        iter __cut;                                                                                 \
                                                                                                    \
        if (__depth <= 0) {                                                                         \
            __algo_sort_##name##_heap(__first, __last);                                             \
            return ;                                                                                \
        }                                                                                           \
        --__depth;                                                                                  \
        __cut = __algo_sort_##name##_unguarded_partition_pivot(__first, __last);                    \
        __algo_sort_##name##_quick_loop(__cut, __last, __depth);                                    \
        __last = __cut;                                                                             \
    }                                                                                               \
}                                                                                                   \
                                                                                                    \
/* algo.h:1949 */                                                                                   \
HEADER_STATIC_INLINE                                                                                \
void __algo_sort_##name(iter __first, iter __last)                                                  \
{                                                                                                   \
    if (!__eq(__first, __last)) {                                                                   \
        __algo_sort_##name##_quick_loop(__first, __last,                                            \
                                        __algo_sort_lg(__dist(__first, __last)) * 2);               \
        __I_SORT_FINAL_INSERT(name, __first, __last);                                               \
    }                                                                                               \
}

#endif /* __J_SORT_MACRO_H */
