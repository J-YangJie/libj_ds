/*
  Vector Implementations
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

#include <vector/vector.h>

#include <_log.h>
#include <_memory.h>
#include <linux/_compiler.h>
#include <iterator/iterator.h>

#define TAG "[vector]"

static __always_inline vector_size_t __vector_size(const vector_t* _this)
{
    return _this->size;
}

static __always_inline vector_size_t _vector_size(const vector_t* _this)
{
    if (unlikely(is_null(_this)))
        return -1;
    return __vector_size(_this);
}

static __always_inline vector_size_t __vector_capacity(const vector_t* _this)
{
    return _this->capacity;
}

static __always_inline vector_size_t _vector_capacity(const vector_t* _this)
{
    if (unlikely(is_null(_this)))
        return -1;
    return __vector_capacity(_this);
}

static __always_inline vector_size_t __vector_capacity_expansion(vector_t* _this)
{
    if (_this->size <= 0)
        _this->capacity = 1;
    else
        _this->capacity = _this->size << 1;
    return _this->capacity;
}

static __always_inline vector_size_t __vector_capacity_try_expansion(vector_t* _this)
{
    return _this->size <= 0 ? 1 : (_this->size << 1);
}

static inline vector_count_t __vector_count(const vector_t* _this, vector_data_t data)
{
    vector_size_t ret = 0;
    vector_size_t i = 0;

    if (is_null(_this->ops) || is_null(_this->ops->__lt)) {
        for (i = 0; i < __vector_size(_this); ++i) {
            if (data == _this->head[i].data)
                ret++;
        }
    } else {
        for (i = 0; i < __vector_size(_this); ++i) {
            if (!_this->ops->__lt(data, _this->head[i].data) && !_this->ops->__lt(_this->head[i].data, data))
                ret++;
        }
    }

    return ret;
}

static __always_inline vector_count_t _vector_count(const vector_t* _this, vector_data_t data)
{
    if (unlikely(is_null(_this)))
        return -1;
    return __vector_count(_this, data);
}

static __always_inline vector_count_t vector_count(const vector_t* _this, vector_data_t data)
{
    if (unlikely(is_null(_this)))
        return -1;

    if (!is_null(_this->ops) && !is_null(_this->ops->valid_data) && !_this->ops->valid_data(data))
        return -1;

    return __vector_count(_this, data);
}

static __always_inline vector_node_t* __vector_first(const vector_t* _this)
{
    if (__vector_size(_this) <= 0)
        return NULL;
    return _this->head;
}

static __always_inline vector_node_t* __vector_last(const vector_t* _this)
{
    if (__vector_size(_this) <= 0)
        return NULL;
    return &_this->head[__vector_size(_this) - 1];
}

static __always_inline vector_node_t* __vector_end(const vector_t* _this)
{
    return (vector_node_t*)iterator_end();
}

static __always_inline vector_node_t* __vector_begin(const vector_t* _this)
{
    if (__vector_size(_this) <= 0)
        return __vector_end(_this);
    return _this->head;
}

static __always_inline vector_node_t* _vector_begin(const vector_t* _this)
{
    if (unlikely(is_null(_this)))
        return NULL;
    return __vector_begin(_this);
}

static __always_inline vector_node_t* __vector_next(const vector_t* _this, const vector_node_t* node)
{
    vector_node_t* t = (vector_node_t*)node;

    if (__vector_size(_this) <= 0 || __vector_end(_this) == node)
        return __vector_end(_this);

    /* The input parameter is `iterator`, and there's no need 
       to check whether it equals `rend` */

    if (__vector_last(_this) == node)
        return __vector_end(_this);

    return t + 1;
}

static __always_inline vector_node_t* _vector_next(const vector_t* _this, const vector_node_t* node)
{
    if (unlikely(is_null(_this) || is_null(node)))
        return NULL;
    return __vector_next(_this, node);
}

static __always_inline vector_node_t* __vector_prev(const vector_t* _this, const vector_node_t* node)
{
    vector_node_t* t = (vector_node_t*)node;

    if (__vector_size(_this) <= 0)
        return __vector_end(_this);

    if (__vector_end(_this) == node)
        return __vector_last(_this);

    if (__vector_first(_this) == node)
        return __vector_end(_this);

    return t - 1;
}

static __always_inline vector_node_t* _vector_prev(const vector_t* _this, const vector_node_t* node)
{
    if (unlikely(is_null(_this) || is_null(node)))
        return NULL;
    return __vector_prev(_this, node);
}

static __always_inline vector_node_t* __vector_rend(const vector_t* _this)
{
    return (vector_node_t*)iterator_rend();
}

static __always_inline vector_node_t* __vector_rbegin(const vector_t* _this)
{
    if (__vector_size(_this) <= 0)
        return __vector_rend(_this);
    return &_this->head[__vector_size(_this) - 1];
}

static __always_inline vector_node_t* _vector_rbegin(const vector_t* _this)
{
    if (unlikely(is_null(_this)))
        return NULL;
    return __vector_rbegin(_this);
}

static __always_inline vector_node_t* __vector_rnext(const vector_t* _this, const vector_node_t* node)
{
    vector_node_t* t = (vector_node_t*)node;

    if (__vector_size(_this) <= 0 || __vector_rend(_this) == node)
        return __vector_rend(_this);

    /* The input parameter is `reverse_iterator`, and there's no need 
       to check whether it equals `end` */

    if (__vector_first(_this) == node)
        return __vector_rend(_this);

    return t - 1;
}

static __always_inline vector_node_t* _vector_rnext(const vector_t* _this, const vector_node_t* node)
{
    if (unlikely(is_null(_this) || is_null(node)))
        return NULL;
    return __vector_rnext(_this, node);
}

static __always_inline vector_node_t* __vector_rprev(const vector_t* _this, const vector_node_t* node)
{
    vector_node_t* t = (vector_node_t*)node;

    if (__vector_size(_this) <= 0)
        return __vector_rend(_this);

    if (__vector_rend(_this) == node)
        return __vector_first(_this);

    if (__vector_last(_this) == node)
        return __vector_rend(_this);

    return t + 1;
}

static __always_inline vector_node_t* _vector_rprev(const vector_t* _this, const vector_node_t* node)
{
    if (unlikely(is_null(_this) || is_null(node)))
        return NULL;
    return __vector_rprev(_this, node);
}

static __always_inline vector_node_t* _vector_at(const vector_t* _this, vector_size_t n)
{
    if (is_null(_this) || __vector_size(_this) <= 0)
        return NULL;
    return &_this->head[n];
}

static __always_inline vector_data_t vector_first(const vector_t* _this, vector_data_t default_data)
{
    if (is_null(_this) || __vector_size(_this) <= 0)
        return default_data;
    return __vector_first(_this)->data;
}

static __always_inline vector_data_t vector_last(const vector_t* _this, vector_data_t default_data)
{
    if (is_null(_this) || __vector_size(_this) <= 0)
        return default_data;
    return __vector_last(_this)->data;
}

static inline vector_node_t* __vector_find(const vector_t* _this, vector_data_t data)
{
    vector_size_t i = 0;
    vector_node_t* t = NULL;

    if (is_null(_this->ops) || is_null(_this->ops->__lt)) {
        for (i = 0; i < __vector_size(_this); ++i) {
            t = &_this->head[i];
            if (data == t->data)
                return t;
        }
    } else {
        for (i = 0; i < __vector_size(_this); ++i) {
            t = &_this->head[i];
            if (!_this->ops->__lt(data, t->data) && !_this->ops->__lt(t->data, data))
                return t;
        }
    }
    return NULL;
}

static __always_inline vector_node_t* _vector_find(const vector_t* _this, vector_data_t data)
{
    if (unlikely(is_null(_this)))
        return NULL;
    return __vector_find(_this, data);
}

static __always_inline vector_node_t* vector_find(const vector_t* _this, vector_data_t data)
{
    vector_node_t* t = NULL;

    if (unlikely(is_null(_this)))
        return NULL;

    if (!is_null(_this->ops) && !is_null(_this->ops->valid_data) && !_this->ops->valid_data(data))
        return NULL;

    t = __vector_find(_this, data);
    return is_null(t) ? __vector_end(_this) : t;
}

static inline vector_node_t* __vector_insert_core(vector_t* _this, vector_size_t index, vector_data_t data)
{
    vector_size_t i = 0;
    vector_size_t d = index;
    vector_node_t* n = NULL;
    vector_data_t tdata = data;

    if (!is_null(_this->ops) && !is_null(_this->ops->valid_data) && !_this->ops->valid_data(data))
        return NULL;

    if (!is_null(_this->ops) && !is_null(_this->ops->copy_data) && !_this->ops->copy_data(data, &tdata))
        return NULL;

    if (__vector_capacity(_this) == __vector_size(_this)) {
        n = p_realloc(__vector_first(_this), __vector_capacity_try_expansion(_this) * sizeof(vector_node_t));
        if (is_null(n))
            goto err;

        if (_this->head != n) {
            pr_notice("Head [ %p -> %p ], capacity [ %zd -> %zd ] by [ insert_core ]", 
                        _this->head, n, _this->capacity, __vector_capacity_try_expansion(_this));
            _this->head = n;
        } else {
            pr_notice("Head [ %p ], capacity [ %zd -> %zd ] by [ insert_core ]", 
                        n, _this->capacity, __vector_capacity_try_expansion(_this));
        }

        __vector_capacity_expansion(_this);
    }

    for (i = __vector_size(_this); i > d; --i) {
        _this->head[i].data = _this->head[i - 1].data;
    }
    _this->head[i].data = tdata;
    _this->size++;

    return &_this->head[i];

err:
    if (!is_null(_this->ops) && !is_null(_this->ops->free_data) && !is_null(_this->ops->copy_data))
        _this->ops->free_data(&tdata);
    return NULL;
}

static __always_inline vector_node_t* vector_push_back(vector_t* _this, vector_data_t data)
{
    if (unlikely(is_null(_this)))
        return NULL;
    return __vector_insert_core(_this, __vector_size(_this), data);
}

static __always_inline vector_node_t* vector_push_front(vector_t* _this, vector_data_t data)
{
    if (unlikely(is_null(_this)))
        return NULL;
    return __vector_insert_core(_this, 0, data);
}

static __always_inline vector_node_t* vector_insert(vector_t* _this, vector_node_t* pos, vector_data_t data)
{
    vector_size_t d = 0;

    /* The input parameter is `iterator`, and there's no need 
       to check whether it equals `rend` */
    if (unlikely(is_null(_this) || is_null(pos)/* || __vector_rend(_this) == pos*/))
        return NULL;

    if (__vector_size(_this) <= 0 && __vector_end(_this) != pos)
        return NULL;

    if (__vector_end(_this) == pos)
        return __vector_insert_core(_this, __vector_size(_this), data);

    d = pos - _this->head;
    if (d >= __vector_size(_this) || d < 0)
        return NULL;

    return __vector_insert_core(_this, d, data);
}

static inline void __vector_erase_core(vector_t* _this, vector_size_t index)
{
    vector_size_t i = 0;
    vector_size_t d = index;

    if (!is_null(_this->ops) && !is_null(_this->ops->free_data))
        _this->ops->free_data(&_this->head[d].data);

    for (i = d; i < __vector_size(_this) - 1; ++i) {
        _this->head[i].data = _this->head[i + 1].data;
    }

    _this->size--;
}

static __always_inline vector_node_t* vector_erase(vector_t* _this, vector_node_t* pos)
{
    vector_size_t d = 0;

    if (unlikely(is_null(_this) || is_null(pos)))
        return NULL;

    /* The input parameter is `iterator`, and there's no need 
       to check whether it equals `rend` */
    if (__vector_size(_this) <= 0 || __vector_end(_this) == pos/* || __vector_rend(_this) == pos*/)
        return NULL;

    d = pos - _this->head;
    if (d >= __vector_size(_this) || d < 0)
        return NULL;

    __vector_erase_core(_this, d);

    return d >= __vector_size(_this) ? __vector_end(_this) : &_this->head[d];
}

static inline vector_node_t* vector_erase_range(vector_t* _this, vector_node_t* begin, vector_node_t* end)
{
    vector_size_t i, d1, d2, dd = 0;

    if (unlikely(is_null(_this) || is_null(begin) || is_null(end)))
        return NULL;

    /* The input parameter is `iterator`, and there's no need 
       to check whether it equals `rend` */
    if (__vector_size(_this) <= 0 || __vector_rend(_this) == begin/* || __vector_rend(_this) == end*/)
        return NULL;

    /* Both the `begin` and `end` are equal to `end()`, because of [begin, end). The idea is derived from the C++ STL. */
    d1 = __vector_end(_this) == begin ? __vector_size(_this) : begin - _this->head;
    d2 = __vector_end(_this) == end   ? __vector_size(_this) : end   - _this->head;
    if (d1 > __vector_size(_this) || d1 < 0 || d2 > __vector_size(_this) || d2 < 0 || d1 > d2)
        return NULL;

    dd = d2 - d1;
    if (0 == dd)
        goto end;

    if (is_null(_this->ops) || is_null(_this->ops->free_data)) {
        for (i = d1; i + dd < __vector_size(_this); ++i) {
            _this->head[i].data = _this->head[i + dd].data;
        }
    } else {
        for (i = d1; i < d2 || i + dd < __vector_size(_this); ++i) {
            if (i < d2)
                _this->ops->free_data(&_this->head[i].data);
            if (i + dd < __vector_size(_this))
                _this->head[i].data = _this->head[i + dd].data;
        }
    }

    _this->size -= dd;
    if (_this->size < 0)
        _this->size = 0;

end:
    return d1 >= __vector_size(_this) ? __vector_end(_this) : &_this->head[d1];
}

static __always_inline void vector_pop_back(vector_t* _this)
{
    if (unlikely(is_null(_this)))
        return ;

    if (__vector_size(_this) <= 0)
        return ;

    __vector_erase_core(_this, __vector_size(_this) - 1);
}

static __always_inline void vector_pop_front(vector_t* _this)
{
    if (unlikely(is_null(_this)))
        return ;

    if (__vector_size(_this) <= 0)
        return ;

    __vector_erase_core(_this, 0);
}

static inline vector_size_t vector_remove(vector_t* _this, vector_data_t data)
{
    vector_size_t ret = 0;
    vector_size_t l = 0, r = 0;

    if (unlikely(is_null(_this)))
        return -1;

    if (!is_null(_this->ops) && !is_null(_this->ops->valid_data) && !_this->ops->valid_data(data))
        return -1;

    if (is_null(_this->ops) || is_null(_this->ops->__lt)) {
        for (r = 0; r < __vector_size(_this); ++r) {
            if (data != _this->head[r].data) {
                _this->head[l].data = _this->head[r].data;
                l++;
            } else {
                if (!is_null(_this->ops) && !is_null(_this->ops->free_data))
                    _this->ops->free_data(&_this->head[r].data);
                ret++;    
            }
        }
    } else {
        for (r = 0; r < __vector_size(_this); ++r) {
            if (_this->ops->__lt(data, _this->head[r].data) || _this->ops->__lt(_this->head[r].data, data)) {
                _this->head[l].data = _this->head[r].data;
                l++;
            } else {
                if (!is_null(_this->ops) && !is_null(_this->ops->free_data))
                    _this->ops->free_data(&_this->head[r].data);
                ret++;    
            }
        }
    }

    _this->size -= ret;
    if (_this->size < 0)
        _this->size = 0;
    return ret;
}

static inline vector_size_t vector_remove_if(vector_t* _this, remove_if_condition cond)
{
    vector_size_t ret = 0;
    vector_size_t l = 0, r = 0;

    if (unlikely(is_null(_this) || is_null(cond)))
        return -1;

    if (is_null(_this->ops) || is_null(_this->ops->free_data)) {
        for (r = 0; r < __vector_size(_this); ++r) {
            if (!cond(_this->head[r].data)) {
                _this->head[l].data = _this->head[r].data;
                l++;
            } else {
                ret++;    
            }
        }
    } else {
        for (r = 0; r < __vector_size(_this); ++r) {
            if (!cond(_this->head[r].data)) {
                _this->head[l].data = _this->head[r].data;
                l++;
            } else {
                _this->ops->free_data(&_this->head[r].data);
                ret++;    
            }
        }
    }

    _this->size -= ret;
    if (_this->size < 0)
        _this->size = 0;
    return ret;
}

static inline bool vector_reserve(vector_t* _this, vector_size_t n)
{
    vector_node_t* head_n = NULL;

    if (unlikely(is_null(_this) || n < 0))
        return false;

    /* TODO: return true/false or no return */
    if (n <= __vector_capacity(_this))
        return true;

    head_n = p_realloc(__vector_first(_this), n * sizeof(vector_node_t));
    if (is_null(head_n))
        return false;

    if (_this->head != head_n) {
        pr_notice("Head [ %p -> %p ], capacity [ %zd -> %zd ] by [ reserve ]", 
                    _this->head, head_n, _this->capacity, n);
        _this->head = head_n;
    } else {
        pr_notice("Head [ %p ], capacity [ %zd -> %zd ] by [ reserve ]", 
                    head_n, _this->capacity, n);
    }

    _this->capacity = n;
    return true;    
}

static inline bool vector_resize(vector_t* _this, vector_size_t n, vector_data_t default_data)
{
    vector_size_t i;
    vector_node_t* head_n = NULL;

    if (unlikely(is_null(_this) || n < 0))
        return false;

    if (n < __vector_size(_this))
        return NULL != vector_erase_range(_this, &_this->head[n], __vector_end(_this));

    if (n <= __vector_capacity(_this)) {
        for (i = __vector_size(_this); i < n; ++i)
            _this->head[i].data = default_data;
        _this->size = n;
        return true;
    }

    if (n <= __vector_capacity_try_expansion(_this))
        head_n = p_realloc(__vector_first(_this), __vector_capacity_try_expansion(_this) * sizeof(vector_node_t));
    else
        head_n = p_realloc(__vector_first(_this), n * sizeof(vector_node_t));
    if (is_null(head_n))
        return false;

    if (_this->head != head_n) {
        pr_notice("Head [ %p -> %p ], capacity [ %zd -> %zd ] by [ resize ]", 
                    _this->head, head_n, _this->capacity, __vector_capacity_try_expansion(_this));
        _this->head = head_n;
    } else {
        pr_notice("Head [ %p ], capacity [ %zd -> %zd ] by [ resize ]", 
                    head_n, _this->capacity, __vector_capacity_try_expansion(_this));
    }

    if (n > __vector_capacity_expansion(_this))
        _this->capacity = n;

    for (i = __vector_size(_this); i < n; ++i)
        _this->head[i].data = default_data;
    _this->size = n;
    return true;
}

#include <sort/sort.h>
static __always_inline bool __vector_sort_comp(vector_data_t l, vector_data_t r)
{
    return l < r;
}

static __always_inline void __vector_sort(vector_t* _this, __comp __comp)
{
    if (is_null(__comp))
        __comp = is_null(_this->ops) || is_null(_this->ops->__lt) ? __vector_sort_comp : _this->ops->__lt;
    __sort_quick(&_this->head[0].data, __vector_size(_this), __comp);
}

static __always_inline void vector_sort(vector_t* _this, __comp __comp)
{
    if (is_null(_this) || __vector_size(_this) < 2)
        return ;
    __vector_sort(_this, __comp);
}

static __always_inline vector_size_t vector_clear(vector_t* _this)
{
    vector_size_t ret = 0;
    vector_node_t* t = NULL;

    if (unlikely(is_null(_this)))
        return -1;

    ret = __vector_size(_this);
    if (ret <= 0)
        return ret;

    t = vector_erase_range(_this, __vector_begin(_this), __vector_end(_this));
    if (is_null(t))
        return -1;

    return ret - __vector_size(_this);
}

__always_inline void __vector_init(vector_t* vector)
{
    vector->head = NULL;
    vector->capacity = 0;
}

__always_inline void __vector_deinit(vector_t* vector)
{
    vector_clear(vector);
    p_free(vector->head);

    vector->ops = NULL;
    vector->head = NULL;
    vector->size = 0;
    vector->capacity = 0;
}

typedef vector_iterator_t* (*fp_end)(const vector_t* _this);
typedef vector_iterator_t* (*fp_begin)(const vector_t* _this);
typedef vector_iterator_t* (*fp_next)(const vector_t* _this, const vector_iterator_t* iterator);
typedef vector_iterator_t* (*fp_prev)(const vector_t* _this, const vector_iterator_t* iterator);
typedef vector_r_iterator_t* (*fp_rend)(const vector_t* _this);
typedef vector_r_iterator_t* (*fp_rbegin)(const vector_t* _this);
typedef vector_r_iterator_t* (*fp_rnext)(const vector_t* _this, const vector_r_iterator_t* r_iterator);
typedef vector_r_iterator_t* (*fp_rprev)(const vector_t* _this, const vector_r_iterator_t* r_iterator);
typedef vector_iterator_t* (*fp_at)(const vector_t* _this, vector_size_t n);
typedef vector_iterator_t* (*fp_find)(const vector_t* _this, vector_data_t data);
typedef vector_iterator_t* (*fp_push_back)(vector_t* _this, vector_data_t data);
typedef vector_iterator_t* (*fp_push_front)(vector_t* _this, vector_data_t data);
typedef vector_iterator_t* (*fp_insert)(vector_t* _this, vector_iterator_t* iterator, vector_data_t data);
typedef vector_iterator_t* (*fp_erase)(vector_t* _this, vector_iterator_t* iterator);
typedef vector_iterator_t* (*fp_erase_range)(vector_t* _this, vector_iterator_t* iterator_begin, vector_iterator_t* iterator_end);

__always_inline const class_vector_t* class_vector_ins(void)
{
    static const class_vector_t ins = {
        .size        = _vector_size,
        .capacity    = _vector_capacity,
        .count       = vector_count,
        .end         = (fp_end)__vector_end,
        .begin       = (fp_begin)_vector_begin,
        .next        = (fp_next)_vector_next,
        .prev        = (fp_prev)_vector_prev,
        .rend        = (fp_rend)__vector_rend,
        .rbegin      = (fp_rbegin)_vector_rbegin,
        .rnext       = (fp_rnext)_vector_rnext,
        .rprev       = (fp_rprev)_vector_rprev,
        .at          = (fp_at)_vector_at,
        .first       = vector_first,
        .last        = vector_last,
        .find        = (fp_find)vector_find,
        .push_back   = (fp_push_back)vector_push_back,
        .push_front  = (fp_push_front)vector_push_front,
        .insert      = (fp_insert)vector_insert,
        .erase       = (fp_erase)vector_erase,
        .erase_range = (fp_erase_range)vector_erase_range,
        .pop_back    = vector_pop_back,
        .pop_front   = vector_pop_front,
        .remove      = vector_remove,
        .remove_if   = vector_remove_if,
        .reserve     = vector_reserve,
        .resize      = vector_resize,
        .sort        = vector_sort,
        .clear       = vector_clear,
    };
    return &ins;
}
