/*
  I-Deque Internal
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

#include <_compiler.h>
#include <deque/deque_ops.h>

#define _I_DEQUE_BKT_SIZE       64
#define _I_DEQUE_ITERATOR_REND  ((deque_data_t*)JDSC_ITERATOR_REND)

#ifndef is_null
#define is_null(X) (!(X))
#endif /* is_null */

typedef struct i_deque {
    const class_deque_ops_t* ops;
    deque_data_t** bkt;
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

/* checked */
static inline
void __i_deque_set_bkt(deque_iterator_t* it, deque_data_t** bkt)
{
    it->bkt   = bkt;
    it->begin = *bkt;
    it->end   = it->begin + _I_DEQUE_BKT_SIZE;
}

/* checked */
static inline
void __i_deque_set_bkt_r(deque_r_iterator_t* rit, deque_data_t** bkt)
{
    rit->bkt   = bkt;
    rit->begin = *bkt;
    rit->end   = rit->begin + _I_DEQUE_BKT_SIZE;
}

/* checked */
static inline
deque_size_t __i_deque_iterator_distance(deque_iterator_t begin, deque_iterator_t end)
{
    if (begin.bkt == end.bkt)
        return end.cur - begin.cur;
    return _I_DEQUE_BKT_SIZE * (end.bkt - begin.bkt - 1)
            + (end.cur - end.begin)
            + (begin.end - begin.cur);
}

/* checked */
static inline
deque_iterator_t i_deque_null_iterator(void)
{
    deque_iterator_t it;
    it.cur = NULL;
    return it;
}

/* checked */
static inline
deque_r_iterator_t i_deque_null_r_iterator(void)
{
    deque_r_iterator_t rit;
    rit.cur = NULL;
    return rit;
}

bool         __i_deque_push_back_alloc(i_deque_t* _this, deque_data_t data);
bool         __i_deque_push_front_alloc(i_deque_t* _this, deque_data_t data);
deque_data_t __i_deque_pop_back_free(i_deque_t* _this);
deque_data_t __i_deque_pop_front_free(i_deque_t* _this);

deque_count_t    i_deque_count(const i_deque_t* _this, deque_data_t data);
deque_iterator_t i_deque_find(const i_deque_t* _this, deque_data_t data);
deque_iterator_t i_deque_insert(i_deque_t* _this, deque_iterator_t pos, deque_data_t data);
deque_iterator_t i_deque_erase(i_deque_t* _this, deque_iterator_t pos);
deque_iterator_t i_deque_erase_range(i_deque_t* _this, deque_iterator_t first, deque_iterator_t last);
deque_size_t     i_deque_remove(i_deque_t* _this, deque_data_t data);
deque_size_t     i_deque_remove_if(i_deque_t* _this, remove_if_condition cond);
deque_size_t     i_deque_clear(i_deque_t* _this);

/* checked */
static inline
deque_size_t i_deque_size(const i_deque_t* _this)
{
    return is_null(_this) ? -1 : __i_deque_iterator_distance(_this->begin, _this->end);
}

/* checked */
static inline
deque_iterator_t __i_deque_end(const i_deque_t* _this)
{
    return _this->end;
}

/* checked */
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

/* checked */
static inline
deque_iterator_t __i_deque_begin(const i_deque_t* _this)
{
    return _this->begin;
}

/* checked */
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

/* checked */
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

    ++it.cur;
    if (it.cur == it.end) {
        __i_deque_set_bkt(&it, it.bkt + 1);
        it.cur = it.begin;
    }
    return it;
}

/* checked */
static inline
deque_iterator_t i_deque_next(deque_iterator_t it)
{
    return __i_deque_next(it);
}

/* checked */
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
    --it.cur;
    return it;
}

/* checked */
static inline
deque_iterator_t i_deque_prev(deque_iterator_t it)
{
    return __i_deque_prev(it);
}

/* checked */
static inline
deque_r_iterator_t __i_deque_rend(const i_deque_t* _this)
{
    deque_r_iterator_t rit;

    rit.cur       = _I_DEQUE_ITERATOR_REND;
    rit.begin     = _this->begin.begin;
    rit.end       = _this->begin.end;
    rit.bkt       = _this->begin.bkt;
    rit.begin_cur = _this->begin.cur;
    return rit;
}

/* checked */
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

/* checked */
static inline
deque_r_iterator_t __i_deque_rbegin(const i_deque_t* _this)
{
    deque_iterator_t it;

    if (__i_deque_empty(_this))
        return __i_deque_rend(_this);

    it = __i_deque_prev(_this->end);
    {
        deque_r_iterator_t rit;
        rit.cur       = it.cur;
        rit.begin     = it.begin;
        rit.end       = it.end;
        rit.bkt       = it.bkt;
        rit.begin_cur = _this->begin.cur;
        return rit;
    }
}

/* checked */
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

/* checked */
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
    --rit.cur;
    return rit;
}

/* checked */
static inline
deque_r_iterator_t i_deque_rnext(deque_r_iterator_t rit)
{
    return __i_deque_rnext(rit);
}

/* checked */
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

    ++rit.cur;
    if (rit.cur == rit.end) {
        __i_deque_set_bkt_r(&rit, rit.bkt + 1);
        rit.cur   = rit.begin;
    }
    return rit;
}

/* checked */
static inline
deque_r_iterator_t i_deque_rprev(deque_r_iterator_t rit)
{
    return __i_deque_rprev(rit);
}

#if 0
static inline
deque_iterator_t i_deque_first(const i_deque_t* _this)
{
    if (!is_null(_this) && !__i_deque_empty(_this))
        return _this->begin;
    return i_deque_null_iterator();
}

static inline
deque_iterator_t i_deque_last(const i_deque_t* _this)
{
    if (!is_null(_this) && !__i_deque_empty(_this))
        return __i_deque_prev(_this->end);
    return i_deque_null_iterator();
}
#endif

/* checked */
static inline
bool i_deque_push_back(i_deque_t* _this, deque_data_t data)
{
    deque_data_t tdata = data;

    if (is_null(_this))
        return false;

    if (!is_null(_this->ops)) {
        if (!is_null(_this->ops->valid_data) && !_this->ops->valid_data(data))
            return false;

        if (!is_null(_this->ops->copy_data) && !_this->ops->copy_data(data, &tdata))
            return false;
    }

    if (_this->end.cur != _this->end.end - 1) {
        *_this->end.cur++ = tdata;
        return true;
    }

    if (!__i_deque_push_back_alloc(_this, tdata))
        goto err;
    return true;

err:
    if (!is_null(_this->ops) && !is_null(_this->ops->free_data))
        _this->ops->free_data(&tdata);
    return false;
}

/* checked */
static inline
bool i_deque_push_front(i_deque_t* _this, deque_data_t data)
{
    deque_data_t tdata = data;

    if (is_null(_this))
        return false;

    if (!is_null(_this->ops)) {
        if (!is_null(_this->ops->valid_data) && !_this->ops->valid_data(data))
            return false;

        if (!is_null(_this->ops->copy_data) && !_this->ops->copy_data(data, &tdata))
            return false;
    }

    if (_this->begin.cur != _this->begin.begin) {
        *--_this->begin.cur = tdata;
        return true;
    }

    if (!__i_deque_push_front_alloc(_this, tdata))
        goto err;
    return true;

err:
    if (!is_null(_this->ops) && !is_null(_this->ops->free_data))
        _this->ops->free_data(&tdata);
    return false;
}

/* checked */
static inline
void i_deque_pop_back(i_deque_t* _this)
{
    deque_data_t tdata;

    if (is_null(_this) || __i_deque_empty(_this))
        return ;

    if (_this->end.cur != _this->end.begin)
        tdata = *--_this->end.cur;
    else
        tdata = __i_deque_pop_back_free(_this);

    if (!is_null(_this->ops) && !is_null(_this->ops->free_data))
        _this->ops->free_data(&tdata);
}

/* checked */
static inline
void i_deque_pop_front(i_deque_t* _this)
{
    deque_data_t tdata;

    if (is_null(_this) || __i_deque_empty(_this))
        return ;

    if (_this->begin.cur != _this->begin.end - 1)
        tdata = *_this->begin.cur++;
    else
        tdata = __i_deque_pop_front_free(_this);

    if (!is_null(_this->ops) && !is_null(_this->ops->free_data))
        _this->ops->free_data(&tdata);
}



#endif /* __J_I_DEQUE_H */
