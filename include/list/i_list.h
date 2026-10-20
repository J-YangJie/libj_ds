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
#include <linux/list.h>

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
    return list_empty(&_this->head);
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

#undef dlist_entry_lhead

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

HEADER_STATIC_INLINE
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

HEADER_STATIC_INLINE
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
    return (list_value_t){ .u8 = __i_dlist_rbegin(_this)->data, };
}

HEADER_STATIC_INLINE
list_value_t i_dlist_back(const i_list_t* _this)
{
    if (!is_null(_this) && !__i_dlist_empty(_this))
        return (list_value_t){ .u8 = __i_dlist_rbegin(_this)->data, };
    return (list_value_t){ .u8 = NULL, };
}

HEADER_STATIC_INLINE
list_value_t __i_dlist_front(const i_list_t* _this)
{
    return (list_value_t){ .u8 = __i_dlist_begin(_this)->data, };
}

HEADER_STATIC_INLINE
list_value_t i_dlist_front(const i_list_t* _this)
{
    if (!is_null(_this) && !__i_dlist_empty(_this))
        return (list_value_t){ .u8 = __i_dlist_begin(_this)->data, };
    return (list_value_t){ .u8 = NULL, };
}





bool i_dlist_push_back(i_list_t* _this, list_data_t data);
bool i_dlist_push_front(i_list_t* _this, list_data_t data);
list_node_t* i_dlist_insert(i_list_t* _this, list_node_t* pos, list_data_t data);
list_node_t* i_dlist_insert_n(i_list_t* _this, list_node_t* pos, list_size_t n, list_data_t data);
void i_dlist_pop_back(i_list_t* _this);
void i_dlist_pop_front(i_list_t* _this);
list_node_t* i_dlist_erase(i_list_t* _this, list_node_t* pos);
list_node_t* i_dlist_erase_range(i_list_t* _this, list_node_t* begin, list_node_t* end);
list_node_t* i_dlist_find(const i_list_t* _this, list_data_t data);
list_count_t i_dlist_count(const i_list_t* _this, list_data_t data);
list_size_t i_dlist_remove(i_list_t* _this, list_data_t data);
list_size_t i_dlist_remove_if(i_list_t* _this, remove_if_condition cond);
list_size_t i_dlist_unique(i_list_t* _this);
void i_dlist_reverse(i_list_t* _this);
void i_dlist_splice(i_list_t* _this, list_iterator_t iterator, i_list_t* other);
void i_dlist_splice_range(i_list_t* _this, list_iterator_t iterator, i_list_t* other,
                          list_iterator_t iterator_begin, list_iterator_t iterator_end);
void i_dlist_merge(i_list_t* _this, i_list_t* other, list_cmp_t cmp);
void i_dlist_swap(i_list_t* _this, i_list_t* other);
bool i_dlist_resize(i_list_t* _this, list_size_t n, list_data_t default_data);
list_size_t i_dlist_assign(i_list_t* _this, list_size_t n, list_data_t data);
list_size_t i_dlist_clear(i_list_t* _this);

/* checked */
HEADER_STATIC_INLINE
list_iterator_t i_clist_find(const i_list_t* _this, list_data_t data)
{
    return __i_dlist_lnode_to_it(i_dlist_find(_this, data));
}

/* checked */
HEADER_STATIC_INLINE
list_iterator_t i_clist_insert(i_list_t* _this, list_iterator_t iterator, list_data_t data)
{
    return __i_dlist_lnode_to_it(i_dlist_insert(_this, __i_dlist_it_to_lnode(iterator), data));
}

/* checked */
HEADER_STATIC_INLINE
list_iterator_t i_clist_insert_n(i_list_t* _this, list_iterator_t iterator, list_size_t n, list_data_t data)
{
    return __i_dlist_lnode_to_it(i_dlist_insert_n(_this, __i_dlist_it_to_lnode(iterator), n, data));
}

/* checked */
HEADER_STATIC_INLINE
list_iterator_t i_clist_erase(i_list_t* _this, list_iterator_t iterator)
{
    return __i_dlist_lnode_to_it(i_dlist_erase(_this, __i_dlist_it_to_lnode(iterator)));
}

/* checked */
HEADER_STATIC_INLINE
list_iterator_t i_clist_erase_range(i_list_t* _this, list_iterator_t iterator_begin, list_iterator_t iterator_end)
{
    return __i_dlist_lnode_to_it(i_dlist_erase_range(_this,
                                   __i_dlist_it_to_lnode(iterator_begin),
                                   __i_dlist_it_to_lnode(iterator_end)));
}


#endif /* __J_I_LIST_H */
