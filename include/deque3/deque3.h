/*
  Deque3 Interfaces

  deque3 = cdeque 的第二代镜像：元素不再是"8 字节槽（装值或装堆指针）"，
  而是 step 字节的对象直接内联在 node 里，step 由 NEW 时传入、运行期固定。
  block 切分、迭代器模型、insert/erase 的"往短的一侧挪"策略都照 bits/stl_deque.h
  翻译成 C —— C++ 里 sizeof(_Tp) / _S_buffer_size() 是编译期常量，
  这里换成运行期的 step，并让迭代器自己也带一份 step。

  deque / deque_stl 原样保留，供后续性能对比。

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

#ifndef __J_DEQUE3_H
#define __J_DEQUE3_H

#include <_types.h>
#include <deque/deque_ops.h>   /* class_deque_ops_t 定义不变，直接复用 */

typedef size_t        deque3_step_t;    /* 单个元素的字节数：1/2/4/8/16/24... 运行期决定 */
typedef deque_size_t  deque3_size_t;
typedef deque_count_t deque3_count_t;
typedef deque_bcount_t deque3_bcount_t;

/* 迭代器：对应 stl_deque.h:113 _Deque_iterator 的 _M_cur/_M_first/_M_last/_M_node，
   外加一个运行期的 step —— 它自己知道 next/prev 该在地址上 ± 多少字节。 */
typedef struct deque3_iterator {
    union {
        uint8_t* cur;      /* 当前元素的首字节地址 */
        void*    d;
    };
    uint8_t*      begin;   /* 本 node 首元素 */
    uint8_t*      end;     /* 本 node 尾后 */
    uint8_t**     bkt;     /* 指向 map 中本 node 的槽位 */
    deque3_step_t step;    /* 元素字节数 */
} deque3_iterator_t;

typedef struct deque3_reverse_iterator {
    union {
        uint8_t* cur;
        void*    d;
    };
    uint8_t*      begin;
    uint8_t*      end;
    uint8_t**     bkt;
    uint8_t*      begin_cur;
    deque3_step_t step;
} deque3_reverse_iterator_t;
typedef deque3_reverse_iterator_t deque3_r_iterator_t;

#include <deque3/i_deque3.h>

typedef i_deque3_t deque3_t;

/* Method 1：与 cdeque_xxx 一一对应 */
static inline deque3_size_t  c3deque_size(const deque3_t* _this)                     { return i_deque3_size(_this); }
static inline deque3_count_t c3deque_count(const deque3_t* _this, deque_data_t data) { return i_deque3_count(_this, data); }
static inline deque3_iterator_t   c3deque_end(const deque3_t* _this)                 { return i_deque3_end(_this); }
static inline deque3_iterator_t   c3deque_begin(const deque3_t* _this)               { return i_deque3_begin(_this); }
static inline deque3_iterator_t   c3deque_next(deque3_iterator_t iterator)           { return i_deque3_next(iterator); }
static inline deque3_iterator_t   c3deque_prev(deque3_iterator_t iterator)           { return i_deque3_prev(iterator); }
static inline deque3_r_iterator_t c3deque_rend(const deque3_t* _this)                { return i_deque3_rend(_this); }
static inline deque3_r_iterator_t c3deque_rbegin(const deque3_t* _this)              { return i_deque3_rbegin(_this); }
static inline deque3_r_iterator_t c3deque_rnext(deque3_r_iterator_t r_iterator)      { return i_deque3_rnext(r_iterator); }
static inline deque3_r_iterator_t c3deque_rprev(deque3_r_iterator_t r_iterator)      { return i_deque3_rprev(r_iterator); }
static inline deque3_iterator_t   c3deque_find(const deque3_t* _this, deque_data_t data) { return i_deque3_find(_this, data); }
static inline bool                c3deque_push_back(deque3_t* _this, deque_data_t data)  { return i_deque3_push_back(_this, data); }
static inline bool                c3deque_push_front(deque3_t* _this, deque_data_t data) { return i_deque3_push_front(_this, data); }
static inline deque3_iterator_t   c3deque_insert(deque3_t* _this, deque3_iterator_t pos, deque_data_t data) { return i_deque3_insert(_this, pos, data); }
static inline deque3_iterator_t   c3deque_insert_n(deque3_t* _this, deque3_iterator_t pos, deque3_size_t n, deque_data_t data) { return i_deque3_insert_n(_this, pos, n, data); }
static inline deque3_iterator_t   c3deque_erase(deque3_t* _this, deque3_iterator_t pos)  { return i_deque3_erase(_this, pos); }
static inline deque3_iterator_t   c3deque_erase_range(deque3_t* _this, deque3_iterator_t iterator_begin, deque3_iterator_t iterator_end) { return i_deque3_erase_range(_this, iterator_begin, iterator_end); }
static inline void                c3deque_pop_back(deque3_t* _this)                  { i_deque3_pop_back(_this); }
static inline void                c3deque_pop_front(deque3_t* _this)                 { i_deque3_pop_front(_this); }
static inline deque3_size_t       c3deque_remove(deque3_t* _this, deque_data_t data) { return i_deque3_remove(_this, data); }
static inline deque3_size_t       c3deque_remove_if(deque3_t* _this, remove_if_condition cond) { return i_deque3_remove_if(_this, cond); }
static inline deque3_size_t       c3deque_clear(deque3_t* _this)                     { return i_deque3_clear(_this); }

/* 元素地址 / 步长 / 迭代器推进 —— 指针运算一律走 i_deque3.h 里的封装 */
static inline void*         c3deque_iter_addr(deque3_iterator_t iterator)            { return i_deque3_iter_addr(iterator); }
static inline deque3_step_t c3deque_iter_step(deque3_iterator_t iterator)            { return i_deque3_iter_step(iterator); }
static inline deque3_size_t c3deque_iter_distance(deque3_iterator_t b, deque3_iterator_t e) { return __i_deque3_iter_distance(b, e); }

/* Method 2：容器元信息 */
static inline deque3_step_t c3deque_step(const deque3_t* _this)     { return _this->step; }
static inline size_t        c3deque_buf_size(const deque3_t* _this) { return _this->buf_size; }

deque3_t* __deque3_new(const class_deque_ops_t* ops, deque3_step_t step);
void      __deque3_delete(deque3_t** _this);

#define DEQUE3_NEW(_step)              __deque3_new(NULL, (_step))
#define DEQUE3_NEW_OPS(_ops, _step)    __deque3_new((_ops), (_step))
#define DEQUE3_NEW_T(_t)               __deque3_new(NULL, sizeof(_t))
#define DEQUE3_NEW_OPS_T(_ops, _t)     __deque3_new((_ops), sizeof(_t))
#define DEQUE3_DELETE(_pptr)           do { __deque3_delete((_pptr)); } while(0)

#endif /* __J_DEQUE3_H */
