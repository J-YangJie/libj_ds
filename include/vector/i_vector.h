/*
  I-Vector Interfaces
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

#ifndef __J_I_VECTOR_H
#define __J_I_VECTOR_H

#include <stddef.h>
#include <_compiler.h>
#include <vector/vector_ops.h>

#ifndef is_null
#define is_null(X) (!(X))
#endif /* is_null */

typedef struct i_vector {
    const class_vector_ops_t* ops;
    vector_step_t step;
    vector_size_t capacity_init;
    uint8_t*      rend;
    uint8_t*      begin;
    uint8_t*      end;
    uint8_t*      end_of_storage;
    bool          sso;
} i_vector_t;

/* checked */
static inline
bool __i_vector_empty(const i_vector_t* _this)
{
    return _this->end == _this->begin;
}

/* checked */
static inline
uint8_t* ___i_vector_ptr_add(const uint8_t* p, ptrdiff_t n, vector_step_t step)
{
    return (uint8_t*)p + (ptrdiff_t)step * n;
}

/* checked */
static inline
uint8_t* ___i_vector_ptr_sub(const uint8_t* p, ptrdiff_t n, vector_step_t step)
{
    return ___i_vector_ptr_add(p, -n, step);
}

/* checked */
static inline
ptrdiff_t ___i_vector_ptr_diff(const uint8_t* p1, const uint8_t* p2, vector_step_t step)
{
    return (ptrdiff_t)((p1 - p2) / (ptrdiff_t)step);
}

#define __i_vector_ptr_add(_p, _n, _step)    ___i_vector_ptr_add((_p), (ptrdiff_t)(_n), (_step))
#define __i_vector_ptr_sub(_p, _n, _step)    ___i_vector_ptr_sub((_p), (ptrdiff_t)(_n), (_step))
#define __i_vector_ptr_diff(_p1, _p2, _step) ___i_vector_ptr_diff((_p1), (_p2), (_step))

/* checked */
static inline
bool i_vector_is_null_iterator(vector_iterator_t it)
{
    return is_null(it.cur);
}

/* checked */
static inline
vector_iterator_t i_vector_null_iterator(void)
{
    vector_iterator_t it;
    it.cur = NULL;
    return it;
}

/* checked */
static inline
vector_r_iterator_t i_vector_null_r_iterator(void)
{
    vector_r_iterator_t rit;
    rit.cur = NULL;
    return rit;
}

/* checked */
static inline
vector_iterator_t __i_vector_make_iterator(vector_step_t step, uint8_t* cur)
{
    vector_iterator_t it;
    it.cur  = cur;
    it.step = step;
    return it;
}

/* checked */
static inline
vector_r_iterator_t __i_vector_make_r_iterator(vector_step_t step, uint8_t* cur)
{
    vector_r_iterator_t rit;
    rit.cur  = cur;
    rit.step = step;
    return rit;
}





/* checked */
static inline
vector_size_t __i_vector_size(const i_vector_t* _this)
{
    return (vector_size_t)__i_vector_ptr_diff(_this->end, _this->begin, _this->step);
}

/* checked */
static inline
vector_size_t i_vector_size(const i_vector_t* _this)
{
    return is_null(_this) ? -1 : __i_vector_size(_this);
}

/* checked */
static inline
vector_size_t __i_vector_capacity(const i_vector_t* _this)
{
    return (vector_size_t)__i_vector_ptr_diff(_this->end_of_storage, _this->begin, _this->step);
}

/* checked */
static inline
vector_size_t i_vector_capacity(const i_vector_t* _this)
{
    return is_null(_this) ? -1 : __i_vector_capacity(_this);
}

/* checked */
static inline
vector_iterator_t __i_vector_end(const i_vector_t* _this)
{
    return __i_vector_make_iterator(_this->step, _this->end);
}

/* checked */
static inline
vector_iterator_t i_vector_end(const i_vector_t* _this)
{
    JDSC_ASSERT(!is_null(_this));
#if JDSC_ITERATOR_ERR_NULL
    if (is_null(_this))
        return i_vector_null_iterator();
#endif /* JDSC_ITERATOR_ERR_NULL */
    return __i_vector_end(_this);
}

/* checked */
static inline
vector_iterator_t __i_vector_begin(const i_vector_t* _this)
{
    return __i_vector_make_iterator(_this->step, _this->begin);
}

/* checked */
static inline
vector_iterator_t i_vector_begin(const i_vector_t* _this)
{
    JDSC_ASSERT(!is_null(_this));
#if JDSC_ITERATOR_ERR_NULL
    if (is_null(_this))
        return i_vector_null_iterator();
#endif /* JDSC_ITERATOR_ERR_NULL */
    return __i_vector_begin(_this);
}

/* checked */
static inline
vector_iterator_t __i_vector_next(vector_iterator_t it)
{
    JDSC_ASSERT(!is_null(it.cur));
#if JDSC_ITERATOR_ERR_NULL
    if (is_null(it.cur))
        return i_vector_null_iterator();
#endif /* JDSC_ITERATOR_ERR_NULL */

    it.cur = __i_vector_ptr_add(it.cur, 1, it.step);
    return it;
}

/* checked */
static inline
vector_iterator_t i_vector_next(vector_iterator_t it)
{
    return __i_vector_next(it);
}

/* checked */
static inline
vector_iterator_t __i_vector_prev(vector_iterator_t it)
{
    JDSC_ASSERT(!is_null(it.cur));
#if JDSC_ITERATOR_ERR_NULL
    if (is_null(it.cur))
        return i_vector_null_iterator();
#endif /* JDSC_ITERATOR_ERR_NULL */

    it.cur = __i_vector_ptr_sub(it.cur, 1, it.step);
    return it;
}

/* checked */
static inline
vector_iterator_t i_vector_prev(vector_iterator_t it)
{
    return __i_vector_prev(it);
}

/* checked */
static inline
vector_r_iterator_t __i_vector_rend(const i_vector_t* _this)
{
    return __i_vector_make_r_iterator(_this->step, _this->rend);
}

/* checked */
static inline
vector_r_iterator_t i_vector_rend(const i_vector_t* _this)
{
    JDSC_ASSERT(!is_null(_this));
#if JDSC_ITERATOR_ERR_NULL
    if (is_null(_this))
        return i_vector_null_r_iterator();
#endif /* JDSC_ITERATOR_ERR_NULL */
    return __i_vector_rend(_this);
}

/* checked */
static inline
vector_r_iterator_t __i_vector_rbegin(const i_vector_t* _this)
{
    return __i_vector_make_r_iterator(_this->step, 
                __i_vector_ptr_sub(_this->end, 1, _this->step));
}

/* checked */
static inline
vector_r_iterator_t i_vector_rbegin(const i_vector_t* _this)
{
    JDSC_ASSERT(!is_null(_this));
#if JDSC_ITERATOR_ERR_NULL
    if (is_null(_this))
        return i_vector_null_r_iterator();
#endif /* JDSC_ITERATOR_ERR_NULL */
    return __i_vector_rbegin(_this);
}

/* checked */
static inline
vector_r_iterator_t __i_vector_rnext(vector_r_iterator_t rit)
{
    JDSC_ASSERT(!is_null(rit.cur));
#if JDSC_ITERATOR_ERR_NULL
    if (is_null(rit.cur))
        return i_vector_null_r_iterator();
#endif /* JDSC_ITERATOR_ERR_NULL */

    rit.cur = __i_vector_ptr_sub(rit.cur, 1, rit.step);
    return rit;
}

/* checked */
static inline
vector_r_iterator_t i_vector_rnext(vector_r_iterator_t rit)
{
    return __i_vector_rnext(rit);
}

/* checked */
static inline
vector_r_iterator_t __i_vector_rprev(vector_r_iterator_t rit)
{
    JDSC_ASSERT(!is_null(rit.cur));
#if JDSC_ITERATOR_ERR_NULL
    if (is_null(rit.cur))
        return i_vector_null_r_iterator();
#endif /* JDSC_ITERATOR_ERR_NULL */

    rit.cur = __i_vector_ptr_add(rit.cur, 1, rit.step);
    return rit;
}

/* checked */
static inline
vector_r_iterator_t i_vector_rprev(vector_r_iterator_t rit)
{
    return __i_vector_rprev(rit);
}





/* checked */
static inline
vector_iterator_t __i_vector_iterator(const i_vector_t* _this, vector_size_t n)
{
    return __i_vector_make_iterator(_this->step, __i_vector_ptr_add(_this->begin, n, _this->step));
}

/* checked */
static inline
vector_iterator_t i_vector_iterator(const i_vector_t* _this, vector_size_t n)
{
    if (!is_null(_this) && n >= 0 && n < __i_vector_size(_this))
        return __i_vector_iterator(_this, n);
    return i_vector_null_iterator();
}

/* checked */
static inline
vector_value_t __i_vector_at(const i_vector_t* _this, vector_size_t n)
{
    return (vector_value_t){ .u8 = __i_vector_iterator(_this, n).cur, };
}

/* checked */
static inline
vector_value_t i_vector_at(const i_vector_t* _this, vector_size_t n)
{
    if (!is_null(_this) && n >= 0 && n < __i_vector_size(_this))
        return __i_vector_at(_this, n);
    return (vector_value_t){ .u8 = NULL, };
}

/* checked */
static inline
vector_value_t i_vector_back(const i_vector_t* _this)
{
    if (!is_null(_this) && !__i_vector_empty(_this))
        return (vector_value_t){ .u8 = __i_vector_ptr_sub(_this->end, 1, _this->step), };
    return (vector_value_t){ .u8 = NULL, };
}

/* checked */
static inline
vector_value_t i_vector_front(const i_vector_t* _this)
{
    if (!is_null(_this) && !__i_vector_empty(_this))
        return (vector_value_t){ .u8 = _this->begin, };
    return (vector_value_t){ .u8 = NULL, };
}





/* checked */
static inline
bool __i_vector_slot_write(const i_vector_t* _this, vector_data_t data, uint8_t* slot)
{
    if (!is_null(_this->ops) && !is_null(_this->ops->copy_data))
        return _this->ops->copy_data(data, (vector_data_t*)slot);

    if (_this->step <= sizeof(vector_data_t)) {
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

/* checked */
static inline
vector_data_t __i_vector_slot_read(const i_vector_t* _this, const uint8_t* slot)
{
    vector_data_t data = 0;

    if (_this->step > sizeof(vector_data_t))
        return (vector_data_t)(uintptr_t)slot;

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

/* checked */
static inline
void __i_vector_slot_move(const i_vector_t* _this, uint8_t* dest, const uint8_t* src)
{
    if (dest != src) {
        switch (_this->step) {
        case 1:  memmove(dest, src, 1);  return ;
        case 2:  memmove(dest, src, 2);  return ;
        case 4:  memmove(dest, src, 4);  return ;
        case 8:  memmove(dest, src, 8);  return ;
        case 16: memmove(dest, src, 16); return ;
        case 24: memmove(dest, src, 24); if (_this->sso) __ds_ops_fix_sso((ds_data_t)dest); return ;
        case 32: memmove(dest, src, 32); if (_this->sso) __ds_ops_fix_sso((ds_data_t)dest); return ;
        default: memmove(dest, src, _this->step); return ;
        }
    }
}

/* checked */
vector_iterator_t __i_vector_insert_run(i_vector_t* _this, uint8_t* pos, vector_size_t n, vector_data_t data);
static inline
bool i_vector_push_back(i_vector_t* _this, vector_data_t data)
{
    uint8_t* slot;

    if (is_null(_this))
        return false;

    if (!is_null(_this->ops) && !is_null(_this->ops->valid_data) && !_this->ops->valid_data(data))
        return false;

    if (_this->end_of_storage != _this->end) {
        slot = _this->end;
        if (!__i_vector_slot_write(_this, data, slot))
            return false;
        _this->end = __i_vector_ptr_add(_this->end, 1, _this->step);
        return true;
    }

    return !i_vector_is_null_iterator(__i_vector_insert_run(_this, _this->end, 1, data));
}

/* checked */
static inline
void i_vector_pop_back(i_vector_t* _this)
{
    if (is_null(_this) || __i_vector_empty(_this))
        return ;

    _this->end = __i_vector_ptr_sub(_this->end, 1, _this->step);

    if (!is_null(_this->ops) && !is_null(_this->ops->free_data))
        _this->ops->free_data((vector_data_t*)_this->end);
}

/* checked */
vector_iterator_t __i_vector_insert(i_vector_t* _this, vector_iterator_t pos, vector_data_t data);
static inline
vector_iterator_t i_vector_insert(i_vector_t* _this, vector_iterator_t pos, vector_data_t data)
{
    if (is_null(_this) || i_vector_is_null_iterator(pos))
        return i_vector_null_iterator();

    if (_this->end == pos.cur) {
        if (i_vector_push_back(_this, data))
            return __i_vector_make_iterator(_this->step, 
                                    __i_vector_ptr_sub(_this->end, 1, _this->step));
        return i_vector_null_iterator();
    }
    return __i_vector_insert(_this, pos, data);
}
vector_iterator_t i_vector_insert_n(i_vector_t* _this, vector_iterator_t pos, vector_size_t n, vector_data_t data);

/* checked */
vector_iterator_t __i_vector_erase(i_vector_t* _this, uint8_t* pos);
static inline
vector_iterator_t i_vector_erase(i_vector_t* _this, vector_iterator_t pos)
{
    if (is_null(_this) || i_vector_is_null_iterator(pos) 
        || _this->end == pos.cur || __i_vector_empty(_this))
        return i_vector_null_iterator();

    if (__i_vector_ptr_sub(_this->end, 1, _this->step) == pos.cur) {
        i_vector_pop_back(_this);
        return __i_vector_end(_this);
    }
    return __i_vector_erase(_this, pos.cur);
}

/* checked */
vector_iterator_t __i_vector_erase_range(i_vector_t* _this, uint8_t* begin, uint8_t* end);
static inline
vector_iterator_t i_vector_erase_range(i_vector_t* _this, vector_iterator_t iterator_begin, vector_iterator_t iterator_end)
{
    if (is_null(_this)
        || i_vector_is_null_iterator(iterator_begin)
        || i_vector_is_null_iterator(iterator_end))
        return i_vector_null_iterator();

    if (_this->end == iterator_end.cur) {
        while (iterator_begin.cur != _this->end)
            i_vector_pop_back(_this);
        return __i_vector_end(_this);
    }
    return __i_vector_erase_range(_this, iterator_begin.cur, iterator_end.cur);
}

/* checked */
static inline
vector_iterator_t i_vector_find(const i_vector_t* _this, vector_data_t data)
{
    uint8_t* p;

    if (is_null(_this))
        return i_vector_null_iterator();

    if (!is_null(_this->ops) && !is_null(_this->ops->valid_data) && !_this->ops->valid_data(data))
        return i_vector_null_iterator();

    if (is_null(_this->ops) || is_null(_this->ops->__eq)) {
        for (p = _this->begin; _this->end != p; p = __i_vector_ptr_add(p, 1, _this->step)) {
            if (data == __i_vector_slot_read(_this, p))
                return __i_vector_make_iterator(_this->step, p);
        }
    } else {
        for (p = _this->begin; _this->end != p; p = __i_vector_ptr_add(p, 1, _this->step)) {
            if (_this->ops->__eq(data, __i_vector_slot_read(_this, p)))
                return __i_vector_make_iterator(_this->step, p);
        }
    }
    return __i_vector_make_iterator(_this->step, _this->end);
}

/* checked */
static inline
vector_count_t i_vector_count(const i_vector_t* _this, vector_data_t data)
{
    uint8_t* p;
    vector_count_t ret = 0;

    if (is_null(_this))
        return -1;

    if (!is_null(_this->ops) && !is_null(_this->ops->valid_data) && !_this->ops->valid_data(data))
        return -1;

    if (is_null(_this->ops) || is_null(_this->ops->__eq)) {
        for (p = _this->begin; _this->end != p; p = __i_vector_ptr_add(p, 1, _this->step)) {
            if (data == __i_vector_slot_read(_this, p))
                ret++;
        }
    } else {
        for (p = _this->begin; _this->end != p; p = __i_vector_ptr_add(p, 1, _this->step)) {
            if (_this->ops->__eq(data, __i_vector_slot_read(_this, p)))
                ret++;
        }
    }
    return ret;
}

/* checked */
static inline
vector_size_t i_vector_remove(i_vector_t* _this, vector_data_t data)
{
    vector_size_t ret = 0;
    uint8_t* l;
    uint8_t* r;
    uint8_t* end;

    if (is_null(_this) 
        || (!is_null(_this->ops) 
            && !is_null(_this->ops->valid_data) 
            && !_this->ops->valid_data(data)))
        return -1;

    const vector_step_t step = _this->step;
    l = r = _this->begin;
    end = _this->end;

    if (is_null(_this->ops) || is_null(_this->ops->__eq)) {
        for ( ; end != r; r = __i_vector_ptr_add(r, 1, step)) {
            if (data != __i_vector_slot_read(_this, r)) {
                __i_vector_slot_move(_this, l, r);
                l = __i_vector_ptr_add(l, 1, step);
            } else {
                if (!is_null(_this->ops) && !is_null(_this->ops->free_data))
                    _this->ops->free_data((vector_data_t*)r);
                ret++;
            }
        }
    } else {
        for ( ; end != r; r = __i_vector_ptr_add(r, 1, step)) {
            if (!_this->ops->__eq(data, __i_vector_slot_read(_this, r))) {
                __i_vector_slot_move(_this, l, r);
                l = __i_vector_ptr_add(l, 1, step);
            } else {
                if (!is_null(_this->ops) && !is_null(_this->ops->free_data))
                    _this->ops->free_data((vector_data_t*)r);
                ret++;
            }
        }
    }

    _this->end = l;
    return ret;
}

/* checked */
static inline
vector_size_t i_vector_remove_if(i_vector_t* _this, remove_if_condition cond)
{
    vector_size_t ret = 0;
    uint8_t* l;
    uint8_t* r;
    uint8_t* end;

    if (is_null(_this) || is_null(cond))
        return -1;

    const vector_step_t step = _this->step;
    l = r = _this->begin;
    end = _this->end;

    if (is_null(_this->ops) || is_null(_this->ops->free_data)) {
        for ( ; end != r; r = __i_vector_ptr_add(r, 1, step)) {
            if (!cond(__i_vector_slot_read(_this, r))) {
                __i_vector_slot_move(_this, l, r);
                l = __i_vector_ptr_add(l, 1, _this->step);
            } else {
                ret++;
            }
        }
    } else {
        for ( ; end != r; r = __i_vector_ptr_add(r, 1, step)) {
            if (!cond(__i_vector_slot_read(_this, r))) {
                __i_vector_slot_move(_this, l, r);
                l = __i_vector_ptr_add(l, 1, _this->step);
            } else {
                _this->ops->free_data((vector_data_t*)r);
                ret++;
            }
        }
    }

    _this->end = l;
    return ret;
}

/* checked */
static inline
bool i_vector_resize(i_vector_t* _this, vector_size_t n, vector_data_t default_data)
{
    vector_size_t size;

    if (is_null(_this) || n < 0)
        return false;

    size = __i_vector_size(_this);
    if (n == size)
        return true;

    if (n < size)
        return !i_vector_is_null_iterator(i_vector_erase_range(_this, __i_vector_iterator(_this, n), __i_vector_end(_this)));
    return !i_vector_is_null_iterator(i_vector_insert_n(_this, __i_vector_end(_this), n - size, default_data));
}

/* checked */
bool __i_vector_reserve(i_vector_t* _this, vector_size_t n);
static inline
bool i_vector_reserve(i_vector_t* _this, vector_size_t n)
{
    if (!is_null(_this))
        return __i_vector_reserve(_this, n);
    return false;
}

/* checked */
void __i_vector_shrink_to_fit(i_vector_t* _this);
static inline
void i_vector_shrink_to_fit(i_vector_t* _this)
{
    if (!is_null(_this))
        return __i_vector_shrink_to_fit(_this);
}

/* checked */
static inline
vector_size_t i_vector_clear(i_vector_t* _this)
{
    vector_size_t ret;

    if (is_null(_this))
        return -1;

    ret = __i_vector_size(_this);
    if (ret <= 0)
        return ret;

    if (!is_null(_this->ops) && !is_null(_this->ops->free_data)) {
        for (uint8_t* p = _this->begin; _this->end != p; p = __i_vector_ptr_add(p, 1, _this->step))
            _this->ops->free_data((vector_data_t*)p);
    }

    _this->end = _this->begin;
    return ret;
}

#endif /* __J_I_VECTOR_H */
