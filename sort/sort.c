/*
  Sorting Algorithms Implementations
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

#include <sort/sort.h>

#include <time.h>
#include <stdlib.h>

/* common */
static inline void __sort_swap(ds_data_t* a, ds_data_t* b)
{
    ds_data_t t = *a;
    *a = *b;
    *b = t;
}

static inline ds_size_t __sort_lg(ds_size_t n)
{
    ds_size_t k;

    for (k = 0; n > 1; n >>= 1)
        ++k;
    return k;
}



/* sort_bubble */
inline void __sort_bubble(ds_data_t* a, ds_size_t size, __comp __comp)
{
    for (ds_size_t i = 0; i < size - 1; ++i) {
        bool flag = false;
        for (ds_size_t j = 0; j < size - 1 - i; ++j) {
            if (__comp(a[j + 1], a[j])) {
                __sort_swap(&a[j], &a[j + 1]);
                flag = true;
            }
        }
        if (!flag)
            break;
    }
}

static inline void __sort_bubble_once_and_check(ds_data_t* a, ds_size_t size, __comp __comp, bool* ordered)
{
    bool flag = true;
    for (ds_data_t j = 0; j < size - 1; ++j) {
        if (__comp(a[j + 1], a[j])) {
            __sort_swap(&a[j], &a[j + 1]);
            flag = false;
        }
    }
    *ordered = flag;
}

inline void sort_bubble(ds_data_t* a, ds_size_t size, __comp __comp)
{
    if (!a || size < 2 || !__comp)
        return ;
    __sort_bubble(a, size, __comp);
}



/* sort_insert */
inline void __sort_insert(ds_data_t* a, ds_size_t size, __comp __comp)
{
    for (ds_size_t i = 0; i < size - 1; ++i) {
        ds_data_t t = a[i + 1];
        ds_size_t j = i;

        while (j >= 0) {
            if (__comp(t, a[j])) {
                a[j + 1] = a[j];
                j--;
            } else {
                break;
            }
        }

        a[j + 1] = t;
    }
}

inline void sort_insert(ds_data_t* a, ds_size_t size, __comp __comp)
{
    if (!a || size < 2 || !__comp)
        return ;
    __sort_insert(a, size, __comp);
}



/* sort_heap */
static inline void __sort_heap_heapify(ds_data_t* a, ds_size_t i, ds_size_t len, __comp __comp)
{
    ds_size_t m;

    for (; (i << 1) + 1 <= len;) {
        ds_size_t ls = (i << 1) + 1;
        ds_size_t rs = (i << 1) + 2;

        m = ls <= len && __comp(a[i], a[ls]) ? ls : i;
        m = rs <= len && __comp(a[m], a[rs]) ? rs : m;

        if (i == m)
            break;

        __sort_swap(&a[i], &a[m]);
        i = m;
    }
}

static inline void __sort_heap_build(ds_data_t* a, ds_size_t len, __comp __comp)
{
    for (ds_size_t i = (len - 1) / 2; i >= 0; --i)
        __sort_heap_heapify(a, i, len, __comp);
}

inline void __sort_heap(ds_data_t* a, ds_size_t size, __comp __comp)
{
    ds_size_t len = size - 1;

    __sort_heap_build(a, len, __comp);
    for (ds_size_t i = len; i >= 1; --i) {
        __sort_swap(&a[i], &a[0]);
        __sort_heap_heapify(a, 0, --len, __comp);
    }
}

inline void sort_heap(ds_data_t* a, ds_size_t size, __comp __comp)
{
    if (!a || size < 2 || !__comp)
        return ;
    __sort_heap(a, size, __comp);
}



/* sort_quick_s */
static inline void __sort_quick_s_move_median_to_first(ds_data_t* a, ds_size_t r, ds_size_t x, ds_size_t y, ds_size_t z, __comp __comp)
{
    /* [ x < y < z ] -> move [ y to r(first) ] */
    if (__comp(a[x], a[y])) {
        if (__comp(a[y], a[z]))
            __sort_swap(&a[r], &a[y]);
        else if (__comp(a[x], a[z]))
            __sort_swap(&a[r], &a[z]);
        else
            __sort_swap(&a[r], &a[x]);
    }
    else if (__comp(a[x], a[z]))
        __sort_swap(&a[r], &a[x]);
    else if (__comp(a[y], a[z]))
        __sort_swap(&a[r], &a[z]);
    else
        __sort_swap(&a[r], &a[y]);
}

static inline ds_size_t __sort_quick_s_unguarded_partition(ds_data_t* a, ds_size_t l, ds_size_t r, ds_size_t p, __comp __comp)
{
    /* limit by [ __sort_quick_s_move_median_to_first ] */
    for (;;) {
        while (__comp(a[l], a[p]))
            ++l;
        --r;
        while (__comp(a[p], a[r]))
            --r;
        if (!(l < r))
            return l;
        __sort_swap(&a[l], &a[r]);
        ++l;
    }
}

static inline ds_size_t __sort_quick_s_unguarded_partition_pivot(ds_data_t* a, ds_size_t l, ds_size_t r, __comp __comp)
{
    /* m = l + 2(min), otherwise 
        -> such as [ l=97, r=100, m=98 ( 97=3, 98=5, 99=2 ) ] 
        -> [ move_m_to_first( r=3, x=5, y=5, z=2 ) ] 
        -> [ 97=5, 98=3, 99=2 ] 
        -> [ unguarded_partition -> while (a[98] < a[97]), while (a[99] < a[97]), while a[100]... -> fault ] */
    ds_size_t m = l + (r - l) / 2;
    __sort_quick_s_move_median_to_first(a, l, l + 1, m, r - 1, __comp);
    return __sort_quick_s_unguarded_partition(a, l + 1, r, l, __comp); /* in unguarded_partition, [ --r ] before while */
}

static inline void __sort_quick_s_loop(ds_data_t* a, ds_size_t l, ds_size_t r, ds_size_t d, __comp __comp)
{
    /* r - l >= 4(min), otherwise segment fault */
    while (r - l > 16) {
        if (d <= 0) {
            __sort_heap(&a[l], r - l + 1, __comp);
            return;
        }

        --d;
        ds_size_t cut = __sort_quick_s_unguarded_partition_pivot(a, l, r, __comp);
        __sort_quick_s_loop(a, cut, r, d, __comp);
        r = cut;
    }
}

static inline void __sort_quick_s(ds_data_t* a, ds_size_t l, ds_size_t r, __comp __comp) /* exclude r -> [l, r) */
{
    if (l < r) {
        __sort_quick_s_loop(a, l, r, __sort_lg(r - l) * 2, __comp);
        __sort_insert(a, r - l, __comp);
    }
}

static inline void _sort_quick_s(ds_data_t* a, ds_size_t size, __comp __comp)
{
    __sort_quick_s(a, 0, size, __comp);
}

inline void __sort_quick(ds_data_t* a, ds_size_t size, __comp __comp)
{
    bool flag = false;

    __sort_bubble_once_and_check(a, size, __comp, &flag);
    if (flag)
        return ;
    _sort_quick_s(a, size - 1, __comp); /* the last element is ordered by bubble */
}

inline void sort_quick(ds_data_t* a, ds_size_t size, __comp __comp)
{
    bool flag = false;

    if (!a || size < 2 || !__comp)
        return ;

    __sort_bubble_once_and_check(a, size, __comp, &flag);
    if (flag)
        return ;
    _sort_quick_s(a, size - 1, __comp); /* the last element is ordered by bubble */
}
