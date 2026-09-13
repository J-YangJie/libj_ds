/*
  Introsort（函数版，调试用）—— 照 bits/stl_algo.h + bits/stl_heap.h 翻译
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

#ifndef __J_SORT_SEQ_H
#define __J_SORT_SEQ_H

#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>

/* ============================================================================
 * 随机访问序列的最小接口。算法只认"下标"，不认容器 —— 容器怎么把下标换成
 * 地址、怎么搬字节，全交给调用方。这样算法本身是普通函数，能下断点、能单步，
 * 调对了再换宏版（sort_intro.h）去博性能。
 *
 * 下标 0 <= i < n；元素步长统一是 step 字节，tmp 是算法内部用的 step 字节暂存。
 * ========================================================================== */
typedef struct i_sort_seq {
    ptrdiff_t n;              /* 元素个数 */
    size_t    step;           /* 单个元素字节数 */
    void*     ctx;            /* 调用方的上下文（容器句柄等） */
    uint8_t*  tmp;            /* step 字节暂存，i_sort_seq_sort 会按需分配 */

    uint8_t* (*at)(void* ctx, ptrdiff_t i);                     /* 第 i 个元素的首地址 */
    void     (*swap)(void* ctx, ptrdiff_t i, ptrdiff_t j);      /* 交换第 i、j 个元素 */
    void     (*copy)(void* ctx, ptrdiff_t dst, ptrdiff_t src);  /* a[dst] = a[src] */
    bool     (*less)(void* ctx, ptrdiff_t i, ptrdiff_t j);      /* a[i] < a[j] */

    /* 这两个方向是反的，别弄混 —— STL 里对应的正是两种不同的比较子：
         linear_insert: comp(__val, __next)      即 tmp < a[j]
         push_heap:     comp(__first+parent, __value) 即 a[j] < tmp   */
    bool     (*less_tmp_elem)(void* ctx, ptrdiff_t j);          /* tmp < a[j] */
    bool     (*less_elem_tmp)(void* ctx, ptrdiff_t j);          /* a[j] < tmp */
} i_sort_seq_t;

/* 排好 [0, n)。tmp 为空时函数内部自己分配再释放 */
void i_sort_seq_sort(i_sort_seq_t* s);

/* 同 stl_algo.h:std::__lg，堆排兜底和深度上限都要用 */
ptrdiff_t i_sort_lg(ptrdiff_t n);

#define I_SORT_THRESHOLD 16

#endif /* __J_SORT_SEQ_H */
