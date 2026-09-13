/*
  Introsort（函数版）—— 照 bits/stl_algo.h + bits/stl_heap.h 翻译

  与 STL 的对应关系（全部用下标 [first, last) 表示区间）：
    __unguarded_linear_insert   -> seq_linear_insert
    __insertion_sort            -> seq_insertion_sort
    __unguarded_insertion_sort  -> seq_unguarded_insertion_sort
    __final_insertion_sort      -> seq_final_insertion_sort
    __move_median_to_first      -> seq_move_median_to_first
    __unguarded_partition       -> seq_partition
    __unguarded_partition_pivot -> seq_partition_pivot
    __push_heap / __adjust_heap -> seq_push_heap / seq_adjust_heap
    __pop_heap / __make_heap    -> seq_pop_heap / seq_make_heap
    __sort_heap                 -> seq_sort_heap
    __introsort_loop / __sort   -> seq_introsort_loop / i_sort_seq_sort

  这是"先调对"的版本：访问元素都过回调，慢，但每一步都能断点。
  调对之后再拿 sort_intro.h 那套宏版去换性能，两者用同一批数据对拍。

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

#include <sort/sort_seq.h>

#include <string.h>
#include <stdlib.h>

ptrdiff_t i_sort_lg(ptrdiff_t n)
{
    ptrdiff_t k;

    for (k = 0; n > 1; n >>= 1)
        ++k;
    return k;
}

/* tmp 的取放：tmp 里存的"当前值"，STL 里是 _Tp __val 那个局部量 */
static inline void seq_take(i_sort_seq_t* s, ptrdiff_t i)
{
    memcpy(s->tmp, s->at(s->ctx, i), s->step);
}

static inline void seq_put(i_sort_seq_t* s, ptrdiff_t i)
{
    memcpy(s->at(s->ctx, i), s->tmp, s->step);
}

/* stl_algo.h:1799 __unguarded_linear_insert —— 不查左边界，靠前面已排好 */
static void seq_linear_insert(i_sort_seq_t* s, ptrdiff_t last)
{
    ptrdiff_t next;

    seq_take(s, last);
    next = last - 1;

    /* 方向要对：STL 是 comp(__val, __next)，值在左 */
    while (s->less_tmp_elem(s->ctx, next)) {
        s->copy(s->ctx, last, next);
        last = next;
        --next;
    }
    seq_put(s, last);
}

/* stl_algo.h:1819 __insertion_sort */
static void seq_insertion_sort(i_sort_seq_t* s, ptrdiff_t first, ptrdiff_t last)
{
    ptrdiff_t i;

    if (first == last)
        return ;

    for (i = first + 1; i != last; ++i) {
        if (s->less(s->ctx, i, first)) {
            ptrdiff_t j;

            seq_take(s, i);
            for (j = i; j != first; --j)          /* _GLIBCXX_MOVE_BACKWARD3(first, i, i+1) */
                s->copy(s->ctx, j, j - 1);
            seq_put(s, first);
        } else {
            seq_linear_insert(s, i);
        }
    }
}

/* stl_algo.h:1843 __unguarded_insertion_sort */
static void seq_unguarded_insertion_sort(i_sort_seq_t* s, ptrdiff_t first, ptrdiff_t last)
{
    ptrdiff_t i;

    for (i = first; i != last; ++i)
        seq_linear_insert(s, i);
}

/* stl_algo.h:1861 __final_insertion_sort */
static void seq_final_insertion_sort(i_sort_seq_t* s, ptrdiff_t first, ptrdiff_t last)
{
    if (last - first > I_SORT_THRESHOLD) {
        seq_insertion_sort(s, first, first + I_SORT_THRESHOLD);
        seq_unguarded_insertion_sort(s, first + I_SORT_THRESHOLD, last);
    } else {
        seq_insertion_sort(s, first, last);
    }
}

/* stl_algo.h:79 __move_median_to_first —— 把 a[a]、a[b]、a[c] 的中位数换到 r */
static void seq_move_median_to_first(i_sort_seq_t* s, ptrdiff_t r, ptrdiff_t a, ptrdiff_t b, ptrdiff_t c)
{
    if (s->less(s->ctx, a, b)) {
        if (s->less(s->ctx, b, c))          s->swap(s->ctx, r, b);
        else if (s->less(s->ctx, a, c))     s->swap(s->ctx, r, c);
        else                                s->swap(s->ctx, r, a);
    } else if (s->less(s->ctx, a, c))       s->swap(s->ctx, r, a);
    else if (s->less(s->ctx, b, c))         s->swap(s->ctx, r, c);
    else                                    s->swap(s->ctx, r, b);
}

/* stl_algo.h:1878 __unguarded_partition —— 基准在 a[pivot]，划分 [first, last) */
static ptrdiff_t seq_partition(i_sort_seq_t* s, ptrdiff_t first, ptrdiff_t last, ptrdiff_t pivot)
{
    for (;;) {
        while (s->less(s->ctx, first, pivot))
            ++first;
        --last;
        while (s->less(s->ctx, pivot, last))
            --last;
        if (!(first < last))
            return first;
        s->swap(s->ctx, first, last);
        ++first;
    }
}

/* stl_algo.h:1900 __unguarded_partition_pivot */
static ptrdiff_t seq_partition_pivot(i_sort_seq_t* s, ptrdiff_t first, ptrdiff_t last)
{
    ptrdiff_t mid = first + (last - first) / 2;

    seq_move_median_to_first(s, first, first + 1, mid, last - 1);
    return seq_partition(s, first + 1, last, first);
}

/* stl_heap.h:134 __push_heap —— 待插入的值在 tmp 里，下标相对 base */
static void seq_push_heap(i_sort_seq_t* s, ptrdiff_t base, ptrdiff_t hole, ptrdiff_t top)
{
    ptrdiff_t parent = (hole - 1) / 2;

    /* 方向与 linear_insert 相反：STL 是 comp(__first + __parent, __value) */
    while (hole > top && s->less_elem_tmp(s->ctx, base + parent)) {
        s->copy(s->ctx, base + hole, base + parent);
        hole = parent;
        parent = (hole - 1) / 2;
    }
    seq_put(s, base + hole);
}

/* stl_heap.h:223 __adjust_heap —— 待下沉的值在 tmp 里 */
static void seq_adjust_heap(i_sort_seq_t* s, ptrdiff_t base, ptrdiff_t hole, ptrdiff_t len)
{
    const ptrdiff_t top = hole;
    ptrdiff_t       second = hole;

    while (second < (len - 1) / 2) {
        second = 2 * (second + 1);
        if (s->less(s->ctx, base + second, base + second - 1))
            --second;
        s->copy(s->ctx, base + hole, base + second);
        hole = second;
    }
    if ((len & 1) == 0 && second == (len - 2) / 2) {
        second = 2 * (second + 1);
        s->copy(s->ctx, base + hole, base + second - 1);
        hole = second - 1;
    }
    seq_push_heap(s, base, hole, top);
}

/* stl_heap.h:339 __make_heap */
static void seq_make_heap(i_sort_seq_t* s, ptrdiff_t first, ptrdiff_t last)
{
    ptrdiff_t len, parent;

    if (last - first < 2)
        return ;

    len = last - first;
    for (parent = (len - 2) / 2; ; --parent) {
        seq_take(s, first + parent);
        seq_adjust_heap(s, first, parent, len);
        if (0 == parent)
            return ;
    }
}

/* stl_heap.h:253 __pop_heap + 418 __sort_heap */
static void seq_sort_heap(i_sort_seq_t* s, ptrdiff_t first, ptrdiff_t last)
{
    while (last - first > 1) {
        --last;
        seq_take(s, last);
        s->copy(s->ctx, last, first);
        seq_adjust_heap(s, first, 0, last - first);
    }
}

/* stl_algo.h:1912 __partial_sort(first, last, last) —— 深度用尽时的兜底 */
static void seq_heap_sort(i_sort_seq_t* s, ptrdiff_t first, ptrdiff_t last)
{
    seq_make_heap(s, first, last);
    seq_sort_heap(s, first, last);
}

/* stl_algo.h:1925 __introsort_loop */
static void seq_introsort_loop(i_sort_seq_t* s, ptrdiff_t first, ptrdiff_t last, ptrdiff_t depth)
{
    while (last - first > I_SORT_THRESHOLD) {
        ptrdiff_t cut;

        if (0 == depth) {
            seq_heap_sort(s, first, last);
            return ;
        }
        --depth;
        cut = seq_partition_pivot(s, first, last);
        seq_introsort_loop(s, cut, last, depth);     /* 右半递归 */
        last = cut;                                  /* 左半继续循环 */
    }
}

/* stl_algo.h:1949 __sort */
void i_sort_seq_sort(i_sort_seq_t* s)
{
    bool own = false;

    if (s->n <= 1)
        return ;

    if (NULL == s->tmp) {
        s->tmp = (uint8_t*)malloc(s->step);
        if (NULL == s->tmp)
            return ;
        own = true;
    }

    seq_introsort_loop(s, 0, s->n, i_sort_lg(s->n) * 2);
    seq_final_insertion_sort(s, 0, s->n);

    if (own)
        free(s->tmp);
    s->tmp = NULL;
}
