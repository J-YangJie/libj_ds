/*
  Priority Queue Inline Implementations
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

#ifndef __J_I_PRIORITY_QUEUE_H
#define __J_I_PRIORITY_QUEUE_H

#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <_types.h>
#include <_compiler.h>
#include <_memory.h>
#include <vector/vector.h>
#include <deque/deque.h>
#include <priority_queue/priority_queue_ops.h>

typedef struct i_priority_queue {
    uint8_t id;
    bool    sso;
    union {
        vector_t* v;
        deque_t*  d;
        void*     p;
    };
    const class_priority_queue_ops_t* ops;
} i_priority_queue_t;

static inline
priority_queue_value_t __i_pq_at(const i_priority_queue_t* _this, size_t index)
{
    switch (_this->id) {
    case __SEQUENCE_CONTAINER_BASE_VECTOR: return __cvector_at(_this->v, index);
    case __SEQUENCE_CONTAINER_BASE_DEQUE:  return __cdeque_at(_this->d, index);
    default:                               return (priority_queue_value_t){ .u8 = NULL };
    }
}

static inline
bool __i_pq_lt(const i_priority_queue_t* _this, priority_queue_data_t left, priority_queue_data_t right)
{
    if (!is_null(_this->ops) && !is_null(_this->ops->__lt))
        return _this->ops->__lt(left, right);
    return left < right;
}

/* Direct slot reads for the heap loops; indices are always valid here. */
#define __I_PQ_VEC_SLOT(_this, _i)   (__cvector_at((_this)->v, (_i)).d)
#define __I_PQ_DEQ_SLOT(_this, _i)   (__cdeque_at((_this)->d, (_i)).d)
#define __I_PQ_VEC_VALUE(_this, _i)  (*(priority_queue_data_t*)__I_PQ_VEC_SLOT(_this, (_i)))
#define __I_PQ_DEQ_VALUE(_this, _i)  (*(priority_queue_data_t*)__I_PQ_DEQ_SLOT(_this, (_i)))
/*
  With SSO the slot holds a ds_sso_t, so the comparators take the slot
  address instead of dereferencing it.
*/
#define __I_PQ_VEC_SSO_VALUE(_this, _i) ((priority_queue_data_t)__I_PQ_VEC_SLOT(_this, (_i)))
#define __I_PQ_DEQ_SSO_VALUE(_this, _i) ((priority_queue_data_t)__I_PQ_DEQ_SLOT(_this, (_i)))

/*
  The vector heap uses the classic "hole" sift: the value being moved is
  held aside and each level shifts exactly one element, instead of paying a
  three-copy swap (and a cross-TU cvector_swap_at call) per level.
*/
#define __I_PQ_TMP_SIZE 64

/*
  Typed fast paths for the two layouts that matter: pointer-sized values
  (ints, char*, any *_NEW_T type of that size) and the inline SSO struct.
  They index the vector buffer directly, so the per-level slot_move switch
  and step multiplication disappear.
*/
static inline
void __i_pq_rise_vector_8(i_priority_queue_t* _this, priority_queue_size_t index)
{
    vector_t* v = _this->v;
    priority_queue_data_t* a = (priority_queue_data_t*)v->begin;
    const class_priority_queue_ops_t* ops = _this->ops;
    bool (*lt)(priority_queue_data_t, priority_queue_data_t) = is_null(ops) ? NULL : ops->__lt;
    priority_queue_data_t t;
    priority_queue_size_t i = index;

    if (i <= 0)
        return;

    t = a[i];
    while (i > 0) {
        priority_queue_size_t parent = (i - 1) >> 1;

        if (!(lt ? lt(a[parent], t) : (a[parent] < t)))
            break;

        a[i] = a[parent];
        i = parent;
    }
    a[i] = t;
}

static inline
void __i_pq_heapify_vector_8(i_priority_queue_t* _this, priority_queue_size_t index)
{
    vector_t* v = _this->v;
    priority_queue_data_t* a = (priority_queue_data_t*)v->begin;
    const class_priority_queue_ops_t* ops = _this->ops;
    bool (*lt)(priority_queue_data_t, priority_queue_data_t) = is_null(ops) ? NULL : ops->__lt;
    priority_queue_size_t len = (priority_queue_size_t)((v->end - v->begin) / (ptrdiff_t)sizeof(priority_queue_data_t));
    priority_queue_data_t t;
    priority_queue_size_t i = index;

    if (i >= len)
        return;

    t = a[i];
    while ((i << 1) + 1 < len) {
        priority_queue_size_t ls = (i << 1) + 1;
        priority_queue_size_t rs = ls + 1;
        priority_queue_size_t m  = ls;

        if (rs < len && (lt ? lt(a[ls], a[rs]) : (a[ls] < a[rs])))
            m = rs;
        if (!(lt ? lt(t, a[m]) : (t < a[m])))
            break;

        a[i] = a[m];
        i = m;
    }
    a[i] = t;
}

static inline
void __i_pq_rise_vector_sso_32(i_priority_queue_t* _this, priority_queue_size_t index)
{
    vector_t* v = _this->v;
    ds_sso_t* a = (ds_sso_t*)v->begin;
    const class_priority_queue_ops_t* ops = _this->ops;
    bool (*lt)(priority_queue_data_t, priority_queue_data_t) = is_null(ops) ? NULL : ops->__lt;
    ds_sso_t t;
    priority_queue_size_t i = index;

    if (i <= 0)
        return;

    t = a[i];
    __ds_ops_fix_sso((ds_data_t)&t);

    while (i > 0) {
        priority_queue_size_t parent = (i - 1) >> 1;

        if (!(lt ? lt((priority_queue_data_t)&a[parent], (priority_queue_data_t)&t)
                 : ((priority_queue_data_t)&a[parent] < (priority_queue_data_t)&t)))
            break;

        a[i] = a[parent];
        __ds_ops_fix_sso((ds_data_t)&a[i]);
        i = parent;
    }
    a[i] = t;
    __ds_ops_fix_sso((ds_data_t)&a[i]);
}

static inline
void __i_pq_heapify_vector_sso_32(i_priority_queue_t* _this, priority_queue_size_t index)
{
    vector_t* v = _this->v;
    ds_sso_t* a = (ds_sso_t*)v->begin;
    const class_priority_queue_ops_t* ops = _this->ops;
    bool (*lt)(priority_queue_data_t, priority_queue_data_t) = is_null(ops) ? NULL : ops->__lt;
    priority_queue_size_t len = (priority_queue_size_t)((v->end - v->begin) / (ptrdiff_t)sizeof(ds_sso_t));
    ds_sso_t t;
    priority_queue_size_t i = index;

    if (i >= len)
        return;

    t = a[i];
    __ds_ops_fix_sso((ds_data_t)&t);

    while ((i << 1) + 1 < len) {
        priority_queue_size_t ls = (i << 1) + 1;
        priority_queue_size_t rs = ls + 1;
        priority_queue_size_t m  = ls;

        if (rs < len && (lt ? lt((priority_queue_data_t)&a[ls], (priority_queue_data_t)&a[rs])
                            : ((priority_queue_data_t)&a[ls] < (priority_queue_data_t)&a[rs])))
            m = rs;
        if (!(lt ? lt((priority_queue_data_t)&t, (priority_queue_data_t)&a[m])
                 : ((priority_queue_data_t)&t < (priority_queue_data_t)&a[m])))
            break;

        a[i] = a[m];
        __ds_ops_fix_sso((ds_data_t)&a[i]);
        i = m;
    }
    a[i] = t;
    __ds_ops_fix_sso((ds_data_t)&a[i]);
}

static inline
void __i_pq_rise_vector(i_priority_queue_t* _this, priority_queue_size_t index)
{
    vector_t* v = _this->v;
    const vector_step_t step = v->step;

    if (step == (vector_step_t)sizeof(priority_queue_data_t) && !v->sso) {
        __i_pq_rise_vector_8(_this, index);
        return;
    }

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
    __i_vector_slot_move(v, t, hole); /* sso: point inline data at the temp */

    while (i > 0) {
        priority_queue_size_t parent = (i - 1) >> 1;
        uint8_t* ps = __i_vector_iterator(v, parent).cur;

        if (!__i_pq_lt(_this, *(priority_queue_data_t*)ps, *(priority_queue_data_t*)t))
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
void __i_pq_heapify_vector(i_priority_queue_t* _this, priority_queue_size_t index)
{
    vector_t* v = _this->v;
    const vector_step_t step = v->step;

    if (step == (vector_step_t)sizeof(priority_queue_data_t) && !v->sso) {
        __i_pq_heapify_vector_8(_this, index);
        return;
    }

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
    __i_vector_slot_move(v, t, hole); /* sso: point inline data at the temp */

    while ((i << 1) + 1 < len) {
        priority_queue_size_t ls = (i << 1) + 1;
        priority_queue_size_t rs = ls + 1;
        priority_queue_size_t m  = ls;
        uint8_t* ms;

        if (rs < len) {
            uint8_t* l = __i_vector_iterator(v, ls).cur;
            uint8_t* r = __i_vector_iterator(v, rs).cur;
            if (__i_pq_lt(_this, *(priority_queue_data_t*)l, *(priority_queue_data_t*)r))
                m = rs;
        }

        ms = __i_vector_iterator(v, m).cur;
        if (!__i_pq_lt(_this, *(priority_queue_data_t*)t, *(priority_queue_data_t*)ms))
            break;

        __i_vector_slot_move(v, hole, ms);
        hole = ms;
        i = m;
    }

    if (hole != start)
        __i_vector_slot_move(v, hole, t);

    if (t != tmp)
        p_free(t);
}

/*
  Typed deque paths.  A deque element index cannot be reached by simple
  pointer arithmetic: the generic __i_deque_advance divides by the runtime
  step and block span on every access, which dominates the heap loops.
  Here the span is a power of two (64 elements for 8-byte slots, 16 for
  the SSO struct), so index -> slot becomes shifts and a block lookup.
*/
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

static inline
void __i_pq_rise_deque_8(i_priority_queue_t* _this, priority_queue_size_t index)
{
    deque_t* d = _this->d;
    const deque_size_t mask = (deque_size_t)d->bkt_size - 1;
    const int shift = __builtin_ctz((unsigned)d->bkt_size);
    const class_priority_queue_ops_t* ops = _this->ops;
    bool (*lt)(priority_queue_data_t, priority_queue_data_t) = is_null(ops) ? NULL : ops->__lt;
    priority_queue_data_t t;
    priority_queue_size_t i = index;
    uint8_t* hole;

    if (i <= 0)
        return;

    hole = __i_pq_deq_slot8(d, i, mask, shift);
    t = *(priority_queue_data_t*)hole;

    while (i > 0) {
        priority_queue_size_t parent = (i - 1) >> 1;
        uint8_t* ps = __i_pq_deq_slot8(d, parent, mask, shift);

        if (!(lt ? lt(*(priority_queue_data_t*)ps, t) : (*(priority_queue_data_t*)ps < t)))
            break;

        *(priority_queue_data_t*)hole = *(priority_queue_data_t*)ps;
        hole = ps;
        i = parent;
    }
    *(priority_queue_data_t*)hole = t;
}

static inline
void __i_pq_heapify_deque_8(i_priority_queue_t* _this, priority_queue_size_t index)
{
    deque_t* d = _this->d;
    const deque_size_t mask = (deque_size_t)d->bkt_size - 1;
    const int shift = __builtin_ctz((unsigned)d->bkt_size);
    const class_priority_queue_ops_t* ops = _this->ops;
    bool (*lt)(priority_queue_data_t, priority_queue_data_t) = is_null(ops) ? NULL : ops->__lt;
    priority_queue_size_t len = __i_pq_deq_size8(d);
    priority_queue_data_t t;
    priority_queue_size_t i = index;
    uint8_t* hole;

    if (i >= len)
        return;

    hole = __i_pq_deq_slot8(d, i, mask, shift);
    t = *(priority_queue_data_t*)hole;

    while ((i << 1) + 1 < len) {
        priority_queue_size_t ls = (i << 1) + 1;
        priority_queue_size_t rs = ls + 1;
        priority_queue_size_t m  = ls;
        uint8_t* ms;

        if (rs < len) {
            priority_queue_data_t l = *(priority_queue_data_t*)__i_pq_deq_slot8(d, ls, mask, shift);
            priority_queue_data_t r = *(priority_queue_data_t*)__i_pq_deq_slot8(d, rs, mask, shift);
            if (lt ? lt(l, r) : (l < r))
                m = rs;
        }

        ms = __i_pq_deq_slot8(d, m, mask, shift);
        if (!(lt ? lt(t, *(priority_queue_data_t*)ms) : (t < *(priority_queue_data_t*)ms)))
            break;

        *(priority_queue_data_t*)hole = *(priority_queue_data_t*)ms;
        hole = ms;
        i = m;
    }
    *(priority_queue_data_t*)hole = t;
}

static inline
void __i_pq_rise_deque_sso_32(i_priority_queue_t* _this, priority_queue_size_t index)
{
    deque_t* d = _this->d;
    const deque_size_t mask = (deque_size_t)d->bkt_size - 1;
    const int shift = __builtin_ctz((unsigned)d->bkt_size);
    const class_priority_queue_ops_t* ops = _this->ops;
    bool (*lt)(priority_queue_data_t, priority_queue_data_t) = is_null(ops) ? NULL : ops->__lt;
    ds_sso_t* hole;
    ds_sso_t t;
    priority_queue_size_t i = index;

    if (i <= 0)
        return;

    hole = (ds_sso_t*)__i_pq_deq_slot32(d, i, mask, shift);
    t = *hole;
    __ds_ops_fix_sso((ds_data_t)&t);

    while (i > 0) {
        priority_queue_size_t parent = (i - 1) >> 1;
        ds_sso_t* ps = (ds_sso_t*)__i_pq_deq_slot32(d, parent, mask, shift);

        if (!lt((priority_queue_data_t)ps, (priority_queue_data_t)&t))
            break;

        *hole = *ps;
        __ds_ops_fix_sso((ds_data_t)hole);
        hole = ps;
        i = parent;
    }
    *hole = t;
    __ds_ops_fix_sso((ds_data_t)hole);
}

static inline
void __i_pq_heapify_deque_sso_32(i_priority_queue_t* _this, priority_queue_size_t index)
{
    deque_t* d = _this->d;
    const deque_size_t mask = (deque_size_t)d->bkt_size - 1;
    const int shift = __builtin_ctz((unsigned)d->bkt_size);
    const class_priority_queue_ops_t* ops = _this->ops;
    bool (*lt)(priority_queue_data_t, priority_queue_data_t) = is_null(ops) ? NULL : ops->__lt;
    priority_queue_size_t len = __i_pq_deq_size32(d);
    ds_sso_t* hole;
    ds_sso_t t;
    priority_queue_size_t i = index;

    if (i >= len)
        return;

    hole = (ds_sso_t*)__i_pq_deq_slot32(d, i, mask, shift);
    t = *hole;
    __ds_ops_fix_sso((ds_data_t)&t);

    while ((i << 1) + 1 < len) {
        priority_queue_size_t ls = (i << 1) + 1;
        priority_queue_size_t rs = ls + 1;
        priority_queue_size_t m  = ls;
        ds_sso_t* ms;

        if (rs < len) {
            ds_sso_t* l = (ds_sso_t*)__i_pq_deq_slot32(d, ls, mask, shift);
            ds_sso_t* r = (ds_sso_t*)__i_pq_deq_slot32(d, rs, mask, shift);
            if (lt((priority_queue_data_t)l, (priority_queue_data_t)r))
                m = rs;
        }

        ms = (ds_sso_t*)__i_pq_deq_slot32(d, m, mask, shift);
        if (!lt((priority_queue_data_t)&t, (priority_queue_data_t)ms))
            break;

        *hole = *ms;
        __ds_ops_fix_sso((ds_data_t)hole);
        hole = ms;
        i = m;
    }
    *hole = t;
    __ds_ops_fix_sso((ds_data_t)hole);
}

static inline
void __i_pq_rise_deque(i_priority_queue_t* _this, priority_queue_size_t index)
{
    deque_t* d = _this->d;
    const deque_size_t bs = (deque_size_t)d->bkt_size;

    if (d->step == (deque_step_t)sizeof(priority_queue_data_t) && !d->sso
        && bs > 0 && 0 == (bs & (bs - 1))) {
        __i_pq_rise_deque_8(_this, index);
        return;
    }

    priority_queue_size_t i = index;

    while (i > 0) {
        priority_queue_size_t parent = (i - 1) >> 1;

        if (!__i_pq_lt(_this, __I_PQ_DEQ_VALUE(_this, parent), __I_PQ_DEQ_VALUE(_this, i)))
            break;

        cdeque_swap_at(_this->d, parent, i);
        i = parent;
    }
}

static inline
void __i_pq_heapify_deque(i_priority_queue_t* _this, priority_queue_size_t index)
{
    deque_t* d = _this->d;
    const deque_size_t bs = (deque_size_t)d->bkt_size;

    if (d->step == (deque_step_t)sizeof(priority_queue_data_t) && !d->sso
        && bs > 0 && 0 == (bs & (bs - 1))) {
        __i_pq_heapify_deque_8(_this, index);
        return;
    }

    priority_queue_size_t i = index;
    priority_queue_size_t len = cdeque_size(_this->d);

    while ((i << 1) + 1 < len) {
        priority_queue_size_t ls = (i << 1) + 1;
        priority_queue_size_t rs = (i << 1) + 2;
        priority_queue_size_t m  = i;

        if (__i_pq_lt(_this, __I_PQ_DEQ_VALUE(_this, m), __I_PQ_DEQ_VALUE(_this, ls)))
            m = ls;
        if (rs < len && __i_pq_lt(_this, __I_PQ_DEQ_VALUE(_this, m), __I_PQ_DEQ_VALUE(_this, rs)))
            m = rs;

        if (m == i)
            break;

        cdeque_swap_at(_this->d, i, m);
        i = m;
    }
}

static inline
void __i_pq_rise_vector_sso(i_priority_queue_t* _this, priority_queue_size_t index)
{
    vector_t* v = _this->v;
    const vector_step_t step = v->step;

    if (step == (vector_step_t)sizeof(ds_sso_t)) {
        __i_pq_rise_vector_sso_32(_this, index);
        return;
    }

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
    __i_vector_slot_move(v, t, hole); /* point inline data at the temp */

    while (i > 0) {
        priority_queue_size_t parent = (i - 1) >> 1;
        uint8_t* ps = __i_vector_iterator(v, parent).cur;

        if (!__i_pq_lt(_this, (priority_queue_data_t)ps, (priority_queue_data_t)t))
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
void __i_pq_heapify_vector_sso(i_priority_queue_t* _this, priority_queue_size_t index)
{
    vector_t* v = _this->v;
    const vector_step_t step = v->step;

    if (step == (vector_step_t)sizeof(ds_sso_t)) {
        __i_pq_heapify_vector_sso_32(_this, index);
        return;
    }

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
    __i_vector_slot_move(v, t, hole); /* point inline data at the temp */

    while ((i << 1) + 1 < len) {
        priority_queue_size_t ls = (i << 1) + 1;
        priority_queue_size_t rs = ls + 1;
        priority_queue_size_t m  = ls;
        uint8_t* ms;

        if (rs < len) {
            uint8_t* l = __i_vector_iterator(v, ls).cur;
            uint8_t* r = __i_vector_iterator(v, rs).cur;
            if (__i_pq_lt(_this, (priority_queue_data_t)l, (priority_queue_data_t)r))
                m = rs;
        }

        ms = __i_vector_iterator(v, m).cur;
        if (!__i_pq_lt(_this, (priority_queue_data_t)t, (priority_queue_data_t)ms))
            break;

        __i_vector_slot_move(v, hole, ms);
        hole = ms;
        i = m;
    }

    if (hole != start)
        __i_vector_slot_move(v, hole, t);

    if (t != tmp)
        p_free(t);
}

static inline
void __i_pq_rise_deque_sso(i_priority_queue_t* _this, priority_queue_size_t index)
{
    deque_t* d = _this->d;
    const deque_size_t bs = (deque_size_t)d->bkt_size;

    if (d->step == (deque_step_t)sizeof(ds_sso_t) && d->sso
        && bs > 0 && 0 == (bs & (bs - 1))) {
        __i_pq_rise_deque_sso_32(_this, index);
        return;
    }

    priority_queue_size_t i = index;

    while (i > 0) {
        priority_queue_size_t parent = (i - 1) >> 1;

        if (!__i_pq_lt(_this, __I_PQ_DEQ_SSO_VALUE(_this, parent), __I_PQ_DEQ_SSO_VALUE(_this, i)))
            break;

        cdeque_swap_at(_this->d, parent, i);
        i = parent;
    }
}

static inline
void __i_pq_heapify_deque_sso(i_priority_queue_t* _this, priority_queue_size_t index)
{
    deque_t* d = _this->d;
    const deque_size_t bs = (deque_size_t)d->bkt_size;

    if (d->step == (deque_step_t)sizeof(ds_sso_t) && d->sso
        && bs > 0 && 0 == (bs & (bs - 1))) {
        __i_pq_heapify_deque_sso_32(_this, index);
        return;
    }

    priority_queue_size_t i = index;
    priority_queue_size_t len = cdeque_size(_this->d);

    while ((i << 1) + 1 < len) {
        priority_queue_size_t ls = (i << 1) + 1;
        priority_queue_size_t rs = (i << 1) + 2;
        priority_queue_size_t m  = i;

        if (__i_pq_lt(_this, __I_PQ_DEQ_SSO_VALUE(_this, m), __I_PQ_DEQ_SSO_VALUE(_this, ls)))
            m = ls;
        if (rs < len && __i_pq_lt(_this, __I_PQ_DEQ_SSO_VALUE(_this, m), __I_PQ_DEQ_SSO_VALUE(_this, rs)))
            m = rs;

        if (m == i)
            break;

        cdeque_swap_at(_this->d, i, m);
        i = m;
    }
}

/* checked */
static inline
priority_queue_size_t i_priority_queue_size(const i_priority_queue_t* _this)
{
    if (!is_null(_this)) {
        switch (_this->id) {
        case __SEQUENCE_CONTAINER_BASE_VECTOR: return cvector_size(_this->v);
        case __SEQUENCE_CONTAINER_BASE_DEQUE:  return cdeque_size(_this->d);
        default:                               break;
        }
    }
    return -1;
}

/* checked */
static inline
bool __i_priority_queue_empty(const i_priority_queue_t* _this)
{
    switch (_this->id) {
    case __SEQUENCE_CONTAINER_BASE_VECTOR: return __cvector_empty(_this->v);
    case __SEQUENCE_CONTAINER_BASE_DEQUE:  return __cdeque_empty(_this->d);
    default:                               return false;
    }
}

static inline
bool i_priority_queue_push(i_priority_queue_t* _this, priority_queue_data_t data)
{
    if (is_null(_this))
        return false;

    switch (_this->id) {
    case __SEQUENCE_CONTAINER_BASE_VECTOR:
        if (!cvector_push_back(_this->v, data))
            return false;
        if (_this->sso)
            __i_pq_rise_vector_sso(_this, cvector_size(_this->v) - 1);
        else
            __i_pq_rise_vector(_this, cvector_size(_this->v) - 1);
        return true;
    case __SEQUENCE_CONTAINER_BASE_DEQUE:
        if (!cdeque_push_back(_this->d, data))
            return false;
        if (_this->sso)
            __i_pq_rise_deque_sso(_this, cdeque_size(_this->d) - 1);
        else
            __i_pq_rise_deque(_this, cdeque_size(_this->d) - 1);
        return true;
    default:
        return false;
    }
}

static inline
priority_queue_value_t i_priority_queue_top(const i_priority_queue_t* _this)
{
    if (!is_null(_this)) {
        switch (_this->id) {
        case __SEQUENCE_CONTAINER_BASE_VECTOR:
            if (!__cvector_empty(_this->v))
                return cvector_front(_this->v);
            break;
        case __SEQUENCE_CONTAINER_BASE_DEQUE:
            if (!__cdeque_empty(_this->d))
                return cdeque_front(_this->d);
            break;
        default:
            break;
        }
    }
    return (priority_queue_value_t){ .u8 = NULL };
}

static inline
void i_priority_queue_pop(i_priority_queue_t* _this)
{
    priority_queue_size_t size;

    if (is_null(_this))
        return;

    switch (_this->id) {
    case __SEQUENCE_CONTAINER_BASE_VECTOR: {
        vector_t* v = _this->v;

        if (v->step == (vector_step_t)sizeof(priority_queue_data_t) && !v->sso) {
            priority_queue_data_t* a = (priority_queue_data_t*)v->begin;

            size = (priority_queue_size_t)((v->end - v->begin) / (ptrdiff_t)sizeof(priority_queue_data_t));
            if (size <= 0)
                return;
            if (size > 1) {
                priority_queue_data_t t = a[0];
                a[0] = a[size - 1];
                a[size - 1] = t;
            }
            cvector_pop_back(v);
            if (size > 1)
                __i_pq_heapify_vector_8(_this, 0);
            return;
        }

        if (v->sso && v->step == (vector_step_t)sizeof(ds_sso_t)) {
            ds_sso_t* a = (ds_sso_t*)v->begin;
            ds_sso_t t;

            size = (priority_queue_size_t)((v->end - v->begin) / (ptrdiff_t)sizeof(ds_sso_t));
            if (size <= 0)
                return;
            if (size > 1) {
                t = a[0];
                a[0] = a[size - 1];
                __ds_ops_fix_sso((ds_data_t)&a[0]);
                a[size - 1] = t;
                __ds_ops_fix_sso((ds_data_t)&a[size - 1]);
            }
            cvector_pop_back(v);
            if (size > 1)
                __i_pq_heapify_vector_sso_32(_this, 0);
            return;
        }

        size = cvector_size(v);
        if (size <= 0)
            return;
        if (size > 1 && !cvector_swap_at(v, 0, (vector_index_t)(size - 1)))
            return;
        cvector_pop_back(v);
        if (size > 1) {
            if (_this->sso)
                __i_pq_heapify_vector_sso(_this, 0);
            else
                __i_pq_heapify_vector(_this, 0);
        }
        return;
    }
    case __SEQUENCE_CONTAINER_BASE_DEQUE: {
        deque_t* d = _this->d;
        const deque_size_t bs = (deque_size_t)d->bkt_size;

        if (d->step == (deque_step_t)sizeof(priority_queue_data_t) && !d->sso
            && bs > 0 && 0 == (bs & (bs - 1))) {
            const deque_size_t mask = bs - 1;
            const int shift = __builtin_ctz((unsigned)bs);

            size = __i_pq_deq_size8(d);
            if (size <= 0)
                return;
            if (size > 1) {
                priority_queue_data_t* a = (priority_queue_data_t*)__i_pq_deq_slot8(d, 0, mask, shift);
                priority_queue_data_t* b = (priority_queue_data_t*)
                    __i_pq_deq_slot8(d, (priority_queue_size_t)(size - 1), mask, shift);
                priority_queue_data_t t = *a;
                *a = *b;
                *b = t;
            }
            cdeque_pop_back(d);
            if (size > 1)
                __i_pq_heapify_deque_8(_this, 0);
            return;
        }

        if (d->sso && d->step == (deque_step_t)sizeof(ds_sso_t)
            && bs > 0 && 0 == (bs & (bs - 1))) {
            const deque_size_t mask = bs - 1;
            const int shift = __builtin_ctz((unsigned)bs);

            size = __i_pq_deq_size32(d);
            if (size <= 0)
                return;
            if (size > 1) {
                ds_sso_t* a = (ds_sso_t*)__i_pq_deq_slot32(d, 0, mask, shift);
                ds_sso_t* b = (ds_sso_t*)__i_pq_deq_slot32(d, (priority_queue_size_t)(size - 1), mask, shift);
                ds_sso_t t = *a;

                *a = *b;
                __ds_ops_fix_sso((ds_data_t)a);
                *b = t;
                __ds_ops_fix_sso((ds_data_t)b);
            }
            cdeque_pop_back(d);
            if (size > 1)
                __i_pq_heapify_deque_sso_32(_this, 0);
            return;
        }

        size = cdeque_size(d);
        if (size <= 0)
            return;
        if (size > 1 && !cdeque_swap_at(d, 0, (deque_index_t)(size - 1)))
            return;
        cdeque_pop_back(d);
        if (size > 1) {
            if (_this->sso)
                __i_pq_heapify_deque_sso(_this, 0);
            else
                __i_pq_heapify_deque(_this, 0);
        }
        return;
    }
    default:
        return;
    }
}

/* checked */
static inline
void i_priority_queue_swap(i_priority_queue_t* _this, i_priority_queue_t* other)
{
    void* p;

    if (!is_null(_this) && !is_null(other) && _this != other && _this->id == other->id) {
        JDSC_ASSERT(_this->ops == other->ops); /* Include sso */
        switch (_this->id) {
        case __SEQUENCE_CONTAINER_BASE_VECTOR:
            JDSC_ASSERT(   _this->v->step  == other->v->step 
                        && _this->v->align == other->v->align);
            break;
        case __SEQUENCE_CONTAINER_BASE_DEQUE:
            JDSC_ASSERT(   _this->d->step  == other->d->step 
                        && _this->d->align == other->d->align);
            break;
        default:
            break;
        }

        p = _this->p;
        _this->p = other->p;
        other->p = p;
    }
}

/* checked */
static inline
priority_queue_size_t i_priority_queue_clear(i_priority_queue_t* _this)
{
    if (!is_null(_this)) {
        switch (_this->id) {
        case __SEQUENCE_CONTAINER_BASE_VECTOR: return cvector_clear(_this->v);
        case __SEQUENCE_CONTAINER_BASE_DEQUE:  return cdeque_clear(_this->d);
        default:                               break;
        }
    }
    return -1;
}

#endif /* __J_I_PRIORITY_QUEUE_H */
