/*
  Deque Implementations
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

#include <deque/deque.h>

#include <string.h>
#include <_log.h>
#include <_memory.h>
#include <_compiler_inter.h>
#include <linux/_compiler.h>

#define TAG "[deque]"

#define DEQUE_CAPACITY_INIT   8
#define _I_DEQUE_BKT_BYTES    512
#define _I_DEQUE_BKT_SIZE_MIN 8

static JDSC_INLINE
bool __i_deque_slot_write_n(const i_deque_t* _this, deque_iterator_t pos, deque_size_t n, deque_data_t data)
{
    const deque_step_t step = _this->step;
    deque_size_t suc = 0;

    if (!is_null(_this->ops) && !is_null(_this->ops->copy_data)) {
        while (n-- > 0) {
            if (_this->ops->copy_data(data, (deque_data_t*)pos.cur))
                suc++;
            else
                goto err;
            pos = __i_deque_next(pos);
        }

        return true;
    } else {
        while (n-- > 0) {
            switch (step)
            {
            case 1: *(uint8_t*)pos.cur  = (uint8_t)data;  break;
            case 2: *(uint16_t*)pos.cur = (uint16_t)data; break;
            case 4: *(uint32_t*)pos.cur = (uint32_t)data; break;
            case 8: __ds_token_store_8(pos.cur, data); break;
            default: return false;
            }

            pos = __i_deque_next(pos);
        }

        return true;
    }

err:
    if (!is_null(_this->ops) && !is_null(_this->ops->free_data)) {
        while (suc-- > 0) {
            pos = __i_deque_prev(pos);
            _this->ops->free_data((deque_data_t*)pos.cur);
        }
    }
    return false;
}

static JDSC_INLINE
bool __i_deque_slot_write_n_range(const i_deque_t* _this, deque_iterator_t dest, deque_iterator_t src, deque_size_t n)
{
    const deque_step_t step = _this->step;
    deque_size_t suc = 0;

    if (is_null(_this->ops) || is_null(_this->ops->copy_data)) {
        while (n-- > 0) {
            memcpy(dest.cur, src.cur, step);
            dest = __i_deque_next(dest);
            src  = __i_deque_next(src);
        }
        return true;
    }

    if (_this->dtype & SEQ_CNTR_DTYPE_MASK_TOKEN_ADDR) {
        if (_this->sso) {
            while (n-- > 0) {
                if (ds_ops_copy_data_same_sso((deque_data_t)src.cur, (deque_data_t*)dest.cur))
                    suc++;
                else
                    goto err;

                dest = __i_deque_next(dest);
                src  = __i_deque_next(src);
            }
        } else {
            while (n-- > 0) {
                if (_this->ops->copy_data((deque_data_t)src.cur, (deque_data_t*)dest.cur))
                    suc++;
                else
                    goto err;

                dest = __i_deque_next(dest);
                src  = __i_deque_next(src);
            }
        }
    } else if (_this->dtype & SEQ_CNTR_DTYPE_MASK_NUM) {
        while (n-- > 0) {
            if (_this->ops->copy_data(__i_deque_slot_read(_this, src.cur), (deque_data_t*)dest.cur))
                suc++;
            else
                goto err;

            dest = __i_deque_next(dest);
            src  = __i_deque_next(src);
        }
    } else {
        return false;
    }

    return true;

err:
    if (!is_null(_this->ops) && !is_null(_this->ops->free_data)) {
        while (suc-- > 0) {
            dest = __i_deque_prev(dest);
            _this->ops->free_data((deque_data_t*)dest.cur);
        }
    }
    return false;
}

static JDSC_INLINE
void __i_deque_slot_move_n_forward(const i_deque_t* _this, deque_iterator_t dest, deque_iterator_t src, deque_size_t n)
{
    const class_deque_ops_t* ops = _this->ops;
    void (*__move)(deque_data_t, deque_data_t) = is_null(ops) ? NULL : ops->__move;

    if (is_null(__move)) {
        for (deque_size_t i = 0; i < n; ++i) {
            __i_deque_slot_memmove(_this, dest.cur, src.cur);
            dest = __i_deque_next(dest);
            src  = __i_deque_next(src);
        }
    } else if (dest.cur != src.cur) {
        for (deque_size_t i = 0; i < n; ++i) {
            __move((deque_data_t)dest.cur, (deque_data_t)src.cur);
            dest = __i_deque_next(dest);
            src  = __i_deque_next(src);
        }
    }
}

static JDSC_INLINE
void __i_deque_slot_move_n_backward(const i_deque_t* _this, deque_iterator_t dest, deque_iterator_t src, deque_size_t n)
{
    const class_deque_ops_t* ops = _this->ops;
    void (*__move)(deque_data_t, deque_data_t);

    if (unlikely(n <= 0))
        return ;

    dest = __i_deque_advance(dest, n - 1);
    src  = __i_deque_advance(src,  n - 1);
    __move = is_null(ops) ? NULL : ops->__move;

    if (is_null(__move)) {
        for (deque_size_t i = 0; i < n; ++i) {
            __i_deque_slot_memmove(_this, dest.cur, src.cur);
            dest = __i_deque_prev(dest);
            src  = __i_deque_prev(src);
        }
    } else if (dest.cur != src.cur) {
        for (deque_size_t i = 0; i < n; ++i) {
            __move((deque_data_t)dest.cur, (deque_data_t)src.cur);
            dest = __i_deque_prev(dest);
            src  = __i_deque_prev(src);
        }
    }
}

#include <limits.h>
#include <stdint.h> /* SIZE_MAX */

#ifndef SSIZE_MAX
#define SSIZE_MAX ((ssize_t)(SIZE_MAX / 2))
#endif /* SSIZE_MAX */

JDSC_INLINE
deque_size_t __i_deque_max_size(const i_deque_t* _this)
{
    return 0 == _this->step ? -1 : (SSIZE_MAX / _this->step) - 1;
}

static
bool __i_deque_reserve(i_deque_t* _this)
{
    deque_bcount_t bcnt_o = _this->bkt_count;
    deque_bcount_t bcnt_n = bcnt_o > 0 ? bcnt_o * 2 : DEQUE_CAPACITY_INIT;
    deque_bcount_t off_b  = _this->begin.bkt - _this->bkt;
    deque_bcount_t off_e  = _this->end.bkt   - _this->bkt;
    deque_bcount_t n      = off_e - off_b + 1;
    deque_bcount_t off_n;
    uint8_t**      bkt_n;

    /* The logic of functions `__i_deque_push_back_bkt_alloc` and `__i_deque_push_front_bkt_alloc` 
       assumes by default that, after `reserve`, at least one empty slot is guaranteed to remain 
       both before and after the reserved region. */
    if (unlikely(bcnt_n < DEQUE_CAPACITY_INIT))
        bcnt_n = DEQUE_CAPACITY_INIT;

    off_n = (bcnt_n - n) / 2;

    bkt_n = (uint8_t**)p_calloc((size_t)bcnt_n, sizeof(uint8_t*));
    if (is_null(bkt_n))
        return false;

    memcpy(bkt_n + off_n, _this->begin.bkt, (size_t)n * sizeof(uint8_t*));
    p_free(_this->bkt);

    _this->bkt = bkt_n;
    _this->bkt_count = bcnt_n;
    _this->begin.bkt = _this->bkt + off_n;
    _this->end.bkt   = _this->bkt + off_n + n - 1;
    return true;
}

static
void __i_deque_shrink_to_fit(i_deque_t* _this)
{
    deque_bcount_t off_b  = _this->begin.bkt - _this->bkt;
    deque_bcount_t off_e  = _this->end.bkt   - _this->bkt;
    deque_bcount_t n      = off_e - off_b + 1;
    uint8_t**      bkt_n;

    if (unlikely(_this->bkt_count == n))
        return ;

    bkt_n = (uint8_t**)p_calloc((size_t)n, sizeof(uint8_t*));
    if (is_null(bkt_n))
        return ;

    memcpy(bkt_n, _this->begin.bkt, (size_t)n * sizeof(uint8_t*));
    p_free(_this->bkt);

    _this->bkt = bkt_n;
    _this->bkt_count = n;
    _this->begin.bkt = _this->bkt;
    _this->end.bkt   = _this->bkt + n - 1;
}

JDSC_INLINE
void i_deque_shrink_to_fit(i_deque_t* _this)
{
    if (unlikely(is_null(_this)))
        return ;
    __i_deque_shrink_to_fit(_this);
}

static JDSC_INLINE_FORCE
uint8_t* __i_deque_alloc_bkt(const i_deque_t* _this)
{
    return (uint8_t*)p_aligned_malloc(_this->align, _this->bkt_size * _this->step);
}

static JDSC_INLINE_FORCE
void __i_deque_free_bkt(const i_deque_t* _this, uint8_t** bkt)
{
    p_aligned_free(*bkt, _this->align);
}

JDSC_INLINE
uint8_t* __i_deque_push_back_bkt_alloc(i_deque_t* _this)
{
    uint8_t** bkt;
    uint8_t*  slot;

    if (_this->end.bkt == _this->bkt + _this->bkt_count - 1) {
        if (!__i_deque_reserve(_this))
            return NULL;
    }

    bkt = _this->end.bkt + 1;
    *bkt = __i_deque_alloc_bkt(_this);
    if (is_null(*bkt))
        return NULL;

    slot = _this->end.cur;
    __i_deque_set_bkt_n(&_this->end, bkt, _this->bkt_size, _this->step);
    _this->end.cur = _this->end.begin;
    return slot;
}

JDSC_INLINE
uint8_t* __i_deque_push_front_bkt_alloc(i_deque_t* _this)
{
    uint8_t** bkt;

    if (_this->begin.bkt == _this->bkt) {
        if (!__i_deque_reserve(_this))
            return NULL;
    }

    bkt = _this->begin.bkt - 1;
    *bkt = __i_deque_alloc_bkt(_this);
    if (is_null(*bkt))
        return NULL;

    __i_deque_set_bkt_n(&_this->begin, bkt, _this->bkt_size, _this->step);
    _this->begin.cur = __i_deque_ptr_sub(_this->begin.end, 1, _this->step);
    return _this->begin.cur;
}

JDSC_INLINE
void __i_deque_pop_back_bkt_free(i_deque_t* _this)
{
    __i_deque_free_bkt(_this, _this->end.bkt);
    __i_deque_set_bkt_n(&_this->end, _this->end.bkt - 1, _this->bkt_size, _this->step);
    _this->end.cur = __i_deque_ptr_sub(_this->end.end, 1, _this->step);
}

JDSC_INLINE
void __i_deque_pop_front_bkt_free(i_deque_t* _this)
{
    __i_deque_free_bkt(_this, _this->begin.bkt);
    __i_deque_set_bkt_n(&_this->begin, _this->begin.bkt + 1, _this->bkt_size, _this->step);
    _this->begin.cur = _this->begin.begin;
}

static JDSC_INLINE
bool __i_deque_push_back_null(i_deque_t* _this)
{
    if (_this->end.cur != __i_deque_ptr_sub(_this->end.end, 1, _this->step)) {
        _this->end.cur = __i_deque_ptr_add(_this->end.cur, 1, _this->step);
        return true;
    }
    return !is_null(__i_deque_push_back_bkt_alloc(_this));
}

static JDSC_INLINE
bool __i_deque_push_front_null(i_deque_t* _this)
{
    if (_this->begin.cur != _this->begin.begin) {
        _this->begin.cur = __i_deque_ptr_sub(_this->begin.cur, 1, _this->step);
        return true;
    }
    return !is_null(__i_deque_push_front_bkt_alloc(_this));
}

static JDSC_INLINE
void __i_deque_pop_back_without_free_data(i_deque_t* _this)
{
    JDSC_ASSERT(!__i_deque_empty(_this));

    if (_this->end.cur != _this->end.begin)
        _this->end.cur = __i_deque_ptr_sub(_this->end.cur, 1, _this->step);
    else
        __i_deque_pop_back_bkt_free(_this);
}

static JDSC_INLINE
void __i_deque_pop_front_without_free_data(i_deque_t* _this)
{
    JDSC_ASSERT(!__i_deque_empty(_this));

    if (_this->begin.cur != __i_deque_ptr_sub(_this->begin.end, 1, _this->step))
        _this->begin.cur = __i_deque_ptr_add(_this->begin.cur, 1, _this->step);
    else
        __i_deque_pop_front_bkt_free(_this);
}

static JDSC_LOGIC_FRAGMENT /* Because of `n` and `data` */
deque_iterator_t __i_deque_insert_run(i_deque_t* _this, deque_iterator_t pos, deque_size_t n, deque_data_t data)
{
    deque_iterator_t ret;
    deque_size_t pushed;
    deque_size_t fn = __i_deque_iterator_distance(_this->begin, pos);
    deque_size_t bn = __i_deque_iterator_distance(pos, _this->end);

    if (unlikely(fn < 0 || bn < 0))
        return i_deque_null_iterator();

    if (fn < bn) {
        for (pushed = 0; pushed < n; ++pushed) {
            if (!__i_deque_push_front_null(_this))
                goto err;
        }

        __i_deque_slot_move_n_forward(_this, _this->begin, __i_deque_advance(_this->begin, n), fn);
        ret = __i_deque_advance(_this->begin, fn);
    } else {
        for (pushed = 0; pushed < n; ++pushed) {
            if (!__i_deque_push_back_null(_this))
                goto err;
        }

        ret = __i_deque_advance(_this->begin, fn);
        __i_deque_slot_move_n_backward(_this, __i_deque_advance(ret, n), ret, bn);
    }

    if (!__i_deque_slot_write_n(_this, ret, n, data))
        goto err_slot;

    return ret;

err_slot:
    if (fn < bn)
        __i_deque_slot_move_n_backward(_this, __i_deque_advance(_this->begin, n), _this->begin, fn);
    else
        __i_deque_slot_move_n_forward(_this, ret, __i_deque_advance(ret, n), bn);

err:
    if (fn < bn) {
        while (pushed-- > 0)
            __i_deque_pop_front_without_free_data(_this);
    } else {
        while (pushed-- > 0)
            __i_deque_pop_back_without_free_data(_this);
    }
    return i_deque_null_iterator();
}

JDSC_INLINE
deque_iterator_t __i_deque_insert(i_deque_t* _this, deque_iterator_t pos, deque_data_t data)
{
    if (unlikely(!is_null(_this->ops) && !is_null(_this->ops->valid_data) && !_this->ops->valid_data(data)))
        return i_deque_null_iterator();
    return __i_deque_insert_run(_this, pos, 1, data);
}

static JDSC_INLINE
deque_iterator_t __i_deque_insert_n(i_deque_t* _this, deque_iterator_t pos, deque_size_t n, deque_data_t data)
{
    if (unlikely(n < 0))
        return i_deque_null_iterator();

    if (unlikely(0 == n))
        return pos;

    if (unlikely(!is_null(_this->ops) && !is_null(_this->ops->valid_data) && !_this->ops->valid_data(data)))
        return i_deque_null_iterator();
    return __i_deque_insert_run(_this, pos, n, data);
}

JDSC_INLINE /* The fast path involves a fallback and is not implemented yet */
deque_iterator_t i_deque_insert_n(i_deque_t* _this, deque_iterator_t pos, deque_size_t n, deque_data_t data)
{
    if (unlikely(is_null(_this) || i_deque_is_null_iterator(pos)))
        return i_deque_null_iterator();
    return __i_deque_insert_n(_this, pos, n, data);
}

static
deque_iterator_t __i_deque_insert_range(i_deque_t* _this, deque_iterator_t pos, deque_iterator_t iterator_begin, deque_iterator_t iterator_end)
{
    deque_iterator_t ret;
    deque_size_t n, pushed;
    deque_size_t fn = __i_deque_iterator_distance(_this->begin, pos);
    deque_size_t bn = __i_deque_iterator_distance(pos, _this->end);

    if (unlikely(fn < 0 || bn < 0))
        return i_deque_null_iterator();

    n = __i_deque_iterator_distance(iterator_begin, iterator_end);
    if (unlikely(n < 0))
        return i_deque_null_iterator();

    if (unlikely(0 == n))
        return pos;

    if (fn < bn) {
        for (pushed = 0; pushed < n; ++pushed) {
            if (!__i_deque_push_front_null(_this))
                goto err;
        }

        __i_deque_slot_move_n_forward(_this, _this->begin, __i_deque_advance(_this->begin, n), fn);
        ret = __i_deque_advance(_this->begin, fn);
    } else {
        for (pushed = 0; pushed < n; ++pushed) {
            if (!__i_deque_push_back_null(_this))
                goto err;
        }

        ret = __i_deque_advance(_this->begin, fn);
        __i_deque_slot_move_n_backward(_this, __i_deque_advance(ret, n), ret, bn);
    }

    if (!__i_deque_slot_write_n_range(_this, ret, iterator_begin, n))
        goto err_slot;

    return ret;

err_slot:
    if (fn < bn)
        __i_deque_slot_move_n_backward(_this, __i_deque_advance(_this->begin, n), _this->begin, fn);
    else
        __i_deque_slot_move_n_forward(_this, ret, __i_deque_advance(ret, n), bn);

err:
    if (fn < bn) {
        while (pushed-- > 0)
            __i_deque_pop_front_without_free_data(_this);
    } else {
        while (pushed-- > 0)
            __i_deque_pop_back_without_free_data(_this);
    }
    return i_deque_null_iterator();
}

JDSC_INLINE
deque_iterator_t i_deque_insert_range(i_deque_t* _this, deque_iterator_t pos, deque_iterator_t iterator_begin, deque_iterator_t iterator_end)
{
    if (unlikely(is_null(_this) || i_deque_is_null_iterator(pos) || i_deque_is_null_iterator(iterator_begin) || i_deque_is_null_iterator(iterator_end)))
        return i_deque_null_iterator();
    return __i_deque_insert_range(_this, pos, iterator_begin, iterator_end);
}

deque_iterator_t __i_deque_erase(i_deque_t* _this, deque_iterator_t pos)
{
    deque_size_t fn = __i_deque_iterator_distance(_this->begin, pos);
    deque_size_t bn = __i_deque_iterator_distance(pos, _this->end);
    deque_iterator_t pnext;

    if (unlikely(fn < 0 || bn <= 0))
        return i_deque_null_iterator();

    pnext = __i_deque_next(pos);

    if (!is_null(_this->ops) && !is_null(_this->ops->free_data))
        _this->ops->free_data((deque_data_t*)pos.cur);

    if (fn < bn - 1) {
        __i_deque_slot_move_n_backward(_this, __i_deque_next(_this->begin), _this->begin, fn);
        __i_deque_pop_front_without_free_data(_this);
        return pnext;
    } else {
        __i_deque_slot_move_n_forward(_this, pos, pnext, bn - 1);
        __i_deque_pop_back_without_free_data(_this);
        return pos;
    }
}

deque_iterator_t __i_deque_erase_range(i_deque_t* _this, deque_iterator_t iterator_begin, deque_iterator_t iterator_end)
{
    deque_size_t fn = __i_deque_iterator_distance(_this->begin, iterator_begin);
    deque_size_t bn = __i_deque_iterator_distance(iterator_end, _this->end);
    deque_size_t en = __i_deque_iterator_distance(iterator_begin, iterator_end);

    if (unlikely(fn < 0 || bn < 0 || en < 0))
        return i_deque_null_iterator();

    if (unlikely(0 == en))
        return iterator_begin;

    if (!is_null(_this->ops) && !is_null(_this->ops->free_data)) {
        for (deque_iterator_t it = iterator_begin; iterator_end.cur != it.cur; it = __i_deque_next(it))
            _this->ops->free_data((deque_data_t*)it.cur);
    }

    if (fn < bn) {
        deque_iterator_t nbegin = __i_deque_advance(_this->begin, en);
        __i_deque_slot_move_n_backward(_this, nbegin, _this->begin, fn);
        for (deque_size_t i = 0; i < en; ++i)
            __i_deque_pop_front_without_free_data(_this);
        return iterator_end;
    } else {
        __i_deque_slot_move_n_forward(_this, iterator_begin, iterator_end, bn);
        for (deque_size_t i = 0; i < en; ++i)
            __i_deque_pop_back_without_free_data(_this);
        return iterator_begin;
    }
}

static JDSC_INLINE_FORCE
uint8_t* __i_deque_bkt_begin(const i_deque_t* _this, uint8_t** bkt)
{
    return bkt == _this->begin.bkt ? _this->begin.cur : *bkt;
}

static JDSC_INLINE_FORCE
uint8_t* __i_deque_bkt_end(const i_deque_t* _this, uint8_t** bkt)
{
    return bkt == _this->end.bkt ? _this->end.cur
                                 : __i_deque_ptr_add(*bkt, _this->bkt_size, _this->step);
}

static JDSC_INLINE
deque_iterator_t __i_deque_make_iterator(const i_deque_t* _this, uint8_t** bkt, uint8_t* cur)
{
    deque_iterator_t it;
    __i_deque_set_bkt_n(&it, bkt, _this->bkt_size, _this->step);
    it.cur = cur;
    return it;
}

deque_iterator_t i_deque_find(const i_deque_t* _this, deque_data_t data)
{
    uint8_t** bkt;

    if (unlikely(is_null(_this)))
        return i_deque_null_iterator();

    if (unlikely(!is_null(_this->ops) && !is_null(_this->ops->valid_data) && !_this->ops->valid_data(data)))
        return i_deque_null_iterator();

    JDSC_ASSERT(!((_this->dtype & SEQ_CNTR_DTYPE_MASK_STUN) && (is_null(_this->ops) || is_null(_this->ops->__eq))));

    const deque_step_t step = _this->step;

    if (is_null(_this->ops) || is_null(_this->ops->__eq)) {
        for (bkt = _this->begin.bkt; bkt <= _this->end.bkt; ++bkt) {
            uint8_t* i = __i_deque_bkt_begin(_this, bkt);
            uint8_t* l = __i_deque_bkt_end(_this, bkt);

            for (; i < l; i = __i_deque_ptr_add(i, 1, step)) {
                if (__i_deque_slot_equal(step, data, i))
                    return __i_deque_make_iterator(_this, bkt, i);
            }
        }
    } else if (_this->sso) {
        for (bkt = _this->begin.bkt; bkt <= _this->end.bkt; ++bkt) {
            uint8_t* i = __i_deque_bkt_begin(_this, bkt);
            uint8_t* l = __i_deque_bkt_end(_this, bkt);

            for (; i < l; i = __i_deque_ptr_add(i, 1, step)) {
                if (__ds_ops_eq_diff_sso(data, (deque_data_t)i))
                    return __i_deque_make_iterator(_this, bkt, i);
            }
        }
    } else {
        for (bkt = _this->begin.bkt; bkt <= _this->end.bkt; ++bkt) {
            uint8_t* i = __i_deque_bkt_begin(_this, bkt);
            uint8_t* l = __i_deque_bkt_end(_this, bkt);

            for (; i < l; i = __i_deque_ptr_add(i, 1, step)) {
                if (_this->ops->__eq(data, __i_deque_slot_read(_this, i)))
                    return __i_deque_make_iterator(_this, bkt, i);
            }
        }
    }

    return _this->end;
}

deque_count_t i_deque_count(const i_deque_t* _this, deque_data_t data)
{
    deque_count_t ret = 0;
    uint8_t** bkt;

    if (unlikely(is_null(_this)))
        return -1;

    if (unlikely(!is_null(_this->ops) && !is_null(_this->ops->valid_data) && !_this->ops->valid_data(data)))
        return -1;

    JDSC_ASSERT(!((_this->dtype & SEQ_CNTR_DTYPE_MASK_STUN) && (is_null(_this->ops) || is_null(_this->ops->__eq))));

    const deque_step_t step = _this->step;

    if (is_null(_this->ops) || is_null(_this->ops->__eq)) {
        for (bkt = _this->begin.bkt; bkt <= _this->end.bkt; ++bkt) {
            uint8_t* i = __i_deque_bkt_begin(_this, bkt);
            uint8_t* l = __i_deque_bkt_end(_this, bkt);

            for (; i < l; i = __i_deque_ptr_add(i, 1, step)) {
                if (__i_deque_slot_equal(step, data, i))
                    ret++;
            }
        }
    } else if (_this->sso) {
        for (bkt = _this->begin.bkt; bkt <= _this->end.bkt; ++bkt) {
            uint8_t* i = __i_deque_bkt_begin(_this, bkt);
            uint8_t* l = __i_deque_bkt_end(_this, bkt);

            for (; i < l; i = __i_deque_ptr_add(i, 1, step)) {
                if (__ds_ops_eq_diff_sso(data, (deque_data_t)i))
                    ret++;
            }
        }
    } else {
        for (bkt = _this->begin.bkt; bkt <= _this->end.bkt; ++bkt) {
            uint8_t* i = __i_deque_bkt_begin(_this, bkt);
            uint8_t* l = __i_deque_bkt_end(_this, bkt);

            for (; i < l; i = __i_deque_ptr_add(i, 1, step)) {
                if (_this->ops->__eq(data, __i_deque_slot_read(_this, i)))
                    ret++;
            }
        }
    }

    return ret;
}

deque_size_t i_deque_remove(i_deque_t* _this, deque_data_t data)
{
    deque_size_t ret = 0;
    deque_iterator_t l, r, end;

    if (unlikely(is_null(_this)))
        return -1;

    if (unlikely(!is_null(_this->ops) && !is_null(_this->ops->valid_data) && !_this->ops->valid_data(data)))
        return -1;

    JDSC_ASSERT(!((_this->dtype & SEQ_CNTR_DTYPE_MASK_STUN) && (is_null(_this->ops) || is_null(_this->ops->__eq))));

    const deque_step_t step = _this->step;
    l = r = _this->begin;
    end = _this->end;

    if (is_null(_this->ops) || is_null(_this->ops->__eq)) {
        if (is_null(_this->ops) || is_null(_this->ops->free_data)) {
            for (; end.cur != r.cur; r = __i_deque_next(r)) {
                if (!__i_deque_slot_equal(step, data, r.cur)) {
                    __i_deque_slot_move(_this, l.cur, r.cur);
                    l = __i_deque_next(l);
                } else {
                    ret++;
                }
            }
        } else {
            for (; end.cur != r.cur; r = __i_deque_next(r)) {
                if (!__i_deque_slot_equal(step, data, r.cur)) {
                    __i_deque_slot_move(_this, l.cur, r.cur);
                    l = __i_deque_next(l);
                } else {
                    _this->ops->free_data((deque_data_t*)r.cur);
                    ret++;
                }
            }
        }
    } else if (_this->sso) {
        for (; end.cur != r.cur; r = __i_deque_next(r)) {
            if (!__ds_ops_eq_diff_sso(data, (deque_data_t)r.cur)) {
                if (l.cur != r.cur) {
                    memmove(l.cur, r.cur, sizeof(ds_sso_t));
                    __ds_ops_fix_sso((deque_data_t)l.cur); /* __i_deque_slot_move(_this, l.cur, r.cur); */
                }
                l = __i_deque_next(l);
            } else {
                /* if (!is_null(_this->ops) && !is_null(_this->ops->free_data)) */
                    _this->ops->free_data((deque_data_t*)r.cur);
                ret++;
            }
        }
    } else {
        for (; end.cur != r.cur; r = __i_deque_next(r)) {
            if (!_this->ops->__eq(data, __i_deque_slot_read(_this, r.cur))) {
                __i_deque_slot_move(_this, l.cur, r.cur);
                l = __i_deque_next(l);
            } else {
                if (!is_null(_this->ops) && !is_null(_this->ops->free_data))
                    _this->ops->free_data((deque_data_t*)r.cur);
                ret++;
            }
        }
    }

    if (0 != ret) {
        r = l;
        while (end.bkt != r.bkt++)
            __i_deque_free_bkt(_this, r.bkt);

        __i_deque_set_bkt_n(&_this->end, l.bkt, _this->bkt_size, step);
        _this->end.cur = l.cur;
    }

    return ret;
}

deque_size_t i_deque_remove_if(i_deque_t* _this, remove_if_condition cond)
{
    deque_size_t ret = 0;
    deque_iterator_t l, r, end;

    if (unlikely(is_null(_this) || is_null(cond)))
        return -1;

    l = r = _this->begin;
    end = _this->end;

    if (is_null(_this->ops) || is_null(_this->ops->free_data)) {
        for (; end.cur != r.cur; r = __i_deque_next(r)) {
            if (!cond(__i_deque_slot_read(_this, r.cur))) {
                __i_deque_slot_move(_this, l.cur, r.cur);
                l = __i_deque_next(l);
            } else {
                ret++;
            }
        }
    } else if (_this->sso) {
        for (; end.cur != r.cur; r = __i_deque_next(r)) {
            if (!cond(__ds_ops_char_addr_sso((deque_data_t)r.cur))) {
                __i_deque_slot_move(_this, l.cur, r.cur);
                l = __i_deque_next(l);
            } else {
                _this->ops->free_data((deque_data_t*)r.cur);
                ret++;
            }
        }
    } else {
        for (; end.cur != r.cur; r = __i_deque_next(r)) {
            if (!cond(__i_deque_slot_read(_this, r.cur))) {
                __i_deque_slot_move(_this, l.cur, r.cur);
                l = __i_deque_next(l);
            } else {
                _this->ops->free_data((deque_data_t*)r.cur);
                ret++;
            }
        }
    }

    if (0 != ret) {
        r = l;
        while (end.bkt != r.bkt++)
            __i_deque_free_bkt(_this, r.bkt);

        __i_deque_set_bkt_n(&_this->end, l.bkt, _this->bkt_size, _this->step);
        _this->end.cur = l.cur;
    }

    return ret;
}

bool i_deque_assign(i_deque_t* _this, deque_size_t n, deque_data_t data)
{
    deque_iterator_t it;

    if (unlikely(is_null(_this) || n < 0))
        return false;

    if (unlikely(0 == n)) {
        i_deque_clear(_this);
        return true;
    }

    it = __i_deque_insert_n(_this, _this->end, n, data);
    if (i_deque_is_null_iterator(it))
        return false;

    __i_deque_erase_range(_this, _this->begin, it);
    return true;
}

bool i_deque_assign_range(i_deque_t* _this, deque_iterator_t iterator_begin, deque_iterator_t iterator_end)
{
    deque_iterator_t it;

    if (unlikely(is_null(_this) || i_deque_is_null_iterator(iterator_begin) || i_deque_is_null_iterator(iterator_end)))
        return false;

    if (unlikely(iterator_begin.cur == iterator_end.cur)) {
        i_deque_clear(_this);
        return true;
    }

    it = __i_deque_insert_range(_this, _this->end, iterator_begin, iterator_end);
    if (i_deque_is_null_iterator(it))
        return false;

    __i_deque_erase_range(_this, _this->begin, it);
    return true;
}

bool i_deque_resize(i_deque_t* _this, deque_size_t n, deque_data_t default_data)
{
    deque_size_t size;

    if (unlikely(is_null(_this) || n < 0))
        return false;

    size = __i_deque_size(_this);
    if (unlikely(n == size))
        return true;

    if (n < size)
        return !i_deque_is_null_iterator(__i_deque_erase_range(_this, __i_deque_advance(_this->begin, n), _this->end));
    return !i_deque_is_null_iterator(__i_deque_insert_n(_this, _this->end, n - size, default_data));
}

deque_size_t i_deque_clear(i_deque_t* _this)
{
    deque_size_t ret = 0;
    uint8_t** bkt;

    if (unlikely(is_null(_this)))
        return -1;

    if (is_null(_this->ops) || is_null(_this->ops->free_data)) {
        for (bkt = _this->begin.bkt; bkt <= _this->end.bkt; ++bkt) {
            uint8_t* i = __i_deque_bkt_begin(_this, bkt);
            uint8_t* l = __i_deque_bkt_end(_this, bkt);

            ret += __i_deque_ptr_diff(l, i, _this->step);

            if (_this->begin.bkt != bkt)
                __i_deque_free_bkt(_this, bkt);
        }
    } else {
        for (bkt = _this->begin.bkt; bkt <= _this->end.bkt; ++bkt) {
            uint8_t* i = __i_deque_bkt_begin(_this, bkt);
            uint8_t* l = __i_deque_bkt_end(_this, bkt);

            ret += __i_deque_ptr_diff(l, i, _this->step);

            for (; i < l; i = __i_deque_ptr_add(i, 1, _this->step))
                _this->ops->free_data((deque_data_t*)i);

            if (_this->begin.bkt != bkt)
                __i_deque_free_bkt(_this, bkt);
        }
    }

    deque_bcount_t m = _this->bkt_count / 2;
    if (_this->begin.bkt != &_this->bkt[m]) {
        _this->bkt[m] = *_this->begin.bkt;
        *_this->begin.bkt = NULL;
    }

    __i_deque_set_bkt_n(&_this->begin, &_this->bkt[m], _this->bkt_size, _this->step);
    _this->begin.cur = _this->begin.begin;
    _this->end       = _this->begin;
    return ret;
}

static
bool __deque_init(deque_t* deque)
{
    deque_bcount_t m = DEQUE_CAPACITY_INIT / 2;

    deque->bkt_count = DEQUE_CAPACITY_INIT;
    deque->bkt       = (uint8_t**)p_calloc(deque->bkt_count, sizeof(uint8_t*));
    if (is_null(deque->bkt)) {
        JDSC_ASSERT_NEW(0, "%s: Failed to allocate %zu bytes for deque bucket map!",
                        __func__, (size_t)deque->bkt_count * sizeof(uint8_t*));
        return false;
    }

    deque->bkt[m] = __i_deque_alloc_bkt(deque);
    if (is_null(deque->bkt[m])) {
        JDSC_ASSERT_NEW(0, "%s: Failed to allocate %zu bytes for deque bucket!",
                        __func__, (size_t)deque->bkt_size * deque->step);
        goto free_bkt;
    }

    deque->swap = (uint8_t*)p_aligned_malloc(deque->align, deque->step);
    if (is_null(deque->swap)) {
        JDSC_ASSERT_NEW(0, "%s: Failed to allocate %zu bytes for swap!",
                        __func__, deque->step);
        goto free_bkt_m;
    }

    __i_deque_set_bkt_n(&deque->begin, &deque->bkt[m], deque->bkt_size, deque->step);
    deque->begin.cur = deque->begin.begin;
    deque->end       = deque->begin;
    return true;

free_bkt_m:
    __i_deque_free_bkt(deque, &deque->bkt[m]);
free_bkt:
    p_free(deque->bkt);
    return false;
}

static JDSC_INLINE
void __deque_deinit(deque_t* deque)
{
    i_deque_clear(deque);
    p_aligned_free(deque->swap, deque->align);
    __i_deque_free_bkt(deque, deque->begin.bkt);
    p_free(deque->bkt);
}

static JDSC_INLINE
deque_size_t __i_deque_bkt_size(deque_step_t step)
{
    deque_size_t target = (deque_size_t)(_I_DEQUE_BKT_BYTES / step);
    deque_size_t size = 1;

    if (target < _I_DEQUE_BKT_SIZE_MIN)
        target = _I_DEQUE_BKT_SIZE_MIN;

    while ((size << 1) <= target)
        size <<= 1;
    return size;
}

deque_t* __deque_new(const class_deque_ops_t* ops, deque_dtype_t data_type, deque_step_t step, deque_align_t align)
{
    deque_t* deque;
    deque_step_t width;

    if (step <= 0 || align > step || (0 != align && (0 != (align & (align - 1)) || 0 != (step % align)))) {
        JDSC_ASSERT_NEW(0, "%s: Invalid step %zu or align %zu "
                        "(step must be greater than zero, align must be zero or a power of two that does not exceed and divides step!",
                        __func__, step, align);
        return NULL;
    }

    if (!__sequence_container_dtype_valid(data_type)) {
        JDSC_ASSERT_NEW(0, "%s: Invalid data type %zu!", __func__, data_type);
        return NULL;
    }

    width = __sequence_container_dtype_width(data_type);
    if (0 != width && width != step) {
        JDSC_ASSERT_NEW(0, "%s: Data type width %zu does not match step %zu!",
                        __func__, width, step);
        return NULL;
    }

    if ((data_type & SEQ_CNTR_DTYPE_MASK_STUN) && (is_null(ops) || is_null(ops->copy_data))) {
        JDSC_ASSERT_NEW(0, "%s: The `ops->copy_data` is required for struct/union element types!",
                        __func__);
        return NULL;
    }

    if (!is_null(ops) && !is_null(ops->copy_data) && is_null(ops->free_data)) {
        JDSC_ASSERT_NEW(0, "%s: The `ops->free_data` is required when `ops->copy_data` is set!",
                        __func__);
        return NULL;
    }

    if (g_class_deque_ops_sso() == ops && (SEQ_CNTR_DTYPE_SSO != data_type || sizeof(ds_sso_t) != step)) {
        JDSC_ASSERT_NEW(0, "%s: SSO ops require the SSO data type and a step of sizeof(ds_sso_t) (got %zu)!",
                        __func__, step);
        return NULL;
    }

    deque = (deque_t*)p_calloc(1, sizeof(deque_t));
    if (is_null(deque)) {
        JDSC_ASSERT_NEW(0, "%s: Failed to allocate %zu bytes for deque!",
                        __func__, sizeof(deque_t));
        return NULL;
    }

    deque->ops      = ops;
    if (g_class_deque_ops_sso() == deque->ops)
        deque->sso = true;
    deque->dtype    = data_type;
    deque->step     = step;
    deque->align    = align;
    deque->bkt_size = __i_deque_bkt_size(step);
    if (!__deque_init(deque))
        goto err;

    return deque;

err:
    p_free(deque);
    return NULL;
}

void __deque_delete(deque_t** _this)
{
    if (is_null(_this) || is_null(*_this))
        return ;

    __deque_deinit(*_this);
    p_free(*_this);
}

const class_deque_t* class_deque_ins(void)
{
    static const class_deque_t ins = {
        .size             = cdeque_size,
        .max_size         = cdeque_max_size,
        .__empty          = __cdeque_empty,
        .count            = cdeque_count,
        .end              = cdeque_end,
        .begin            = cdeque_begin,
        .next             = cdeque_next,
        .prev             = cdeque_prev,
        .rend             = cdeque_rend,
        .rbegin           = cdeque_rbegin,
        .rnext            = cdeque_rnext,
        .rprev            = cdeque_rprev,
        .__it             = __cdeque_it,
        .it               = cdeque_it,
        .__at             = __cdeque_at,
        .at               = cdeque_at,
        .__back           = __cdeque_back,
        .back             = cdeque_back,
        .__front          = __cdeque_front,
        .front            = cdeque_front,
        .__replace_it     = __cdeque_replace_it,
        .replace_it       = cdeque_replace_it,
        .__replace_index  = __cdeque_replace_index,
        .replace_index    = cdeque_replace_index,
        .find             = cdeque_find,
        .push_back        = cdeque_push_back,
        .push_front       = cdeque_push_front,
        .insert           = cdeque_insert,
        .insert_n         = cdeque_insert_n,
        .insert_range     = cdeque_insert_range,
        .erase            = cdeque_erase,
        .erase_range      = cdeque_erase_range,
        .pop_back         = cdeque_pop_back,
        .pop_front        = cdeque_pop_front,
        .remove           = cdeque_remove,
        .remove_if        = cdeque_remove_if,
        .assign           = cdeque_assign,
        .assign_range     = cdeque_assign_range,
        .resize           = cdeque_resize,
        .shrink_to_fit    = cdeque_shrink_to_fit,
        .swap             = cdeque_swap,
        .clear            = cdeque_clear,
    };
    return &ins;
}
