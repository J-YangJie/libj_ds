/*
  I-Deque Interfaces
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

#ifndef __J_I_DEQUE_H
#define __J_I_DEQUE_H

#include <stddef.h>
#include <_compiler.h>
#include <deque/deque_ops.h>

#define _I_DEQUE_ITERATOR_REND  ((uint8_t*)(uintptr_t)JDSC_ITERATOR_REND)

#ifndef is_null
#define is_null(X) (!(X))
#endif /* is_null */

typedef struct i_deque {
    const class_deque_ops_t* ops;
    deque_step_t   step;
    deque_size_t   bkt_size;
    uint8_t**      bkt;
    deque_bcount_t bkt_count;
    deque_bcount_t bkt_count_init;
    deque_iterator_t begin;
    deque_iterator_t end;
} i_deque_t;

static inline
bool __i_deque_empty(const i_deque_t* _this)
{
    return _this->end.cur == _this->begin.cur;
}

static inline
uint8_t* ___i_deque_ptr_add(const uint8_t* p, ptrdiff_t n, deque_step_t step)
{
    return (uint8_t*)p + (ptrdiff_t)step * n;
}

static inline
uint8_t* ___i_deque_ptr_sub(const uint8_t* p, ptrdiff_t n, deque_step_t step)
{
    return ___i_deque_ptr_add(p, -n, step);
}

static inline
ptrdiff_t ___i_deque_ptr_diff(const uint8_t* p1, const uint8_t* p2, deque_step_t step)
{
    return (ptrdiff_t)((p1 - p2) / (ptrdiff_t)step);
}

#define __i_deque_ptr_add(_p, _n, _step)    ___i_deque_ptr_add((_p), (ptrdiff_t)(_n), (_step))
#define __i_deque_ptr_sub(_p, _n, _step)    ___i_deque_ptr_sub((_p), (ptrdiff_t)(_n), (_step))
#define __i_deque_ptr_diff(_p1, _p2, _step) ___i_deque_ptr_diff((_p1), (_p2), (_step))

/* TODO: 保留在这里？ */
static inline
bool i_deque_iter_lt(deque_iterator_t a, deque_iterator_t b)
{
    if (a.bkt != b.bkt)
        return a.bkt < b.bkt;
    return a.cur < b.cur;
}

static inline
void __i_deque_set_bkt(deque_iterator_t* it, uint8_t** bkt)
{
    ptrdiff_t n = it->end - it->begin;
    it->bkt   = bkt;
    it->begin = *bkt;
    it->end   = it->begin + n;
}

static inline
void __i_deque_set_bkt_r(deque_r_iterator_t* rit, uint8_t** bkt)
{
    ptrdiff_t n = rit->end - rit->begin;
    rit->bkt   = bkt;
    rit->begin = *bkt;
    rit->end   = rit->begin + n;
}

static inline
void __i_deque_set_bkt_n(deque_iterator_t* it, uint8_t** bkt, deque_size_t n, deque_step_t step)
{
    it->step  = step;
    it->bkt   = bkt;
    it->begin = *bkt;
    it->end   = __i_deque_ptr_add(it->begin, n, step);
}

static inline
void __i_deque_set_bkt_n_r(deque_r_iterator_t* rit, uint8_t** bkt, deque_size_t n, deque_step_t step)
{
    rit->step  = step;
    rit->bkt   = bkt;
    rit->begin = *bkt;
    rit->end   = __i_deque_ptr_add(rit->begin, n, step);
}

static inline
deque_iterator_t __i_deque_advance(deque_iterator_t it, deque_size_t n)
{
    const ptrdiff_t off = n + __i_deque_ptr_diff(it.cur, it.begin, it.step);
    const ptrdiff_t s = __i_deque_ptr_diff(it.end, it.begin, it.step);

    if (off >= 0 && off < s) {
        it.cur = __i_deque_ptr_add(it.cur, n, it.step);
        return it;
    }

    const ptrdiff_t noff = off > 0 ? off / s : -((-off - 1) / s) - 1;
    __i_deque_set_bkt(&it, it.bkt + noff);
    it.cur = __i_deque_ptr_add(it.begin, (off - noff * s), it.step);
    return it;
}

static inline
deque_size_t __i_deque_iterator_distance(deque_iterator_t begin, deque_iterator_t end)
{
    if (begin.bkt == end.bkt)
        return (deque_size_t)__i_deque_ptr_diff(end.cur, begin.cur, begin.step);
    return (deque_size_t)(
            __i_deque_ptr_diff(begin.end, begin.begin, begin.step) * (end.bkt - begin.bkt - 1)
            + __i_deque_ptr_diff(end.cur, end.begin, begin.step)
            + __i_deque_ptr_diff(begin.end, begin.cur, begin.step));
}

static inline
bool i_deque_is_null_iterator(deque_iterator_t it)
{
    return is_null(it.cur);
}

static inline
deque_iterator_t i_deque_null_iterator(void)
{
    deque_iterator_t it;
    it.cur = NULL;
    return it;
}

static inline
deque_r_iterator_t i_deque_null_r_iterator(void)
{
    deque_r_iterator_t rit;
    rit.cur = NULL;
    return rit;
}





static inline
deque_size_t i_deque_size(const i_deque_t* _this)
{
    return is_null(_this) ? -1 : __i_deque_iterator_distance(_this->begin, _this->end);
}

static inline
deque_iterator_t __i_deque_end(const i_deque_t* _this)
{
    return _this->end;
}

static inline
deque_iterator_t i_deque_end(const i_deque_t* _this)
{
    JDSC_ASSERT(!is_null(_this));
#if JDSC_ITERATOR_ERR_NULL
    if (is_null(_this))
        return i_deque_null_iterator();
#endif /* JDSC_ITERATOR_ERR_NULL */
    return __i_deque_end(_this);
}

static inline
deque_iterator_t __i_deque_begin(const i_deque_t* _this)
{
    return _this->begin;
}

static inline
deque_iterator_t i_deque_begin(const i_deque_t* _this)
{
    JDSC_ASSERT(!is_null(_this));
#if JDSC_ITERATOR_ERR_NULL
    if (is_null(_this))
        return i_deque_null_iterator();
#endif /* JDSC_ITERATOR_ERR_NULL */
    return __i_deque_begin(_this);
}

static inline
deque_iterator_t __i_deque_next(deque_iterator_t it)
{
    JDSC_ASSERT(!is_null(it.cur) && !is_null(it.begin)
                && !is_null(it.end) && !is_null(it.bkt));

#if JDSC_ITERATOR_ERR_NULL
    if (is_null(it.cur) || is_null(it.begin)
        || is_null(it.end) || is_null(it.bkt))
        return i_deque_null_iterator();
#endif /* JDSC_ITERATOR_ERR_NULL */

    it.cur = __i_deque_ptr_add(it.cur, 1, it.step);
    if (it.cur == it.end) {
        __i_deque_set_bkt(&it, it.bkt + 1);
        it.cur = it.begin;
    }
    return it;
}

static inline
deque_iterator_t i_deque_next(deque_iterator_t it)
{
    return __i_deque_next(it);
}

static inline
deque_iterator_t __i_deque_prev(deque_iterator_t it)
{
    JDSC_ASSERT(!is_null(it.cur) && !is_null(it.begin)
                && !is_null(it.end) && !is_null(it.bkt));

#if JDSC_ITERATOR_ERR_NULL
    if (is_null(it.cur) || is_null(it.begin)
        || is_null(it.end) || is_null(it.bkt))
        return i_deque_null_iterator();
#endif /* JDSC_ITERATOR_ERR_NULL */

    if (it.cur == it.begin) {
        __i_deque_set_bkt(&it, it.bkt - 1);
        it.cur = it.end;
    }
    it.cur = __i_deque_ptr_sub(it.cur, 1, it.step);
    return it;
}

static inline
deque_iterator_t i_deque_prev(deque_iterator_t it)
{
    return __i_deque_prev(it);
}

static inline
deque_r_iterator_t __i_deque_rend(const i_deque_t* _this)
{
    deque_r_iterator_t rit;

    rit.cur       = _I_DEQUE_ITERATOR_REND;
    rit.begin     = _this->begin.begin;
    rit.end       = _this->begin.end;
    rit.bkt       = _this->begin.bkt;
    rit.begin_cur = _this->begin.cur;
    rit.step      = _this->step;
    return rit;
}

static inline
deque_r_iterator_t i_deque_rend(const i_deque_t* _this)
{
    JDSC_ASSERT(!is_null(_this));
#if JDSC_ITERATOR_ERR_NULL
    if (is_null(_this))
        return i_deque_null_r_iterator();
#endif /* JDSC_ITERATOR_ERR_NULL */
    return __i_deque_rend(_this);
}

static inline
deque_r_iterator_t __i_deque_rbegin(const i_deque_t* _this)
{
    deque_r_iterator_t rit;
    deque_iterator_t   it;

    if (__i_deque_empty(_this))
        return __i_deque_rend(_this);

    it = __i_deque_prev(_this->end);
    rit.cur       = it.cur;
    rit.begin     = it.begin;
    rit.end       = it.end;
    rit.bkt       = it.bkt;
    rit.begin_cur = _this->begin.cur;
    rit.step      = _this->step;
    return rit;
}

static inline
deque_r_iterator_t i_deque_rbegin(const i_deque_t* _this)
{
    JDSC_ASSERT(!is_null(_this));
#if JDSC_ITERATOR_ERR_NULL
    if (is_null(_this))
        return i_deque_null_r_iterator();
#endif /* JDSC_ITERATOR_ERR_NULL */
    return __i_deque_rbegin(_this);
}

static inline
deque_r_iterator_t __i_deque_rnext(deque_r_iterator_t rit)
{
    JDSC_ASSERT(!is_null(rit.cur) && !is_null(rit.begin)
                && !is_null(rit.end) && !is_null(rit.bkt)
                && !is_null(rit.begin_cur)
                && _I_DEQUE_ITERATOR_REND != rit.cur);

#if JDSC_ITERATOR_ERR_NULL
    if (is_null(rit.cur) || is_null(rit.begin)
        || is_null(rit.end) || is_null(rit.bkt)
        || is_null(rit.begin_cur)
        || _I_DEQUE_ITERATOR_REND == rit.cur)
        return i_deque_null_r_iterator();
#endif /* JDSC_ITERATOR_ERR_NULL */

    if (rit.cur == rit.begin_cur) {
        rit.cur = _I_DEQUE_ITERATOR_REND;
        return rit;
    }

    if (rit.cur == rit.begin) {
        __i_deque_set_bkt_r(&rit, rit.bkt - 1);
        rit.cur = rit.end;
    }
    rit.cur = __i_deque_ptr_sub(rit.cur, 1, rit.step);
    return rit;
}

static inline
deque_r_iterator_t i_deque_rnext(deque_r_iterator_t rit)
{
    return __i_deque_rnext(rit);
}

static inline
deque_r_iterator_t __i_deque_rprev(deque_r_iterator_t rit)
{
    JDSC_ASSERT(!is_null(rit.cur) && !is_null(rit.begin)
                && !is_null(rit.end) && !is_null(rit.bkt)
                && !is_null(rit.begin_cur));

#if JDSC_ITERATOR_ERR_NULL
    if (is_null(rit.cur) || is_null(rit.begin)
        || is_null(rit.end) || is_null(rit.bkt)
        || is_null(rit.begin_cur))
        return i_deque_null_r_iterator();
#endif /* JDSC_ITERATOR_ERR_NULL */

    if (rit.cur == _I_DEQUE_ITERATOR_REND) {
        rit.cur = rit.begin_cur;
        return rit;
    }

    rit.cur = __i_deque_ptr_add(rit.cur, 1, rit.step);
    if (rit.cur == rit.end) {
        __i_deque_set_bkt_r(&rit, rit.bkt + 1);
        rit.cur = rit.begin;
    }
    return rit;
}

static inline
deque_r_iterator_t i_deque_rprev(deque_r_iterator_t rit)
{
    return __i_deque_rprev(rit);
}



uint8_t* __i_deque_push_back_bkt_alloc(i_deque_t* _this);
uint8_t* __i_deque_push_front_bkt_alloc(i_deque_t* _this);
void __i_deque_pop_back_bkt_free(i_deque_t* _this);
void __i_deque_pop_front_bkt_free(i_deque_t* _this);

static inline
bool __i_deque_slot_write(const i_deque_t* _this, deque_data_t data, uint8_t* slot)
{
    if (!is_null(_this->ops) && !is_null(_this->ops->copy_data))
        return _this->ops->copy_data(data, (deque_data_t*)slot);

    if (_this->step <= sizeof(deque_data_t)) {
        switch (_this->step)
        {
        case 1: *(uint8_t*)slot  = (uint8_t)data;  break;
        case 2: *(uint16_t*)slot = (uint16_t)data; break;
        case 4: *(uint32_t*)slot = (uint32_t)data; break;
        case 8: *(uint64_t*)slot = (uint64_t)data; break;
        default: return false;
        }
        return true;
    }
    return false;
}

static inline
bool i_deque_push_back(i_deque_t* _this, deque_data_t data)
{
    uint8_t* slot;

    if (is_null(_this))
        return false;

    if (!is_null(_this->ops) && !is_null(_this->ops->valid_data) && !_this->ops->valid_data(data))
        return false;

    if (_this->end.cur != __i_deque_ptr_sub(_this->end.end, 1, _this->step)) {
        if (!__i_deque_slot_write(_this, data, _this->end.cur))
            return false;
        _this->end.cur = __i_deque_ptr_add(_this->end.cur, 1, _this->step);
        return true;
    }

    slot = __i_deque_push_back_bkt_alloc(_this);
    if (is_null(slot))
        return false;

    if (!__i_deque_slot_write(_this, data, slot)) {
        __i_deque_pop_back_bkt_free(_this);
        return false;
    }
    return true;
}

static inline
bool i_deque_push_front(i_deque_t* _this, deque_data_t data)
{
    uint8_t* slot;

    if (is_null(_this))
        return false;

    if (!is_null(_this->ops) && !is_null(_this->ops->valid_data) && !_this->ops->valid_data(data))
        return false;

    if (_this->begin.cur != _this->begin.begin) {
        _this->begin.cur = __i_deque_ptr_sub(_this->begin.cur, 1, _this->step);
        if (!__i_deque_slot_write(_this, data, _this->begin.cur)) {
            _this->begin.cur = __i_deque_ptr_add(_this->begin.cur, 1, _this->step);
            return false;
        }
        return true;
    }

    slot = __i_deque_push_front_bkt_alloc(_this);
    if (is_null(slot))
        return false;

    if (!__i_deque_slot_write(_this, data, slot)) {
        __i_deque_pop_front_bkt_free(_this);
        return false;
    }
    return true;
}

static inline
void i_deque_pop_back(i_deque_t* _this)
{
    if (is_null(_this) || __i_deque_empty(_this))
        return ;

    if (is_null(_this->ops) || is_null(_this->ops->free_data)) {
        if (_this->end.cur != _this->end.begin)
            _this->end.cur = __i_deque_ptr_sub(_this->end.cur, 1, _this->step);
        else
            __i_deque_pop_back_bkt_free(_this);
    } else {
        if (_this->end.cur != _this->end.begin)
            _this->end.cur = __i_deque_ptr_sub(_this->end.cur, 1, _this->step);
        else
            __i_deque_pop_back_bkt_free(_this);

        _this->ops->free_data((deque_data_t*)_this->end.cur);
    }
}

static inline
void i_deque_pop_front(i_deque_t* _this)
{
    if (is_null(_this) || __i_deque_empty(_this))
        return ;

    if (is_null(_this->ops) || is_null(_this->ops->free_data)) {
        if (_this->begin.cur != __i_deque_ptr_sub(_this->begin.end, 1, _this->step))
            _this->begin.cur = __i_deque_ptr_add(_this->begin.cur, 1, _this->step);
        else
            __i_deque_pop_front_bkt_free(_this);
    } else {
        _this->ops->free_data((deque_data_t*)_this->begin.cur);

        if (_this->begin.cur != __i_deque_ptr_sub(_this->begin.end, 1, _this->step))
            _this->begin.cur = __i_deque_ptr_add(_this->begin.cur, 1, _this->step);
        else
            __i_deque_pop_front_bkt_free(_this);
    }
}

deque_iterator_t __i_deque_insert(i_deque_t* _this, deque_iterator_t pos, deque_data_t data);
static inline
deque_iterator_t i_deque_insert(i_deque_t* _this, deque_iterator_t pos, deque_data_t data)
{
    if (is_null(_this) || i_deque_is_null_iterator(pos))
        return i_deque_null_iterator();

    if (_this->begin.cur == pos.cur) {
        if (i_deque_push_front(_this, data))
            return _this->begin;
        return i_deque_null_iterator();
    }

    if (_this->end.cur == pos.cur) {
        if (i_deque_push_back(_this, data))
            return i_deque_prev(_this->end);
        return i_deque_null_iterator();
    }

    return __i_deque_insert(_this, pos, data);
}

deque_iterator_t __i_deque_erase(i_deque_t* _this, deque_iterator_t pos);
static inline
deque_iterator_t i_deque_erase(i_deque_t* _this, deque_iterator_t pos)
{
    if (is_null(_this) || i_deque_is_null_iterator(pos) 
        || _this->end.cur == pos.cur || __i_deque_empty(_this))
        return i_deque_null_iterator();

    if (_this->begin.cur == pos.cur) {
        i_deque_pop_front(_this);
        return _this->begin;
    }

    if (__i_deque_prev(_this->end).cur == pos.cur) {
        i_deque_pop_back(_this);
        return _this->end;
    }

    return __i_deque_erase(_this, pos);
}

deque_iterator_t __i_deque_erase_range(i_deque_t* _this, deque_iterator_t iterator_begin, deque_iterator_t iterator_end);
static inline
deque_iterator_t i_deque_erase_range(i_deque_t* _this, deque_iterator_t iterator_begin, deque_iterator_t iterator_end)
{
    if (is_null(_this)
        || i_deque_is_null_iterator(iterator_begin)
        || i_deque_is_null_iterator(iterator_end))
        return i_deque_null_iterator();

    if (_this->begin.cur == iterator_begin.cur) {
        while (iterator_end.cur != _this->begin.cur)
            i_deque_pop_front(_this);
        return _this->begin;
    }

    if (_this->end.cur == iterator_end.cur) {
        while (iterator_begin.cur != _this->end.cur)
            i_deque_pop_back(_this);
        return _this->end;
    }

    return __i_deque_erase_range(_this, iterator_begin, iterator_end);
}

deque_iterator_t i_deque_insert_n(i_deque_t* _this, deque_iterator_t pos, deque_size_t n, deque_data_t data);
deque_iterator_t i_deque_find(const i_deque_t* _this, deque_data_t data);
deque_count_t    i_deque_count(const i_deque_t* _this, deque_data_t data);
deque_size_t     i_deque_remove(i_deque_t* _this, deque_data_t data);
deque_size_t     i_deque_remove_if(i_deque_t* _this, remove_if_condition cond);
bool             i_deque_resize(i_deque_t* _this, deque_size_t n, deque_data_t default_data);
deque_size_t     i_deque_clear(i_deque_t* _this);


#endif /* __J_I_DEQUE_H */
