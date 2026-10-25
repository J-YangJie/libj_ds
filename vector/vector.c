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

    if (!_this->sso && (is_null(_this->ops) || is_null(_this->ops->__move))) {
        memmove(dest, src, (size_t)(n * step));
    } else {
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
    }
}

static
uint8_t* __vector_realloc_buf(i_vector_t* _this, vector_size_t ncap)
{
    const vector_step_t step = _this->step;
    const vector_size_t used = __i_vector_size(_this);
    uint8_t* oh = _this->rend;
    uint8_t* nh;
    uint8_t* nb;
    vector_size_t i;

    JDSC_ASSERT(ncap >= used);

    if (is_null(_this->ops) || is_null(_this->ops->__move)) {
        nh = (uint8_t*)p_aligned_realloc(oh, _this->align, (size_t)(ncap + 1) * step, 
                                                           (size_t)(used + 1) * step);
        if (is_null(nh))
            return NULL;

        if (nh != oh && _this->sso) {
            nb = __i_vector_ptr_add(nh, 1, step);
            for (i = 0; i < used; ++i)
                __ds_ops_fix_sso((vector_data_t)__i_vector_ptr_add(nb, i, step));
        }
        return nh;
    } else {
        nh = (uint8_t*)p_aligned_malloc(_this->align, (size_t)(ncap + 1) * step);
        if (is_null(nh))
            return NULL;

        {
            nb = __i_vector_ptr_add(nh, 1, step);
            for (i = 0; i < used; ++i)
                _this->ops->__move( (vector_data_t)__i_vector_ptr_add(nb, i, step),
                                    (vector_data_t)__i_vector_ptr_add(_this->begin, i, step));
        }
        p_aligned_free(oh);
        return nh;
    }
}

/* checked */
bool __i_vector_reserve(i_vector_t* _this, vector_size_t n)
{
    const vector_step_t step = _this->step;
    const vector_size_t size = __i_vector_size(_this);
    const vector_size_t ncap = n, ocap = __i_vector_capacity(_this);
    uint8_t* nh;

    if (unlikely(ncap < 0))
        return false;

    if (ncap <= ocap)
        return true;

    /* TODO: 确定这样设置界限？ */
    if (unlikely((size_t)ncap >= (SIZE_MAX / step) - 1))
        return false;

    nh = __vector_realloc_buf(_this, ncap);
    if (is_null(nh))
        return false;

    pr_notice("Head [ %p -> %p ], capacity [ %zd -> %zd ] by [ reserve ]",
                _this->rend, nh, ocap, ncap);

    _this->rend = nh;
    _this->begin = __i_vector_ptr_add(_this->rend, 1, step);
    _this->end = __i_vector_ptr_add(_this->begin, size, step);
    _this->end_of_storage = __i_vector_ptr_add(_this->begin, ncap, step);
    return true;
}

/* checked */
void __i_vector_shrink_to_fit(i_vector_t* _this)
{
    const vector_step_t step = _this->step;
    const vector_size_t size = __i_vector_size(_this);
    const vector_size_t ncap = size, ocap = __i_vector_capacity(_this);
    uint8_t* nh;

    if (ncap == ocap)
        return ;

    nh = __vector_realloc_buf(_this, ncap);
    if (is_null(nh))
        return ;

    pr_notice("Head [ %p -> %p ], capacity [ %zd -> %zd ] by [ shrink_to_fit ]",
                _this->rend, nh, ocap, ncap);

    _this->rend = nh;
    _this->begin = __i_vector_ptr_add(_this->rend, 1, step);
    _this->end = __i_vector_ptr_add(_this->begin, size, step);
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

    vector->rend  = (uint8_t*)p_aligned_malloc(vector->align, alloc * vector->step);
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
    p_aligned_free(vector->rend);
}

/* checked */
vector_t* __vector_new(const class_vector_ops_t* ops, vector_dtype_t data_type, vector_step_t step, vector_align_t align, vector_size_t capacity_init)
{
    vector_t* vector;

    JDSC_ASSERT(0 != step && capacity_init >= 0);
    if (0 == step || capacity_init < 0) {
        pr_err("%s: invalid step [ %zu ] or capacity_init [ %zd ]!", __func__, step, capacity_init);
        return NULL;
    }

    JDSC_ASSERT(__sequence_container_dtype_valid(data_type));
    if (!__sequence_container_dtype_valid(data_type)) {
        pr_err("%s: invalid data type [ %zu ]!", __func__, data_type);
        return NULL;
    }

    size_t width = __sequence_container_dtype_width(data_type);
    JDSC_ASSERT(0 == width || width == step);
    if (0 != width && width != step) {
        pr_err("%s: data type width [ %zu ] does not match step [ %zu ]!", __func__, width, step);
        return NULL;
    }

    JDSC_ASSERT(!(data_type & SEQ_CNTR_DTYPE_MASK_STUN)
                || (!is_null(ops) && !is_null(ops->copy_data)));
    if ((data_type & SEQ_CNTR_DTYPE_MASK_STUN) && (is_null(ops) || is_null(ops->copy_data))) {
        pr_err("%s: aggregate data type requires copy_data!", __func__);
        return NULL;
    }

    JDSC_ASSERT(g_class_vector_ops_sso() != ops
                || (SEQ_CNTR_DTYPE_SSO == data_type && sizeof(ds_sso_t) == step));
    if (g_class_vector_ops_sso() == ops
        && (SEQ_CNTR_DTYPE_SSO != data_type || sizeof(ds_sso_t) != step)) {
        pr_err("%s: SSO ops require SSO data type and sizeof(ds_sso_t) step!", __func__);
        return NULL;
    }

    vector = (vector_t*)p_calloc(1, sizeof(vector_t));
    if (is_null(vector))
        return NULL;

    vector->dtype = data_type;
    vector->step = step;
    vector->align = align;
    vector->capacity_init = 0 != capacity_init && capacity_init < _I_VECTOR_CAPACITY_INIT ? _I_VECTOR_CAPACITY_INIT : capacity_init;
    if (!__vector_init(vector))
        goto err;

    // vector->capacity_init = 0 == vector->capacity_init ? _I_VECTOR_CAPACITY_INIT : vector->capacity_init;

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

bool cvector_swap_at(vector_t* _this, vector_index_t i, vector_index_t j)
{
    vector_step_t step;
    uint8_t* a;
    uint8_t* b;
    uint8_t* t;
    uint8_t tmp[32];

    if (is_null(_this)
        || i >= (vector_index_t)__i_vector_size(_this)
        || j >= (vector_index_t)__i_vector_size(_this))
        return false;

    if (i == j)
        return true;

    step = _this->step;

    a = __i_vector_ptr_add(_this->begin, i, step);
    b = __i_vector_ptr_add(_this->begin, j, step);

    switch (step) {
    case 1: {
        uint8_t v = *(uint8_t*)a; *(uint8_t*)a = *(uint8_t*)b; *(uint8_t*)b = v; return true;
    }
    case 2: {
        uint16_t v = *(uint16_t*)a; *(uint16_t*)a = *(uint16_t*)b; *(uint16_t*)b = v; return true;
    }
    case 4: {
        uint32_t v = *(uint32_t*)a; *(uint32_t*)a = *(uint32_t*)b; *(uint32_t*)b = v; return true;
    }
    case 8: {
        uint64_t v = *(uint64_t*)a; *(uint64_t*)a = *(uint64_t*)b; *(uint64_t*)b = v; return true;
    }
    case 16:
        if (!_this->sso) {
            uint64_t a0 = *(uint64_t*)a,       a1 = *(uint64_t*)(a + 8);
            uint64_t b0 = *(uint64_t*)b,       b1 = *(uint64_t*)(b + 8);
            *(uint64_t*)a = b0; *(uint64_t*)(a + 8) = b1;
            *(uint64_t*)b = a0; *(uint64_t*)(b + 8) = a1;
            return true;
        }
        break;
    default:
        break;
    }

    if (step <= sizeof(tmp)) {
        t = tmp;
    } else {
        t = (uint8_t*)p_malloc(step);
        if (is_null(t))
            return false;
    }

    memcpy(t, a, step);
    __i_vector_slot_move(_this, a, b);
    memcpy(b, t, step);
    if (_this->sso)
        __ds_ops_fix_sso((ds_data_t)b);

    if (t != tmp)
        p_free(t);

    return true;
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
        .swap_at        = cvector_swap_at,
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
