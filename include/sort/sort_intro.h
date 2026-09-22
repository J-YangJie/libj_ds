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
 * C 里没有模板，对应做法是「宏展开出一族 static inline 函数」：元素类型 _T、
 * 迭代器 _IT、比较 _LESS 全在展开时确定，于是比较和交换都能内联 —— 这一点
 * 很关键，用函数指针传比较子的话每次比较多一次间接调用，跟 std::sort 就没得比了。
 *
 * 用法（每个元素类型 + 容器组合展开一次）：
 *
 *   I_SORT_DEFINE(名字前缀, 元素类型, 迭代器类型,
 *                 _ADD(it, n)  随机访问：第 n 个位置（n 可为负）
 *                 _NEXT(it)    ++it
 *                 _PREV(it)    --it
 *                              ⚠ 逐格挪一定要用 NEXT/PREV，别写成 _ADD(it, ±1)：
 *                                通用随机访问里通常含"除以 step"或跨 node 判断，
 *                                放在按元素扫描的热路径上会被放大。STL 也是分开的。
 *                 _DIST(a, b)  从 a 到 b 的距离（即 b - a；区间长度写 _DIST(first, last)）
 *                 _EQ(a, b)    两个迭代器是否指向同一位置
 *                 _GET(it)     _T 类型的左值，可直接读也可赋值
 *                 _MOVE(dp, sp) 把 *sp 搬到 *dp（两个都是 _T*）。**所有**元素搬运
 *                              都必须走它，不能写 `*dp = *sp`。
 *                              POD 传 `*(dp) = *(sp)`，编译出来跟原来一模一样；
 *                              自带指针指向自己内部的类型（DS_SSO 的内联串）
 *                              要传带 rebase 的版本，否则 p 会指着旧位置，
 *                              之后被当堆指针 free。
 *                 _LESS(a, b)  a < b，两个 _T 表达式
 *                 _LT(a, b)    迭代器位置先后：a 在 b 之前。⚠ 不能写成 a.cur < b.cur ——
 *                              node 内部的指针跨 node 之间没有可比性，必须按容器
 *                              自己的顺序来（deque 是先比 map 槽再比槽内偏移）
 *                 )
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

#define I_SORT_DEFINE(_name, _T, _IT, _ADD, _NEXT, _PREV, _DIST, _EQ, _GET, _MOVE, _LESS, _LT) \
                                                                                           \
/* stl_algo.h:iter_swap */                                                                 \
static inline void _name##_iter_swap(_IT _a, _IT _b)                                       \
{                                                                                          \
    _T _t;                                                                                 \
                                                                                           \
    _MOVE(&_t, &_GET(_a));                                                                 \
    _MOVE(&_GET(_a), &_GET(_b));                                                           \
    _MOVE(&_GET(_b), &_t);                                                                 \
}                                                                                          \
                                                                                           \
/* 排完自检：整个区间是否非降序。基准里顺手验一下，省得排错了还看不出来 */                 \
static inline int _name##_sorted(_IT _first, _IT _last)                                    \
{                                                                                          \
    _IT _p, _q;                                                                            \
                                                                                           \
    if (_EQ(_first, _last))                                                           \
        return 1;                                                                          \
                                                                                           \
    for (_p = _first, _q = _NEXT(_first); !_EQ(_q, _last); _p = _q, _q = _NEXT(_q))\
        if (_LESS(_GET(_q), _GET(_p)))                                                     \
            return 0;                                                                      \
    return 1;                                                                              \
}                                                                                          \
                                                                                           \
/* stl_algo.h:1799 __unguarded_linear_insert —— 不查边界，靠前面已排好 */                  \
static inline void _name##_linear_insert(_IT _last)                                        \
{                                                                                          \
    _T  _val;                                                                              \
    _IT _next = _PREV(_last);                                                           \
                                                                                           \
    _MOVE(&_val, &_GET(_last));                                                            \
                                                                                           \
    /* 注意方向：STL 这里是 comp(__val, __next)，值在左。调过来会多退一格踩到区间外 */     \
    while (_LESS(_val, _GET(_next))) {                                                     \
        _MOVE(&_GET(_last), &_GET(_next));                                                 \
        _last = _next;                                                                     \
        _next = _PREV(_next);                                                           \
    }                                                                                      \
    _MOVE(&_GET(_last), &_val);                                                            \
}                                                                                          \
                                                                                           \
/* stl_algo.h:1819 __insertion_sort */                                                     \
static inline void _name##_insertion_sort(_IT _first, _IT _last)                           \
{                                                                                          \
    _IT _i;                                                                                \
                                                                                           \
    if (_EQ(_first, _last))                                                           \
        return ;                                                                           \
                                                                                           \
    for (_i = _NEXT(_first); !_EQ(_i, _last); _i = _NEXT(_i)) {                    \
        if (_LESS(_GET(_i), _GET(_first))) {                                               \
            /* _GLIBCXX_MOVE_BACKWARD3(_first, _i, _i + 1) */                              \
            _T  _val;                                                                      \
            _IT _j   = _i;                                                                 \
                                                                                           \
            _MOVE(&_val, &_GET(_i));                                                       \
            while (!_EQ(_j, _first)) {                                                 \
                _IT _jp = _PREV(_j);                                                    \
                _MOVE(&_GET(_j), &_GET(_jp));                                              \
                _j = _jp;                                                                  \
            }                                                                              \
            _MOVE(&_GET(_first), &_val);                                                   \
        } else {                                                                           \
            _name##_linear_insert(_i);                                                     \
        }                                                                                  \
    }                                                                                      \
}                                                                                          \
                                                                                           \
/* stl_algo.h:1843 __unguarded_insertion_sort */                                           \
static inline void _name##_unguarded_insertion_sort(_IT _first, _IT _last)                 \
{                                                                                          \
    _IT _i;                                                                                \
                                                                                           \
    for (_i = _first; !_EQ(_i, _last); _i = _NEXT(_i))                               \
        _name##_linear_insert(_i);                                                         \
}                                                                                          \
                                                                                           \
/* stl_algo.h:1861 __final_insertion_sort */                                               \
static inline void _name##_final_insertion_sort(_IT _first, _IT _last)                     \
{                                                                                          \
    if (_DIST(_first, _last) > I_SORT_THRESHOLD) {                                          \
        _name##_insertion_sort(_first, _ADD(_first, I_SORT_THRESHOLD));                    \
        _name##_unguarded_insertion_sort(_ADD(_first, I_SORT_THRESHOLD), _last);           \
    } else {                                                                               \
        _name##_insertion_sort(_first, _last);                                             \
    }                                                                                      \
}                                                                                          \
                                                                                           \
/* stl_algo.h:79 __move_median_to_first —— 三数取中，把中位数换到 _result */               \
static inline void _name##_move_median_to_first(_IT _result, _IT _a, _IT _b, _IT _c)       \
{                                                                                          \
    if (_LESS(_GET(_a), _GET(_b))) {                                                       \
        if (_LESS(_GET(_b), _GET(_c)))                                                     \
            _name##_iter_swap(_result, _b);                                                \
        else if (_LESS(_GET(_a), _GET(_c)))                                                \
            _name##_iter_swap(_result, _c);                                                \
        else                                                                               \
            _name##_iter_swap(_result, _a);                                                \
    } else if (_LESS(_GET(_a), _GET(_c))) {                                                \
        _name##_iter_swap(_result, _a);                                                    \
    } else if (_LESS(_GET(_b), _GET(_c))) {                                                \
        _name##_iter_swap(_result, _c);                                                    \
    } else {                                                                               \
        _name##_iter_swap(_result, _b);                                                    \
    }                                                                                      \
}                                                                                          \
                                                                                           \
/* stl_algo.h:1878 __unguarded_partition —— 无守卫划分，基准已在 *_pivot 上 */             \
static inline _IT _name##_unguarded_partition(_IT _first, _IT _last, _IT _pivot)           \
{                                                                                          \
    for (;;) {                                                                             \
        while (_LESS(_GET(_first), _GET(_pivot)))                                          \
            _first = _NEXT(_first);                                                      \
        _last = _PREV(_last);                                                           \
        while (_LESS(_GET(_pivot), _GET(_last)))                                           \
            _last = _PREV(_last);                                                       \
        if (!(_LT(_first, _last)))                                                         \
            return _first;                                                                 \
        _name##_iter_swap(_first, _last);                                                  \
        _first = _NEXT(_first);                                                          \
    }                                                                                      \
}                                                                                          \
                                                                                           \
/* stl_algo.h:1900 __unguarded_partition_pivot */                                          \
static inline _IT _name##_partition_pivot(_IT _first, _IT _last)                           \
{                                                                                          \
    _IT _mid = _ADD(_first, _DIST(_first, _last) / 2);                                      \
                                                                                           \
    _name##_move_median_to_first(_first, _NEXT(_first), _mid, _PREV(_last));          \
    return _name##_unguarded_partition(_NEXT(_first), _last, _first);                    \
}                                                                                          \
                                                                                           \
/* stl_heap.h:134 __push_heap */                                                           \
static inline void _name##_push_heap(_IT _first, ptrdiff_t _hole, ptrdiff_t _top, _T* _val)\
{                                                                                          \
    ptrdiff_t _parent = (_hole - 1) / 2;                                                   \
                                                                                           \
    while (_hole > _top && _LESS(_GET(_ADD(_first, _parent)), (*_val))) {                  \
        _MOVE(&_GET(_ADD(_first, _hole)), &_GET(_ADD(_first, _parent)));                   \
        _hole = _parent;                                                                   \
        _parent = (_hole - 1) / 2;                                                         \
    }                                                                                      \
    _MOVE(&_GET(_ADD(_first, _hole)), _val);                                               \
}                                                                                          \
                                                                                           \
/* stl_heap.h:223 __adjust_heap */                                                         \
static inline void _name##_adjust_heap(_IT _first, ptrdiff_t _hole, ptrdiff_t _len, _T* _val)\
{                                                                                          \
    const ptrdiff_t _top = _hole;                                                          \
    ptrdiff_t       _second = _hole;                                                       \
                                                                                           \
    while (_second < (_len - 1) / 2) {                                                     \
        _second = 2 * (_second + 1);                                                       \
        if (_LESS(_GET(_ADD(_first, _second)), _GET(_ADD(_first, _second - 1))))           \
            _second--;                                                                     \
        _MOVE(&_GET(_ADD(_first, _hole)), &_GET(_ADD(_first, _second)));                   \
        _hole = _second;                                                                   \
    }                                                                                      \
                                                                                           \
    if ((_len & 1) == 0 && _second == (_len - 2) / 2) {                                    \
        _second = 2 * (_second + 1);                                                       \
        _MOVE(&_GET(_ADD(_first, _hole)), &_GET(_ADD(_first, _second - 1)));               \
        _hole = _second - 1;                                                               \
    }                                                                                      \
    _name##_push_heap(_first, _hole, _top, _val);                                          \
}                                                                                          \
                                                                                           \
/* stl_heap.h:253 __pop_heap */                                                            \
static inline void _name##_pop_heap(_IT _first, _IT _last, _IT _result)                    \
{                                                                                          \
    _T _val;                                                                               \
                                                                                           \
    _MOVE(&_val, &_GET(_result));                                                          \
    _MOVE(&_GET(_result), &_GET(_first));                                                  \
    _name##_adjust_heap(_first, 0, _DIST(_first, _last), &_val);                            \
}                                                                                          \
                                                                                           \
/* stl_heap.h:339 __make_heap */                                                           \
static inline void _name##_make_heap(_IT _first, _IT _last)                                \
{                                                                                          \
    ptrdiff_t _len, _parent;                                                               \
                                                                                           \
    if (_DIST(_first, _last) < 2)                                                           \
        return ;                                                                           \
                                                                                           \
    _len = _DIST(_first, _last);                                                            \
    for (_parent = (_len - 2) / 2; ; --_parent) {                                          \
        _T _val;                                                                           \
                                                                                           \
        _MOVE(&_val, &_GET(_ADD(_first, _parent)));                                        \
        _name##_adjust_heap(_first, _parent, _len, &_val);                                 \
        if (0 == _parent)                                                                  \
            return ;                                                                       \
    }                                                                                      \
}                                                                                          \
                                                                                           \
/* stl_heap.h:418 __sort_heap */                                                           \
static inline void _name##_sort_heap(_IT _first, _IT _last)                                \
{                                                                                          \
    while (_DIST(_first, _last) > 1) {                                                      \
        _last = _PREV(_last);                                                           \
        _name##_pop_heap(_first, _last, _last);                                            \
    }                                                                                      \
}                                                                                          \
                                                                                           \
/* stl_algo.h:1912 __partial_sort(first, last, last) —— 深度用尽时的堆排兜底 */            \
static inline void _name##_heap_sort(_IT _first, _IT _last)                                \
{                                                                                          \
    _name##_make_heap(_first, _last);                                                      \
    _name##_sort_heap(_first, _last);                                                      \
}                                                                                          \
                                                                                           \
/* stl_algo.h:1925 __introsort_loop —— 右半边递归、左半边循环，省一次递归 */               \
static inline void _name##_introsort_loop(_IT _first, _IT _last, ptrdiff_t _depth)         \
{                                                                                          \
    while (_DIST(_first, _last) > I_SORT_THRESHOLD) {                                       \
        _IT _cut;                                                                          \
                                                                                           \
        if (0 == _depth) {                                                                 \
            _name##_heap_sort(_first, _last);                                              \
            return ;                                                                       \
        }                                                                                  \
        --_depth;                                                                          \
        _cut = _name##_partition_pivot(_first, _last);                                     \
        _name##_introsort_loop(_cut, _last, _depth);                                       \
        _last = _cut;                                                                      \
    }                                                                                      \
}                                                                                          \
                                                                                           \
/* stl_algo.h:1949 __sort —— 公开入口就是 _name 本身 */                                     \
static inline void _name(_IT _first, _IT _last)                                            \
{                                                                                          \
    if (!_EQ(_first, _last)) {                                                             \
        _name##_introsort_loop(_first, _last, i_sort_lg(_DIST(_first, _last)) * 2);         \
        _name##_final_insertion_sort(_first, _last);                                       \
    }                                                                                      \
}

#endif /* __J_SORT_INTRO_H */
