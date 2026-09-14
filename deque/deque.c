/*
  Deque Implementations
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

#include <deque/deque.h>

#include <string.h>
#include <_memory.h>
#include <_compiler_inter.h>
#include <linux/_compiler.h>

#define DEQUE_CAPACITY_INIT   8
#define _I_DEQUE_BKT_BYTES    512
#define _I_DEQUE_BKT_SIZE_MIN 8

static
bool __i_deque_reserve(i_deque_t* _this)
{
    deque_bcount_t bcnt_o = _this->bkt_count;
    deque_bcount_t bcnt_n = bcnt_o > 0 ? bcnt_o * 2 : DEQUE_CAPACITY_INIT;
    deque_bcount_t off_b  = _this->begin.bkt - _this->bkt;
    deque_bcount_t off_e  = _this->end.bkt   - _this->bkt;
    deque_bcount_t n      = off_e - off_b + 1;
    deque_bcount_t off_n  = (bcnt_n - n) / 2;
    uint8_t**      bkt_n;

    bkt_n = (uint8_t**)p_calloc(bcnt_n, sizeof(uint8_t*));
    if (is_null(bkt_n))
        return false;

    memcpy(bkt_n + off_n, _this->begin.bkt, n * sizeof(uint8_t*));
    p_free(_this->bkt);

    _this->bkt = bkt_n;
    _this->bkt_count = bcnt_n;
    _this->begin.bkt = _this->bkt + off_n;
    _this->end.bkt   = _this->bkt + off_n + n - 1;
    return true;
}

static JDSC_INLINE_FORCE
uint8_t* i_deque_alloc_bkt(const i_deque_t* _this)
{
    return (uint8_t*)p_malloc(_this->bkt_size * _this->step);
}

uint8_t* __i_deque_push_back_bkt_alloc(i_deque_t* _this)
{
    uint8_t** bkt;
    uint8_t*  slot;

    if (_this->end.bkt == _this->bkt + _this->bkt_count - 1) {
        if (!__i_deque_reserve(_this))
            return NULL;
    }

    bkt = _this->end.bkt + 1;
    *bkt = i_deque_alloc_bkt(_this);
    if (is_null(*bkt))
        return NULL;

    slot = _this->end.cur;
    __i_deque_set_bkt_n(&_this->end, bkt, _this->bkt_size, _this->step);
    _this->end.cur = _this->end.begin;
    return slot;
}

uint8_t* __i_deque_push_front_bkt_alloc(i_deque_t* _this)
{
    uint8_t** bkt;

    if (_this->begin.bkt == _this->bkt) {
        if (!__i_deque_reserve(_this))
            return NULL;
    }

    bkt = _this->begin.bkt - 1;
    *bkt = i_deque_alloc_bkt(_this);
    if (is_null(*bkt))
        return NULL;

    __i_deque_set_bkt_n(&_this->begin, bkt, _this->bkt_size, _this->step);
    _this->begin.cur = __i_deque_ptr_sub(_this->begin.end, 1, _this->step);
    return _this->begin.cur;
}

JDSC_INLINE
void __i_deque_pop_back_bkt_free(i_deque_t* _this)
{
    p_free(_this->end.begin);
    __i_deque_set_bkt_n(&_this->end, _this->end.bkt - 1, _this->bkt_size, _this->step);
    _this->end.cur = __i_deque_ptr_sub(_this->end.end, 1, _this->step);
}

JDSC_INLINE
void __i_deque_pop_front_bkt_free(i_deque_t* _this)
{
    p_free(_this->begin.begin);
    __i_deque_set_bkt_n(&_this->begin, _this->begin.bkt + 1, _this->bkt_size, _this->step);
    _this->begin.cur = _this->begin.begin;
}

static JDSC_INLINE_FORCE_POLICY
void __i_deque_slot_move(const i_deque_t* _this, uint8_t* dest, const uint8_t* src)
{
    if (dest != src) {
        switch (_this->step) {
        case 1:  memmove(dest, src, 1);  return ;
        case 2:  memmove(dest, src, 2);  return ;
        case 4:  memmove(dest, src, 4);  return ;
        case 8:  memmove(dest, src, 8);  return ;
        case 16: memmove(dest, src, 16); return ;
        case 32: memmove(dest, src, 32); return ;
        default: memmove(dest, src, _this->step); return ;
        }
    }
}

static JDSC_INLINE
void __i_deque_memmove_forward(const i_deque_t* _this, deque_iterator_t dest, deque_iterator_t src, deque_size_t n)
{
    for (deque_size_t i = 0; i < n; ++i) {
        __i_deque_slot_move(_this, dest.cur, src.cur);
        dest = __i_deque_next(dest);
        src  = __i_deque_next(src);
    }
}

static JDSC_INLINE
void __i_deque_memmove_backward(const i_deque_t* _this, deque_iterator_t dest, deque_iterator_t src, deque_size_t n)
{
    if (unlikely(n <= 0))
        return ;

    dest = __i_deque_advance(dest, n - 1);
    src  = __i_deque_advance(src,  n - 1);
    for (deque_size_t i = 0; i < n; ++i) {
        __i_deque_slot_move(_this, dest.cur, src.cur);
        dest = __i_deque_prev(dest);
        src  = __i_deque_prev(src);
    }
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
    if (__i_deque_empty(_this))
        return ;

    if (_this->end.cur != _this->end.begin)
        _this->end.cur = __i_deque_ptr_sub(_this->end.cur, 1, _this->step);
    else
        __i_deque_pop_back_bkt_free(_this);
}

static JDSC_INLINE
void __i_deque_pop_front_without_free_data(i_deque_t* _this)
{
    if (__i_deque_empty(_this))
        return ;

    if (_this->begin.cur != __i_deque_ptr_sub(_this->begin.end, 1, _this->step))
        _this->begin.cur = __i_deque_ptr_add(_this->begin.cur, 1, _this->step);
    else
        __i_deque_pop_front_bkt_free(_this);
}

static JDSC_INLINE_FORCE_POLICY
bool __i_deque_slot_write_n(const i_deque_t* _this, deque_iterator_t pos, deque_size_t n, deque_data_t data)
{
    deque_size_t succ = 0;

    if (!is_null(_this->ops) && !is_null(_this->ops->copy_data)) {
        while (n-- > 0) {
            if (_this->ops->copy_data(data, (deque_data_t*)pos.cur))
                succ++;
            else
                goto err;
            pos = __i_deque_next(pos);
        }

        return true;
    } else {
        if (_this->step > sizeof(deque_data_t))
            return false;

        while (n-- > 0) {
            switch (_this->step)
            {
            case 1: *(uint8_t*)pos.cur  = (uint8_t)data;  break;
            case 2: *(uint16_t*)pos.cur = (uint16_t)data; break;
            case 4: *(uint32_t*)pos.cur = (uint32_t)data; break;
            case 8: *(uint64_t*)pos.cur = (uint64_t)data; break;
            default: break;
            }

            succ++;
            pos = __i_deque_next(pos);
        }

        return true;
    }

err:
    if (!is_null(_this->ops) && !is_null(_this->ops->free_data)) {
        for (deque_size_t i = 0; i < succ; ++i) {
            pos = __i_deque_prev(pos);
            _this->ops->free_data((deque_data_t*)pos.cur);
        }
    }
    return false;
}

static
deque_iterator_t __i_deque_insert_run(i_deque_t* _this, deque_iterator_t pos, deque_size_t n, deque_data_t data)
{
    deque_size_t i;
    deque_iterator_t ret;

    deque_size_t fn = __i_deque_iterator_distance(_this->begin, pos);
    deque_size_t bn = __i_deque_iterator_distance(pos, _this->end);
    if (unlikely(fn < 0 || bn < 0))
        return i_deque_null_iterator();

    if (fn < bn) {
        for (i = 0; i < n; ++i) {
            if (!__i_deque_push_front_null(_this))
                goto err;
        }

        __i_deque_memmove_forward(_this, _this->begin, __i_deque_advance(_this->begin, n), fn);
        ret = __i_deque_advance(_this->begin, fn);
        if (!__i_deque_slot_write_n(_this, ret, n, data))
            goto err_slot;
        return ret;
    } else {
        for (i = 0; i < n; ++i) {
            if (!__i_deque_push_back_null(_this))
                goto err;
        }

        ret = __i_deque_advance(_this->begin, fn);
        __i_deque_memmove_backward(_this, __i_deque_advance(ret, n), ret, bn);
        if (!__i_deque_slot_write_n(_this, ret, n, data))
            goto err_slot;
        return ret;
    }

err_slot:
    if (fn < bn) {
        __i_deque_memmove_backward(_this, __i_deque_advance(_this->begin, n), _this->begin, fn);
    } else {
        __i_deque_memmove_forward(_this, ret, __i_deque_advance(ret, n), bn);
    }

err:
    if (fn < bn) {
        for (; i > 0; --i)
            __i_deque_pop_front_without_free_data(_this);
    } else {
        for (; i > 0; --i)
            __i_deque_pop_back_without_free_data(_this);
    }
    return i_deque_null_iterator();
}

deque_iterator_t __i_deque_insert(i_deque_t* _this, deque_iterator_t pos, deque_data_t data)
{
    if (unlikely(!is_null(_this->ops) && !is_null(_this->ops->valid_data) && !_this->ops->valid_data(data)))
        return i_deque_null_iterator();
    return __i_deque_insert_run(_this, pos, 1, data);
}

deque_iterator_t i_deque_insert_n(i_deque_t* _this, deque_iterator_t pos, deque_size_t n, deque_data_t data)
{
    if (unlikely(is_null(_this) || i_deque_is_null_iterator(pos) || n < 0))
        return i_deque_null_iterator();

    if (unlikely(0 == n))
        return pos;

    if (unlikely(!is_null(_this->ops) && !is_null(_this->ops->valid_data) && !_this->ops->valid_data(data)))
        return i_deque_null_iterator();
    return __i_deque_insert_run(_this, pos, n, data);
}

deque_iterator_t __i_deque_erase(i_deque_t* _this, deque_iterator_t pos)
{
    deque_size_t fn = __i_deque_iterator_distance(_this->begin, pos);
    deque_size_t bn = __i_deque_iterator_distance(pos, _this->end);
    if (unlikely(fn < 0 || bn <= 0))
        return i_deque_null_iterator();

    deque_iterator_t pnext = __i_deque_next(pos);

    if (!is_null(_this->ops) && !is_null(_this->ops->free_data))
        _this->ops->free_data((deque_data_t*)pos.cur);

    if (fn < bn - 1) {
        __i_deque_memmove_backward(_this, __i_deque_next(_this->begin), _this->begin, fn);
        __i_deque_pop_front_without_free_data(_this);
        return pnext;
    } else {
        __i_deque_memmove_forward(_this, pos, pnext, bn - 1);
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
        __i_deque_memmove_backward(_this, nbegin, _this->begin, fn);
        for (deque_size_t i = 0; i < en; ++i)
            __i_deque_pop_front_without_free_data(_this);
        return iterator_end;
    } else {
        __i_deque_memmove_forward(_this, iterator_begin, iterator_end, bn);
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

static JDSC_INLINE_FORCE
deque_iterator_t __i_deque_make_iterator(const i_deque_t* _this, uint8_t** bkt, uint8_t* cur)
{
    deque_iterator_t it;
    __i_deque_set_bkt_n(&it, bkt, _this->bkt_size, _this->step);
    it.cur = cur;
    return it;
}

static JDSC_INLINE_FORCE_POLICY
deque_data_t __i_deque_slot_read(const i_deque_t* _this, const uint8_t* slot)
{
    deque_data_t data = 0;

    if (_this->step > sizeof(deque_data_t))
        return (deque_data_t)(uintptr_t)slot;

    switch (_this->step)
    {
    case 1: data = *(const uint8_t*)slot;  break;
    case 2: data = *(const uint16_t*)slot; break;
    case 4: data = *(const uint32_t*)slot; break;
    case 8: data = *(const uint64_t*)slot; break;
    default: break;
    }
    return data;
}

deque_iterator_t i_deque_find(const i_deque_t* _this, deque_data_t data)
{
    uint8_t** bkt;

    if (unlikely(is_null(_this)))
        return i_deque_null_iterator();

    if (unlikely(!is_null(_this->ops) && !is_null(_this->ops->valid_data) && !_this->ops->valid_data(data)))
        return i_deque_null_iterator();

    if (is_null(_this->ops) || is_null(_this->ops->__eq)) {
        for (bkt = _this->begin.bkt; bkt <= _this->end.bkt; ++bkt) {
            uint8_t* i = __i_deque_bkt_begin(_this, bkt);
            uint8_t* l = __i_deque_bkt_end(_this, bkt);

            for ( ; i < l; i = __i_deque_ptr_add(i, 1, _this->step)) {
                if (__i_deque_slot_read(_this, i) == data)
                    return __i_deque_make_iterator(_this, bkt, i);
            }
        }
    } else {
        for (bkt = _this->begin.bkt; bkt <= _this->end.bkt; ++bkt) {
            uint8_t* i = __i_deque_bkt_begin(_this, bkt);
            uint8_t* l = __i_deque_bkt_end(_this, bkt);

            for ( ; i < l; i = __i_deque_ptr_add(i, 1, _this->step)) {
                if (_this->ops->__eq(__i_deque_slot_read(_this, i), data))
                    return __i_deque_make_iterator(_this, bkt, i);
            }
        }
    }
    return _this->end;
}

deque_count_t i_deque_count(const i_deque_t* _this, deque_data_t data)
{
    uint8_t** bkt;
    deque_count_t ret = 0;

    if (unlikely(is_null(_this)))
        return -1;

    if (unlikely(!is_null(_this->ops) && !is_null(_this->ops->valid_data) && !_this->ops->valid_data(data)))
        return -1;

    if (is_null(_this->ops) || is_null(_this->ops->__eq)) {
        for (bkt = _this->begin.bkt; bkt <= _this->end.bkt; ++bkt) {
            uint8_t* i = __i_deque_bkt_begin(_this, bkt);
            uint8_t* l = __i_deque_bkt_end(_this, bkt);

            for ( ; i < l; i = __i_deque_ptr_add(i, 1, _this->step)) {
                if (__i_deque_slot_read(_this, i) == data)
                    ret++;
            }
        }
    } else {
        for (bkt = _this->begin.bkt; bkt <= _this->end.bkt; ++bkt) {
            uint8_t* i = __i_deque_bkt_begin(_this, bkt);
            uint8_t* l = __i_deque_bkt_end(_this, bkt);

            for ( ; i < l; i = __i_deque_ptr_add(i, 1, _this->step)) {
                if (_this->ops->__eq(__i_deque_slot_read(_this, i), data))
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

    l = r = _this->begin;
    end = _this->end;

    if (is_null(_this->ops) || is_null(_this->ops->__eq)) {
        for ( ; end.cur != r.cur; r = __i_deque_next(r)) {
            if (data != __i_deque_slot_read(_this, r.cur)) {
                __i_deque_slot_move(_this, l.cur, r.cur);
                l = __i_deque_next(l);
            } else {
                if (!is_null(_this->ops) && !is_null(_this->ops->free_data))
                    _this->ops->free_data((deque_data_t*)r.cur);
                ret++;
            }
        }
    } else {
        for ( ; end.cur != r.cur; r = __i_deque_next(r)) {
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
            p_free(*r.bkt);

        __i_deque_set_bkt_n(&_this->end, l.bkt, _this->bkt_size, _this->step);
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
        for ( ; end.cur != r.cur; r = __i_deque_next(r)) {
            if (!cond(__i_deque_slot_read(_this, r.cur))) {
                __i_deque_slot_move(_this, l.cur, r.cur);
                l = __i_deque_next(l);
            } else {
                ret++;    
            }
        }
    } else {
        for ( ; end.cur != r.cur; r = __i_deque_next(r)) {
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
            p_free(*r.bkt);

        __i_deque_set_bkt_n(&_this->end, l.bkt, _this->bkt_size, _this->step);
        _this->end.cur = l.cur;
    }

    return ret;
}

bool i_deque_resize(i_deque_t* _this, deque_size_t n, deque_data_t default_data)
{
    deque_size_t size;

    if (unlikely(is_null(_this) || n < 0))
        return false;

    size = i_deque_size(_this);
    if (n == size)
        return true;

    if (n < size)
        return !i_deque_is_null_iterator(i_deque_erase_range(_this, __i_deque_advance(_this->begin, n), _this->end));
    return !i_deque_is_null_iterator(i_deque_insert_n(_this, _this->end, n - size, default_data));
}

deque_size_t i_deque_clear(i_deque_t* _this)
{
    uint8_t** bkt;
    deque_count_t ret = 0;

    if (unlikely(is_null(_this)))
        return -1;

    if (!is_null(_this->ops) && !is_null(_this->ops->free_data)) {
        for (bkt = _this->begin.bkt; bkt <= _this->end.bkt; ++bkt) {
            uint8_t* i = __i_deque_bkt_begin(_this, bkt);
            uint8_t* l = __i_deque_bkt_end(_this, bkt);

            for ( ; i < l; i = __i_deque_ptr_add(i, 1, _this->step))
                _this->ops->free_data((deque_data_t*)i);

            ret += __i_deque_ptr_diff(l, i, _this->step);

            if (_this->begin.bkt != bkt)
                p_free(*bkt);
        }
    } else {
        for (bkt = _this->begin.bkt; bkt <= _this->end.bkt; ++bkt) {
            uint8_t* i = __i_deque_bkt_begin(_this, bkt);
            uint8_t* l = __i_deque_bkt_end(_this, bkt);

            ret += __i_deque_ptr_diff(l, i, _this->step);

            if (_this->begin.bkt != bkt)
                p_free(*bkt);
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

    deque->bkt_count_init = DEQUE_CAPACITY_INIT;
    deque->bkt_count      = deque->bkt_count_init;
    deque->bkt            = (uint8_t**)p_calloc(deque->bkt_count, sizeof(uint8_t*));
    if (is_null(deque->bkt))
        return false;

    deque->bkt[m] = i_deque_alloc_bkt(deque);
    if (is_null(deque->bkt[m]))
        goto err;

    __i_deque_set_bkt_n(&deque->begin, &deque->bkt[m], deque->bkt_size, deque->step);
    deque->begin.cur = deque->begin.begin;
    deque->end       = deque->begin;
    return true;

err:
    p_free(deque->bkt);
    return false;
}

static
void __deque_deinit(deque_t* deque)
{
    i_deque_clear(deque);
    p_free(*deque->begin.bkt);
    p_free(deque->bkt);
}

deque_t* __deque_new(const class_deque_ops_t* ops, deque_step_t step)
{
    deque_t* deque;

    if (0 == step)
        return NULL;

    deque = (deque_t*)p_calloc(1, sizeof(deque_t));
    if (is_null(deque))
        return NULL;

    deque->step     = step;
    deque->bkt_size = step < (_I_DEQUE_BKT_BYTES / _I_DEQUE_BKT_SIZE_MIN)
                        ? _I_DEQUE_BKT_BYTES / step
                        : _I_DEQUE_BKT_SIZE_MIN;

    if (!__deque_init(deque))
        goto err;

    deque->ops = ops;
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
        .size        = cdeque_size,
        .count       = cdeque_count,
        .end         = cdeque_end,
        .begin       = cdeque_begin,
        .next        = cdeque_next,
        .prev        = cdeque_prev,
        .rend        = cdeque_rend,
        .rbegin      = cdeque_rbegin,
        .rnext       = cdeque_rnext,
        .rprev       = cdeque_rprev,
        .find        = cdeque_find,
        .push_back   = cdeque_push_back,
        .push_front  = cdeque_push_front,
        .insert      = cdeque_insert,
        .insert_n    = cdeque_insert_n,
        .erase       = cdeque_erase,
        .erase_range = cdeque_erase_range,
        .pop_back    = cdeque_pop_back,
        .pop_front   = cdeque_pop_front,
        .remove      = cdeque_remove,
        .remove_if   = cdeque_remove_if,
        .resize      = cdeque_resize,
        .clear       = cdeque_clear,
    };
    return &ins;
}
