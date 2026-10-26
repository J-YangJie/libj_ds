/*
  List Implementations
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

#include <list/list.h>

#include <_log.h>
#include <_memory.h>
#include <_compiler_inter.h>
#include <linux/_compiler.h>
#include <string.h>

#define TAG "[list]"

/* checked */
static inline
bool __i_dlist_slot_write(const i_list_t* _this, uint8_t* slot, list_data_t data)
{
    if (!is_null(_this->ops) && !is_null(_this->ops->copy_data))
        return _this->ops->copy_data(data, (list_data_t*)slot);

    switch (_this->step)
    {
    case 1: *(uint8_t*)slot  = (uint8_t)data;  break;
    case 2: *(uint16_t*)slot = (uint16_t)data; break;
    case 4: *(uint32_t*)slot = (uint32_t)data; break;
#ifdef JDSC_64BIT
    case 8: *(uint64_t*)slot = (uint64_t)data; break;
#endif /* JDSC_64BIT */
    default: return false;
    }
    return true;
}

/* checked */
static inline
list_data_t __i_dlist_slot_read(const i_list_t* _this, const uint8_t* slot)
{
    const list_dtype_t dtype = _this->dtype;

    if (dtype & SEQ_CNTR_DTYPE_MASK_STUN) {
        return (list_data_t)(uintptr_t)slot;
    } else if (dtype & SEQ_CNTR_DTYPE_MASK_SINT) {
        switch (_this->step)
        {
        case 1: return (int8_t)*(const int8_t*)slot;
        case 2: return (int16_t)*(const int16_t*)slot;
        case 4: return (int32_t)*(const int32_t*)slot;
#ifdef JDSC_64BIT
        case 8: return (int64_t)*(const int64_t*)slot;
#endif /* JDSC_64BIT */
        default: return (list_data_t)(uintptr_t)slot;
        }
    } else if (dtype & SEQ_CNTR_DTYPE_MASK_UINT) {
        switch (_this->step)
        {
        case 1: return *(const uint8_t*)slot;
        case 2: return *(const uint16_t*)slot;
        case 4: return *(const uint32_t*)slot;
#ifdef JDSC_64BIT
        case 8: return (list_data_t)*(const uint64_t*)slot;
#endif /* JDSC_64BIT */
        default: return (list_data_t)(uintptr_t)slot;
        }
    } else {
        return (list_data_t)(uintptr_t)slot;
    }
}

/* checked */
static inline
bool __i_dlist_slot_equal(list_step_t step, list_data_t data, const uint8_t* slot)
{
    switch (step) {
    case 1: return (uint8_t)data  == *(const uint8_t*)slot;
    case 2: return (uint16_t)data == *(const uint16_t*)slot;
    case 4: return (uint32_t)data == *(const uint32_t*)slot;
#ifdef JDSC_64BIT
    case 8: return (uint64_t)data == *(const uint64_t*)slot;
#endif /* JDSC_64BIT */
    default: return false;
    }
}

#if JDSC_ALIGN
#define __i_dlist_node_pad(_this)   ((_this)->pad)
#else
#define __i_dlist_node_pad(_this)   0
#endif

/* checked */
static inline
list_node_t* __i_dlist_make_list_node(const i_list_t* _this, list_data_t data)
{
    size_t pad = __i_dlist_node_pad(_this);
    uint8_t* base = (uint8_t*)p_aligned_malloc(_this->align, pad + sizeof(list_node_t) + _this->step);
    list_node_t* n;

    if (is_null(base))
        return NULL;

    n = (list_node_t*)(base + pad);

    if (!__i_dlist_slot_write(_this, n->data, data)) {
        p_aligned_free(base);
        return NULL;
    }

    return n;
}

/* checked */
static inline
void __i_dlist_destroy_list_node(const i_list_t* _this, list_node_t* node)
{
    size_t pad = __i_dlist_node_pad(_this);
    uint8_t* base = (uint8_t*)node - pad;

    if (!is_null(_this->ops) && !is_null(_this->ops->free_data))
        _this->ops->free_data((list_data_t*)node->data);

    p_aligned_free(base);
}

/* checked */
static inline
list_node_t* __i_dlist_push_back(i_list_t* _this, list_data_t data)
{
    list_node_t* n;

    if (unlikely(!is_null(_this->ops) && !is_null(_this->ops->valid_data) && !_this->ops->valid_data(data)))
        return NULL;

    n = __i_dlist_make_list_node(_this, data);
    if (is_null(n))
        return NULL;

    list_add_tail(&n->node, &_this->head);
    _this->size++;
    return n;
}

/* checked */
bool i_dlist_push_back(i_list_t* _this, list_data_t data)
{
    if (unlikely(is_null(_this)))
        return false;
    return !is_null(__i_dlist_push_back(_this, data));
}

/* checked */
static inline
list_node_t* __i_dlist_push_front(i_list_t* _this, list_data_t data)
{
    list_node_t* n;

    if (unlikely(!is_null(_this->ops) && !is_null(_this->ops->valid_data) && !_this->ops->valid_data(data)))
        return NULL;

    n = __i_dlist_make_list_node(_this, data);
    if (is_null(n))
        return NULL;

    list_add(&n->node, &_this->head);
    _this->size++;
    return n;
}

/* checked */
bool i_dlist_push_front(i_list_t* _this, list_data_t data)
{
    if (unlikely(is_null(_this)))
        return false;
    return !is_null(__i_dlist_push_front(_this, data));
}

/* checked */
static inline
list_node_t* ___i_dlist_insert(i_list_t* _this, list_node_t* pos, list_node_t* node)
{
    __list_add(&node->node, pos->node.prev, &pos->node);
    _this->size++;
    return node;
}

/* checked */
static inline
list_node_t* __i_dlist_insert(i_list_t* _this, list_node_t* pos, list_data_t data)
{
    list_node_t* n;

    if (unlikely(!is_null(_this->ops) && !is_null(_this->ops->valid_data) && !_this->ops->valid_data(data)))
        return NULL;

    n = __i_dlist_make_list_node(_this, data);
    if (is_null(n))
        return NULL;

    ___i_dlist_insert(_this, pos, n);
    return n;
}

/* checked */
list_node_t* i_dlist_insert(i_list_t* _this, list_node_t* pos, list_data_t data)
{
    if (unlikely(is_null(_this) || is_null(pos)))
        return NULL;
    return __i_dlist_insert(_this, pos, data);
}

/* checked */
static inline list_node_t* __i_dlist_erase(i_list_t* _this, list_node_t* pos);
static
list_node_t* __i_dlist_insert_n(i_list_t* _this, list_node_t* pos, list_size_t n, list_data_t data)
{
    list_size_t i;
    list_node_t* t, * first;

    if (unlikely(!is_null(_this->ops) && !is_null(_this->ops->valid_data) && !_this->ops->valid_data(data)))
        return NULL;

    first = __i_dlist_insert(_this, pos, data);
    if (unlikely(is_null(first)))
        return NULL;

    for (i = 1; i < n; ++i) {
        t = __i_dlist_make_list_node(_this, data);
        if (is_null(t))
            goto err;

        ___i_dlist_insert(_this, pos, t);
    }

    return first;

err:
    while ((first =__i_dlist_erase(_this, first)) != pos);
    return NULL;
}

/* checked */
list_node_t* i_dlist_insert_n(i_list_t* _this, list_node_t* pos, list_size_t n, list_data_t data)
{
    if (unlikely(is_null(_this) || is_null(pos) || n < 0))
        return NULL;

    if (unlikely(0 == n))
        return pos;

    if (!is_null(_this->ops) && !is_null(_this->ops->valid_data) && !_this->ops->valid_data(data))
        return NULL;
    return __i_dlist_insert_n(_this, pos, n, data);
}

/* checked */
static JDSC_INLINE_FORCE JDSC_ONLY_WRAPPER JDSC_NO_ITERATOR
void ___i_dlist_erase(i_list_t* _this, list_node_t* pos)
{
    list_del_init(&pos->node);
    _this->size--;
}

/* checked */
static inline
void __i_dlist_pop_back(i_list_t* _this)
{
    list_node_t* n;

    if (unlikely(__i_dlist_empty(_this)))
        return ;

    n = __i_dlist_rbegin(_this); /* iterator at back */
    ___i_dlist_erase(_this, n);
    __i_dlist_destroy_list_node(_this, n);
}

/* checked */
void i_dlist_pop_back(i_list_t* _this)
{
    if (unlikely(is_null(_this)))
        return ;
    __i_dlist_pop_back(_this);
}

/* checked */
static inline
void __i_dlist_pop_front(i_list_t* _this)
{
    list_node_t* n;

    if (unlikely(__i_dlist_empty(_this)))
        return ;

    n = __i_dlist_begin(_this); /* iterator at front */
    ___i_dlist_erase(_this, n);
    __i_dlist_destroy_list_node(_this, n);
}

/* checked */
void i_dlist_pop_front(i_list_t* _this)
{
    if (unlikely(is_null(_this)))
        return ;
    __i_dlist_pop_front(_this);
}

/* checked */
static inline
list_node_t* __i_dlist_erase(i_list_t* _this, list_node_t* pos)
{
    list_node_t* n;

    if (unlikely(__i_dlist_empty(_this) || __i_dlist_end(_this) == pos) /* || __i_dlist_rend(_this) == pos*/)
        return NULL;

    n = __i_dlist_next(_this, pos);
    if (unlikely(is_null(n)))
        return NULL;

    ___i_dlist_erase(_this, pos);
    __i_dlist_destroy_list_node(_this, pos);
    return n;
}

/* checked */
list_node_t* i_dlist_erase(i_list_t* _this, list_node_t* pos)
{
    if (unlikely(is_null(_this) || is_null(pos)))
        return NULL;
    return __i_dlist_erase(_this, pos);
}

/* checked */
static
list_node_t* __i_dlist_erase_range(i_list_t* _this, list_node_t* begin, list_node_t* end)
{
    list_node_t* pos = begin;

    if (unlikely(__i_dlist_end(_this) == begin))
        return begin == end ? end : NULL;

    while (__i_dlist_end(_this) != pos && end != pos)
        pos = __i_dlist_erase(_this, pos);

    if (__i_dlist_end(_this) == pos) {
        JDSC_ASSERT(end == pos);
    }
    return pos;
}

/* checked */
list_node_t* i_dlist_erase_range(i_list_t* _this, list_node_t* begin, list_node_t* end)
{
    if (unlikely(is_null(_this) || is_null(begin) || is_null(end)))
        return NULL;
    return __i_dlist_erase_range(_this, begin, end);
}

/* checked */
static JDSC_ONLY_WRAPPER JDSC_NO_ITERATOR
list_node_t* ___i_dlist_find(const i_list_t* _this, list_data_t data)
{
    list_node_t* n;

    if (is_null(_this->ops) || is_null(_this->ops->__eq)) {
        list_for_each_entry(n, &_this->head, node) {
            if (__i_dlist_slot_equal(_this->step, data, n->data))
                return n;
        }
    } else if (_this->sso) {
        list_for_each_entry(n, &_this->head, node) {
            if (__ds_ops_eq_sso(data, (list_data_t)(uintptr_t)n->data))
                return n;
        }
    } else {
        list_for_each_entry(n, &_this->head, node) {
            if (_this->ops->__eq(data, __i_dlist_slot_read(_this, n->data)))
                return n;
        }
    }
    return NULL;
}

/* checked */
list_node_t* i_dlist_find(const i_list_t* _this, list_data_t data)
{
    list_node_t* n;

    if (unlikely(is_null(_this)))
        return NULL;

    if (unlikely(!is_null(_this->ops) && !is_null(_this->ops->valid_data) && !_this->ops->valid_data(data)))
        return NULL;

    if ((_this->dtype & SEQ_CNTR_DTYPE_MASK_STUN) && (is_null(_this->ops) || is_null(_this->ops->__eq))) {
        JDSC_ASSERT(!"struct/union container has no __eq");
        return NULL;
    }

    n = ___i_dlist_find(_this, data);
    return is_null(n) ? i_dlist_end(_this) : n;
}

/* checked */
static JDSC_ONLY_WRAPPER JDSC_NO_ITERATOR
list_count_t ___i_dlist_count(const i_list_t* _this, list_data_t data)
{
    list_size_t ret = 0;
    list_node_t* t, * n;

    if (is_null(_this->ops) || is_null(_this->ops->__eq)) {
        list_for_each_entry_safe(n, t, &_this->head, node) {
            if (__i_dlist_slot_equal(_this->step, data, n->data))
                ret++;
        }
    } else if (_this->sso) {
        list_for_each_entry_safe(n, t, &_this->head, node) {
            if (__ds_ops_eq_sso(data, (list_data_t)(uintptr_t)n->data))
                ret++;
        }
    } else {
        list_for_each_entry_safe(n, t, &_this->head, node) {
            if (_this->ops->__eq(data, __i_dlist_slot_read(_this, n->data)))
                ret++;
        }
    }
    return ret;
}

/* checked */
list_count_t i_dlist_count(const i_list_t* _this, list_data_t data)
{
    if (unlikely(is_null(_this)))
        return -1;

    if (unlikely(!is_null(_this->ops) && !is_null(_this->ops->valid_data) && !_this->ops->valid_data(data)))
        return -1;

    if ((_this->dtype & SEQ_CNTR_DTYPE_MASK_STUN) && (is_null(_this->ops) || is_null(_this->ops->__eq))) {
        JDSC_ASSERT(!"struct/union container has no __eq");
        return -1;
    }

    return ___i_dlist_count(_this, data);
}

/* checked */
list_size_t i_dlist_remove(i_list_t* _this, list_data_t data)
{
    list_size_t ret = 0;
    list_node_t* t, *n;

    if (unlikely(is_null(_this)))
        return -1;

    if (!is_null(_this->ops) && !is_null(_this->ops->valid_data) && !_this->ops->valid_data(data))
        return -1;

    if ((_this->dtype & SEQ_CNTR_DTYPE_MASK_STUN) && (is_null(_this->ops) || is_null(_this->ops->__eq))) {
        JDSC_ASSERT(!"struct/union container has no __eq");
        return -1;
    }

    if (is_null(_this->ops) || is_null(_this->ops->__eq)) {
        list_for_each_entry_safe(n, t, &_this->head, node) {
            if (!__i_dlist_slot_equal(_this->step, data, n->data))
                continue;

            ___i_dlist_erase(_this, n);
            __i_dlist_destroy_list_node(_this, n);
            ret++;
        }
    } else if (_this->sso) {
        list_for_each_entry_safe(n, t, &_this->head, node) {
            if (!__ds_ops_eq_sso(data, (list_data_t)(uintptr_t)n->data))
                continue;

            ___i_dlist_erase(_this, n);
            __i_dlist_destroy_list_node(_this, n);
            ret++;
        }
    } else {
        list_for_each_entry_safe(n, t, &_this->head, node) {
            if (!_this->ops->__eq(data, __i_dlist_slot_read(_this, n->data)))
                continue;

            ___i_dlist_erase(_this, n);
            __i_dlist_destroy_list_node(_this, n);
            ret++;
        }
    }

    return ret;
}

/* checked */
list_size_t i_dlist_remove_if(i_list_t* _this, remove_if_condition cond)
{
    list_size_t ret = 0;
    list_node_t* t, *n;

    if (unlikely(is_null(_this) || is_null(cond)))
        return -1;

    list_for_each_entry_safe(n, t, &_this->head, node) {
        if (!cond(__i_dlist_slot_read(_this, n->data)))
            continue;

        ___i_dlist_erase(_this, n);
        __i_dlist_destroy_list_node(_this, n);
        ret++;
    }

    return ret;
}

/* checked */
static inline
bool __i_dlist_value_eq_stored(const i_list_t* _this, list_data_t left, list_data_t right)
{
    if (is_null(_this->ops) || is_null(_this->ops->__eq)) {
        return left == right;
    } else if (_this->sso) {
        return __ds_ops_eq_default_sso(left, right);
    } else {
        return _this->ops->__eq(left, right);
    }
}

/* checked */
list_size_t i_dlist_unique(i_list_t* _this)
{
    list_size_t ret = 0;
    list_node_t* first;
    list_node_t* next;

    if (unlikely(is_null(_this)))
        return -1;

    if ((_this->dtype & SEQ_CNTR_DTYPE_MASK_STUN) && (is_null(_this->ops) || is_null(_this->ops->__eq))) {
        JDSC_ASSERT(!"struct/union container has no __eq");
        return -1;
    }

    if (__i_dlist_size(_this) < 2)
        return 0;

    first = __i_dlist_begin(_this);
    next  = __i_dlist_next(_this, first);

    while (__i_dlist_end(_this) != next) {
        if (__i_dlist_value_eq_stored(_this,
                __i_dlist_slot_read(_this, first->data),
                __i_dlist_slot_read(_this, next->data))) {
            next = __i_dlist_erase(_this, next);
            ret++;
        } else {
            first = next;
            next  = __i_dlist_next(_this, next);
        }
    }

    return ret;
}

/* checked */
static inline
void __i_dlist_reverse_self(i_list_t* _this)
{
    struct list_head* p;

    if (__i_dlist_size(_this) < 2)
        return ;

    p = &_this->head;
    do {
        struct list_head* t = p->next;

        p->next = p->prev;
        p->prev = t;
        p = p->prev;
    } while (p != &_this->head);
}

/* checked */
void i_dlist_reverse(i_list_t* _this)
{
    if (unlikely(is_null(_this)))
        return ;

    __i_dlist_reverse_self(_this);
}

/* checked */
static inline
void __i_dlist_transfer(struct list_head* pos, struct list_head* first, struct list_head* last)
{
    struct list_head* tmp;

    if (pos == last)
        return ;

    /* Remove [first, last) from its old position. */
    last->prev->next  = pos;
    first->prev->next = last;
    pos->prev->next   = first;

    /* Splice [first, last) into its new position. */
    tmp         = pos->prev;
    pos->prev   = last->prev;
    last->prev  = first->prev;
    first->prev = tmp;
}

/* checked */
void i_dlist_splice(i_list_t* _this, list_iterator_t iterator, i_list_t* other)
{
    list_node_t* pos;

    if (unlikely(is_null(_this) || is_null(other) || is_null(iterator.d)))
        return ;

    if (_this == other || __i_dlist_empty(other))
        return ;

    if (_this->step != other->step || _this->ops != other->ops || _this->dtype != other->dtype)
        return ;

    pos = __i_dlist_it_to_lnode(iterator);
    __i_dlist_transfer(&pos->node, other->head.next, &other->head);

    _this->size += other->size;
    other->size = 0;
}

/* checked */
void i_dlist_splice_range(i_list_t* _this, list_iterator_t iterator, i_list_t* other,
                          list_iterator_t iterator_begin, list_iterator_t iterator_end)
{
    list_node_t* pos;
    list_node_t* first;
    list_node_t* last;
    list_node_t* it;
    list_size_t n = 0;

    if (unlikely(is_null(_this) || is_null(other)
                 || is_null(iterator.d) || is_null(iterator_begin.d) || is_null(iterator_end.d)))
        return ;

    if (_this->step != other->step || _this->ops != other->ops || _this->dtype != other->dtype)
        return ;

    if (__i_dlist_empty(other) || iterator_begin.d == iterator_end.d)
        return ;

    pos   = __i_dlist_it_to_lnode(iterator);
    first = __i_dlist_it_to_lnode(iterator_begin);
    last  = __i_dlist_it_to_lnode(iterator_end);

    for (it = first; __i_dlist_end(other) != it && it != last && n <= other->size; it = __i_dlist_next(other, it)) {
        if (_this == other && it == pos)
            return ;
        n++;
    }

    if (it != last) /* `last` is not reachable from `first` in `other` */
        return ;

    __i_dlist_transfer(&pos->node, &first->node, &last->node);

    _this->size += n;
    other->size -= n;
}

/* checked */
void i_dlist_merge(i_list_t* _this, i_list_t* other, list_cmp_t cmp)
{
    struct list_head* first1;
    struct list_head* last1;
    struct list_head* first2;
    struct list_head* last2;

    if (unlikely(is_null(_this) || is_null(other) || is_null(cmp)))
        return ;

    if (_this == other || __i_dlist_empty(other))
        return ;

    JDSC_ASSERT(_this->step == other->step
                && _this->ops == other->ops
                && _this->dtype == other->dtype);
    if (_this->step != other->step || _this->ops != other->ops || _this->dtype != other->dtype)
        return ;

    first1 = _this->head.next;
    last1  = &_this->head;
    first2 = other->head.next;
    last2  = &other->head;

    while (first1 != last1 && first2 != last2) {
        if (cmp(__i_dlist_slot_read(_this, ((list_node_t*)(void*)first2)->data),
                __i_dlist_slot_read(_this, ((list_node_t*)(void*)first1)->data))) {
            struct list_head* next = first2->next;

            __i_dlist_transfer(first1, first2, next);
            first2 = next;
        } else {
            first1 = first1->next;
        }
    }

    if (first2 != last2)
        __i_dlist_transfer(last1, first2, last2);

    _this->size += other->size;
    other->size = 0;
}

/* checked */
void __i_dlist_swap_head(i_list_t* _this, i_list_t* other)
{
    struct list_head* a = &_this->head;
    struct list_head* b = &other->head;
    list_size_t s;

    if (a->next != a) {
        if (b->next != b) {
            struct list_head* t;

            t = a->next; a->next = b->next; b->next = t;
            t = a->prev; a->prev = b->prev; b->prev = t;
            a->next->prev = a;
            a->prev->next = a;
            b->next->prev = b;
            b->prev->next = b;
        } else {
            b->next = a->next;
            b->prev = a->prev;
            b->next->prev = b;
            b->prev->next = b;
            INIT_LIST_HEAD(a);
        }
    } else if (b->next != b) {
        a->next = b->next;
        a->prev = b->prev;
        a->next->prev = a;
        a->prev->next = a;
        INIT_LIST_HEAD(b);
    }

    s = _this->size;
    _this->size = other->size;
    other->size = s;
}

/* checked */
void i_dlist_swap(i_list_t* _this, i_list_t* other)
{
    if (unlikely(is_null(_this) || is_null(other) || _this == other))
        return ;

    if (_this->step != other->step || _this->ops != other->ops || _this->dtype != other->dtype)
        return ;

    __i_dlist_swap_head(_this, other);
}

/* checked */
static inline
bool __i_dlist_resize(i_list_t* _this, list_size_t n, list_data_t default_data)
{
    list_size_t size = __i_dlist_size(_this);

    if (n == size)
        return true;

    if (n < size)
        return !is_null(__i_dlist_erase_range(_this, __i_dlist_it_list_node(_this, n), __i_dlist_end(_this)));
    return !is_null(__i_dlist_insert_n(_this, __i_dlist_end(_this), n - size, default_data));
}

/* checked */
bool i_dlist_resize(i_list_t* _this, list_size_t n, list_data_t default_data)
{
    if (unlikely(is_null(_this)))
        return false;
    return __i_dlist_resize(_this, n, default_data);
}

/* checked */
static list_size_t __i_dlist_clear(i_list_t* _this);
static inline
list_size_t __i_dlist_assign(i_list_t* _this, list_size_t n, list_data_t data)
{
    list_node_t* e;

    if (unlikely(0 == n)) {
        __i_dlist_clear(_this);
        return 0;
    }

    e = __i_dlist_insert_n(_this, __i_dlist_end(_this), n, data);
    if (unlikely(is_null(e)))
        return -1;

    __i_dlist_erase_range(_this, __i_dlist_begin(_this), e);
    return n;
}

/* checked */
list_size_t i_dlist_assign(i_list_t* _this, list_size_t n, list_data_t data)
{
    if (unlikely(is_null(_this)))
        return -1;
    return __i_dlist_assign(_this, n, data);
}

/* checked */
static
list_size_t __i_dlist_clear(i_list_t* _this)
{
    list_size_t ret = 0;
    list_node_t* t, *n;

    list_for_each_entry_safe(n, t, &_this->head, node) {
        ___i_dlist_erase(_this, n);
        __i_dlist_destroy_list_node(_this, n);
        ret++;
    }

    return ret;
}

/* checked */
list_size_t i_dlist_clear(i_list_t* _this)
{
    if (unlikely(is_null(_this)))
        return -1;
    return __i_dlist_clear(_this);
}

/* checked */
list_t* __list_new(const class_list_ops_t* ops, list_dtype_t data_type, list_step_t step, list_align_t align)
{
    list_t* list;
    size_t  width;

    JDSC_ASSERT(0 != step);
    if (0 == step) {
        pr_err("%s: invalid step [ %zu ]!", __func__, step);
        return NULL;
    }

    JDSC_ASSERT(__sequence_container_dtype_valid(data_type));
    if (!__sequence_container_dtype_valid(data_type)) {
        pr_err("%s: invalid data type [ %zu ]!", __func__, data_type);
        return NULL;
    }

    width = __sequence_container_dtype_width(data_type);
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

    JDSC_ASSERT(g_class_list_ops_sso() != ops
                || (SEQ_CNTR_DTYPE_SSO == data_type && sizeof(ds_sso_t) == step));
    if (g_class_list_ops_sso() == ops
        && (SEQ_CNTR_DTYPE_SSO != data_type || sizeof(ds_sso_t) != step)) {
        pr_err("%s: SSO ops require SSO data type and sizeof(ds_sso_t) step!", __func__);
        return NULL;
    }

    list = (list_t*)p_calloc(1, sizeof(list_t));
    if (is_null(list))
        return NULL;

    list->ops   = ops;
    if (g_class_list_ops_sso() == list->ops)
        list->sso = true;
    list->dtype = data_type;
    list->step  = step;
    list->align = align ? align : JDSC_ALIGN_DEFAULT;
#if JDSC_ALIGN
    list->pad   = list->align > JDSC_ALIGN_DEFAULT
                    ? list->align - offsetof(list_node_t, data)
                    : 0;
#endif
    INIT_LIST_HEAD(&list->head);
    return list;
}

/* checked */
void __list_delete(list_t** _this)
{
    if (is_null(_this) || is_null(*_this))
        return ;

    i_dlist_clear(*_this);
    p_free(*_this);
}

const class_list_t* class_list_ins(void)
{
    static const class_list_t ins = {
        .size         = clist_size,
        .__empty      = __clist_empty,
        .count        = clist_count,
        .end          = clist_end,
        .begin        = clist_begin,
        .next         = clist_next,
        .prev         = clist_prev,
        .rend         = clist_rend,
        .rbegin       = clist_rbegin,
        .rnext        = clist_rnext,
        .rprev        = clist_rprev,
        .__it         = __clist_it,
        .it           = clist_it,
        .__at         = __clist_at,
        .at           = clist_at,
        .back         = clist_back,
        .front        = clist_front,
        .find         = clist_find,
        .push_back    = clist_push_back,
        .push_front   = clist_push_front,
        .insert       = clist_insert,
        .insert_n     = clist_insert_n,
        .erase        = clist_erase,
        .erase_range  = clist_erase_range,
        .pop_back     = clist_pop_back,
        .pop_front    = clist_pop_front,
        .remove       = clist_remove,
        .remove_if    = clist_remove_if,
        .unique       = clist_unique,
        .reverse      = clist_reverse,
        .splice       = clist_splice,
        .splice_range = clist_splice_range,
        .merge        = clist_merge,
        .swap         = clist_swap,
        .resize       = clist_resize,
        .assign       = clist_assign,
        .clear        = clist_clear,
    };
    return &ins;
}
