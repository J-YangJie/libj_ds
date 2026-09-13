/*
  I-Deque3 Internal
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

#ifndef __J_I_DEQUE3_H
#define __J_I_DEQUE3_H

#include <string.h>
#include <_compiler.h>
#include <deque/deque_ops.h>

/* 依赖调用方（deque3.h）先定义好下列类型，再 include 本文件：
     deque3_step_t / deque3_size_t / deque3_count_t / deque3_bcount_t
     deque3_iterator_t / deque3_r_iterator_t
   与 i_deque.h 的既有约定一致，避免两个头文件互相 include。 */

/* 参照 bits/stl_deque.h:
     _S_initial_map_size   —— 初始 map 槽数
     __deque_buf_size      —— 每个 node 的元素个数：size < 512 ? 512 / size : 1
   C++ 里 sizeof(_Tp) 是编译期常量，这里换成运行期的 step。 */
#define _I_DEQUE3_BUF_BYTES        512
#define _I_DEQUE3_INITIAL_MAP_SIZE 8
#define _I_DEQUE3_ITERATOR_REND    ((uint8_t*)(uintptr_t)JDSC_ITERATOR_REND)

#ifndef is_null
#define is_null(X) (!(X))
#endif /* is_null */


/* ============================================================================
 * 一、指针运算封装
 *
 * 元素在 node 里是首尾相接的 step 字节，"第 n 个元素"就是地址 ± n * step。
 * 裸的指针 ± / 相减只允许出现在这一节里；其余任何地方都必须走这些封装
 * （static inline 函数打底 + 宏做语法糖，对应 C++ 的 it += n / it1 - it2）。
 * ========================================================================== */

/* 元素指针 + n 个元素（n 可为负） */
static inline
uint8_t* i_deque3_ptr_add(const uint8_t* p, deque3_step_t step, ptrdiff_t n)
{
    return (uint8_t*)p + (ptrdiff_t)step * n;
}

/* 元素指针 - n 个元素 */
static inline
uint8_t* i_deque3_ptr_sub(const uint8_t* p, deque3_step_t step, ptrdiff_t n)
{
    return i_deque3_ptr_add(p, step, -n);
}

/* 同一 node 内两个元素指针相差多少个元素（对应 C++ 的 p1 - p2） */
static inline
ptrdiff_t i_deque3_ptr_diff(const uint8_t* a, const uint8_t* b, deque3_step_t step)
{
    return (ptrdiff_t)(((const uint8_t*)a - (const uint8_t*)b) / (ptrdiff_t)step);
}

/* 元素地址 —— 调用方自行把这 step 个字节解释成自己的类型 */
static inline
void* i_deque3_ptr_addr(uint8_t* p)
{
    return (void*)p;
}

static inline
const void* i_deque3_ptr_caddr(const uint8_t* p)
{
    return (const void*)p;
}

#define I_DEQUE3_PTR_ADD(_p, _step, _n)   i_deque3_ptr_add((_p), (_step), (ptrdiff_t)(_n))
#define I_DEQUE3_PTR_SUB(_p, _step, _n)   i_deque3_ptr_sub((_p), (_step), (ptrdiff_t)(_n))
#define I_DEQUE3_PTR_DIFF(_a, _b, _step)  i_deque3_ptr_diff((_a), (_b), (_step))
#define I_DEQUE3_PTR_ADDR(_p)             i_deque3_ptr_addr((_p))
#define I_DEQUE3_PTR_CADDR(_p)            i_deque3_ptr_caddr((_p))

/* 每个 node 的元素容量：stl_deque.h:96 __deque_buf_size */
static inline
size_t i_deque3_buf_size(deque3_step_t step)
{
    if (0 == step)
        return 1;
    return step < _I_DEQUE3_BUF_BYTES ? (size_t)(_I_DEQUE3_BUF_BYTES / step) : (size_t)1;
}


/* ============================================================================
 * 二、迭代器 —— 对应 stl_deque.h:113 _Deque_iterator
 *
 * C++ 里 _S_buffer_size() 是编译期常量，这里 node 容量随 step 运行期变化，
 * 所以迭代器额外带一个 step 字段，自己知道 next/prev 该在地址上 ± 多少字节。
 * ========================================================================== */

/* stl_deque.h:259 _M_set_node —— 切到 bkt 指向的新 node（_M_cur 由调用方随后设置）。
   node 容量从本迭代器旧的 begin/end 推出（所有 node 容量相同）。 */
static inline
void __i_deque3_iter_set_node(deque3_iterator_t* it, uint8_t** bkt)
{
    const ptrdiff_t buf = I_DEQUE3_PTR_DIFF(it->end, it->begin, it->step);

    it->bkt   = bkt;
    it->begin = *bkt;
    it->end   = I_DEQUE3_PTR_ADD(it->begin, it->step, buf);
}

/* 同上，但容量显式给出：用于容器侧初始化 / 换 node，此时旧 begin/end 未必可用 */
static inline
void __i_deque3_iter_set_node_n(deque3_iterator_t* it, deque3_step_t step, size_t buf, uint8_t** bkt)
{
    it->step  = step;
    it->bkt   = bkt;
    it->begin = *bkt;
    it->end   = I_DEQUE3_PTR_ADD(it->begin, step, (ptrdiff_t)buf);
}

static inline
void __i_deque3_iter_set_node_r(deque3_r_iterator_t* rit, uint8_t** bkt)
{
    const ptrdiff_t buf = I_DEQUE3_PTR_DIFF(rit->end, rit->begin, rit->step);

    rit->bkt   = bkt;
    rit->begin = *bkt;
    rit->end   = I_DEQUE3_PTR_ADD(rit->begin, rit->step, buf);
}

static inline
void __i_deque3_iter_set_node_rn(deque3_r_iterator_t* rit, deque3_step_t step, size_t buf, uint8_t** bkt)
{
    rit->step  = step;
    rit->bkt   = bkt;
    rit->begin = *bkt;
    rit->end   = I_DEQUE3_PTR_ADD(rit->begin, step, (ptrdiff_t)buf);
}

/* stl_deque.h:188 operator++ */
static inline
deque3_iterator_t __i_deque3_iter_next(deque3_iterator_t it)
{
    it.cur = I_DEQUE3_PTR_ADD(it.cur, it.step, 1);
    if (it.cur == it.end) {
        __i_deque3_iter_set_node(&it, it.bkt + 1);
        it.cur = it.begin;
    }
    return it;
}

/* stl_deque.h:207 operator-- */
static inline
deque3_iterator_t __i_deque3_iter_prev(deque3_iterator_t it)
{
    if (it.cur == it.begin) {
        __i_deque3_iter_set_node(&it, it.bkt - 1);
        it.cur = it.end;
    }
    it.cur = I_DEQUE3_PTR_SUB(it.cur, it.step, 1);
    return it;
}

/* stl_deque.h:227 operator+= */
static inline
deque3_iterator_t __i_deque3_iter_add(deque3_iterator_t it, ptrdiff_t n)
{
    const ptrdiff_t buf = I_DEQUE3_PTR_DIFF(it.end, it.begin, it.step);
    const ptrdiff_t off = n + I_DEQUE3_PTR_DIFF(it.cur, it.begin, it.step);

    if (off >= 0 && off < buf) {
        it.cur = I_DEQUE3_PTR_ADD(it.cur, it.step, n);
        return it;
    }

    {
        const ptrdiff_t noff = off > 0 ? off / buf : -((-off - 1) / buf) - 1;

        __i_deque3_iter_set_node(&it, it.bkt + noff);
        it.cur = I_DEQUE3_PTR_ADD(it.begin, it.step, off - noff * buf);
    }
    return it;
}

/* stl_deque.h:246 operator-= */
static inline
deque3_iterator_t __i_deque3_iter_sub(deque3_iterator_t it, ptrdiff_t n)
{
    return __i_deque3_iter_add(it, -n);
}

/* 对应 C++ 的 it += n / it -= n / ++it / --it */
#define I_DEQUE3_IT_ADD(_it, _n)  do { (_it) = __i_deque3_iter_add((_it), (ptrdiff_t)(_n)); } while (0)
#define I_DEQUE3_IT_SUB(_it, _n)  do { (_it) = __i_deque3_iter_sub((_it), (ptrdiff_t)(_n)); } while (0)
#define I_DEQUE3_IT_INC(_it)      do { (_it) = __i_deque3_iter_next(_it); } while (0)
#define I_DEQUE3_IT_DEC(_it)      do { (_it) = __i_deque3_iter_prev(_it); } while (0)

/* stl_deque.h:355 friend operator- —— 两个迭代器相差多少个元素 */
static inline
deque3_size_t __i_deque3_iter_distance(deque3_iterator_t begin, deque3_iterator_t end)
{
    if (begin.bkt == end.bkt)
        return (deque3_size_t)I_DEQUE3_PTR_DIFF(end.cur, begin.cur, begin.step);

    return (deque3_size_t)(
              I_DEQUE3_PTR_DIFF(begin.end, begin.begin, begin.step) * (end.bkt - begin.bkt - 1)
            + I_DEQUE3_PTR_DIFF(end.cur, end.begin, begin.step)
            + I_DEQUE3_PTR_DIFF(begin.end, begin.cur, begin.step));
}

/* 位置先后比较 —— 对应 STL 的 it1 < it2。
   注意不能直接比 it.cur：那是 node 内部的指针，两个迭代器落在不同 node 时
   比的是两块独立 malloc 的地址，毫无意义。要先比 map 槽（map 是连续数组，
   槽序即顺序），同槽内再比 node 内偏移。相等判断可以直接比 cur。 */
static inline
bool i_deque3_iter_lt(deque3_iterator_t a, deque3_iterator_t b)
{
    if (a.bkt != b.bkt)
        return a.bkt < b.bkt;
    return a.cur < b.cur;
}

static inline
bool i_deque3_is_null_iterator(deque3_iterator_t it)
{
    return is_null(it.cur);
}

static inline
deque3_iterator_t i_deque3_null_iterator(void)
{
    deque3_iterator_t it;

    it.cur   = NULL;
    it.begin = NULL;
    it.end   = NULL;
    it.bkt   = NULL;
    it.step  = 0;
    return it;
}

static inline
deque3_r_iterator_t i_deque3_null_r_iterator(void)
{
    deque3_r_iterator_t rit;

    rit.cur       = NULL;
    rit.begin     = NULL;
    rit.end       = NULL;
    rit.bkt       = NULL;
    rit.begin_cur = NULL;
    rit.step      = 0;
    return rit;
}

/* 元素槽地址 —— 迭代器只负责定位，取值/存值由调用方按自己的类型来 */
static inline
void* i_deque3_iter_addr(deque3_iterator_t it)
{
    return I_DEQUE3_PTR_ADDR(it.cur);
}

static inline
deque3_step_t i_deque3_iter_step(deque3_iterator_t it)
{
    return it.step;
}


/* ============================================================================
 * 三、容器
 * ========================================================================== */

typedef struct i_deque3 {
    const class_deque_ops_t* ops;
    deque3_step_t   step;          /* 单个元素的字节数：NEW 时传入，此后不变 */
    size_t          buf_size;      /* 每个 node 的元素个数 = i_deque3_buf_size(step) */
    uint8_t**       bkt;           /* node map：指向各 node 的指针数组 */
    deque3_bcount_t bkt_count;     /* map 槽数 */
    deque3_bcount_t bkt_count_init;
    deque3_iterator_t begin;
    deque3_iterator_t end;
} i_deque3_t;

static inline
bool __i_deque3_empty(const i_deque3_t* _this)
{
    return _this->end.cur == _this->begin.cur;
}

static inline
deque3_size_t i_deque3_size(const i_deque3_t* _this)
{
    return is_null(_this) ? -1 : __i_deque3_iter_distance(_this->begin, _this->end);
}


/* ============================================================================
 * 四、元素读写原语
 *
 * node 槽里是 step 字节的裸数据，进出容器都只有一个信封 deque_data_t。
 * 信封里装的是"值"还是"地址"，容器分不出来 —— 所以优先交给 ops 实现者：
 *   copy_data(data, out)   out 就是 node 槽地址，实现者自己决定怎么落进去
 *   free_data(data)        data 就是 node 槽地址，实现者自己知道要释放什么
 * 实现者没提供回调时，才按 step 规则兜底：
 *   step <= sizeof(deque_data_t)：信封里就是元素本身，取低 step 字节
 *   step >  sizeof(deque_data_t)：信封装不下，信封里是元素的地址
 * ========================================================================== */

/* 是否该按"信封里是地址"来解释 */
static inline
bool __i_deque3_is_addr(const i_deque3_t* _this)
{
    return _this->step > sizeof(deque_data_t);
}

/* 元素槽 -> 信封：供 __eq / valid_data / remove_if 使用。
   step <= 8 时取槽内 step 字节零扩展成信封（与 key 侧规范化一致），
   step >  8 时装不下，只好把槽地址本身当信封传出去。 */
static inline
deque_data_t __i_deque3_slot_to_data(const i_deque3_t* _this, const uint8_t* slot)
{
    uintptr_t v = 0;
    uint8_t*  dp = (uint8_t*)&v;

    if (__i_deque3_is_addr(_this))
        return (deque_data_t)(uintptr_t)slot;

#if defined(__BYTE_ORDER__) && (__BYTE_ORDER__ == __ORDER_BIG_ENDIAN__)
    dp += sizeof(uintptr_t) - _this->step;
#endif
    memcpy(dp, slot, _this->step);
    return (deque_data_t)v;
}

/* 用户传入的信封 -> 用来比较的那个信封（与 __i_deque3_slot_to_data 同一套规范化）。
   注意：信封里是地址时原样返回，实现者在 __eq 里自己转回地址。 */
static inline
deque_data_t __i_deque3_key_to_data(const i_deque3_t* _this, deque_data_t data)
{
    uintptr_t v  = 0;
    uint8_t*  dp = (uint8_t*)&v;

    if (__i_deque3_is_addr(_this))
        return data;

#if defined(__BYTE_ORDER__) && (__BYTE_ORDER__ == __ORDER_BIG_ENDIAN__)
    dp += sizeof(uintptr_t) - _this->step;
#endif
    memcpy(dp, &data, _this->step);
    return (deque_data_t)v;
}

/* 信封 -> 该元素 step 字节的源地址。
   是宏而不是函数：信封里是"值"时源就在调用方这个局部量里，函数返回它的地址会悬空。 */
#define I_DEQUE3_DATA_ADDR(_this, _data)                                                  \
    (__i_deque3_is_addr(_this) ? (const void*)(uintptr_t)(_data) : (const void*)&(_data))

/* 把信封写进 node 槽 */
static inline
bool __i_deque3_slot_write(const i_deque3_t* _this, deque_data_t data, uint8_t* slot)
{
    if (!is_null(_this->ops) && !is_null(_this->ops->copy_data))
        return _this->ops->copy_data(data, (deque_data_t*)slot);

    if (!__i_deque3_is_addr(_this)) {
        memcpy(slot, &data, _this->step);
        return true;
    }

    if (is_null((void*)(uintptr_t)data))
        return false;
    memmove(slot, (const void*)(uintptr_t)data, _this->step);
    return true;
}

/* ---- 定长搬运 / 清零 / 比较的快路径 ----------------------------------------
 *
 * step 是运行期值。直接写 memmove/memset/memcmp 的话，编译器只能发一次 libc 调用,
 * 而 deque3 的每一次元素平移、每一次相等判定都要付这个钱 —— 这正是它在整数
 * 基准上落后老 deque 与 STL 的原因（那边是 *dst = *src、a == b 直接内联）。
 *
 * 按 step 展开成编译期常量后，编译器会把它们内联成几条 mov/cmp。单独列出
 * 1/2/4/8/16/32：前四个覆盖整数与指针，16 是 SSO 串那种对象最常见的尺寸，
 * 32 给更大的结构体留一手。其余尺寸仍走通用路径。
 *
 * 粒度是"单个元素"，dst/src 是不同元素、字节区间不重叠，所以 memcpy 也安全；
 * 这里用 memmove 只是不想为这点差异再分一次支。
 * -------------------------------------------------------------------------- */

/* 元素搬移：对应 STL 的 *dest = *src */
static inline
void __i_deque3_slot_move(const i_deque3_t* _this, uint8_t* dst, const uint8_t* src)
{
    switch (_this->step) {
    case 1:  memmove(dst, src, 1);  return ;
    case 2:  memmove(dst, src, 2);  return ;
    case 4:  memmove(dst, src, 4);  return ;
    case 8:  memmove(dst, src, 8);  return ;
    case 16: memmove(dst, src, 16); return ;
    case 32: memmove(dst, src, 32); return ;
    default: memmove(dst, src, _this->step); return ;
    }
}

static inline
void __i_deque3_slot_zero(const i_deque3_t* _this, uint8_t* slot)
{
    switch (_this->step) {
    case 1:  memset(slot, 0, 1);  return ;
    case 2:  memset(slot, 0, 2);  return ;
    case 4:  memset(slot, 0, 4);  return ;
    case 8:  memset(slot, 0, 8);  return ;
    case 16: memset(slot, 0, 16); return ;
    case 32: memset(slot, 0, 32); return ;
    default: memset(slot, 0, _this->step); return ;
    }
}

/* step 字节内容是否相等 —— ops->__eq 没提供时的兜底 */
static inline
bool __i_deque3_bytes_eq(const void* a, const void* b, deque3_step_t step)
{
    switch (step) {
    case 1:  return 0 == memcmp(a, b, 1);
    case 2:  return 0 == memcmp(a, b, 2);
    case 4:  return 0 == memcmp(a, b, 4);
    case 8:  return 0 == memcmp(a, b, 8);
    case 16: return 0 == memcmp(a, b, 16);
    case 32: return 0 == memcmp(a, b, 32);
    default: return 0 == memcmp(a, b, step);
    }
}

static inline
void __i_deque3_slot_free(const i_deque3_t* _this, uint8_t* slot)
{
    if (!is_null(_this->ops) && !is_null(_this->ops->free_data))
        _this->ops->free_data((deque_data_t*)slot);
}

static inline
bool __i_deque3_valid(const i_deque3_t* _this, deque_data_t data)
{
    if (is_null(_this->ops) || is_null(_this->ops->valid_data))
        return true;
    return _this->ops->valid_data(data);
}

/* 槽与信封是否相等：ops->__eq 优先（key 已按同一套规则规范化好），
   未提供时按 step 字节 memcmp —— POD 语义下与 __eq 的结论一致。 */
static inline
bool __i_deque3_slot_eq(const i_deque3_t* _this, deque_data_t key, const uint8_t* slot)
{
    if (!is_null(_this->ops) && !is_null(_this->ops->__eq))
        return _this->ops->__eq(key, __i_deque3_slot_to_data(_this, slot));

    return __i_deque3_bytes_eq(I_DEQUE3_DATA_ADDR(_this, key), slot, _this->step);
}


/* ============================================================================
 * 五、对外接口（deque3.h 里的 c3deque_xxx 逐个转发到这里）
 * ========================================================================== */

/* 本 node 已满：扩出下一个 node 并把 begin/end 切过去，
   返回"新元素该落进去的槽"（= 扩之前的 end.cur / begin.cur），失败返回 NULL。
   调用方负责往里写数据；写失败时用 __i_deque3_pop_back_raw / __i_deque3_pop_front_raw 回滚。 */
uint8_t*     __i_deque3_push_back_open(i_deque3_t* _this);
uint8_t*     __i_deque3_push_front_open(i_deque3_t* _this);

/* 丢掉 begin/end 那一端的空 node（元素数据不动）。 */
void         __i_deque3_pop_back_free(i_deque3_t* _this);
void         __i_deque3_pop_front_free(i_deque3_t* _this);

/* checked */
static inline
deque3_iterator_t __i_deque3_begin(const i_deque3_t* _this)
{
    return _this->begin;
}

/* checked */
static inline
deque3_iterator_t i_deque3_begin(const i_deque3_t* _this)
{
    JDSC_ASSERT(!is_null(_this));
#if JDSC_ITERATOR_ERR_NULL
    if (is_null(_this))
        return i_deque3_null_iterator();
#endif /* JDSC_ITERATOR_ERR_NULL */
    return __i_deque3_begin(_this);
}

/* checked */
static inline
deque3_iterator_t __i_deque3_end(const i_deque3_t* _this)
{
    return _this->end;
}

/* checked */
static inline
deque3_iterator_t i_deque3_end(const i_deque3_t* _this)
{
    JDSC_ASSERT(!is_null(_this));
#if JDSC_ITERATOR_ERR_NULL
    if (is_null(_this))
        return i_deque3_null_iterator();
#endif /* JDSC_ITERATOR_ERR_NULL */
    return __i_deque3_end(_this);
}

/* checked */
static inline
deque3_iterator_t i_deque3_next(deque3_iterator_t it)
{
    return __i_deque3_iter_next(it);
}

/* checked */
static inline
deque3_iterator_t i_deque3_prev(deque3_iterator_t it)
{
    return __i_deque3_iter_prev(it);
}

/* checked */
static inline
deque3_r_iterator_t __i_deque3_rend(const i_deque3_t* _this)
{
    deque3_r_iterator_t rit;

    rit.cur       = _I_DEQUE3_ITERATOR_REND;
    rit.begin     = _this->begin.begin;
    rit.end       = _this->begin.end;
    rit.bkt       = _this->begin.bkt;
    rit.begin_cur = _this->begin.cur;
    rit.step      = _this->step;
    return rit;
}

/* checked */
static inline
deque3_r_iterator_t i_deque3_rend(const i_deque3_t* _this)
{
    JDSC_ASSERT(!is_null(_this));
#if JDSC_ITERATOR_ERR_NULL
    if (is_null(_this))
        return i_deque3_null_r_iterator();
#endif /* JDSC_ITERATOR_ERR_NULL */
    return __i_deque3_rend(_this);
}

/* checked */
static inline
deque3_r_iterator_t __i_deque3_rbegin(const i_deque3_t* _this)
{
    deque3_r_iterator_t rit;
    deque3_iterator_t   it;

    if (__i_deque3_empty(_this))
        return __i_deque3_rend(_this);

    it = __i_deque3_iter_prev(_this->end);

    rit.cur       = it.cur;
    rit.begin     = it.begin;
    rit.end       = it.end;
    rit.bkt       = it.bkt;
    rit.begin_cur = _this->begin.cur;
    rit.step      = _this->step;
    return rit;
}

/* checked */
static inline
deque3_r_iterator_t i_deque3_rbegin(const i_deque3_t* _this)
{
    JDSC_ASSERT(!is_null(_this));
#if JDSC_ITERATOR_ERR_NULL
    if (is_null(_this))
        return i_deque3_null_r_iterator();
#endif /* JDSC_ITERATOR_ERR_NULL */
    return __i_deque3_rbegin(_this);
}

/* checked */
static inline
deque3_r_iterator_t __i_deque3_rnext(deque3_r_iterator_t rit)
{
    JDSC_ASSERT(!is_null(rit.cur) && !is_null(rit.begin)
                && !is_null(rit.end) && !is_null(rit.bkt)
                && !is_null(rit.begin_cur)
                && _I_DEQUE3_ITERATOR_REND != rit.cur);

#if JDSC_ITERATOR_ERR_NULL
    if (is_null(rit.cur) || is_null(rit.begin)
        || is_null(rit.end) || is_null(rit.bkt)
        || is_null(rit.begin_cur)
        || _I_DEQUE3_ITERATOR_REND == rit.cur)
        return i_deque3_null_r_iterator();
#endif /* JDSC_ITERATOR_ERR_NULL */

    if (rit.cur == rit.begin_cur) {
        rit.cur = _I_DEQUE3_ITERATOR_REND;
        return rit;
    }

    if (rit.cur == rit.begin) {
        __i_deque3_iter_set_node_r(&rit, rit.bkt - 1);
        rit.cur = rit.end;
    }
    rit.cur = I_DEQUE3_PTR_SUB(rit.cur, rit.step, 1);
    return rit;
}

/* checked */
static inline
deque3_r_iterator_t i_deque3_rnext(deque3_r_iterator_t rit)
{
    return __i_deque3_rnext(rit);
}

/* checked */
static inline
deque3_r_iterator_t __i_deque3_rprev(deque3_r_iterator_t rit)
{
    JDSC_ASSERT(!is_null(rit.cur) && !is_null(rit.begin)
                && !is_null(rit.end) && !is_null(rit.bkt)
                && !is_null(rit.begin_cur));

#if JDSC_ITERATOR_ERR_NULL
    if (is_null(rit.cur) || is_null(rit.begin)
        || is_null(rit.end) || is_null(rit.bkt)
        || is_null(rit.begin_cur))
        return i_deque3_null_r_iterator();
#endif /* JDSC_ITERATOR_ERR_NULL */

    if (rit.cur == _I_DEQUE3_ITERATOR_REND) {
        rit.cur = rit.begin_cur;
        return rit;
    }

    rit.cur = I_DEQUE3_PTR_ADD(rit.cur, rit.step, 1);
    if (rit.cur == rit.end) {
        __i_deque3_iter_set_node_r(&rit, rit.bkt + 1);
        rit.cur = rit.begin;
    }
    return rit;
}

/* checked */
static inline
deque3_r_iterator_t i_deque3_rprev(deque3_r_iterator_t rit)
{
    return __i_deque3_rprev(rit);
}

/* 弹出 begin/end 那一端的元素。free_data 决定要不要销毁元素数据。
   注意跨 node 的情形：元素就躺在即将被释放的那个 node 里，所以销毁必须在这里、
   释放 node 之前当场做掉 —— 不能把槽地址返回给调用方，那样拿到的是悬空指针。 */
static inline
void __i_deque3_pop_back_impl(i_deque3_t* _this, bool free_data)
{
    if (_this->end.cur != _this->end.begin) {
        uint8_t* slot;

        _this->end.cur = I_DEQUE3_PTR_SUB(_this->end.cur, _this->step, 1);
        slot = _this->end.cur;
        if (free_data)
            __i_deque3_slot_free(_this, slot);
        return ;
    }

    /* 本 node 已空：元素在上一个 node 的末槽，趁 node 还在先销毁 */
    if (free_data)
        __i_deque3_slot_free(_this, I_DEQUE3_PTR_ADD(_this->end.bkt[-1], _this->step,
                                                     (ptrdiff_t)(_this->buf_size - 1)));
    __i_deque3_pop_back_free(_this);
}

static inline
void __i_deque3_pop_front_impl(i_deque3_t* _this, bool free_data)
{
    if (_this->begin.cur != I_DEQUE3_PTR_SUB(_this->begin.end, _this->step, 1)) {
        uint8_t* slot = _this->begin.cur;

        _this->begin.cur = I_DEQUE3_PTR_ADD(_this->begin.cur, _this->step, 1);
        if (free_data)
            __i_deque3_slot_free(_this, slot);
        return ;
    }

    /* 本 node 只剩末槽：也是本 node 要被丢掉的情形 */
    if (free_data)
        __i_deque3_slot_free(_this, _this->begin.cur);
    __i_deque3_pop_front_free(_this);
}

/* 只挪迭代器、不碰元素数据（洞槽、旧影子槽用这个） */
static inline
void __i_deque3_pop_back_raw(i_deque3_t* _this)
{
    __i_deque3_pop_back_impl(_this, false);
}

static inline
void __i_deque3_pop_front_raw(i_deque3_t* _this)
{
    __i_deque3_pop_front_impl(_this, false);
}

/* checked */
static inline
bool i_deque3_push_back(i_deque3_t* _this, deque_data_t data)
{
    uint8_t* slot;

    if (is_null(_this))
        return false;

    if (!__i_deque3_valid(_this, data))
        return false;

    /* 本 node 还有空位：写当前 end 槽，再把 end 往后挪一个元素 */
    if (_this->end.cur != I_DEQUE3_PTR_SUB(_this->end.end, _this->step, 1)) {
        if (!__i_deque3_slot_write(_this, data, _this->end.cur))
            return false;
        _this->end.cur = I_DEQUE3_PTR_ADD(_this->end.cur, _this->step, 1);
        return true;
    }

    /* 本 node 已满：扩 node（可能重建 map），旧 end 槽就是新元素的落点 */
    slot = __i_deque3_push_back_open(_this);
    if (is_null(slot))
        return false;

    if (!__i_deque3_slot_write(_this, data, slot)) {
        __i_deque3_pop_back_raw(_this);            /* 回滚：新槽是全 0，直接丢掉 */
        return false;
    }
    return true;
}

/* checked */
static inline
bool i_deque3_push_front(i_deque3_t* _this, deque_data_t data)
{
    uint8_t* slot;

    if (is_null(_this))
        return false;

    if (!__i_deque3_valid(_this, data))
        return false;

    /* 本 node 还有空位：begin 往前挪一个元素再写 */
    if (_this->begin.cur != _this->begin.begin) {
        _this->begin.cur = I_DEQUE3_PTR_SUB(_this->begin.cur, _this->step, 1);
        if (!__i_deque3_slot_write(_this, data, _this->begin.cur)) {
            _this->begin.cur = I_DEQUE3_PTR_ADD(_this->begin.cur, _this->step, 1);
            return false;
        }
        return true;
    }

    slot = __i_deque3_push_front_open(_this);
    if (is_null(slot))
        return false;

    if (!__i_deque3_slot_write(_this, data, slot)) {
        __i_deque3_pop_front_raw(_this);           /* 回滚：新槽是全 0，直接丢掉 */
        return false;
    }
    return true;
}

/* checked */
static inline
void i_deque3_pop_back(i_deque3_t* _this)
{
    if (is_null(_this) || __i_deque3_empty(_this))
        return ;

    __i_deque3_pop_back_impl(_this, true);
}

/* checked */
static inline
void i_deque3_pop_front(i_deque3_t* _this)
{
    if (is_null(_this) || __i_deque3_empty(_this))
        return ;

    __i_deque3_pop_front_impl(_this, true);
}

/* 压入一个"空槽"（全 0），用于 insert 开洞 */
static inline
bool __i_deque3_push_back_zero(i_deque3_t* _this)
{
    uint8_t* slot;

    if (_this->end.cur != I_DEQUE3_PTR_SUB(_this->end.end, _this->step, 1)) {
        __i_deque3_slot_zero(_this, _this->end.cur);
        _this->end.cur = I_DEQUE3_PTR_ADD(_this->end.cur, _this->step, 1);
        return true;
    }

    slot = __i_deque3_push_back_open(_this);
    if (is_null(slot))
        return false;
    __i_deque3_slot_zero(_this, slot);
    return true;
}

static inline
bool __i_deque3_push_front_zero(i_deque3_t* _this)
{
    uint8_t* slot;

    if (_this->begin.cur != _this->begin.begin) {
        _this->begin.cur = I_DEQUE3_PTR_SUB(_this->begin.cur, _this->step, 1);
        __i_deque3_slot_zero(_this, _this->begin.cur);
        return true;
    }

    slot = __i_deque3_push_front_open(_this);
    if (is_null(slot))
        return false;
    __i_deque3_slot_zero(_this, slot);
    return true;
}

deque3_iterator_t __i_deque3_insert(i_deque3_t* _this, deque3_iterator_t pos, deque_data_t data);
static inline
deque3_iterator_t i_deque3_insert(i_deque3_t* _this, deque3_iterator_t pos, deque_data_t data)
{
    if (is_null(_this) || i_deque3_is_null_iterator(pos))
        return i_deque3_null_iterator();

    if (_this->begin.cur == pos.cur) {
        if (i_deque3_push_front(_this, data))
            return _this->begin;
        return i_deque3_null_iterator();
    }

    if (_this->end.cur == pos.cur) {
        if (i_deque3_push_back(_this, data))
            return __i_deque3_iter_prev(_this->end);
        return i_deque3_null_iterator();
    }

    return __i_deque3_insert(_this, pos, data);
}

deque3_iterator_t __i_deque3_erase(i_deque3_t* _this, deque3_iterator_t pos);
static inline
deque3_iterator_t i_deque3_erase(i_deque3_t* _this, deque3_iterator_t pos)
{
    if (is_null(_this) || i_deque3_is_null_iterator(pos) || __i_deque3_empty(_this))
        return i_deque3_null_iterator();            /* 空容器：erase 非法，返回 null */

    if (_this->begin.cur == pos.cur) {
        i_deque3_pop_front(_this);
        return _this->begin;
    }

    if (_this->end.cur == pos.cur)                  /* erase(end()) 非法：返回 null，不要静默删末尾 */
        return i_deque3_null_iterator();

    return __i_deque3_erase(_this, pos);
}

deque3_iterator_t __i_deque3_erase_range(i_deque3_t* _this, deque3_iterator_t iterator_begin, deque3_iterator_t iterator_end);
static inline
deque3_iterator_t i_deque3_erase_range(i_deque3_t* _this, deque3_iterator_t iterator_begin, deque3_iterator_t iterator_end)
{
    if (is_null(_this)
        || i_deque3_is_null_iterator(iterator_begin)
        || i_deque3_is_null_iterator(iterator_end))
        return i_deque3_null_iterator();

    if (_this->begin.cur == iterator_begin.cur) {          /* 从头部连续删：逐个 pop_front 即可 */
        while (iterator_end.cur != _this->begin.cur)
            i_deque3_pop_front(_this);
        return _this->begin;
    }

    if (_this->end.cur == iterator_end.cur) {              /* 删到尾部连续：逐个 pop_back 即可 */
        while (iterator_begin.cur != _this->end.cur)
            i_deque3_pop_back(_this);
        return _this->end;
    }

    return __i_deque3_erase_range(_this, iterator_begin, iterator_end);
}

deque3_iterator_t i_deque3_insert_n(i_deque3_t* _this, deque3_iterator_t pos, deque3_size_t n, deque_data_t data);
deque3_iterator_t i_deque3_find(const i_deque3_t* _this, deque_data_t data);
deque3_count_t    i_deque3_count(const i_deque3_t* _this, deque_data_t data);
deque3_size_t     i_deque3_remove(i_deque3_t* _this, deque_data_t data);
deque3_size_t     i_deque3_remove_if(i_deque3_t* _this, remove_if_condition cond);
deque3_size_t     i_deque3_clear(i_deque3_t* _this);


#endif /* __J_I_DEQUE3_H */
