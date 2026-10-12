/*
  I-List Interfaces
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

#include <stddef.h>
#include <stdint.h>
#include <_compiler.h>
#include <linux/list.h>
#include <linux/_compiler.h>
#include <list/list_ops.h>

#ifndef is_null
#define is_null(X) (!(X))
#endif /* is_null */

typedef struct list_node {
    struct list_head node;
    uint8_t          data[];
} list_node_t;

typedef struct i_list {
    const class_list_ops_t* ops;
    list_step_t             step;
    struct list_head        head; /* 注：不能是最后一个，否则地址不合法 */
    list_size_t             size;
} i_list_t;

/* checked */
static inline
list_size_t __i_dlist_size(const i_list_t* _this)
{
    return _this->size;
}

/* checked */
static inline
list_size_t i_dlist_size(const i_list_t* _this)
{
    return unlikely(is_null(_this)) ? -1 : __i_dlist_size(_this);
}

/* checked */
static inline
bool __i_dlist_empty(const i_list_t* _this)
{
    return list_empty(&_this->head);
}





/* checked */
#ifdef dlist_entry_lhead
#undef dlist_entry_lhead
#endif
#define dlist_entry_lhead(_ptr) ((list_node_t*)(_ptr))

/* checked */
static inline
list_node_t* __i_dlist_end(const i_list_t* _this)
{
    return dlist_entry_lhead(&_this->head);
}

/* checked */
static inline
list_node_t* i_dlist_end(const i_list_t* _this)
{
    if (unlikely(is_null(_this)))
        return NULL;
    return __i_dlist_end(_this);
}

/* checked */
static inline
list_node_t* __i_dlist_begin(const i_list_t* _this)
{
    return dlist_entry_lhead(_this->head.next);
}

/* checked */
static inline
list_node_t* i_dlist_begin(const i_list_t* _this)
{
    if (unlikely(is_null(_this)))
        return NULL;
    return __i_dlist_begin(_this);
}

/* checked */
static inline
list_node_t* __i_dlist_next(const i_list_t* _this, const list_node_t* node)
{
    JDSC_ASSERT(!__i_dlist_empty(_this));
    JDSC_ASSERT(__i_dlist_end(_this) != node);
#if JDSC_ITERATOR_ERR_NULL
    if (unlikely(__i_dlist_end(_this) == node))
        return NULL;
#endif /* JDSC_ITERATOR_ERR_NULL */
    return dlist_entry_lhead(node->node.next);
}

/* checked */
static inline
list_node_t* i_dlist_next(const i_list_t* _this, const list_node_t* node)
{
    if (unlikely(is_null(_this) || is_null(node)))
        return NULL;
    return __i_dlist_next(_this, node);
}

/* checked */
static inline
list_node_t* __i_dlist_prev(const i_list_t* _this, const list_node_t* node)
{
    JDSC_ASSERT(__i_dlist_begin(_this) != node);
    return dlist_entry_lhead(node->node.prev);
}

/* checked */
static inline
list_node_t* i_dlist_prev(const i_list_t* _this, const list_node_t* node)
{
    if (unlikely(is_null(_this) || is_null(node)))
        return NULL;
    return __i_dlist_prev(_this, node);
}

/* checked */
static inline
list_node_t* __i_dlist_rend(const i_list_t* _this)
{
    return dlist_entry_lhead(&_this->head);
}

/* checked */
static inline
list_node_t* i_dlist_rend(const i_list_t* _this)
{
    if (unlikely(is_null(_this)))
        return NULL;
    return __i_dlist_rend(_this);
}

/* checked */
static inline
list_node_t* __i_dlist_rbegin(const i_list_t* _this)
{
    return dlist_entry_lhead(_this->head.rnext);
}

/* checked */
static inline
list_node_t* i_dlist_rbegin(const i_list_t* _this)
{
    if (unlikely(is_null(_this)))
        return NULL;
    return __i_dlist_rbegin(_this);
}

/* checked */
static inline
list_node_t* __i_dlist_rnext(const i_list_t* _this, const list_node_t* node)
{
    JDSC_ASSERT(!__i_dlist_empty(_this));
    JDSC_ASSERT(__i_dlist_rend(_this) != node);
#if JDSC_ITERATOR_ERR_NULL
    if (unlikely(__i_dlist_rend(_this) == node))
        return NULL;
#endif /* JDSC_ITERATOR_ERR_NULL */
    return dlist_entry_lhead(node->node.rnext);
}

/* checked */
static inline
list_node_t* i_dlist_rnext(const i_list_t* _this, const list_node_t* node)
{
    if (unlikely(is_null(_this) || is_null(node)))
        return NULL;
    return __i_dlist_rnext(_this, node);
}

/* checked */
static inline
list_node_t* __i_dlist_rprev(const i_list_t* _this, const list_node_t* node)
{
    JDSC_ASSERT(__i_dlist_rbegin(_this) != node);
    return dlist_entry_lhead(node->node.rprev);
}

/* checked */
static inline
list_node_t* i_dlist_rprev(const i_list_t* _this, const list_node_t* node)
{
    if (unlikely(is_null(_this) || is_null(node)))
        return NULL;
    return __i_dlist_rprev(_this, node);
}

/* checked */
#undef dlist_entry_lhead

/* checked */
#define dlist_entry(_pdata) ((list_node_t*)((uint8_t*)(_pdata) - offsetof(list_node_t, data)))

/* checked */
static inline
list_iterator_t __i_dlist_lnode_to_it(const list_node_t* node)
{
    list_iterator_t it;
    it.d = is_null(node) ? NULL : (list_data_t*)node->data;
    return it;
}

/* checked */
static inline
list_node_t* __i_dlist_it_to_lnode(list_iterator_t it)
{
    return is_null(it.d) ? NULL : dlist_entry(it.d);
}

/* checked */
static inline
list_r_iterator_t __i_dlist_lnode_to_rit(const list_node_t* node)
{
    list_r_iterator_t r_it;
    r_it.d = is_null(node) ? NULL : (list_data_t*)node->data;
    return r_it;
}

/* checked */
static inline
list_node_t* __i_dlist_rit_to_lnode(list_r_iterator_t r_it)
{
    return is_null(r_it.d) ? NULL : dlist_entry(r_it.d);
}

/* checked */
static inline
list_iterator_t i_clist_end(const i_list_t* _this)
{
    return __i_dlist_lnode_to_it(i_dlist_end(_this));
}

/* checked */
static inline
list_iterator_t i_clist_begin(const i_list_t* _this)
{
    return __i_dlist_lnode_to_it(i_dlist_begin(_this));
}

/* checked */
static inline
list_iterator_t i_clist_next(const i_list_t* _this, list_iterator_t iterator)
{
    return __i_dlist_lnode_to_it(i_dlist_next(_this, __i_dlist_it_to_lnode(iterator)));
}

/* checked */
static inline
list_iterator_t i_clist_prev(const i_list_t* _this, list_iterator_t iterator)
{
    return __i_dlist_lnode_to_it(i_dlist_prev(_this, __i_dlist_it_to_lnode(iterator)));
}

/* checked */
static inline
list_r_iterator_t i_clist_rend(const i_list_t* _this)
{
    return __i_dlist_lnode_to_rit(i_dlist_rend(_this));
}

/* checked */
static inline
list_r_iterator_t i_clist_rbegin(const i_list_t* _this)
{
    return __i_dlist_lnode_to_rit(i_dlist_rbegin(_this));
}

/* checked */
static inline
list_r_iterator_t i_clist_rnext(const i_list_t* _this, list_r_iterator_t r_iterator)
{
    return __i_dlist_lnode_to_rit(i_dlist_rnext(_this, __i_dlist_rit_to_lnode(r_iterator)));
}

/* checked */
static inline
list_r_iterator_t i_clist_rprev(const i_list_t* _this, list_r_iterator_t r_iterator)
{
    return __i_dlist_lnode_to_rit(i_dlist_rprev(_this, __i_dlist_rit_to_lnode(r_iterator)));
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
bool i_dlist_resize(i_list_t* _this, list_size_t n, list_data_t default_data);
list_size_t i_dlist_assign(i_list_t* _this, list_size_t n, list_data_t data);
list_size_t i_dlist_clear(i_list_t* _this);


/* checked */
static inline
list_iterator_t i_clist_find(const i_list_t* _this, list_data_t data)
{
    return __i_dlist_lnode_to_it(i_dlist_find(_this, data));
}

/* checked */
static inline
list_iterator_t i_clist_insert(i_list_t* _this, list_iterator_t iterator, list_data_t data)
{
    return __i_dlist_lnode_to_it(i_dlist_insert(_this, __i_dlist_it_to_lnode(iterator), data));
}

/* checked */
static inline
list_iterator_t i_clist_insert_n(i_list_t* _this, list_iterator_t iterator, list_size_t n, list_data_t data)
{
    return __i_dlist_lnode_to_it(i_dlist_insert_n(_this, __i_dlist_it_to_lnode(iterator), n, data));
}

/* checked */
static inline
list_iterator_t i_clist_erase(i_list_t* _this, list_iterator_t iterator)
{
    return __i_dlist_lnode_to_it(i_dlist_erase(_this, __i_dlist_it_to_lnode(iterator)));
}

/* checked */
static inline
list_iterator_t i_clist_erase_range(i_list_t* _this, list_iterator_t iterator_begin, list_iterator_t iterator_end)
{
    return __i_dlist_lnode_to_it(i_dlist_erase_range(_this,
                                   __i_dlist_it_to_lnode(iterator_begin),
                                   __i_dlist_it_to_lnode(iterator_end)));
}

/* ============================================================================
 * sort —— STL: list::sort()，carry + tmp[64] 的 64 路归并。
 * 排序对象是节点里存的值（step > 8 时就是槽位地址，例如 ds_sso_t*），
 * 读法同 __i_dlist_slot_read，也就是和 ops->__eq 的 right 一致。
 *
 * cmp 的约定：
 *   NULL 或者 (list_cmp_t)1  -> 对普通数字升序（不看 ops）
 *   (list_cmp_t)2            -> 对普通数字降序（不看 ops）
 *   其它                      -> 直接当比较器调用
 * 便捷宏见 list/list.h 的 LIST_SORT_ASC / LIST_SORT_DESC。
 * ========================================================================== */
/* checked */
static inline
bool __i_dlist_lt_num_asc(list_data_t left, list_data_t right)
{
    return left < right;
}

/* checked */
static inline
bool __i_dlist_lt_num_desc(list_data_t left, list_data_t right)
{
    return left > right;
}

void __i_dlist_swap_head(i_list_t* _this, i_list_t* other);
void __i_dlist_merge_run(i_list_t* _this, i_list_t* other, list_cmp_t cmp);
void __i_dlist_sort(i_list_t* _this, list_cmp_t cmp);

/* checked */
static inline
void i_clist_sort(i_list_t* _this, list_cmp_t cmp)
{
    if (is_null(_this))
        return ;

    if (is_null(cmp) || (uintptr_t)cmp == 1)
        cmp = __i_dlist_lt_num_asc;
    else if ((uintptr_t)cmp == 2)
        cmp = __i_dlist_lt_num_desc;

    __i_dlist_sort(_this, cmp);
}





/* checked */
static inline
list_iterator_t i_dlist_null_iterator(void)
{
    list_iterator_t it;
    it.d = NULL;
    return it;
}

/* checked */
static inline
list_r_iterator_t i_dlist_null_r_iterator(void)
{
    list_r_iterator_t rit;
    rit.d = NULL;
    return rit;
}

/* checked */
static inline
list_iterator_t __i_dlist_make_iterator(const void* ptr)
{
    list_iterator_t it;
    it.d = (list_data_t*)ptr;
    return it;
}

/* checked */
static inline
list_node_t* __i_dlist_it_list_node(const i_list_t* _this, list_index_t index)
{
    list_node_t* it;
    list_size_t size = __i_dlist_size(_this);

    if (index < size) {
        if (index * 2 >= size) {
            index = size - index;
            it = __i_dlist_end(_this);
            while (index--) it = __i_dlist_prev(_this, it);
        } else {
            it = __i_dlist_begin(_this);
            while (index--) it = __i_dlist_next(_this, it);
        }
        return it;
    }

    return NULL;
}

/* checked */
static inline
list_iterator_t __i_dlist_iterator(const i_list_t* _this, list_index_t index)
{
    list_node_t* it = __i_dlist_it_list_node(_this, index);
    return is_null(it) ? i_dlist_null_iterator() : __i_dlist_make_iterator(it->data);
}

/* checked */
static inline
list_iterator_t i_dlist_iterator(const i_list_t* _this, list_index_t index)
{
    if (!is_null(_this))
        return __i_dlist_iterator(_this, index);
    return i_dlist_null_iterator();
}

/* checked */
static inline
list_value_t __i_dlist_at(const i_list_t* _this, list_index_t index)
{
    return (list_value_t){ .u8 = (uint8_t*)__i_dlist_iterator(_this, index).d, };
}

/* checked */
static inline
list_value_t i_dlist_at(const i_list_t* _this, list_index_t index)
{
    if (!is_null(_this))
        return __i_dlist_at(_this, index);
    return (list_value_t){ .u8 = NULL, };
}

/* checked */
static inline
list_value_t i_dlist_front(const i_list_t* _this)
{
    if (!is_null(_this) && !__i_dlist_empty(_this))
        return (list_value_t){ .u8 = __i_dlist_begin(_this)->data, };
    return (list_value_t){ .u8 = NULL, };
}

/* checked */
static inline
list_value_t i_dlist_back(const i_list_t* _this)
{
    if (!is_null(_this) && !__i_dlist_empty(_this))
        return (list_value_t){ .u8 = __i_dlist_rbegin(_this)->data, };
    return (list_value_t){ .u8 = NULL, };
}

#endif /* __J_I_LIST_H */
