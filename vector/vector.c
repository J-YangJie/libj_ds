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

#include <string.h>
#include <_log.h>
#include <_memory.h>
#include <_compiler_inter.h>
#include <linux/_compiler.h>

#define TAG "[vector]"

#define _I_VECTOR_CAPACITY_INIT  4

static JDSC_INLINE
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
        while (n-- > 0) {
            switch (_this->step)
            {
            case 1: *(uint8_t*)pos  = (uint8_t)data;  break;
            case 2: *(uint16_t*)pos = (uint16_t)data; break;
            case 4: *(uint32_t*)pos = (uint32_t)data; break;
#ifdef JDSC_64BIT
            case 8: *(uint64_t*)pos = (uint64_t)data; break;
#endif /* JDSC_64BIT */
            default: return false;
            }

            pos = __i_vector_ptr_add(pos, 1, step);
        }

        return true;
    }

err:
    if (!is_null(_this->ops) && !is_null(_this->ops->free_data)) {
        while (suc-- > 0) {
            pos = __i_vector_ptr_sub(pos, 1, step);
            _this->ops->free_data((vector_data_t*)pos);
        }
    }
    return false;
}

static JDSC_INLINE
bool __i_vector_slot_write_n_range(const i_vector_t* _this, uint8_t* dest, const uint8_t* src, vector_size_t n)
{
    const vector_step_t step = _this->step;
    vector_size_t suc = 0;

    if (is_null(_this->ops) || is_null(_this->ops->copy_data)) {
        memcpy(dest, src, (size_t)(n * step));
        return true;
    }

    if (_this->dtype & SEQ_CNTR_DTYPE_MASK_STUN) {
        if (_this->sso) {
            while (n-- > 0) {
                if (ds_ops_copy_data_same_sso((vector_data_t)src, (vector_data_t*)dest))
                    suc++;
                else
                    goto err;

                dest = __i_vector_ptr_add(dest, 1, step);
                src  = __i_vector_ptr_add(src, 1, step);
            }
        } else {
            while (n-- > 0) {
                if (_this->ops->copy_data((vector_data_t)src, (vector_data_t*)dest))
                    suc++;
                else
                    goto err;

                dest = __i_vector_ptr_add(dest, 1, step);
                src  = __i_vector_ptr_add(src, 1, step);
            }
        }
    } else if (_this->dtype & SEQ_CNTR_DTYPE_MASK_NUM) {
        while (n-- > 0) {
            if (_this->ops->copy_data(__i_vector_slot_read(_this, src), (vector_data_t*)dest))
                suc++;
            else
                goto err;
            dest = __i_vector_ptr_add(dest, 1, step);
            src  = __i_vector_ptr_add(src, 1, step);
        }
    } else {
        return false;
    }

    return true;

err:
    if (!is_null(_this->ops) && !is_null(_this->ops->free_data)) {
        while (suc-- > 0) {
            dest = __i_vector_ptr_sub(dest, 1, step);
            _this->ops->free_data((vector_data_t*)dest);
        }
    }
    return false;
}

static JDSC_INLINE
void __i_vector_slot_move_n(const i_vector_t* _this, uint8_t* dest, const uint8_t* src, vector_size_t n)
{
    const vector_step_t step = _this->step;

    if (unlikely(n <= 0 || dest == src))
        return ;

    if (is_null(_this->ops) || is_null(_this->ops->__move)) {
        memmove(dest, src, (size_t)(n * step));

        if (_this->sso) {
            uint8_t* p   = dest;
            uint8_t* end = __i_vector_ptr_add(p, n, step);

            for (; end != p; p += step)
                __ds_ops_fix_sso((vector_data_t)p);
        }
    } else {
        void (*__move)(vector_data_t, vector_data_t) = _this->ops->__move;
        const size_t d = (size_t)dest;
        const size_t s = (size_t)src;
        uint8_t* _src  = (uint8_t*)src;
        vector_size_t i;

        if (d > s && d < s + (size_t)(n * step)) {
            dest = __i_vector_ptr_add(dest, n - 1, step);
            _src = __i_vector_ptr_add(_src, n - 1, step);
            for (i = 0; i < n; ++i, dest -= step, _src -= step)
                __move((vector_data_t)dest, (vector_data_t)_src);
        } else {
            for (i = 0; i < n; ++i, dest += step, _src += step)
                __move((vector_data_t)dest, (vector_data_t)_src);
        }
    }
}

static JDSC_INLINE
uint8_t* __i_vector_realloc_buf(i_vector_t* _this, vector_size_t ncap)
{
    const vector_step_t step = _this->step;
    const vector_size_t used = __i_vector_size(_this);
    uint8_t* nh, * oh = _this->rend;
    vector_size_t i;

    JDSC_ASSERT(ncap >= used);

    if (is_null(_this->ops) || is_null(_this->ops->__move)) {
        nh = (uint8_t*)p_aligned_realloc(oh, _this->align, (size_t)(ncap + 1) * step, 
                                                           (size_t)(used + 1) * step);
        if (is_null(nh))
            return NULL;

        if (oh != nh && _this->sso) {
            uint8_t* p   = __i_vector_ptr_add(nh, 1, step);
            uint8_t* end = __i_vector_ptr_add(p, used, step);

            for (; end != p; p += step)
                __ds_ops_fix_sso((vector_data_t)p);
        }
        return nh;
    } else {
        nh = (uint8_t*)p_aligned_malloc(_this->align, (size_t)(ncap + 1) * step);
        if (is_null(nh))
            return NULL;

        {
            uint8_t* nb = __i_vector_ptr_add(nh, 1, step);
            for (i = 0; i < used; ++i)
                _this->ops->__move( (vector_data_t)__i_vector_ptr_add(nb, i, step),
                                    (vector_data_t)__i_vector_ptr_add(_this->begin, i, step));
        }
        p_aligned_free(oh, _this->align);
        return nh;
    }
}

#include <limits.h>
#include <stdint.h> /* SIZE_MAX */

#ifndef SSIZE_MAX
#define SSIZE_MAX ((ssize_t)(SIZE_MAX / 2))
#endif

static JDSC_INLINE_FORCE
vector_size_t ___i_vector_max_size(vector_step_t step)
{
    return 0 == step ? -1 : (SSIZE_MAX / step) - 1;
}

JDSC_INLINE
vector_size_t __i_vector_max_size(const i_vector_t* _this)
{
    return ___i_vector_max_size(_this->step);
}

JDSC_INLINE
bool __i_vector_reserve(i_vector_t* _this, vector_size_t n)
{
    const vector_step_t step = _this->step;
    const vector_size_t size = __i_vector_size(_this);
    const vector_size_t ncap = n, ocap = __i_vector_capacity(_this);
    uint8_t* nh;

    if (unlikely(n < 0 || n > __i_vector_max_size(_this)))
        return false;

    if (ncap <= ocap)
        return true;

    nh = __i_vector_realloc_buf(_this, ncap);
    if (is_null(nh))
        return false;

    pr_notice("Head [ %p -> %p ], capacity [ %zd -> %zd ] by [ reserve ]",
                _this->rend, nh, ocap, ncap);

    _this->rend  = nh;
    _this->begin = __i_vector_ptr_add(_this->rend, 1, step);
    _this->end   = __i_vector_ptr_add(_this->begin, size, step);
    _this->end_of_storage = __i_vector_ptr_add(_this->begin, ncap, step);
    return true;
}

void __i_vector_shrink_to_fit(i_vector_t* _this)
{
    const vector_step_t step = _this->step;
    const vector_size_t size = __i_vector_size(_this);
    const vector_size_t ncap = size, ocap = __i_vector_capacity(_this);
    uint8_t* nh;

    if (unlikely(ncap == ocap))
        return ;

    nh = __i_vector_realloc_buf(_this, ncap);
    if (is_null(nh))
        return ;

    pr_notice("Head [ %p -> %p ], capacity [ %zd -> %zd ] by [ shrink_to_fit ]",
                _this->rend, nh, ocap, ncap);

    _this->rend  = nh;
    _this->begin = __i_vector_ptr_add(_this->rend, 1, step);
    _this->end   = __i_vector_ptr_add(_this->begin, size, step);
    _this->end_of_storage = _this->end;
}

static JDSC_INLINE_FORCE_POLICY
vector_size_t __i_vector_check_len(const i_vector_t* _this, vector_size_t n)
{
    const vector_size_t size = __i_vector_size(_this);
    const vector_size_t max_grow = __i_vector_max_size(_this) - size;

    JDSC_ASSERT(max_grow >= n);
    if (max_grow < n)
        return -1;

    return size + (max_grow > size ? size > n ? size : n : max_grow);
}

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
    avail = (vector_size_t)__i_vector_ptr_diff(_this->end_of_storage, _this->end, step);

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

vector_iterator_t __i_vector_insert(i_vector_t* _this, vector_iterator_t pos, vector_data_t data)
{
    if (unlikely(!is_null(_this->ops) && !is_null(_this->ops->valid_data) && !_this->ops->valid_data(data)))
        return i_vector_null_iterator();
    return __i_vector_insert_run(_this, pos.cur, 1, data);
}

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

vector_iterator_t i_vector_insert_range(i_vector_t* _this, vector_iterator_t pos, vector_iterator_t iterator_begin, vector_iterator_t iterator_end)
{
    const uint8_t* src = iterator_begin.cur;
    ptrdiff_t off;
    vector_size_t n, move, avail;

    if (unlikely(is_null(_this) || i_vector_is_null_iterator(pos) || i_vector_is_null_iterator(iterator_begin) || i_vector_is_null_iterator(iterator_end)))
        return i_vector_null_iterator();

    if (unlikely(pos.cur < _this->begin || pos.cur > _this->end))
        return i_vector_null_iterator();

    const vector_step_t step = _this->step;
    n = (vector_size_t)__i_vector_ptr_diff(iterator_end.cur, iterator_begin.cur, step);
    if (unlikely(n < 0))
        return i_vector_null_iterator();

    if (unlikely(0 == n))
        return pos;

    off   = __i_vector_ptr_diff(pos.cur, _this->begin, step);
    move  = (vector_size_t)__i_vector_ptr_diff(_this->end, pos.cur, step);
    avail = (vector_size_t)__i_vector_ptr_diff(_this->end_of_storage, _this->end, step);

    if (n > avail) {
        if (!__i_vector_reserve(_this, __i_vector_check_len(_this, n)))
            return i_vector_null_iterator();
        pos.cur = __i_vector_ptr_add(_this->begin, off, step);
    }

    __i_vector_slot_move_n(_this, __i_vector_ptr_add(pos.cur, n, step), pos.cur, move);
    _this->end = __i_vector_ptr_add(_this->end, n, step);

    if (!__i_vector_slot_write_n_range(_this, pos.cur, src, n))
        goto err;

    return __i_vector_make_iterator(step, pos.cur);

err:
    __i_vector_slot_move_n(_this, pos.cur, __i_vector_ptr_add(pos.cur, n, step), move);
    _this->end = __i_vector_ptr_sub(_this->end, n, step);
    return i_vector_null_iterator();
}

/* Non independent logical function */
vector_iterator_t __i_vector_erase(i_vector_t* _this, uint8_t* pos)
{
    const vector_step_t step = _this->step;
    const vector_size_t n = (vector_size_t)__i_vector_ptr_diff(_this->end, pos, step);

    if (!is_null(_this->ops) && !is_null(_this->ops->free_data))
        _this->ops->free_data((vector_data_t*)pos);

    __i_vector_slot_move_n(_this, pos, __i_vector_ptr_add(pos, 1, step), n - 1);
    _this->end = __i_vector_ptr_sub(_this->end, 1, step);
    return __i_vector_make_iterator(step, pos);
}

vector_iterator_t __i_vector_erase_range(i_vector_t* _this, uint8_t* begin, uint8_t* end)
{
    const vector_step_t step = _this->step;
    const vector_size_t n = (vector_size_t)__i_vector_ptr_diff(_this->end, end, step);

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

bool __i_vector_assign_grow_fill(i_vector_t* _this, vector_size_t n, vector_data_t data)
{
    const vector_step_t step = _this->step;
    vector_size_t cap = __i_vector_capacity(_this);
    uint8_t* nh, * nb, * p;

    if (unlikely(n <= 0 || n > ___i_vector_max_size(step)))
        return false;

    if (unlikely(!is_null(_this->ops) && !is_null(_this->ops->valid_data) && !_this->ops->valid_data(data)))
        return false;

    cap = cap >= n ? cap : n;
    nh = (uint8_t*)p_aligned_malloc(_this->align, (size_t)(cap + 1) * step);
    if (is_null(nh))
        return false;

    nb = __i_vector_ptr_add(nh, 1, step);
    if (!__i_vector_slot_write_n(_this, nb, n, data))
        goto err;

    if (!is_null(_this->ops) && !is_null(_this->ops->free_data)) {
        for (p = _this->begin; _this->end != p; p = __i_vector_ptr_add(p, 1, step))
            _this->ops->free_data((vector_data_t*)p);
    }
    p_aligned_free(_this->rend, _this->align);

    _this->rend  = nh;
    _this->begin = nb;
    _this->end   = __i_vector_ptr_add(_this->begin, n, step);
    _this->end_of_storage = __i_vector_ptr_add(_this->begin, cap, step);
    return true;

err:
    p_aligned_free(nh, _this->align);
    return false;
}

bool __i_vector_assign_grow_range(i_vector_t* _this, vector_size_t n, const uint8_t* src)
{
    const vector_step_t step = _this->step;
    vector_size_t cap = __i_vector_capacity(_this);
    uint8_t* nh, * nb, * p;

    if (unlikely(n <= 0 || n > ___i_vector_max_size(step)))
        return false;

    cap = cap >= n ? cap : n;
    nh = (uint8_t*)p_aligned_malloc(_this->align, (size_t)(cap + 1) * step);
    if (is_null(nh))
        return false;

    nb = __i_vector_ptr_add(nh, 1, step);
    if (!__i_vector_slot_write_n_range(_this, nb, src, n))
        goto err;

    if (!is_null(_this->ops) && !is_null(_this->ops->free_data)) {
        for (p = _this->begin; _this->end != p; p = __i_vector_ptr_add(p, 1, step))
            _this->ops->free_data((vector_data_t*)p);
    }
    p_aligned_free(_this->rend, _this->align);

    _this->rend  = nh;
    _this->begin = nb;
    _this->end   = __i_vector_ptr_add(_this->begin, n, step);
    _this->end_of_storage = __i_vector_ptr_add(_this->begin, cap, step);
    return true;

err:
    p_aligned_free(nh, _this->align);
    return false;
}

static JDSC_INLINE
bool __vector_init(vector_t* vector)
{
    vector_size_t alloc = vector->capacity_init > 0 ? vector->capacity_init + 1 : 1;

    vector->rend = (uint8_t*)p_aligned_malloc(vector->align, alloc * vector->step);
    if (is_null(vector->rend)) {
        JDSC_ASSERT_NEW(0, "%s: Failed to allocate %zu(%zd * %zu) bytes for vector buffer!",
                        __func__, alloc * vector->step, alloc, vector->step);
        return false;
    }

    vector->begin = __i_vector_ptr_add(vector->rend, 1, vector->step);
    vector->end   = vector->begin;
    vector->end_of_storage = __i_vector_ptr_add(vector->begin, alloc - 1, vector->step);
    return true;
}

static JDSC_INLINE
void __vector_deinit(vector_t* vector)
{
    i_vector_clear(vector);
    p_aligned_free(vector->rend, vector->align);
}

vector_t* __vector_new(const class_vector_ops_t* ops, vector_dtype_t data_type, vector_step_t step, vector_align_t align, vector_size_t capacity_init)
{
    vector_t* vector;

    if (step <= 0 || align > step || (0 != align && (0 != (align & (align - 1)) || 0 != (step % align))) || capacity_init < 0 || capacity_init > ___i_vector_max_size(step)) {
        JDSC_ASSERT_NEW(0, "%s: Invalid step %zu, align %zu or capacity_init %zd "
                        "(step must be greater than zero, align must be zero or a power of two that does not exceed and divides step, "
                        "and capacity_init must be in [0, max_size(%zd)])!",
                        __func__, step, align, capacity_init, ___i_vector_max_size(step));
        return NULL;
    }

    if (!__sequence_container_dtype_valid(data_type)) {
        JDSC_ASSERT_NEW(0, "%s: Invalid data type %zu!", __func__, data_type);
        return NULL;
    }

    size_t width = __sequence_container_dtype_width(data_type);
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

    if (g_class_vector_ops_sso() == ops && (SEQ_CNTR_DTYPE_SSO != data_type || sizeof(ds_sso_t) != step)) {
        JDSC_ASSERT_NEW(0, "%s: SSO ops require the SSO data type and a step of sizeof(ds_sso_t) (got %zu)!",
                        __func__, step);
        return NULL;
    }

    vector = (vector_t*)p_calloc(1, sizeof(vector_t));
    if (is_null(vector)) {
        JDSC_ASSERT_NEW(0, "%s: Failed to allocate %zu bytes for vector!",
                        __func__, sizeof(vector_t));
        return NULL;
    }

    vector->dtype = data_type;
    vector->step  = step;
    vector->align = align;
    vector->capacity_init = 0 != capacity_init && capacity_init < _I_VECTOR_CAPACITY_INIT ? _I_VECTOR_CAPACITY_INIT : capacity_init;
    if (!__vector_init(vector))
        goto err;

    vector->ops = ops;
    if (g_class_vector_ops_sso() == vector->ops)
        vector->sso = true;
    return vector;

err:
    p_free(vector);
    return NULL;
}

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
        .max_size       = cvector_max_size,
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
        .__back         = __cvector_back,
        .back           = cvector_back,
        .__front        = __cvector_front,
        .front          = cvector_front,
        .find           = cvector_find,
        .push_back      = cvector_push_back,
        .insert         = cvector_insert,
        .insert_n       = cvector_insert_n,
        .insert_range   = cvector_insert_range,
        .replace_it     = cvector_replace_it,
        .replace_index  = cvector_replace_index,
        .erase          = cvector_erase,
        .erase_range    = cvector_erase_range,
        .pop_back       = cvector_pop_back,
        .remove         = cvector_remove,
        .remove_if      = cvector_remove_if,
        .assign         = cvector_assign,
        .assign_range   = cvector_assign_range,
        .resize         = cvector_resize,
        .reserve        = cvector_reserve,
        .shrink_to_fit  = cvector_shrink_to_fit,
        .swap           = cvector_swap,
        .clear          = cvector_clear,
    };
    return &ins;
}
