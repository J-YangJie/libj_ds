/*
  List Implementations
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

#include <list/list.h>

#include <_memory.h>
#include <linux/list.h>
#include <linux/_compiler.h>
#include <iterator/iterator.h>

#define dlist_entry(ptr) list_entry((ptr), struct list_node, node)

static __always_inline list_size_t __dlist_size(const list_t* _this)
{
    return _this->size;
}

static __always_inline list_size_t _dlist_size(const list_t* _this)
{
    if (unlikely(is_null(_this)))
        return -1;
    return __dlist_size(_this);
}

static inline list_count_t __dlist_count(const list_t* _this, list_data_t data)
{
    list_size_t ret = 0;
    list_node_t* t = NULL;
    list_node_t* n = NULL;

    if (is_null(_this->ops) || is_null(_this->ops->__lt)) {
        list_for_each_entry_safe(t, n, &_this->head, node) {
            if (data != t->data)
                continue;
            ret++;
        }
    } else {
        list_for_each_entry_safe(t, n, &_this->head, node) {
            if (_this->ops->__lt(data, t->data) || _this->ops->__lt(t->data, data))
                continue;
            ret++;
        }
    }

    return ret;
}

static __always_inline list_count_t dlist_count(const list_t* _this, list_data_t data)
{
    if (unlikely(is_null(_this)))
        return -1;

    if (!is_null(_this->ops) && !is_null(_this->ops->valid_data) && !_this->ops->valid_data(data))
        return -1;

    return __dlist_count(_this, data);
}

static __always_inline list_node_t* __dlist_first(const list_t* _this)
{
    if (is_null(_this->head.next) || list_empty(&_this->head))
        return NULL;
    return dlist_entry(_this->head.next);
}

static __always_inline list_node_t* __dlist_last(const list_t* _this)
{
    if (is_null(_this->head.prev) || list_empty(&_this->head))
        return NULL;
    return dlist_entry(_this->head.prev);
}

static __always_inline list_node_t* __dlist_end(const list_t* _this)
{
    return (list_node_t*)iterator_end();
}

static __always_inline list_node_t* __dlist_begin(const list_t* _this)
{
    struct list_head* t = _this->head.next;

    if (unlikely(is_null(t)))
        return NULL; /* Err: memory has been modified illegally */

    if (list_empty(&_this->head))
        return __dlist_end(_this);
    return dlist_entry(t); /* Be careful about the situation when `t` is `NULL` */
}

static __always_inline list_node_t* _dlist_begin(const list_t* _this)
{
    if (unlikely(is_null(_this)))
        return NULL;
    return __dlist_begin(_this);
}

static __always_inline list_node_t* __dlist_next(const list_t* _this, const list_node_t* node)
{
    struct list_head* t = NULL;

    if (list_empty(&_this->head) || __dlist_end(_this) == node)
        return __dlist_end(_this);

    /* The input parameter is `iterator`, and there's no need 
       to check whether it equals `rend` */

    /* This check should come after `end` or `rend` */
    t = node->node.next;
    if (unlikely(is_null(t)))
        return NULL; /* Err: `node` doesn't belong to current list or memory has been modified illegally */

    if (&_this->head == t)
        return __dlist_end(_this);

    return dlist_entry(t);
}

static __always_inline list_node_t* _dlist_next(const list_t* _this, const list_node_t* node)
{
    if (unlikely(is_null(_this) || is_null(node)))
        return NULL;
    return __dlist_next(_this, node);
}

static __always_inline list_node_t* __dlist_prev(const list_t* _this, const list_node_t* node)
{
    struct list_head* t = NULL;

    if (list_empty(&_this->head))
        return __dlist_end(_this);

    if (__dlist_end(_this) == node)
        return __dlist_last(_this); /* Err: since the `ds` is non-empty, 
                                            the return value includes the error case of `NULL` */

    /* This check should come after `end` or `rend` */
    t = node->node.prev;
    if (unlikely(is_null(t)))
        return NULL; /* Err: `node` doesn't belong to current list or memory has been modified illegally */

    if (&_this->head == t)
        return __dlist_end(_this);

    return dlist_entry(t);
}

static __always_inline list_node_t* _dlist_prev(const list_t* _this, const list_node_t* node)
{
    if (unlikely(is_null(_this) || is_null(node)))
        return NULL;
    return __dlist_prev(_this, node);
}

static __always_inline list_node_t* __dlist_rend(const list_t* _this)
{
    return (list_node_t*)iterator_rend();
}

static __always_inline list_node_t* __dlist_rbegin(const list_t* _this)
{
    struct list_head* t = _this->head.prev;

    if (unlikely(is_null(t)))
        return NULL; /* Err: memory has been modified illegally */

    if (list_empty(&_this->head))
        return __dlist_rend(_this);
    return dlist_entry(t); /* Be careful about the situation when `t` is `NULL` */
}

static __always_inline list_node_t* _dlist_rbegin(const list_t* _this)
{
    if (unlikely(is_null(_this)))
        return NULL;
    return __dlist_rbegin(_this);
}

static __always_inline list_node_t* __dlist_rnext(const list_t* _this, const list_node_t* node)
{
    struct list_head* t = NULL;

    if (list_empty(&_this->head) || __dlist_rend(_this) == node)
        return __dlist_rend(_this);

    /* The input parameter is `reverse_iterator`, and there's no need 
       to check whether it equals `end` */

    /* This check should come after `end` or `rend` */
    t = node->node.prev;
    if (unlikely(is_null(t)))
        return NULL; /* Err: `node` doesn't belong to current list or memory has been modified illegally */

    if (&_this->head == t)
        return __dlist_rend(_this);

    return dlist_entry(t);
}

static __always_inline list_node_t* _dlist_rnext(const list_t* _this, const list_node_t* node)
{
    if (unlikely(is_null(_this) || is_null(node)))
        return NULL;
    return __dlist_rnext(_this, node);
}

static __always_inline list_node_t* __dlist_rprev(const list_t* _this, const list_node_t* node)
{
    struct list_head* t = NULL;

    if (list_empty(&_this->head))
        return __dlist_rend(_this);

    if (__dlist_rend(_this) == node)
        return __dlist_first(_this); /* Err: since the `ds` is non-empty, 
                                             the return value includes the error case of `NULL` */

    /* This check should come after `end` or `rend` */
    t = node->node.next;
    if (unlikely(is_null(t)))
        return NULL; /* Err: `node` doesn't belong to current list or memory has been modified illegally */

    if (&_this->head == t)
        return __dlist_rend(_this);

    return dlist_entry(t);
}

static __always_inline list_node_t* _dlist_rprev(const list_t* _this, const list_node_t* node)
{
    if (unlikely(is_null(_this) || is_null(node)))
        return NULL;
    return __dlist_rprev(_this, node);
}

static __always_inline list_data_t dlist_first(const list_t* _this, list_data_t default_data)
{
    if (is_null(_this) || is_null(_this->head.next) || list_empty(&_this->head))
        return default_data;
    return dlist_entry(_this->head.next)->data;
}

static __always_inline list_data_t dlist_last(const list_t* _this, list_data_t default_data)
{
    if (is_null(_this) || is_null(_this->head.prev) || list_empty(&_this->head))
        return default_data;
    return dlist_entry(_this->head.prev)->data;
}

static inline list_node_t* __dlist_find(const list_t* _this, list_data_t data)
{
    list_node_t* t = NULL;

    if (is_null(_this->ops) || is_null(_this->ops->__lt)) {
        list_for_each_entry(t, &_this->head, node) {
            if (data == t->data)
                return t;
        }
    } else {
        list_for_each_entry(t, &_this->head, node) {
            if (!_this->ops->__lt(data, t->data) && !_this->ops->__lt(t->data, data))
                return t;
        }
    }
    return NULL;
}

static __always_inline list_node_t* dlist_find(const list_t* _this, list_data_t data)
{
    list_node_t* t = NULL;

    if (unlikely(is_null(_this)))
        return NULL;

    if (!is_null(_this->ops) && !is_null(_this->ops->valid_data) && !_this->ops->valid_data(data))
        return NULL;

    t = __dlist_find(_this, data);
    return is_null(t) ? __dlist_end(_this) : t;
}

static inline list_node_t* dlist_alloc_node_and_copy_data(const list_t* _this, list_data_t data)
{
    list_node_t* t = NULL;

    if (unlikely(is_null(_this)))
        return NULL;

    if (!is_null(_this->ops) && !is_null(_this->ops->valid_data) && !_this->ops->valid_data(data))
        return NULL;

    t = (list_node_t*)p_malloc(sizeof(list_node_t));
    if (unlikely(is_null(t)))
        return NULL;

    if (is_null(_this->ops) || is_null(_this->ops->copy_data)) {
        t->data = data;
    } else {
        list_data_t tdata = t->data;

        if (!_this->ops->copy_data(data, &t->data)) {
            t->data = tdata;
            goto err;
        }
    }

    return t;

err:
    p_free(t);
    return NULL;
}

static __always_inline list_node_t* __dlist_push_back(list_t* _this, list_node_t* node)
{
    list_add_tail(&node->node, &_this->head);
    _this->size++;
    return node;
}

static __always_inline list_node_t* dlist_push_back(list_t* _this, list_data_t data)
{
    list_node_t* t = NULL;

    t = dlist_alloc_node_and_copy_data(_this, data);
    if (is_null(t))
        return NULL;

    return __dlist_push_back(_this, t);
}

static __always_inline list_node_t* __dlist_push_front(list_t* _this, list_node_t* node)
{
    list_add(&node->node, &_this->head);
    _this->size++;
    return node;
}

static __always_inline list_node_t* dlist_push_front(list_t* _this, list_data_t data)
{
    list_node_t* t = NULL;

    t = dlist_alloc_node_and_copy_data(_this, data);
    if (is_null(t))
        return NULL;

    return __dlist_push_front(_this, t);
}

static __always_inline list_node_t* __dlist_insert(list_t* _this, list_node_t* pos, list_node_t* node)
{
    __list_add(&node->node, pos->node.prev, &pos->node);
    _this->size++;
    return node;
}

static __always_inline list_node_t* dlist_insert(list_t* _this, list_node_t* pos, list_data_t data)
{
    list_node_t* t = NULL;

    /* The input parameter is `iterator`, and there's no need 
       to check whether it equals `rend` */
    if (is_null(_this) || is_null(pos)/* || __dlist_rend(_this) == pos*/)
        return NULL;

    t = dlist_alloc_node_and_copy_data(_this, data);
    if (is_null(t))
        return NULL;

    if (__dlist_end(_this) == pos)
        return __dlist_push_back(_this, t);
    return __dlist_insert(_this, pos, t);
}

static __always_inline void __dlist_erase(list_t* _this, list_node_t* pos)
{
    list_del_init(&pos->node);
    _this->size--;
}

static __always_inline list_node_t* dlist_erase(list_t* _this, list_node_t* pos)
{
    list_node_t* t = NULL;

    if (unlikely(is_null(_this) || is_null(pos)))
        return NULL;

    /* The input parameter is `iterator`, and there's no need 
       to check whether it equals `rend` */
    if (list_empty(&_this->head) || __dlist_end(_this) == pos/* || __dlist_rend(_this) == pos*/)
        return NULL;

    t = __dlist_next(_this, pos);
    if (is_null(t))
        return NULL;

    __dlist_erase(_this, pos);

    if (!is_null(_this->ops) && !is_null(_this->ops->free_data))
        _this->ops->free_data(&pos->data);

    p_free(pos);

    return t;
}

static inline list_node_t* dlist_erase_range(list_t* _this, list_node_t* begin, list_node_t* end)
{
    list_node_t* pos = begin;
    list_node_t* n = NULL;

    if (unlikely(is_null(_this) || is_null(begin) || is_null(end)))
        return NULL;

    /* Both `begin` and `end` can be equal to end() */
    if (list_empty(&_this->head)/* || __dlist_rend(_this) == begin || __dlist_rend(_this) == end*/)
        return NULL;

    if (__dlist_end(_this) == begin)
        return __dlist_end(_this) == end ? __dlist_end(_this) : NULL;

    list_for_each_entry_safe_from(pos, n, &_this->head, node) {
        if (__dlist_end(_this) != end && (&end->node == &pos->node))
            break;

        __dlist_erase(_this, pos);

        if (!is_null(_this->ops) && !is_null(_this->ops->free_data))
            _this->ops->free_data(&pos->data);

        p_free(pos);
    }

    /* TODO: if `end` is not equal to end(), but still satisfies the judgment, indicating
             that there is an error in the parameters. Do we need to return an error? */
    if (&_this->head == &pos->node)
        return __dlist_end(_this);
    return end;
}

static __always_inline void __dlist_pop_back(list_t* _this)
{
    struct list_head* t = _this->head.prev;

    if (unlikely(is_null(t)))
        return ; /* Err: memory has been modified illegally */

    if (!list_empty(&_this->head)) {
        list_del_init(t);
        _this->size--;
    }
}

static __always_inline void dlist_pop_back(list_t* _this)
{
    list_node_t* t = NULL;

    if (unlikely(is_null(_this)))
        return ;

    t = __dlist_last(_this);
    if (is_null(t))
        return ;

    __dlist_pop_back(_this);

    if (!is_null(_this->ops) && !is_null(_this->ops->free_data))
        _this->ops->free_data(&t->data);

    p_free(t);
}

static __always_inline void __dlist_pop_front(list_t* _this)
{
    struct list_head* t = _this->head.next;

    if (unlikely(is_null(t)))
        return ; /* Err: memory has been modified illegally */

    if (!list_empty(&_this->head)) {
        list_del_init(t);
        _this->size--;
    }
}

static __always_inline void dlist_pop_front(list_t* _this)
{
    list_node_t* t = NULL;

    if (unlikely(is_null(_this)))
        return ;

    t = __dlist_first(_this);
    if (is_null(t))
        return ;

    __dlist_pop_front(_this);

    if (!is_null(_this->ops) && !is_null(_this->ops->free_data))
        _this->ops->free_data(&t->data);

    p_free(t);
}

static inline list_size_t dlist_remove(list_t* _this, list_data_t data)
{
    list_size_t ret = 0;
    list_node_t* t = NULL;
    list_node_t* n = NULL;

    if (unlikely(is_null(_this)))
        return -1;

    if (!is_null(_this->ops) && !is_null(_this->ops->valid_data) && !_this->ops->valid_data(data))
        return -1;

    if (is_null(_this->ops) || is_null(_this->ops->__lt)) {
        list_for_each_entry_safe(t, n, &_this->head, node) {
            if (data != t->data)
                continue;

            __dlist_erase(_this, t);

            if (!is_null(_this->ops) && !is_null(_this->ops->free_data))
                _this->ops->free_data(&t->data);

            p_free(t);
            ret++;
        }
    } else {
        list_for_each_entry_safe(t, n, &_this->head, node) {
            if (_this->ops->__lt(data, t->data) || _this->ops->__lt(t->data, data))
                continue;

            __dlist_erase(_this, t);

            if (!is_null(_this->ops) && !is_null(_this->ops->free_data))
                _this->ops->free_data(&t->data);

            p_free(t);
            ret++;
        }
    }

    return ret;
}

static inline list_size_t dlist_remove_if(list_t* _this, remove_if_condition cond)
{
    list_size_t ret = 0;
    list_node_t* t = NULL;
    list_node_t* n = NULL;

    if (unlikely(is_null(_this) || is_null(cond)))
        return -1;

    list_for_each_entry_safe(t, n, &_this->head, node) {
        if (!cond(t->data))
            continue;

        __dlist_erase(_this, t);

        if (!is_null(_this->ops) && !is_null(_this->ops->free_data))
            _this->ops->free_data(&t->data);

        p_free(t);
        ret++;
    }

    return ret;
}

static inline list_size_t dlist_clear(list_t* _this)
{
    list_size_t ret = 0;
    list_node_t* t = NULL;
    list_node_t* n = NULL;

    if (unlikely(is_null(_this)))
        return -1;

    list_for_each_entry_safe(t, n, &_this->head, node) {
        __dlist_erase(_this, t);

        if (!is_null(_this->ops) && !is_null(_this->ops->free_data))
            _this->ops->free_data(&t->data);

        p_free(t);
        ret++;
    }

    return ret;
}

__always_inline void __list_init(list_t* list)
{
    INIT_LIST_HEAD(&list->head);
}

__always_inline void __list_deinit(list_t* list)
{
    dlist_clear(list);

    list->ops = NULL;
    INIT_LIST_HEAD(&list->head);
    list->size = 0;
}

typedef list_iterator_t* (*fp_end)(const list_t* _this);
typedef list_iterator_t* (*fp_begin)(const list_t* _this);
typedef list_iterator_t* (*fp_next)(const list_t* _this, const list_iterator_t* iterator);
typedef list_iterator_t* (*fp_prev)(const list_t* _this, const list_iterator_t* iterator);
typedef list_r_iterator_t* (*fp_rend)(const list_t* _this);
typedef list_r_iterator_t* (*fp_rbegin)(const list_t* _this);
typedef list_r_iterator_t* (*fp_rnext)(const list_t* _this, const list_r_iterator_t* r_iterator);
typedef list_r_iterator_t* (*fp_rprev)(const list_t* _this, const list_r_iterator_t* r_iterator);
typedef list_iterator_t* (*fp_find)(const list_t* _this, list_data_t data);
typedef list_iterator_t* (*fp_push_back)(list_t* _this, list_data_t data);
typedef list_iterator_t* (*fp_push_front)(list_t* _this, list_data_t data);
typedef list_iterator_t* (*fp_insert)(list_t* _this, list_iterator_t* iterator, list_data_t data);
typedef list_iterator_t* (*fp_erase)(list_t* _this, list_iterator_t* iterator);
typedef list_iterator_t* (*fp_erase_range)(list_t* _this, list_iterator_t* iterator_begin, list_iterator_t* iterator_end);

__always_inline const class_list_t* class_list_ins(void)
{
    static const class_list_t ins = {
        .size        = _dlist_size,
        .count       = dlist_count,
        .end         = (fp_end)__dlist_end,
        .begin       = (fp_begin)_dlist_begin,
        .next        = (fp_next)_dlist_next,
        .prev        = (fp_prev)_dlist_prev,
        .rend        = (fp_rend)__dlist_rend,
        .rbegin      = (fp_rbegin)_dlist_rbegin,
        .rnext       = (fp_rnext)_dlist_rnext,
        .rprev       = (fp_rprev)_dlist_rprev,
        .first       = dlist_first,
        .last        = dlist_last,
        .find        = (fp_find)dlist_find,
        .push_back   = (fp_push_back)dlist_push_back,
        .push_front  = (fp_push_front)dlist_push_front,
        .insert      = (fp_insert)dlist_insert,
        .erase       = (fp_erase)dlist_erase,
        .erase_range = (fp_erase_range)dlist_erase_range,
        .pop_back    = dlist_pop_back,
        .pop_front   = dlist_pop_front,
        .remove      = dlist_remove,
        .remove_if   = dlist_remove_if,
        .clear       = dlist_clear,
    };
    return &ins;
}





// #include <multimap/multimap.h>
// typedef multimap_value_node_t* (*fp_multimap_values_end)(const multimap_values_t* _this);
// typedef multimap_value_node_t* (*fp_multimap_values_rend)(const multimap_values_t* _this);
// typedef multimap_value_node_t* (*fp_multimap_values_begin)(const multimap_values_t* _this);
// typedef multimap_value_node_t* (*fp_multimap_values_rbegin)(const multimap_values_t* _this);
// typedef multimap_value_node_t* (*fp_multimap_values_next)(const multimap_values_t* _this, const multimap_value_node_t* node);
// typedef multimap_value_node_t* (*fp_multimap_values_prev)(const multimap_values_t* _this, const multimap_value_node_t* node);
// typedef multimap_value_node_t* (*fp_multimap_values_find)(const multimap_values_t* _this, multimap_value_t value);
// typedef multimap_value_node_t* (*fp_multimap_values_push_back)(multimap_values_t* _this, multimap_value_t value);
// typedef multimap_value_node_t* (*fp_multimap_values_push_front)(multimap_values_t* _this, multimap_value_t value);
// typedef multimap_value_node_t* (*fp_multimap_values_insert)(multimap_values_t* _this, multimap_value_node_t* pos, multimap_value_t value);
// typedef multimap_value_node_t* (*fp_multimap_values_erase)(multimap_values_t* _this, multimap_value_node_t* pos);
// typedef multimap_value_node_t* (*fp_multimap_values_erase_range)(multimap_values_t* _this, multimap_value_node_t* begin, multimap_value_node_t* end);
// __always_inline const class_multimap_values_t* class_multimap_values_ins(void)
// {
//     static const class_multimap_values_t ins = {
//         .size        = _dlist_size,
//         .count       = dlist_count,
//         .end         = (fp_multimap_values_end)__dlist_end,
//         .rend        = (fp_multimap_values_rend)__dlist_rend,
//         .begin       = (fp_multimap_values_begin)_dlist_begin,
//         .rbegin      = (fp_multimap_values_rbegin)_dlist_rbegin,
//         .next        = (fp_multimap_values_next)_dlist_next,
//         .prev        = (fp_multimap_values_prev)_dlist_prev,
//         .first       = _dlist_first,
//         .last        = _dlist_last,
//         .find        = (fp_multimap_values_find)dlist_find,
//         .push_back   = (fp_multimap_values_push_back)dlist_push_back,
//         .push_front  = (fp_multimap_values_push_front)dlist_push_front,
//         .insert      = (fp_multimap_values_insert)dlist_insert,
//         .erase       = (fp_multimap_values_erase)dlist_erase,
//         .erase_range = (fp_multimap_values_erase_range)dlist_erase_range,
//         .pop_back    = dlist_pop_back,
//         .pop_front   = dlist_pop_front,
//         .remove      = dlist_remove,
//         .remove_if   = dlist_remove_if,
//         .clear       = dlist_clear,
//     };
//     return &ins;
// }
