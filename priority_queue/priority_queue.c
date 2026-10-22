/*
  Priority Queue Implementations
  Copyright (C) 2026  YangJie <yangjie98765@yeah.net>

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

#include <priority_queue/priority_queue.h>

#include <_log.h>
#include <_memory.h>
#include <linux/_compiler.h>

#define TAG "[priority_queue]"

/* ==========================================================================
 * Heap core
 *
 * The sift loops are O(log n), so they are compiled once here instead of
 * being static-inline in the public header.  One instantiation of the
 * template covers each meaningful layout:
 *   - 8-byte slots: ints, pointers, char*, any *_NEW_T type of that size
 *   - the inline SSO struct
 * Other element sizes use the generic hole-sift fallback at the bottom.
 * ========================================================================== */

#define __I_PQ_TMP_SIZE 64

static inline
bool __i_pq_lt(const i_priority_queue_t* _this, priority_queue_data_t left, priority_queue_data_t right)
{
    if (!is_null(_this->ops) && !is_null(_this->ops->__lt))
        return _this->ops->__lt(left, right);
    return left < right;
}

/* Compare value: dereference the slot, or hand over the slot address (SSO). */
#define __i_pq_val_data(p)  (*(priority_queue_data_t*)(p))
#define __i_pq_val_addr(p)  ((priority_queue_data_t)(p))
/* Held-aside temp: by value for plain slots so it stays in a register. */
#define __i_pq_tval_data(t) (t)
#define __i_pq_tval_addr(t) ((priority_queue_data_t)&(t))
#define __i_pq_fix_none(p)  ((void)0)
#define __i_pq_fix_sso(p)   __ds_ops_fix_sso((ds_data_t)(p))

/* Division-free deque index for the two power-of-two block layouts. */
static inline
uint8_t* __i_pq_deq_slot8(const deque_t* d, priority_queue_size_t i, deque_size_t mask, int shift)
{
    const ptrdiff_t off = (d->begin.cur - d->begin.begin) >> 3;
    const ptrdiff_t total = off + (ptrdiff_t)i;

    return d->begin.bkt[total >> shift] + ((total & (ptrdiff_t)mask) << 3);
}

static inline
uint8_t* __i_pq_deq_slot32(const deque_t* d, priority_queue_size_t i, deque_size_t mask, int shift)
{
    const ptrdiff_t off = (d->begin.cur - d->begin.begin) >> 5;
    const ptrdiff_t total = off + (ptrdiff_t)i;

    return d->begin.bkt[total >> shift] + ((total & (ptrdiff_t)mask) << 5);
}

static inline
priority_queue_size_t __i_pq_deq_size8(const deque_t* d)
{
    const ptrdiff_t ob = (d->begin.cur - d->begin.begin) >> 3;
    const ptrdiff_t oe = (d->end.cur   - d->end.begin)   >> 3;
    const ptrdiff_t nb = d->end.bkt - d->begin.bkt;

    return (priority_queue_size_t)(nb * (ptrdiff_t)d->bkt_size - ob + oe);
}

static inline
priority_queue_size_t __i_pq_deq_size32(const deque_t* d)
{
    const ptrdiff_t ob = (d->begin.cur - d->begin.begin) >> 5;
    const ptrdiff_t oe = (d->end.cur   - d->end.begin)   >> 5;
    const ptrdiff_t nb = d->end.bkt - d->begin.bkt;

    return (priority_queue_size_t)(nb * (ptrdiff_t)d->bkt_size - ob + oe);
}

#define __i_pq_vec_prep      vector_t* __v = _this->v;
#define __i_pq_vec_at8(i)    ((priority_queue_data_t*)__v->begin + (i))
#define __i_pq_vec_at32(i)   ((ds_sso_t*)__v->begin + (i))
#define __i_pq_vec_len(type) ((priority_queue_size_t)((__v->end - __v->begin) / (ptrdiff_t)sizeof(type)))

#define __i_pq_deq_prep                                          \
    deque_t* __d = _this->d;                                     \
    const deque_size_t __mask = (deque_size_t)__d->bkt_size - 1; \
    const int __shift = __builtin_ctz((unsigned)__d->bkt_size);
#define __i_pq_deq_at8(i)  ((priority_queue_data_t*)__i_pq_deq_slot8(__d, (i), __mask, __shift))
#define __i_pq_deq_at32(i) ((ds_sso_t*)__i_pq_deq_slot32(__d, (i), __mask, __shift))
#define __i_pq_deq_len8    __i_pq_deq_size8(__d)
#define __i_pq_deq_len32   __i_pq_deq_size32(__d)

/*
 * Heap sift, expanded once per slot layout.
 *
 *   name  generated function suffix
 *   type  type stored in one slot
 *   prep  binds __v / __d and, for deque, __mask and __shift
 *   at    slot pointer for an index
 *   val   slot pointer -> comparable value
 *   tval  held temp -> comparable value (by value for plain slots)
 *   fix   slot pointer -> SSO inline-pointer repair (no-op for plain slots)
 *   len   element count
 *
 * The generated functions are static inline, so the comparator and the slot
 * moves still inline; that is why they are macro arguments and not function
 * pointers.
 */
#define __i_pq_define_heap(name, type, prep, at, val, tval, fix, len) \
static inline void __i_pq_rise_##name(i_priority_queue_t* _this,      \
                                      priority_queue_size_t index)    \
{                                                                     \
    prep                                                              \
    const class_priority_queue_ops_t* __ops = _this->ops;             \
    bool (*__lt)(priority_queue_data_t, priority_queue_data_t) =      \
        is_null(__ops) ? NULL : __ops->__lt;                          \
    type* __hole;                                                     \
    type  __t;                                                        \
    priority_queue_size_t __i = index;                                \
                                                                      \
    if (__i <= 0)                                                     \
        return;                                                       \
                                                                      \
    __hole = at(__i);                                                 \
    __t = *__hole;                                                    \
    fix(&__t);                                                        \
                                                                      \
    while (__i > 0) {                                                 \
        priority_queue_size_t __parent = (__i - 1) >> 1;              \
        type* __ps = at(__parent);                                    \
                                                                      \
        if (!(__lt ? __lt(val(__ps), tval(__t))                       \
                  : (val(__ps) < tval(__t))))                         \
            break;                                                    \
                                                                      \
        *__hole = *__ps;                                              \
        fix(__hole);                                                  \
        __hole = __ps;                                                \
        __i = __parent;                                               \
    }                                                                 \
    *__hole = __t;                                                    \
    fix(__hole);                                                      \
}                                                                     \
static inline void __i_pq_heapify_##name(i_priority_queue_t* _this,   \
                                         priority_queue_size_t index) \
{                                                                     \
    prep                                                              \
    const class_priority_queue_ops_t* __ops = _this->ops;             \
    bool (*__lt)(priority_queue_data_t, priority_queue_data_t) =      \
        is_null(__ops) ? NULL : __ops->__lt;                          \
    priority_queue_size_t __len = len;                                \
    type* __hole;                                                     \
    type  __t;                                                        \
    priority_queue_size_t __i = index;                                \
                                                                      \
    if (__i >= __len)                                                 \
        return;                                                       \
                                                                      \
    __hole = at(__i);                                                 \
    __t = *__hole;                                                    \
    fix(&__t);                                                        \
                                                                      \
    while (((__i << 1) + 1) < __len) {                                \
        priority_queue_size_t __ls = (__i << 1) + 1;                  \
        priority_queue_size_t __rs = __ls + 1;                        \
        priority_queue_size_t __m  = __ls;                            \
        type* __ms = at(__ls);                                        \
                                                                      \
        if (__rs < __len) {                                           \
            type* __rp = at(__rs);                                    \
                                                                      \
            if (__lt ? __lt(val(__ms), val(__rp))                     \
                     : (val(__ms) < val(__rp))) {                     \
                __m  = __rs;                                          \
                __ms = __rp;                                          \
            }                                                         \
        }                                                             \
        if (!(__lt ? __lt(tval(__t), val(__ms))                       \
                  : (tval(__t) < val(__ms))))                         \
            break;                                                    \
                                                                      \
        *__hole = *__ms;                                              \
        fix(__hole);                                                  \
        __hole = __ms;                                                \
        __i = __m;                                                    \
    }                                                                 \
    *__hole = __t;                                                    \
    fix(__hole);                                                      \
}

__i_pq_define_heap(vector_8, priority_queue_data_t,
                   __i_pq_vec_prep, __i_pq_vec_at8, __i_pq_val_data,
                   __i_pq_tval_data, __i_pq_fix_none,
                   __i_pq_vec_len(priority_queue_data_t))
__i_pq_define_heap(vector_sso, ds_sso_t,
                   __i_pq_vec_prep, __i_pq_vec_at32, __i_pq_val_addr,
                   __i_pq_tval_addr, __i_pq_fix_sso,
                   __i_pq_vec_len(ds_sso_t))
__i_pq_define_heap(deque_8, priority_queue_data_t,
                   __i_pq_deq_prep, __i_pq_deq_at8, __i_pq_val_data,
                   __i_pq_tval_data, __i_pq_fix_none,
                   __i_pq_deq_len8)
__i_pq_define_heap(deque_sso, ds_sso_t,
                   __i_pq_deq_prep, __i_pq_deq_at32, __i_pq_val_addr,
                   __i_pq_tval_addr, __i_pq_fix_sso,
                   __i_pq_deq_len32)

/* Generic fallback: arbitrary slot size, vector */
static inline
void __i_pq_rise_vector_generic(i_priority_queue_t* _this, priority_queue_size_t index)
{
    vector_t* v = _this->v;
    const vector_step_t step = v->step;
    const bool sso = v->sso;
    uint8_t tmp[__I_PQ_TMP_SIZE];
    uint8_t* t = tmp;
    uint8_t* start;
    uint8_t* hole;
    priority_queue_size_t i = index;

    if (i <= 0)
        return;

    if (step > (vector_step_t)sizeof(tmp)) {
        t = (uint8_t*)p_malloc((size_t)step);
        if (is_null(t))
            return ;
    }

    start = __i_vector_iterator(v, i).cur;
    hole  = start;
    __i_vector_slot_move(v, t, hole);

    while (i > 0) {
        priority_queue_size_t parent = (i - 1) >> 1;
        uint8_t* ps = __i_vector_iterator(v, parent).cur;
        bool less = sso ? __i_pq_lt(_this, (priority_queue_data_t)ps, (priority_queue_data_t)t)
                        : __i_pq_lt(_this, __i_pq_val_data(ps), __i_pq_val_data(t));

        if (!less)
            break;

        __i_vector_slot_move(v, hole, ps);
        hole = ps;
        i = parent;
    }

    if (hole != start)
        __i_vector_slot_move(v, hole, t);

    if (t != tmp)
        p_free(t);
}

static inline
void __i_pq_heapify_vector_generic(i_priority_queue_t* _this, priority_queue_size_t index)
{
    vector_t* v = _this->v;
    const vector_step_t step = v->step;
    const bool sso = v->sso;
    uint8_t tmp[__I_PQ_TMP_SIZE];
    uint8_t* t = tmp;
    uint8_t* start;
    uint8_t* hole;
    priority_queue_size_t i = index;
    priority_queue_size_t len = cvector_size(v);

    if (i >= len)
        return;

    if (step > (vector_step_t)sizeof(tmp)) {
        t = (uint8_t*)p_malloc((size_t)step);
        if (is_null(t))
            return ;
    }

    start = __i_vector_iterator(v, i).cur;
    hole  = start;
    __i_vector_slot_move(v, t, hole);

    while ((i << 1) + 1 < len) {
        priority_queue_size_t ls = (i << 1) + 1;
        priority_queue_size_t rs = ls + 1;
        priority_queue_size_t m  = ls;
        uint8_t* ms;

        if (rs < len) {
            uint8_t* l = __i_vector_iterator(v, ls).cur;
            uint8_t* r = __i_vector_iterator(v, rs).cur;
            bool less = sso ? __i_pq_lt(_this, (priority_queue_data_t)l, (priority_queue_data_t)r)
                            : __i_pq_lt(_this, __i_pq_val_data(l), __i_pq_val_data(r));
            if (less)
                m = rs;
        }

        ms = __i_vector_iterator(v, m).cur;
        {
            bool less = sso ? __i_pq_lt(_this, (priority_queue_data_t)t, (priority_queue_data_t)ms)
                            : __i_pq_lt(_this, __i_pq_val_data(t), __i_pq_val_data(ms));
            if (!less)
                break;
        }

        __i_vector_slot_move(v, hole, ms);
        hole = ms;
        i = m;
    }

    if (hole != start)
        __i_vector_slot_move(v, hole, t);

    if (t != tmp)
        p_free(t);
}

/* Generic fallback: arbitrary slot size, deque */
static inline
void __i_pq_rise_deque_generic(i_priority_queue_t* _this, priority_queue_size_t index)
{
    deque_t* d = _this->d;
    const bool sso = d->sso;
    priority_queue_size_t i = index;

    while (i > 0) {
        priority_queue_size_t parent = (i - 1) >> 1;
        uint8_t* ps = __cdeque_at(d, parent).u8;
        uint8_t* is = __cdeque_at(d, i).u8;
        bool less = sso ? __i_pq_lt(_this, (priority_queue_data_t)ps, (priority_queue_data_t)is)
                        : __i_pq_lt(_this, __i_pq_val_data(ps), __i_pq_val_data(is));

        if (!less)
            break;

        cdeque_swap_at(d, parent, i);
        i = parent;
    }
}

static inline
void __i_pq_heapify_deque_generic(i_priority_queue_t* _this, priority_queue_size_t index)
{
    deque_t* d = _this->d;
    const bool sso = d->sso;
    priority_queue_size_t i = index;
    priority_queue_size_t len = cdeque_size(d);

    while ((i << 1) + 1 < len) {
        priority_queue_size_t ls = (i << 1) + 1;
        priority_queue_size_t rs = ls + 1;
        priority_queue_size_t m  = ls;
        uint8_t* ms;

        if (rs < len) {
            uint8_t* l = __cdeque_at(d, ls).u8;
            uint8_t* r = __cdeque_at(d, rs).u8;
            bool less = sso ? __i_pq_lt(_this, (priority_queue_data_t)l, (priority_queue_data_t)r)
                            : __i_pq_lt(_this, __i_pq_val_data(l), __i_pq_val_data(r));
            if (less)
                m = rs;
        }

        ms = __cdeque_at(d, m).u8;
        {
            bool less = sso ? __i_pq_lt(_this, (priority_queue_data_t)__cdeque_at(d, i).u8,
                                               (priority_queue_data_t)ms)
                            : __i_pq_lt(_this, __i_pq_val_data(__cdeque_at(d, i).u8),
                                               __i_pq_val_data(ms));
            if (!less)
                break;
        }

        cdeque_swap_at(d, i, m);
        i = m;
    }
}

static inline
bool __i_pq_deq_pow2(const deque_t* d)
{
    const deque_size_t bs = (deque_size_t)d->bkt_size;

    return bs > 0 && 0 == (bs & (bs - 1));
}

/* ======================= vector base ======================= */

static inline
bool __i_pq_vector_push(i_priority_queue_t* _this, priority_queue_data_t data)
{
    return cvector_push_back(_this->v, data);
}

static inline
void __i_pq_vector_rise(i_priority_queue_t* _this)
{
    vector_t* v = _this->v;
    priority_queue_size_t last = cvector_size(v) - 1;

    if (v->sso) {
        if (v->step == (vector_step_t)sizeof(ds_sso_t))
            __i_pq_rise_vector_sso(_this, last);
        else
            __i_pq_rise_vector_generic(_this, last);
    } else if (v->step == (vector_step_t)sizeof(priority_queue_data_t)) {
        __i_pq_rise_vector_8(_this, last);
    } else {
        __i_pq_rise_vector_generic(_this, last);
    }
}

static inline
void __i_pq_vector_pop(i_priority_queue_t* _this)
{
    vector_t* v = _this->v;
    priority_queue_size_t size;

    if (v->sso && v->step == (vector_step_t)sizeof(ds_sso_t)) {
        ds_sso_t* a = (ds_sso_t*)v->begin;
        ds_sso_t tmp;

        size = (priority_queue_size_t)((v->end - v->begin) / (ptrdiff_t)sizeof(ds_sso_t));
        if (size <= 0)
            return;
        if (size > 1) {
            tmp = a[0];
            a[0] = a[size - 1];
            __ds_ops_fix_sso((ds_data_t)&a[0]);
            a[size - 1] = tmp;
            __ds_ops_fix_sso((ds_data_t)&a[size - 1]);
        }
        cvector_pop_back(v);
        return;
    }

    if (!v->sso && v->step == (vector_step_t)sizeof(priority_queue_data_t)) {
        priority_queue_data_t* a = (priority_queue_data_t*)v->begin;
        priority_queue_data_t tmp;

        size = (priority_queue_size_t)((v->end - v->begin) / (ptrdiff_t)sizeof(priority_queue_data_t));
        if (size <= 0)
            return;
        if (size > 1) {
            tmp = a[0];
            a[0] = a[size - 1];
            a[size - 1] = tmp;
        }
        cvector_pop_back(v);
        return;
    }

    size = cvector_size(v);
    if (size <= 0)
        return;
    if (size > 1 && !cvector_swap_at(v, 0, (vector_index_t)(size - 1)))
        return;
    cvector_pop_back(v);
}

static inline
void __i_pq_vector_heapify(i_priority_queue_t* _this)
{
    vector_t* v = _this->v;

    if (v->sso) {
        if (v->step == (vector_step_t)sizeof(ds_sso_t))
            __i_pq_heapify_vector_sso(_this, 0);
        else
            __i_pq_heapify_vector_generic(_this, 0);
    } else if (v->step == (vector_step_t)sizeof(priority_queue_data_t)) {
        __i_pq_heapify_vector_8(_this, 0);
    } else {
        __i_pq_heapify_vector_generic(_this, 0);
    }
}

/* ======================= deque base ======================= */

static inline
bool __i_pq_deque_push(i_priority_queue_t* _this, priority_queue_data_t data)
{
    return cdeque_push_back(_this->d, data);
}

static inline
void __i_pq_deque_rise(i_priority_queue_t* _this)
{
    deque_t* d = _this->d;
    priority_queue_size_t last = cdeque_size(d) - 1;
    const bool pow2 = __i_pq_deq_pow2(d);

    if (d->sso) {
        if (pow2 && d->step == (deque_step_t)sizeof(ds_sso_t))
            __i_pq_rise_deque_sso(_this, last);
        else
            __i_pq_rise_deque_generic(_this, last);
    } else if (pow2 && d->step == (deque_step_t)sizeof(priority_queue_data_t)) {
        __i_pq_rise_deque_8(_this, last);
    } else {
        __i_pq_rise_deque_generic(_this, last);
    }
}

static inline
void __i_pq_deque_pop(i_priority_queue_t* _this)
{
    deque_t* d = _this->d;
    const bool pow2 = __i_pq_deq_pow2(d);
    const deque_size_t mask = (deque_size_t)d->bkt_size - 1;
    const int shift = __builtin_ctz((unsigned)d->bkt_size);
    priority_queue_size_t size;

    if (pow2 && d->sso && d->step == (deque_step_t)sizeof(ds_sso_t)) {
        ds_sso_t* a;
        ds_sso_t* b;
        ds_sso_t tmp;

        size = __i_pq_deq_size32(d);
        if (size <= 0)
            return;
        if (size > 1) {
            a = (ds_sso_t*)__i_pq_deq_slot32(d, 0, mask, shift);
            b = (ds_sso_t*)__i_pq_deq_slot32(d, (priority_queue_size_t)(size - 1), mask, shift);
            tmp = *a;
            *a = *b;
            __ds_ops_fix_sso((ds_data_t)a);
            *b = tmp;
            __ds_ops_fix_sso((ds_data_t)b);
        }
        cdeque_pop_back(d);
        return;
    }

    if (pow2 && !d->sso && d->step == (deque_step_t)sizeof(priority_queue_data_t)) {
        priority_queue_data_t* a;
        priority_queue_data_t* b;
        priority_queue_data_t tmp;

        size = __i_pq_deq_size8(d);
        if (size <= 0)
            return;
        if (size > 1) {
            a = (priority_queue_data_t*)__i_pq_deq_slot8(d, 0, mask, shift);
            b = (priority_queue_data_t*)
                __i_pq_deq_slot8(d, (priority_queue_size_t)(size - 1), mask, shift);
            tmp = *a;
            *a = *b;
            *b = tmp;
        }
        cdeque_pop_back(d);
        return;
    }

    size = cdeque_size(d);
    if (size <= 0)
        return;
    if (size > 1 && !cdeque_swap_at(d, 0, (deque_index_t)(size - 1)))
        return;
    cdeque_pop_back(d);
}

static inline
void __i_pq_deque_heapify(i_priority_queue_t* _this)
{
    deque_t* d = _this->d;
    const bool pow2 = __i_pq_deq_pow2(d);

    if (d->sso) {
        if (pow2 && d->step == (deque_step_t)sizeof(ds_sso_t))
            __i_pq_heapify_deque_sso(_this, 0);
        else
            __i_pq_heapify_deque_generic(_this, 0);
    } else if (pow2 && d->step == (deque_step_t)sizeof(priority_queue_data_t)) {
        __i_pq_heapify_deque_8(_this, 0);
    } else {
        __i_pq_heapify_deque_generic(_this, 0);
    }
}













/*
static void __priority_queue_heapify(priority_queue_t* _this, priority_queue_size_t index)
{
    priority_queue_size_t i = index;
    priority_queue_size_t len = __priority_queue_size(_this) - 1;
    priority_queue_size_t m;

    if (index >= __priority_queue_size(_this))
        return ;

    if (is_null(_this->ops) || is_null(_this->ops->__lt)) {
        for (; (i << 1) + 1 <= len;) {
            priority_queue_size_t ls = (i << 1) + 1;
            priority_queue_size_t rs = (i << 1) + 2;

            m = ls <= len && _this->head[i].data < _this->head[ls].data ? ls : i;
            m = rs <= len && _this->head[m].data < _this->head[rs].data ? rs : m;

            if (i == m)
                break;

            __priority_queue_exchange_data(&_this->head[m].data, &_this->head[i].data);
            i = m;
        }
    } else {
        for (; (i << 1) + 1 <= len;) {
            priority_queue_size_t ls = (i << 1) + 1;
            priority_queue_size_t rs = (i << 1) + 2;

            m = ls <= len && _this->ops->__lt(_this->head[i].data, _this->head[ls].data) ? ls : i;
            m = rs <= len && _this->ops->__lt(_this->head[m].data, _this->head[rs].data) ? rs : m;

            if (i == m)
                break;

            __priority_queue_exchange_data(&_this->head[m].data, &_this->head[i].data);
            i = m;
        }
    }
}

static inline
void __i_priority_queue_heapify_vector(i_vector_t* _this, vector_size_t index)
{
    const vector_step_t step = _this->step;
    const vector_size_t size = __i_vector_size(_this);
    vector_size_t _m, _l, _r, _i = index;
    uint8_t* pm, * pl, * pr, * pi;
    bool less;

    if (is_null(_this->ops) || is_null(_this->ops->__lt)) {
        for (; (_l = (_i << 1) + 1) < size;) {
            {
                pi = __i_vector_ptr_add(_this->begin, _i, step);
                pl = __i_vector_ptr_add(_this->begin, _l, step);

                less = __i_vector_slot_read(_this, pi) < __i_vector_slot_read(_this, pl);
                _m = less ? _l : _i;
                pm = less ? pl : pi;
            }

            {
                _r = _l + 1;
                pr = pl + step;

                less = _r < size && __i_vector_slot_read(_this, pm) < __i_vector_slot_read(_this, pr);
                _m = less ? _r : _m;
                pm = less ? pr : pm;
            }

            if (_i == _m)
                break;

            {
                vector_data_t data = __i_vector_slot_read(_this, pm);
                __i_vector_slot_move(_this, pm, pi);
                __i_vector_slot_write(_this, pi, data);
            }
            _i = _m;
        }
    } else {
        for (; (i << 1) + 1 <= len;) {
            priority_queue_size_t ls = (i << 1) + 1;
            priority_queue_size_t rs = (i << 1) + 2;

            m = ls <= len && _this->ops->__lt(_this->head[i].data, _this->head[ls].data) ? ls : i;
            m = rs <= len && _this->ops->__lt(_this->head[m].data, _this->head[rs].data) ? rs : m;

            if (i == m)
                break;

            __priority_queue_exchange_data(&_this->head[m].data, &_this->head[i].data);
            i = m;
        }
    }
}
*/


bool i_priority_queue_push(i_priority_queue_t* _this, priority_queue_data_t data)
{
    if (is_null(_this))
        return false;

    switch (_this->id) {
    case __SEQUENCE_CONTAINER_BASE_VECTOR:
        if (!__i_pq_vector_push(_this, data))
            return false;
        __i_pq_vector_rise(_this);
        return true;
    case __SEQUENCE_CONTAINER_BASE_DEQUE:
        if (!__i_pq_deque_push(_this, data))
            return false;
        __i_pq_deque_rise(_this);
        return true;
    default:
        return false;
    }
}

void i_priority_queue_pop(i_priority_queue_t* _this)
{
    if (is_null(_this))
        return;

    switch (_this->id) {
    case __SEQUENCE_CONTAINER_BASE_VECTOR:
        __i_pq_vector_pop(_this);
        __i_pq_vector_heapify(_this);
        return;
    case __SEQUENCE_CONTAINER_BASE_DEQUE:
        __i_pq_deque_pop(_this);
        __i_pq_deque_heapify(_this);
        return;
    default:
        return;
    }
}

priority_queue_t* __priority_queue_new(uint8_t id, void* pbase, const class_priority_queue_ops_t* ops)
{
    priority_queue_t* priority_queue;

    if (is_null(pbase))
        return NULL;

    priority_queue = (priority_queue_t*)p_calloc(1, sizeof(priority_queue_t));
    if (!is_null(priority_queue)) {
        priority_queue->p   = pbase;
        priority_queue->id  = id;
        priority_queue->ops = ops;
        if (g_class_priority_queue_ops_sso() == ops) {
            priority_queue->sso = true;
            switch (id) {
            case __SEQUENCE_CONTAINER_BASE_VECTOR: priority_queue->v->sso = true; break;
            case __SEQUENCE_CONTAINER_BASE_DEQUE:  priority_queue->d->sso = true; break;
            default: break;
            }
        }
        return priority_queue;
    }

    switch (id) {
    case __SEQUENCE_CONTAINER_BASE_VECTOR: VECTOR_DELETE((vector_t**)&pbase); break;
    case __SEQUENCE_CONTAINER_BASE_DEQUE:  DEQUE_DELETE((deque_t**)&pbase);   break;
    default: break;
    }
    return NULL;
}

void __priority_queue_delete(priority_queue_t** _this)
{
    if (unlikely(is_null(_this) || is_null(*_this)))
        return;

    switch ((*_this)->id) {
    case __SEQUENCE_CONTAINER_BASE_VECTOR: VECTOR_DELETE(&(*_this)->v); break;
    case __SEQUENCE_CONTAINER_BASE_DEQUE:  DEQUE_DELETE(&(*_this)->d);  break;
    default: break;
    }

    p_free(*_this);
}

const class_priority_queue_t* class_priority_queue_ins(void)
{
    static const class_priority_queue_t ins = {
        .size    = cpqueue_size,
        .__empty = __cpqueue_empty,
        .push    = cpqueue_push,
        .top     = cpqueue_top,
        .pop     = cpqueue_pop,
        .swap    = cpqueue_swap,
        .clear   = cpqueue_clear,
    };
    return &ins;
}
