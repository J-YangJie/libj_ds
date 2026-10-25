/*
  Introsort —— 照 bits/stl_algo.h + bits/stl_heap.h 翻成 C 的宏版
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

#ifndef __J_SORT_INTRO_H
#define __J_SORT_INTRO_H

#include <stddef.h>

/* ============================================================================
 * std::sort 在 libstdc++ 里是三段合起来的 introsort：
 *
 *   __introsort_loop         快排，三数取中 + 无守卫划分；递归深度用尽（2*lg(n)）
 *                            就换成堆排收尾 —— 这是防快排最坏 O(n^2) 的那道保险
 *   __partial_sort(堆排)     make_heap + sort_heap，深度超限时的兜底
 *   __final_insertion_sort   最后一遍插入排序：前 16 个走带守卫的版本，
 *                            其余走无守卫版本（省掉边界判断，因为前面已经排好）
 *
 * C 里没有模板，对应做法是「宏展开出一族 static inline 函数」：元素类型 type、
 * 迭代器 iter、比较 less 全在展开时确定，于是比较和交换都能内联 —— 这一点
 * 很关键，用函数指针传比较子的话每次比较多一次间接调用，跟 std::sort 就没得比了。
 *
 * 用法（每个元素类型 + 容器组合展开一次）：
 *
 *   __i_sort_define(名字前缀, 元素类型, 迭代器类型,
 *                   add(it, n)   随机访问：第 n 个位置（n 可为负）
 *                   next(it)     ++it
 *                   prev(it)     --it
 *                                ⚠ 逐格挪一定要用 next/prev，别写成 add(it, ±1)：
 *                                  通用随机访问里通常含"除以 step"或跨 node 判断，
 *                                  放在按元素扫描的热路径上会被放大。STL 也是分开的。
 *                   dist(a, b)   从 a 到 b 的距离（即 b - a；区间长度写 dist(first, last)）
 *                   eq(a, b)     两个迭代器是否指向同一位置
 *                   get(type, it) type 类型的左值，可直接读也可赋值。容器侧提供：
 *                                纯委托 - 容器知道怎么从迭代器落到元素上，
 *                                而元素类型由这里的 type 给。
 *                   move(dp, sp) 把 *sp 搬到 *dp（两个都是 type*）。**所有**元素搬运
 *                                都必须走它，不能写 `*dp = *sp`。
 *                                POD 传 `*(dp) = *(sp)`，编译出来跟原来一模一样；
 *                                自带指针指向自己内部的类型（DS_SSO 的内联串）
 *                                要传带 rebase 的版本，否则 p 会指着旧位置，
 *                                之后被当堆指针 free。
 *                   less(a, b)   a < b，两个 type 表达式
 *                   lt(a, b)     迭代器位置先后：a 在 b 之前。⚠ 不能写成 a.cur < b.cur ——
 *                                node 内部的指针跨 node 之间没有可比性，必须按容器
 *                                自己的顺序来（deque 是先比 map 槽再比槽内偏移）
 *                   )
 *
 *   展开后直接调 名字(first, last) 排序；另有 名字_sorted(first, last) 做有序自检。
 *
 * 迭代器只要求：能 +n / 两点相减 / 判相等 / 判先后 —— 这四件事都由参数给，
 * 所以连续内存的容器（将来的 vector）把迭代器取成裸指针即可，无需 .cur 之类的字段。
 *
 * 要求迭代器随机访问（+n / 两点相减），元素可赋值（POD 或结构体整体赋值都行）。
 * 排序过程只搬运、不释放任何东西，所以带堆指针的元素（SSO 串那种）也安全。
 * ========================================================================== */

#include <sort/sort_seq.h>   /* I_SORT_THRESHOLD 与 i_sort_lg 与函数版共用一份 */

#define __i_sort_define(name, type, iter, add, next, prev, dist, eq, get, move, less, lt) \
                                                                                          \
/* stl_algo.h:iter_swap */                                                                \
static inline void name##_iter_swap(iter __a, iter __b)                                   \
{                                                                                         \
    type __tmp;                                                                           \
                                                                                          \
    move(&__tmp, &get(type, __a));                                                        \
    move(&get(type, __a), &get(type, __b));                                               \
    move(&get(type, __b), &__tmp);                                                        \
}                                                                                         \
                                                                                          \
/* 排完自检：整个区间是否非降序。基准里顺手验一下，省得排错了还看不出来 */                \
static inline int name##_sorted(iter __first, iter __last)                                \
{                                                                                         \
    iter __cur, __nxt;                                                                    \
                                                                                          \
    if (eq(__first, __last))                                                              \
        return 1;                                                                         \
                                                                                          \
    for (__cur = __first, __nxt = next(__first); !eq(__nxt, __last);                      \
         __cur = __nxt, __nxt = next(__nxt))                                              \
        if (less(get(type, __nxt), get(type, __cur)))                                     \
            return 0;                                                                     \
    return 1;                                                                             \
}                                                                                         \
                                                                                          \
/* stl_algo.h:1799 __unguarded_linear_insert —— 不查边界，靠前面已排好 */                 \
static inline void name##_linear_insert(iter __last)                                      \
{                                                                                         \
    type __value;                                                                         \
    iter __prev = prev(__last);                                                           \
                                                                                          \
    move(&__value, &get(type, __last));                                                   \
                                                                                          \
    /* 注意方向：STL 这里是 comp(__val, __next)，值在左。调过来会多退一格踩到区间外 */    \
    while (less(__value, get(type, __prev))) {                                            \
        move(&get(type, __last), &get(type, __prev));                                     \
        __last = __prev;                                                                  \
        __prev = prev(__prev);                                                            \
    }                                                                                     \
    move(&get(type, __last), &__value);                                                   \
}                                                                                         \
                                                                                          \
/* stl_algo.h:1819 __insertion_sort */                                                    \
static inline void name##_insertion_sort(iter __first, iter __last)                       \
{                                                                                         \
    iter __pos;                                                                           \
                                                                                          \
    if (eq(__first, __last))                                                              \
        return;                                                                           \
                                                                                          \
    for (__pos = next(__first); !eq(__pos, __last); __pos = next(__pos)) {                \
        if (less(get(type, __pos), get(type, __first))) {                                 \
            /* _GLIBCXX_MOVE_BACKWARD3(_first, _i, _i + 1) */                             \
            type __value;                                                                 \
            iter __cur = __pos;                                                           \
            iter __prev;                                                                  \
                                                                                          \
            move(&__value, &get(type, __pos));                                            \
            while (!eq(__cur, __first)) {                                                 \
                __prev = prev(__cur);                                                     \
                move(&get(type, __cur), &get(type, __prev));                              \
                __cur = __prev;                                                           \
            }                                                                             \
            move(&get(type, __first), &__value);                                          \
        } else {                                                                          \
            name##_linear_insert(__pos);                                                  \
        }                                                                                 \
    }                                                                                     \
}                                                                                         \
                                                                                          \
/* stl_algo.h:1843 __unguarded_insertion_sort */                                          \
static inline void name##_unguarded_insertion_sort(iter __first, iter __last)             \
{                                                                                         \
    iter __pos;                                                                           \
                                                                                          \
    for (__pos = __first; !eq(__pos, __last); __pos = next(__pos))                        \
        name##_linear_insert(__pos);                                                      \
}                                                                                         \
                                                                                          \
/* stl_algo.h:1861 __final_insertion_sort */                                              \
static inline void name##_final_insertion_sort(iter __first, iter __last)                 \
{                                                                                         \
    if (dist(__first, __last) > I_SORT_THRESHOLD) {                                       \
        name##_insertion_sort(__first, add(__first, I_SORT_THRESHOLD));                   \
        name##_unguarded_insertion_sort(add(__first, I_SORT_THRESHOLD), __last);          \
    } else {                                                                              \
        name##_insertion_sort(__first, __last);                                           \
    }                                                                                     \
}                                                                                         \
                                                                                          \
/* stl_algo.h:79 __move_median_to_first —— 三数取中，把中位数换到 __result */             \
static inline void name##_move_median_to_first(iter __result, iter __left,                \
                                               iter __mid, iter __right)                  \
{                                                                                         \
    if (less(get(type, __left), get(type, __mid))) {                                      \
        if (less(get(type, __mid), get(type, __right)))                                   \
            name##_iter_swap(__result, __mid);                                            \
        else if (less(get(type, __left), get(type, __right)))                             \
            name##_iter_swap(__result, __right);                                          \
        else                                                                              \
            name##_iter_swap(__result, __left);                                           \
    } else if (less(get(type, __left), get(type, __right))) {                             \
        name##_iter_swap(__result, __left);                                               \
    } else if (less(get(type, __mid), get(type, __right))) {                              \
        name##_iter_swap(__result, __right);                                              \
    } else {                                                                              \
        name##_iter_swap(__result, __mid);                                                \
    }                                                                                     \
}                                                                                         \
                                                                                          \
/* stl_algo.h:1878 __unguarded_partition —— 无守卫划分，基准已在 __pivot 上 */            \
static inline iter name##_unguarded_partition(iter __first, iter __last, iter __pivot)    \
{                                                                                         \
    for (;;) {                                                                            \
        while (less(get(type, __first), get(type, __pivot)))                              \
            __first = next(__first);                                                      \
        __last = prev(__last);                                                            \
        while (less(get(type, __pivot), get(type, __last)))                               \
            __last = prev(__last);                                                        \
        if (!(lt(__first, __last)))                                                       \
            return __first;                                                               \
        name##_iter_swap(__first, __last);                                                \
        __first = next(__first);                                                          \
    }                                                                                     \
}                                                                                         \
                                                                                          \
/* stl_algo.h:1900 __unguarded_partition_pivot */                                         \
static inline iter name##_partition_pivot(iter __first, iter __last)                      \
{                                                                                         \
    iter __mid = add(__first, dist(__first, __last) / 2);                                 \
                                                                                          \
    name##_move_median_to_first(__first, next(__first), __mid, prev(__last));             \
    return name##_unguarded_partition(next(__first), __last, __first);                    \
}                                                                                         \
                                                                                          \
/* stl_heap.h:134 __push_heap */                                                          \
static inline void name##_push_heap(iter __first, ptrdiff_t __hole, ptrdiff_t __top,      \
                                    type* __value)                                        \
{                                                                                         \
    ptrdiff_t __parent = (__hole - 1) / 2;                                                \
                                                                                          \
    while (__hole > __top && less(get(type, add(__first, __parent)), *__value)) {         \
        move(&get(type, add(__first, __hole)), &get(type, add(__first, __parent)));       \
        __hole = __parent;                                                                \
        __parent = (__hole - 1) / 2;                                                      \
    }                                                                                     \
    move(&get(type, add(__first, __hole)), __value);                                      \
}                                                                                         \
                                                                                          \
/* stl_heap.h:223 __adjust_heap */                                                        \
static inline void name##_adjust_heap(iter __first, ptrdiff_t __hole, ptrdiff_t __len,    \
                                      type* __value)                                      \
{                                                                                         \
    const ptrdiff_t __top = __hole;                                                       \
    ptrdiff_t __child = __hole;                                                           \
                                                                                          \
    while (__child < (__len - 1) / 2) {                                                   \
        __child = 2 * (__child + 1);                                                      \
        if (less(get(type, add(__first, __child)),                                        \
                 get(type, add(__first, __child - 1))))                                   \
            __child--;                                                                    \
        move(&get(type, add(__first, __hole)), &get(type, add(__first, __child)));        \
        __hole = __child;                                                                 \
    }                                                                                     \
                                                                                          \
    if ((__len & 1) == 0 && __child == (__len - 2) / 2) {                                 \
        __child = 2 * (__child + 1);                                                      \
        move(&get(type, add(__first, __hole)), &get(type, add(__first, __child - 1)));    \
        __hole = __child - 1;                                                             \
    }                                                                                     \
    name##_push_heap(__first, __hole, __top, __value);                                    \
}                                                                                         \
                                                                                          \
/* stl_heap.h:253 __pop_heap */                                                           \
static inline void name##_pop_heap(iter __first, iter __last, iter __result)              \
{                                                                                         \
    type __value;                                                                         \
                                                                                          \
    move(&__value, &get(type, __result));                                                 \
    move(&get(type, __result), &get(type, __first));                                      \
    name##_adjust_heap(__first, 0, dist(__first, __last), &__value);                      \
}                                                                                         \
                                                                                          \
/* stl_heap.h:339 __make_heap */                                                          \
static inline void name##_make_heap(iter __first, iter __last)                            \
{                                                                                         \
    ptrdiff_t __len, __parent;                                                            \
                                                                                          \
    if (dist(__first, __last) < 2)                                                        \
        return;                                                                           \
                                                                                          \
    __len = dist(__first, __last);                                                        \
    for (__parent = (__len - 2) / 2; ; --__parent) {                                      \
        type __value;                                                                     \
                                                                                          \
        move(&__value, &get(type, add(__first, __parent)));                               \
        name##_adjust_heap(__first, __parent, __len, &__value);                           \
        if (0 == __parent)                                                                \
            return;                                                                       \
    }                                                                                     \
}                                                                                         \
                                                                                          \
/* stl_heap.h:418 __sort_heap */                                                          \
static inline void name##_sort_heap(iter __first, iter __last)                            \
{                                                                                         \
    while (dist(__first, __last) > 1) {                                                   \
        __last = prev(__last);                                                            \
        name##_pop_heap(__first, __last, __last);                                         \
    }                                                                                     \
}                                                                                         \
                                                                                          \
/* stl_algo.h:1912 __partial_sort(first, last, last) —— 深度用尽时的堆排兜底 */           \
static inline void name##_heap_sort(iter __first, iter __last)                            \
{                                                                                         \
    name##_make_heap(__first, __last);                                                    \
    name##_sort_heap(__first, __last);                                                    \
}                                                                                         \
                                                                                          \
/* stl_algo.h:1925 __introsort_loop —— 右半边递归、左半边循环，省一次递归 */              \
static inline void name##_introsort_loop(iter __first, iter __last, ptrdiff_t __depth)    \
{                                                                                         \
    while (dist(__first, __last) > I_SORT_THRESHOLD) {                                    \
        iter __cut;                                                                       \
                                                                                          \
        if (0 == __depth) {                                                               \
            name##_heap_sort(__first, __last);                                            \
            return;                                                                       \
        }                                                                                 \
        --__depth;                                                                        \
        __cut = name##_partition_pivot(__first, __last);                                  \
        name##_introsort_loop(__cut, __last, __depth);                                    \
        __last = __cut;                                                                   \
    }                                                                                     \
}                                                                                         \
                                                                                          \
/* stl_algo.h:1949 __sort —— 公开入口就是 name 本身 */                                    \
static inline void name(iter __first, iter __last)                                        \
{                                                                                         \
    if (!eq(__first, __last)) {                                                           \
        name##_introsort_loop(__first, __last,                                            \
                              i_sort_lg(dist(__first, __last)) * 2);                      \
        name##_final_insertion_sort(__first, __last);                                     \
    }                                                                                     \
}

#endif /* __J_SORT_INTRO_H */
