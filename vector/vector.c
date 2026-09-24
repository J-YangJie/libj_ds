/*
  Vector Implementations
  Copyright (C) 2021  YangJie <yangjie98765@yeah.net>
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

#include <vector/vector.h>

#include <_log.h>
#include <_memory.h>
#include <_compiler_inter.h>
#include <linux/_compiler.h>
#include <string.h>

#define TAG "[vector]"

#define _I_VECTOR_CAPACITY_INIT  4

/* checked */
static JDSC_INLINE
void __i_vector_slot_move_n(const i_vector_t* _this, uint8_t* dest, const uint8_t* src, vector_size_t n)
{
    const vector_step_t step = _this->step;

    if (unlikely(n <= 0 || dest == src))
        return ;

    if (_this->sso) {
        const size_t d = (size_t)dest;
        const size_t s = (size_t)src;
        uint8_t* _src  = (uint8_t*)src;
        vector_size_t i;

        if (d > s && d < s + (size_t)(n * step)) {
            dest = __i_vector_ptr_add(dest, n - 1, step);
            _src = __i_vector_ptr_add(_src, n - 1, step);
            for (i = 0; i < n; ++i, dest -= step, _src -= step)
                __i_vector_slot_move(_this, dest, _src);
        } else {
            for (i = 0; i < n; ++i, dest += step, _src += step)
                __i_vector_slot_move(_this, dest, _src);
        }
    } else {
        memmove(dest, src, (size_t)(n * step));
    }
}

/* checked */
bool __i_vector_reserve(i_vector_t* _this, vector_size_t n)
{
    uint8_t* nh = NULL;
    const vector_step_t step = _this->step;
    vector_size_t size = __i_vector_size(_this);
    vector_size_t cap  = __i_vector_capacity(_this);

    if (unlikely(n < 0))
        return false;

    if (n <= cap)
        return true;

    /* TODO: 确定这样设置界限？ */
    if (unlikely((size_t)n >= (SIZE_MAX / step) - 1))
        return false;

    nh = p_realloc(_this->rend, (n + 1) * step);
    if (is_null(nh))
        return false;

    if (_this->rend != nh) {
        pr_notice("Head [ %p -> %p ], capacity [ %zd -> %zd ] by [ reserve ]", 
                    _this->rend, nh, cap, n);
        _this->rend = nh;
        _this->begin = __i_vector_ptr_add(_this->rend, 1, step);
        _this->end = __i_vector_ptr_add(_this->begin, size, step);
        if (_this->sso) {
            for (uint8_t* it = _this->begin; _this->end != it; it = __i_vector_ptr_add(it, 1, step))
                __ds_ops_fix_sso((ds_data_t)it);
        }
    } else {
        pr_notice("Head [ %p ], capacity [ %zd -> %zd ] by [ reserve ]", 
                    nh, cap, n);
    }
    _this->end_of_storage = __i_vector_ptr_add(_this->begin, n, step);

    return true;
}

/* checked */
void __i_vector_shrink_to_fit(i_vector_t* _this)
{
    const vector_step_t step = _this->step;
    const vector_size_t used = __i_vector_size(_this);
    vector_size_t ncap, cap = __i_vector_capacity(_this);
    uint8_t* nh;

    ncap = used; /* Allow capacity reduction to zero */
    if (cap == ncap)
        return ;

    nh = p_realloc(_this->rend, (ncap + 1) * step);
    if (is_null(nh))
        return ;

    if (_this->rend != nh) {
        pr_notice("Head [ %p -> %p ], capacity [ %zd -> %zd ] by [ shrink_to_fit ]", 
                    _this->rend, nh, cap, ncap);
        _this->rend = nh;
        _this->begin = __i_vector_ptr_add(_this->rend, 1, step);
        _this->end = __i_vector_ptr_add(_this->begin, used, step);
        if (_this->sso) {
            for (uint8_t* it = _this->begin; _this->end != it; it = __i_vector_ptr_add(it, 1, step))
                __ds_ops_fix_sso((ds_data_t)it);
        }
    } else {
        pr_notice("Head [ %p ], capacity [ %zd -> %zd ] by [ shrink_to_fit ]", 
                    nh, cap, ncap);
    }
    _this->end_of_storage = _this->end;
}

/* checked */
static JDSC_INLINE_FORCE_POLICY
bool __i_vector_slot_write_n(const i_vector_t* _this, uint8_t* pos, vector_size_t n, vector_data_t data)
{
    const vector_step_t step = _this->step;
    vector_size_t suc = 0;

    if (!is_null(_this->ops) && !is_null(_this->ops->copy_data)) {
        while (n-- > 0) {
            if (_this->ops->copy_data(data, (vector_data_t*)pos))
                suc++;
            else
                goto err;
            pos = __i_vector_ptr_add(pos, 1, step);
        }

        return true;
    } else {
        if (_this->step > sizeof(vector_data_t))
            return false;

        while (n-- > 0) {
            switch (_this->step)
            {
            case 1: *(uint8_t*)pos  = (uint8_t)data;  break;
            case 2: *(uint16_t*)pos = (uint16_t)data; break;
            case 4: *(uint32_t*)pos = (uint32_t)data; break;
            case 8: *(uint64_t*)pos = (uint64_t)data; break;
            default: break;
            }

            suc++;
            pos = __i_vector_ptr_add(pos, 1, step);
        }

        return true;
    }

err:
    if (!is_null(_this->ops) && !is_null(_this->ops->free_data)) {
        for (vector_size_t i = 0; i < suc; ++i) {
            pos = __i_vector_ptr_sub(pos, 1, step);
            _this->ops->free_data((vector_data_t*)pos);
        }
    }
    return false;
}

/* checked */
static JDSC_INLINE_FORCE_POLICY
vector_size_t __i_vector_check_len(const i_vector_t* _this, vector_size_t n)
{
    const vector_size_t size = __i_vector_size(_this);
    vector_size_t len;

    if (n < 1)
        n = 1;

    len = size + (size > n ? size : n);

    if (unlikely(len < size)) /* overflow */
        return -1;

    return len < _this->capacity_init ? _this->capacity_init : len;
}

/* checked */
/* Non independent logical function */
JDSC_INLINE_POLICY
vector_iterator_t __i_vector_insert_run(i_vector_t* _this, uint8_t* pos, vector_size_t n, vector_data_t data)
{
    const vector_step_t step = _this->step;
    ptrdiff_t off;
    vector_size_t move, avail;

    if (unlikely(pos < _this->begin || pos > _this->end))
        return i_vector_null_iterator();

    off   = __i_vector_ptr_diff(pos, _this->begin, step);
    move  = (vector_size_t)__i_vector_ptr_diff(_this->end, pos, step);
    avail = __i_vector_capacity(_this) - __i_vector_size(_this);

    if (n > avail) {
        if (!__i_vector_reserve(_this, __i_vector_check_len(_this, n)))
            return i_vector_null_iterator();
        pos = __i_vector_ptr_add(_this->begin, off, step);
    }

    __i_vector_slot_move_n(_this, __i_vector_ptr_add(pos, n, step), pos, move);
    _this->end = __i_vector_ptr_add(_this->end, n, step);
    if (!__i_vector_slot_write_n(_this, pos, n, data))
        goto err_slot;
    return __i_vector_make_iterator(step, pos);

err_slot:
    __i_vector_slot_move_n(_this, pos, __i_vector_ptr_add(pos, n, step), move);
    _this->end = __i_vector_ptr_sub(_this->end, n, step);
    return i_vector_null_iterator();
}

/* checked */
vector_iterator_t __i_vector_insert(i_vector_t* _this, vector_iterator_t pos, vector_data_t data)
{
    if (unlikely(!is_null(_this->ops) && !is_null(_this->ops->valid_data) && !_this->ops->valid_data(data)))
        return i_vector_null_iterator();
    return __i_vector_insert_run(_this, pos.cur, 1, data);
}

/* checked */
vector_iterator_t i_vector_insert_n(i_vector_t* _this, vector_iterator_t pos, vector_size_t n, vector_data_t data)
{
    if (unlikely(is_null(_this) || i_vector_is_null_iterator(pos) || n < 0))
        return i_vector_null_iterator();

    if (unlikely(0 == n))
        return pos;

    if (unlikely(!is_null(_this->ops) && !is_null(_this->ops->valid_data) && !_this->ops->valid_data(data)))
        return i_vector_null_iterator();
    return __i_vector_insert_run(_this, pos.cur, n, data);
}

/* checked */
vector_iterator_t __i_vector_erase(i_vector_t* _this, uint8_t* pos)
{
    const vector_step_t step = _this->step;
    vector_size_t n = (vector_size_t)__i_vector_ptr_diff(_this->end, pos, step);

    if (!is_null(_this->ops) && !is_null(_this->ops->free_data))
        _this->ops->free_data((vector_data_t*)pos);

    __i_vector_slot_move_n(_this, pos, __i_vector_ptr_add(pos, 1, step), n - 1);
    _this->end = __i_vector_ptr_sub(_this->end, 1, step);
    return __i_vector_make_iterator(step, pos);
}

/* checked */
vector_iterator_t __i_vector_erase_range(i_vector_t* _this, uint8_t* begin, uint8_t* end)
{
    const vector_step_t step = _this->step;
    vector_size_t n = __i_vector_ptr_diff(_this->end, end, step);

    if (unlikely(begin > end || begin < _this->begin || end > _this->end))
        return i_vector_null_iterator();

    if (unlikely(begin == end))
        return __i_vector_make_iterator(step, begin);

    if (!is_null(_this->ops) && !is_null(_this->ops->free_data)) {
        for (uint8_t* it = begin; end != it; it = __i_vector_ptr_add(it, 1, step))
            _this->ops->free_data((vector_data_t*)it);
    }

    __i_vector_slot_move_n(_this, begin, end, n);
    _this->end = __i_vector_ptr_add(begin, n, step);
    return __i_vector_make_iterator(step, begin);
}

/* checked */
static
bool __vector_init(vector_t* vector)
{
    vector_size_t alloc = vector->capacity_init > 0 ? vector->capacity_init + 1 : 1;

    vector->rend  = (uint8_t*)p_malloc(alloc * vector->step);
    if (is_null(vector->rend))
        return false;

    vector->begin = __i_vector_ptr_add(vector->rend, 1, vector->step);
    vector->end   = vector->begin;
    vector->end_of_storage = __i_vector_ptr_add(vector->begin, alloc - 1, vector->step);
    return true;
}

/* checked */
static
void __vector_deinit(vector_t* vector)
{
    i_vector_clear(vector);
    p_free(vector->rend);
}

/* checked */
vector_t* __vector_new(const class_vector_ops_t* ops, vector_step_t step, vector_size_t capacity_init)
{
    vector_t* vector;

    if (0 == step || capacity_init < 0)
        return NULL;

    vector = (vector_t*)p_calloc(1, sizeof(vector_t));
    if (is_null(vector))
        return NULL;

    vector->step = step;
    vector->capacity_init = 0 != capacity_init && capacity_init < _I_VECTOR_CAPACITY_INIT ? _I_VECTOR_CAPACITY_INIT : capacity_init;
    if (!__vector_init(vector))
        goto err;

    vector->capacity_init = 0 == vector->capacity_init ? _I_VECTOR_CAPACITY_INIT : vector->capacity_init;

    vector->ops = ops;
    if (g_class_vector_ops_sso() == vector->ops)
        vector->sso = true;
    return vector;

err:
    p_free(vector);
    return NULL;
}

/* checked */
void __vector_delete(vector_t** _this)
{
    if (is_null(_this) || is_null(*_this))
        return ;

    __vector_deinit(*_this);
    p_free(*_this);
}

const class_vector_t* class_vector_ins(void)
{
    static const class_vector_t ins = {
        .size           = cvector_size,
        .capacity       = cvector_capacity,
        .__empty        = __cvector_empty,
        .count          = cvector_count,
        .end            = cvector_end,
        .begin          = cvector_begin,
        .next           = cvector_next,
        .prev           = cvector_prev,
        .rend           = cvector_rend,
        .rbegin         = cvector_rbegin,
        .rnext          = cvector_rnext,
        .rprev          = cvector_rprev,
        .__it           = __cvector_it,
        .it             = cvector_it,
        .__at           = __cvector_at,
        .at             = cvector_at,
        .back           = cvector_back,
        .front          = cvector_front,
        .find           = cvector_find,
        .push_back      = cvector_push_back,
        .insert         = cvector_insert,
        .insert_n       = cvector_insert_n,
        .erase          = cvector_erase,
        .erase_range    = cvector_erase_range,
        .pop_back       = cvector_pop_back,
        .remove         = cvector_remove,
        .remove_if      = cvector_remove_if,
        .resize         = cvector_resize,
        .reserve        = cvector_reserve,
        .shrink_to_fit  = cvector_shrink_to_fit,
        .clear          = cvector_clear,
    };
    return &ins;
}
