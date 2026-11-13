/*
  List Inline Implementations
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

#ifndef __J_I_LIST_H
#define __J_I_LIST_H

#include <_compiler.h>
#include <linux/_types.h>

#ifndef is_null
#define is_null(X) (!(X))
#endif /* is_null */

typedef struct list_node {
    struct list_head node;
    uint8_t          data[];
} list_node_t;

typedef struct i_list {
    const class_list_ops_t* ops;
    list_dtype_t     dtype;
    list_step_t      step;
    list_align_t     align;
    list_pad_t       pad;
    struct list_head head; /* Err：It cannot be the last one, otherwise the address is invalid! */
    uint8_t*         swap;
    list_size_t      size;
    bool             sso;
} i_list_t;

HEADER_STATIC_INLINE
list_size_t __i_dlist_size(const i_list_t* _this)
{
    return _this->size;
}

HEADER_STATIC_INLINE
list_size_t i_dlist_size(const i_list_t* _this)
{
    return is_null(_this) ? -1 : __i_dlist_size(_this);
}

HEADER_STATIC_INLINE
bool __i_dlist_empty(const i_list_t* _this)
{
    const struct list_head* head = &_this->head;
    return head->next == head; /* list_empty(&_this->head); */
}

#ifdef dlist_entry_lhead
#undef dlist_entry_lhead
#endif
#define dlist_entry_lhead(_ptr)  ((list_node_t*)(_ptr))

HEADER_STATIC_INLINE
list_node_t* __i_dlist_end(const i_list_t* _this)
{
    return dlist_entry_lhead(&_this->head);
}

HEADER_STATIC_INLINE
list_node_t* i_dlist_end(const i_list_t* _this)
{
    if (!is_null(_this))
        return __i_dlist_end(_this);
    return NULL;
}

HEADER_STATIC_INLINE
list_node_t* __i_dlist_begin(const i_list_t* _this)
{
    return dlist_entry_lhead(_this->head.next);
}

HEADER_STATIC_INLINE
list_node_t* i_dlist_begin(const i_list_t* _this)
{
    if (!is_null(_this))
        return __i_dlist_begin(_this);
    return NULL;
}

HEADER_STATIC_INLINE
list_node_t* __i_dlist_next(const list_node_t* node)
{
    return dlist_entry_lhead(node->node.next);
}

HEADER_STATIC_INLINE
list_node_t* i_dlist_next(const list_node_t* node)
{
    if (!is_null(node))
        return __i_dlist_next(node);
    return NULL;
}

HEADER_STATIC_INLINE
list_node_t* __i_dlist_prev(const list_node_t* node)
{
    return dlist_entry_lhead(node->node.prev);
}

HEADER_STATIC_INLINE
list_node_t* i_dlist_prev(const list_node_t* node)
{
    if (!is_null(node))
        return __i_dlist_prev(node);
    return NULL;
}

HEADER_STATIC_INLINE
list_node_t* __i_dlist_rend(const i_list_t* _this)
{
    return dlist_entry_lhead(&_this->head);
}

HEADER_STATIC_INLINE
list_node_t* i_dlist_rend(const i_list_t* _this)
{
    if (!is_null(_this))
        return __i_dlist_rend(_this);
    return NULL;
}

HEADER_STATIC_INLINE
list_node_t* __i_dlist_rbegin(const i_list_t* _this)
{
    return dlist_entry_lhead(_this->head.rnext);
}

HEADER_STATIC_INLINE
list_node_t* i_dlist_rbegin(const i_list_t* _this)
{
    if (!is_null(_this))
        return __i_dlist_rbegin(_this);
    return NULL;
}

HEADER_STATIC_INLINE
list_node_t* __i_dlist_rnext(const list_node_t* node)
{
    return dlist_entry_lhead(node->node.rnext);
}

HEADER_STATIC_INLINE
list_node_t* i_dlist_rnext(const list_node_t* node)
{
    if (!is_null(node))
        return __i_dlist_rnext(node);
    return NULL;
}

HEADER_STATIC_INLINE
list_node_t* __i_dlist_rprev(const list_node_t* node)
{
    return dlist_entry_lhead(node->node.rprev);
}

HEADER_STATIC_INLINE
list_node_t* i_dlist_rprev(const list_node_t* node)
{
    if (!is_null(node))
        return __i_dlist_rprev(node);
    return NULL;
}

// #undef dlist_entry_lhead

#include <stddef.h> /* offsetof */
#define dlist_entry(_pdata)  ((list_node_t*)((uint8_t*)(_pdata) - offsetof(list_node_t, data)))

HEADER_STATIC_INLINE
list_iterator_t __i_dlist_lnode_to_it(const list_node_t* node)
{
    list_iterator_t it;
    it.d = is_null(node) ? NULL : (list_data_t*)node->data;
    return it;
}

HEADER_STATIC_INLINE
list_node_t* __i_dlist_it_to_lnode(const list_iterator_t it)
{
    return is_null(it.d) ? NULL : dlist_entry(it.d);
}

HEADER_STATIC_INLINE
list_r_iterator_t __i_dlist_lnode_to_rit(const list_node_t* node)
{
    list_r_iterator_t rit;
    rit.d = is_null(node) ? NULL : (list_data_t*)node->data;
    return rit;
}

HEADER_STATIC_INLINE
list_node_t* __i_dlist_rit_to_lnode(const list_r_iterator_t rit)
{
    return is_null(rit.d) ? NULL : dlist_entry(rit.d);
}

HEADER_STATIC_INLINE
list_iterator_t i_clist_end(const i_list_t* _this)
{
    return __i_dlist_lnode_to_it(i_dlist_end(_this));
}

HEADER_STATIC_INLINE
list_iterator_t i_clist_begin(const i_list_t* _this)
{
    return __i_dlist_lnode_to_it(i_dlist_begin(_this));
}

HEADER_STATIC_INLINE
list_iterator_t i_clist_next(const list_iterator_t iterator)
{
    return __i_dlist_lnode_to_it(i_dlist_next(__i_dlist_it_to_lnode(iterator)));
}

HEADER_STATIC_INLINE
list_iterator_t i_clist_prev(const list_iterator_t iterator)
{
    return __i_dlist_lnode_to_it(i_dlist_prev(__i_dlist_it_to_lnode(iterator)));
}

HEADER_STATIC_INLINE
list_r_iterator_t i_clist_rend(const i_list_t* _this)
{
    return __i_dlist_lnode_to_rit(i_dlist_rend(_this));
}

HEADER_STATIC_INLINE
list_r_iterator_t i_clist_rbegin(const i_list_t* _this)
{
    return __i_dlist_lnode_to_rit(i_dlist_rbegin(_this));
}

HEADER_STATIC_INLINE
list_r_iterator_t i_clist_rnext(const list_r_iterator_t r_iterator)
{
    return __i_dlist_lnode_to_rit(i_dlist_rnext(__i_dlist_rit_to_lnode(r_iterator)));
}

HEADER_STATIC_INLINE
list_r_iterator_t i_clist_rprev(const list_r_iterator_t r_iterator)
{
    return __i_dlist_lnode_to_rit(i_dlist_rprev(__i_dlist_rit_to_lnode(r_iterator)));
}





HEADER_STATIC_INLINE
list_iterator_t i_dlist_null_iterator(void)
{
    list_iterator_t it;
    it.d = NULL;
    return it;
}

HEADER_STATIC_INLINE
list_r_iterator_t i_dlist_null_r_iterator(void)
{
    list_r_iterator_t rit;
    rit.d = NULL;
    return rit;
}

HEADER_STATIC_INLINE
list_iterator_t __i_dlist_make_iterator(const void* ptr)
{
    list_iterator_t it;
    it.d = (list_data_t*)ptr;
    return it;
}

HEADER_STATIC_INLINE
list_node_t* __i_dlist_it_list_node(const i_list_t* _this, list_index_t index)
{
    const list_size_t size = __i_dlist_size(_this);
    list_node_t* it;

    /* if (index < size) */
    {
        if (index * 2 >= size) {
            index = size - index;
            it = __i_dlist_end(_this);
            while (index--) it = __i_dlist_prev(it);
        } else {
            it = __i_dlist_begin(_this);
            while (index--) it = __i_dlist_next(it);
        }
        return it;
    }
    /* return NULL; */
}

HEADER_STATIC_INLINE /* JDSC_LOGIC_FRAGMENT */ /* Because of performance */
list_iterator_t __i_dlist_iterator(const i_list_t* _this, list_index_t index)
{
    return __i_dlist_make_iterator(__i_dlist_it_list_node(_this, index)->data);
}

HEADER_STATIC_INLINE
list_iterator_t i_dlist_iterator(const i_list_t* _this, list_index_t index)
{
    if (!is_null(_this) && index < __i_dlist_size(_this))
        return __i_dlist_iterator(_this, index);
    return i_dlist_null_iterator();
}

HEADER_STATIC_INLINE /* JDSC_LOGIC_FRAGMENT */ /* Because of performance */
list_value_t __i_dlist_at(const i_list_t* _this, list_index_t index)
{
    return (list_value_t){ .u8 = (uint8_t*)__i_dlist_iterator(_this, index).d, };
}

HEADER_STATIC_INLINE
list_value_t i_dlist_at(const i_list_t* _this, list_index_t index)
{
    if (!is_null(_this) && index < __i_dlist_size(_this))
        return __i_dlist_at(_this, index);
    return (list_value_t){ .u8 = NULL, };
}

HEADER_STATIC_INLINE
list_value_t __i_dlist_back(const i_list_t* _this)
{
    JDSC_ASSERT(!__i_dlist_empty(_this));
    return (list_value_t){ .u8 = __i_dlist_rbegin(_this)->data, };
}

HEADER_STATIC_INLINE
list_value_t i_dlist_back(const i_list_t* _this)
{
    if (!is_null(_this))
        return __i_dlist_back(_this);
    return (list_value_t){ .u8 = NULL, };
}

HEADER_STATIC_INLINE
list_value_t __i_dlist_front(const i_list_t* _this)
{
    JDSC_ASSERT(!__i_dlist_empty(_this));
    return (list_value_t){ .u8 = __i_dlist_begin(_this)->data, };
}

HEADER_STATIC_INLINE
list_value_t i_dlist_front(const i_list_t* _this)
{
    if (!is_null(_this))
        return __i_dlist_front(_this);
    return (list_value_t){ .u8 = NULL, };
}





HEADER_STATIC_INLINE
bool __i_dlist_slot_write(const i_list_t* _this, void* slot, list_data_t data)
{
    if (!is_null(_this->ops) && !is_null(_this->ops->copy_data))
        return _this->ops->copy_data(data, (list_data_t*)slot);

    switch (_this->step)
    {
    case 1: *(uint8_t*)slot  = (uint8_t)data;  break;
    case 2: *(uint16_t*)slot = (uint16_t)data; break;
    case 4: *(uint32_t*)slot = (uint32_t)data; break;
    case 8: __ds_token_store_8(slot, data); break;
    default: return false;
    }
    return true;
}

HEADER_STATIC_INLINE
list_data_t __i_dlist_slot_read(const i_list_t* _this, const void* slot)
{
    const list_dtype_t dtype = _this->dtype;

    if (dtype & SEQ_CNTR_DTYPE_MASK_TOKEN_ADDR) {
        return (list_data_t)(uintptr_t)slot;
    } else if (dtype & SEQ_CNTR_DTYPE_MASK_SINT) {
        switch (_this->step)
        {
        case 1: return (int8_t)*(const int8_t*)slot;
        case 2: return (int16_t)*(const int16_t*)slot;
        case 4: return (int32_t)*(const int32_t*)slot;
        case 8: return __ds_token_load_8(slot);
        default: return (list_data_t)(uintptr_t)slot;
        }
    } else if (dtype & (SEQ_CNTR_DTYPE_MASK_UINT | SEQ_CNTR_DTYPE_MASK_FLOAT)) {
        switch (_this->step)
        {
        case 1: return (uint8_t)*(const uint8_t*)slot;
        case 2: return (uint16_t)*(const uint16_t*)slot;
        case 4: return (uint32_t)*(const uint32_t*)slot;
        case 8: return __ds_token_load_8(slot);
        default: return (list_data_t)(uintptr_t)slot;
        }
    } else {
        return (list_data_t)(uintptr_t)slot;
    }
}

HEADER_STATIC_INLINE
bool __i_dlist_slot_equal(list_step_t step, list_data_t data, const void* slot)
{
    switch (step) {
    case 1: return (uint8_t)data  == *(const uint8_t*)slot;
    case 2: return (uint16_t)data == *(const uint16_t*)slot;
    case 4: return (uint32_t)data == *(const uint32_t*)slot;
    case 8: return __ds_token_equal_8(data, slot);
    default: return false;
    }
}

HEADER_STATIC_INLINE
bool __i_dlist_slot_equal_slot(list_step_t step, const void* slot1, const void* slot2)
{
    switch (step) {
    case 1: return *(const uint8_t*)slot1  == *(const uint8_t*)slot2;
    case 2: return *(const uint16_t*)slot1 == *(const uint16_t*)slot2;
    case 4: return *(const uint32_t*)slot1 == *(const uint32_t*)slot2;
    case 8: return *(const uint64_t*)slot1 == *(const uint64_t*)slot2;
    default: return false;
    }
}

HEADER_STATIC_INLINE
void __i_dlist_slot_move(const i_list_t* _this, void* dest, const void* src)
{
    if (dest != src) {
        if (is_null(_this->ops) || is_null(_this->ops->__move)) {
            switch (_this->step) {
            case 1:  memmove(dest, src, 1);  return ;
            case 2:  memmove(dest, src, 2);  return ;
            case 4:  memmove(dest, src, 4);  return ;
            case 8:  memmove(dest, src, 8);  return ;
            case 16: memmove(dest, src, 16); return ;
            case 24: memmove(dest, src, 24);
#ifdef JDSC_32BIT
                    if (_this->sso) __ds_ops_fix_sso((list_data_t)dest);
#endif /* JDSC_32BIT */
                return ;
            case 32: memmove(dest, src, 32);
#ifdef JDSC_64BIT
                    if (_this->sso) __ds_ops_fix_sso((list_data_t)dest);
#endif /* JDSC_64BIT */
                return ;
            default: memmove(dest, src, _this->step); return ;
            }
        } else {
            _this->ops->__move((list_data_t)dest, (list_data_t)src);
        }
    }
}

HEADER_STATIC_INLINE /* JDSC_LOGIC_FRAGMENT */ /* Because of `pos` */
bool __i_dlist_replace(i_list_t* _this, list_node_t* pos, list_data_t data)
{
    if (!is_null(_this->ops) && !is_null(_this->ops->valid_data) && !_this->ops->valid_data(data))
        return false;

    if (_this->sso) {
        if (data == __ds_ops_char_addr_sso((list_data_t)pos->data))
            return true;
    } else if ((_this->dtype & SEQ_CNTR_DTYPE_MASK_TOKEN_ADDR) && data == (list_data_t)pos->data) {
        return true;
    } else if (!is_null(_this->ops) && is_null(_this->ops->copy_data)
               && !is_null(_this->ops->free_data) && data == __i_dlist_slot_read(_this, pos->data)) {
        return true;
    }

    if (is_null(_this->ops) || is_null(_this->ops->copy_data)) {
        if (!is_null(_this->ops) && !is_null(_this->ops->free_data))
            _this->ops->free_data((list_data_t*)pos->data);

        __i_dlist_slot_write(_this, pos->data, data);
        return true;
    }

    if (!__i_dlist_slot_write(_this, _this->swap, data))
        return false;

    if (!is_null(_this->ops) && !is_null(_this->ops->free_data))
        _this->ops->free_data((list_data_t*)pos->data);

    __i_dlist_slot_move(_this, pos->data, _this->swap);

    return true;
}

HEADER_STATIC_INLINE
bool i_dlist_replace(i_list_t* _this, list_node_t* pos, list_data_t data)
{
    if (!is_null(_this) && !is_null(pos) && !__i_dlist_empty(_this) && __i_dlist_end(_this) != pos)
        return __i_dlist_replace(_this, pos, data);
    return false;
}

HEADER_STATIC_INLINE
bool __i_clist_replace_it(i_list_t* _this, list_iterator_t pos, list_data_t data)
{
    return __i_dlist_replace(_this, __i_dlist_it_to_lnode(pos), data);
}

HEADER_STATIC_INLINE
bool i_clist_replace_it(i_list_t* _this, list_iterator_t pos, list_data_t data)
{
    return i_dlist_replace(_this, __i_dlist_it_to_lnode(pos), data);
}

HEADER_STATIC_INLINE
bool __i_clist_replace_index(i_list_t* _this, list_index_t index, list_data_t data)
{
    list_iterator_t pos = __i_dlist_iterator(_this, index);
    return __i_clist_replace_it(_this, pos, data);
}

HEADER_STATIC_INLINE
bool i_clist_replace_index(i_list_t* _this, list_index_t index, list_data_t data)
{
    list_iterator_t pos = i_dlist_iterator(_this, index);
    return i_clist_replace_it(_this, pos, data);
}





list_size_t __i_dlist_max_size(const i_list_t* _this);
bool i_dlist_push_back(i_list_t* _this, list_data_t data);
bool i_dlist_push_front(i_list_t* _this, list_data_t data);
list_node_t* i_dlist_insert(i_list_t* _this, list_node_t* pos, list_data_t data);
list_node_t* i_dlist_insert_n(i_list_t* _this, list_node_t* pos, list_size_t n, list_data_t data);
list_node_t* i_dlist_insert_range(i_list_t* _this, list_node_t* pos, list_node_t* begin, list_node_t* end);
void i_dlist_pop_back(i_list_t* _this);
void i_dlist_pop_front(i_list_t* _this);
list_node_t* i_dlist_erase(i_list_t* _this, list_node_t* pos);
list_node_t* i_dlist_erase_range(i_list_t* _this, list_node_t* begin, list_node_t* end);
list_node_t* i_dlist_find(const i_list_t* _this, list_data_t data);
list_count_t i_dlist_count(const i_list_t* _this, list_data_t data);
list_size_t i_dlist_remove(i_list_t* _this, list_data_t data);
list_size_t i_dlist_remove_if(i_list_t* _this, remove_if_condition cond);
list_size_t i_dlist_unique(i_list_t* _this);
list_size_t i_dlist_unique_if(i_list_t* _this, unique_if_condition cond);
bool i_dlist_assign(i_list_t* _this, list_size_t n, list_data_t data);
bool i_dlist_assign_range(i_list_t* _this, list_node_t* begin, list_node_t* end);
bool i_dlist_resize(i_list_t* _this, list_size_t n, list_data_t default_data);
void i_dlist_sort(i_list_t* _this, __list_lt_t __lt);
void i_dlist_swap(i_list_t* _this, i_list_t* other);
list_size_t i_dlist_clear(i_list_t* _this);

HEADER_STATIC_INLINE
list_size_t i_dlist_max_size(const i_list_t* _this)
{
    return !is_null(_this) ? __i_dlist_max_size(_this) : -1;
}

HEADER_STATIC_INLINE /* JDSC_ONLY_WRAPPER JDSC_NO_ITERATOR */
void ___i_dlist_splice_range(struct list_head* pos, struct list_head* first, struct list_head* last)
{
    struct list_head* tmp;

    /* Keep the std algorithm. */
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

HEADER_STATIC_INLINE
bool __i_dlist_splice_compatible(const i_list_t* _this, const i_list_t* other)
{
    return _this->ops   == other->ops
        && _this->dtype == other->dtype
        && _this->step  == other->step
        && _this->align == other->align
        && _this->pad   == other->pad;
}

HEADER_STATIC_INLINE
void i_dlist_splice(i_list_t* _this, list_node_t* pos, i_list_t* other)
{
    if ((is_null(_this) || is_null(pos) || is_null(other)))
        return ;

    if ((_this == other || __i_dlist_empty(other)))
        return ;

    JDSC_ASSERT(__i_dlist_splice_compatible(_this, other));

    /* list_splice_tail_init(&other->head, &pos->node); */
    ___i_dlist_splice_range(&pos->node, other->head.next, &other->head);

    _this->size += other->size;
    other->size = 0;
}

HEADER_STATIC_INLINE
void i_dlist_splice_range(i_list_t* _this, list_node_t* pos, i_list_t* other, list_node_t* begin, list_node_t* end)
{
    list_node_t* it;
    list_size_t n = 0;

    if ((is_null(_this) || is_null(pos) || is_null(other) || is_null(begin) || is_null(end)))
        return ;

    JDSC_ASSERT(__i_dlist_splice_compatible(_this, other));

    if (__i_dlist_empty(other) || begin == end)
        return ;

    /* Self-splice is allowed: move [begin, end) of `_this`, but `pos` must not be inside it. */
    for (it = begin; __i_dlist_end(other) != it && end != it && n <= other->size; it = __i_dlist_next(it)) {
        if (_this == other && it == pos) {
            JDSC_ASSERT(!"Self-splice: `pos` must not be inside [begin, end)");
            return ;
        }
        n++;
    }

    if (end != it) { /* Err: `end` is not reachable from `begin` in `other` */
        JDSC_ASSERT(!"The `end` is not reachable from `begin` in `other`");
        return ;
    }

    ___i_dlist_splice_range(&pos->node, &begin->node, &end->node);

    _this->size += n;
    other->size -= n;
}

HEADER_STATIC_INLINE /* JDSC_ONLY_WRAPPER JDSC_NO_ITERATOR */
void ___i_dlist_merge(const i_list_t* _this, struct list_head* dest, struct list_head* src, __list_lt_t __lt)
{
    struct list_head* first1 = dest->next;
    struct list_head* last1  = dest;
    struct list_head* first2 = src->next;
    struct list_head* last2  = src;

    /* Keep the std algorithm. */
    while (first1 != last1 && first2 != last2) {
        if (__lt(__i_dlist_slot_read(_this, dlist_entry_lhead(first2)->data),
                 __i_dlist_slot_read(_this, dlist_entry_lhead(first1)->data))) {
            struct list_head* next = first2->next;

            ___i_dlist_splice_range(first1, first2, next);
            first2 = next;
        } else {
            first1 = first1->next;
        }
    }

    if (first2 != last2)
        ___i_dlist_splice_range(last1, first2, last2);
}

HEADER_STATIC_INLINE
void i_dlist_merge(i_list_t* _this, i_list_t* other, __list_lt_t __lt)
{
    if (is_null(_this) || is_null(other))
        return ;

    if (_this == other || __i_dlist_empty(other))
        return ;

    JDSC_ASSERT(__i_dlist_splice_compatible(_this, other));

    /* Unlike std merge, a null `__lt` appends `other` directly to the end of `_this`. */
    if (is_null(__lt)) {
        ___i_dlist_splice_range(&__i_dlist_end(_this)->node, 
                                &__i_dlist_begin(other)->node, &__i_dlist_end(other)->node);
    } else {
        ___i_dlist_merge(_this, &_this->head, &other->head, __lt);
    }

    _this->size += other->size;
    other->size = 0;
}

HEADER_STATIC_INLINE
void __i_dlist_reverse(i_list_t* _this)
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

HEADER_STATIC_INLINE
void i_dlist_reverse(i_list_t* _this)
{
    if (!is_null(_this))
        __i_dlist_reverse(_this);
}

#undef dlist_entry_lhead





HEADER_STATIC_INLINE
list_iterator_t i_clist_find(const i_list_t* _this, list_data_t data)
{
    return __i_dlist_lnode_to_it(i_dlist_find(_this, data));
}

HEADER_STATIC_INLINE
list_iterator_t i_clist_insert(i_list_t* _this, list_iterator_t iterator, list_data_t data)
{
    return __i_dlist_lnode_to_it(i_dlist_insert(_this, __i_dlist_it_to_lnode(iterator), data));
}

HEADER_STATIC_INLINE
list_iterator_t i_clist_insert_n(i_list_t* _this, list_iterator_t iterator, list_size_t n, list_data_t data)
{
    return __i_dlist_lnode_to_it(i_dlist_insert_n(_this, __i_dlist_it_to_lnode(iterator), n, data));
}

HEADER_STATIC_INLINE
list_iterator_t i_clist_insert_range(i_list_t* _this, list_iterator_t iterator_pos, list_iterator_t iterator_begin, list_iterator_t iterator_end)
{
    return __i_dlist_lnode_to_it(i_dlist_insert_range(_this, __i_dlist_it_to_lnode(iterator_pos),
                                                      __i_dlist_it_to_lnode(iterator_begin), __i_dlist_it_to_lnode(iterator_end)));
}

HEADER_STATIC_INLINE
list_iterator_t i_clist_erase(i_list_t* _this, list_iterator_t iterator)
{
    return __i_dlist_lnode_to_it(i_dlist_erase(_this, __i_dlist_it_to_lnode(iterator)));
}

HEADER_STATIC_INLINE
list_iterator_t i_clist_erase_range(i_list_t* _this, list_iterator_t iterator_begin, list_iterator_t iterator_end)
{
    return __i_dlist_lnode_to_it(i_dlist_erase_range(_this, __i_dlist_it_to_lnode(iterator_begin), __i_dlist_it_to_lnode(iterator_end)));
}

HEADER_STATIC_INLINE
bool i_clist_assign_range(i_list_t* _this, list_iterator_t iterator_begin, list_iterator_t iterator_end)
{
    return i_dlist_assign_range(_this, __i_dlist_it_to_lnode(iterator_begin), __i_dlist_it_to_lnode(iterator_end));
}

HEADER_STATIC_INLINE
void i_clist_splice(i_list_t* _this, list_iterator_t iterator, i_list_t* other)
{
    return i_dlist_splice(_this, __i_dlist_it_to_lnode(iterator), other);
}

HEADER_STATIC_INLINE
void i_clist_splice_range(i_list_t* _this, list_iterator_t iterator_pos, i_list_t* other, list_iterator_t iterator_begin, list_iterator_t iterator_end)
{
    return i_dlist_splice_range(_this, __i_dlist_it_to_lnode(iterator_pos), other, __i_dlist_it_to_lnode(iterator_begin), __i_dlist_it_to_lnode(iterator_end));
}

#endif /* __J_I_LIST_H */
