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

#include <string.h>
#include <_log.h>
#include <_memory.h>
#include <_compiler_inter.h>
#include <linux/_compiler.h>
#include <linux/list.h>

#define TAG "[list]"

#if JDSC_ALIGNMEM
#define __i_dlist_node_pad(_this)   ((_this)->pad)
#else
#define __i_dlist_node_pad(_this)   0
#endif

static JDSC_INLINE
list_node_t* __i_dlist_make_list_node(const i_list_t* _this, list_data_t data)
{
    list_pad_t pad = __i_dlist_node_pad(_this);
    uint8_t* base = (uint8_t*)p_aligned_malloc(_this->align, pad + sizeof(list_node_t) + _this->step);
    list_node_t* n;

    if (is_null(base))
        return NULL;

    n = (list_node_t*)(base + pad);

    if (!__i_dlist_slot_write(_this, n->data, data)) {
        p_aligned_free(base, _this->align);
        return NULL;
    }

    return n;
}

static JDSC_INLINE
list_node_t* __i_dlist_make_list_node_copy_sso(const i_list_t* _this, const list_node_t* src)
{
    list_pad_t pad = __i_dlist_node_pad(_this);
    uint8_t* base = (uint8_t*)p_aligned_malloc(_this->align, pad + sizeof(list_node_t) + _this->step);
    list_node_t* n;

    if (is_null(base))
        return NULL;

    n = (list_node_t*)(base + pad);

    if (!ds_ops_copy_data_same_sso((list_data_t)src->data, (list_data_t*)n->data)) {
        p_aligned_free(base, _this->align);
        return NULL;
    }

    return n;
}

static JDSC_INLINE
void __i_dlist_destroy_list_node(const i_list_t* _this, list_node_t* node)
{
    list_pad_t pad = __i_dlist_node_pad(_this);
    uint8_t* base = (uint8_t*)node - pad;

    if (!is_null(_this->ops) && !is_null(_this->ops->free_data))
        _this->ops->free_data((list_data_t*)node->data);

    p_aligned_free(base, _this->align);
}

static JDSC_INLINE
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

bool i_dlist_push_back(i_list_t* _this, list_data_t data)
{
    if (unlikely(is_null(_this)))
        return false;
    return !is_null(__i_dlist_push_back(_this, data));
}

static JDSC_INLINE
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

bool i_dlist_push_front(i_list_t* _this, list_data_t data)
{
    if (unlikely(is_null(_this)))
        return false;
    return !is_null(__i_dlist_push_front(_this, data));
}

static JDSC_INLINE_FORCE JDSC_ONLY_WRAPPER JDSC_NO_ITERATOR
list_node_t* ___i_dlist_insert(i_list_t* _this, list_node_t* pos, list_node_t* node)
{
    __list_add(&node->node, pos->node.prev, &pos->node);
    _this->size++;
    return node;
}

static JDSC_INLINE
list_node_t* __i_dlist_insert_valid(i_list_t* _this, list_node_t* pos, list_data_t data)
{
    list_node_t* n;

    n = __i_dlist_make_list_node(_this, data);
    if (is_null(n))
        return NULL;

    ___i_dlist_insert(_this, pos, n);
    return n;
}

static JDSC_INLINE
list_node_t* __i_dlist_insert(i_list_t* _this, list_node_t* pos, list_data_t data)
{
    if (unlikely(__i_dlist_empty(_this) && __i_dlist_end(_this) != pos))
        return NULL;

    if (unlikely(!is_null(_this->ops) && !is_null(_this->ops->valid_data) && !_this->ops->valid_data(data)))
        return NULL;

    return __i_dlist_insert_valid(_this, pos, data);
}

list_node_t* i_dlist_insert(i_list_t* _this, list_node_t* pos, list_data_t data)
{
    if (unlikely(is_null(_this) || is_null(pos)))
        return NULL;
    return __i_dlist_insert(_this, pos, data);
}

static JDSC_INLINE list_node_t* __i_dlist_erase(i_list_t* _this, list_node_t* pos);
static
list_node_t* __i_dlist_insert_n(i_list_t* _this, list_node_t* pos, list_size_t n, list_data_t data)
{
    list_size_t i;
    list_node_t* t, * first;

    if (unlikely(n < 0))
        return NULL;

    if (unlikely(__i_dlist_empty(_this) && __i_dlist_end(_this) != pos))
        return NULL;

    if (unlikely(!is_null(_this->ops) && !is_null(_this->ops->valid_data) && !_this->ops->valid_data(data)))
        return NULL;

    if (unlikely(0 == n))
        return pos;

    first = __i_dlist_insert(_this, pos, data);
    if (is_null(first))
        return NULL;

    for (i = 1; i < n; ++i) {
        t = __i_dlist_make_list_node(_this, data);
        if (is_null(t))
            goto err;

        ___i_dlist_insert(_this, pos, t);
    }

    return first;

err:
    while ((first = __i_dlist_erase(_this, first)) != pos);
    return NULL;
}

list_node_t* i_dlist_insert_n(i_list_t* _this, list_node_t* pos, list_size_t n, list_data_t data)
{
    if (unlikely(is_null(_this) || is_null(pos)))
        return NULL;
    return __i_dlist_insert_n(_this, pos, n, data);
}

static JDSC_INLINE
list_node_t* __i_dlist_insert_range(i_list_t* _this, list_node_t* pos, list_node_t* begin, list_node_t* end)
{
    list_size_t suc = 0;
    list_node_t* it, * n, * first = __i_dlist_prev(pos);

    if (unlikely(__i_dlist_empty(_this) && __i_dlist_end(_this) != pos))
        return NULL;

    if (unlikely(begin == end))
        return pos;

    if (_this->sso) {
        for (it = begin; end != it; it = __i_dlist_next(it)) {
            n = __i_dlist_make_list_node_copy_sso(_this, it);
            if (is_null(n))
                goto err;

            ___i_dlist_insert(_this, pos, n);
            suc++;
        }
    } else {
        for (it = begin; end != it; it = __i_dlist_next(it)) {
            if (is_null(__i_dlist_insert_valid(_this, pos, __i_dlist_slot_read(_this, it->data))))
                goto err;
            suc++;
        }
    }
    return __i_dlist_next(first);

err:
    while (suc-- > 0)
        __i_dlist_erase(_this, __i_dlist_next(first));
    return NULL;
}

list_node_t* i_dlist_insert_range(i_list_t* _this, list_node_t* pos, list_node_t* begin, list_node_t* end)
{
    if (unlikely(is_null(_this) || is_null(pos) || is_null(begin) || is_null(end)))
        return NULL;
    return __i_dlist_insert_range(_this, pos, begin, end);
}

static JDSC_INLINE
list_node_t* __i_dlist_replace(i_list_t* _this, list_node_t* pos, list_data_t data)
{
    list_node_t* n;

    n = __i_dlist_insert(_this, pos, data);

    if (!is_null(n))
        __i_dlist_erase(_this, pos);

    return n;
}

list_node_t* i_dlist_replace(i_list_t* _this, list_node_t* pos, list_data_t data)
{
    if (unlikely(is_null(_this) || is_null(pos)))
        return NULL;
    return __i_dlist_replace(_this, pos, data);
}

static JDSC_INLINE_FORCE JDSC_ONLY_WRAPPER JDSC_NO_ITERATOR
void ___i_dlist_erase(i_list_t* _this, list_node_t* pos)
{
    list_del_init(&pos->node);
    _this->size--;
}

static JDSC_INLINE
void __i_dlist_pop_back(i_list_t* _this)
{
    list_node_t* n;

    JDSC_ASSERT(!__i_dlist_empty(_this));

    n = __i_dlist_rbegin(_this);
    ___i_dlist_erase(_this, n);
    __i_dlist_destroy_list_node(_this, n);
}

void i_dlist_pop_back(i_list_t* _this)
{
    if (unlikely(is_null(_this)))
        return ;
    __i_dlist_pop_back(_this);
}

static JDSC_INLINE
void __i_dlist_pop_front(i_list_t* _this)
{
    list_node_t* n;

    JDSC_ASSERT(!__i_dlist_empty(_this));

    n = __i_dlist_begin(_this);
    ___i_dlist_erase(_this, n);
    __i_dlist_destroy_list_node(_this, n);
}

void i_dlist_pop_front(i_list_t* _this)
{
    if (unlikely(is_null(_this)))
        return ;
    __i_dlist_pop_front(_this);
}

static JDSC_INLINE
list_node_t* __i_dlist_erase(i_list_t* _this, list_node_t* pos)
{
    list_node_t* n;

    /* The input parameter is `iterator`, and there's no need 
       to check whether it equals `rend` */
    if (unlikely(__i_dlist_empty(_this) || __i_dlist_end(_this) == pos) /* || __i_dlist_rend(_this) == pos*/)
        return NULL;

    n = __i_dlist_next(pos); /* No need to check null */
    ___i_dlist_erase(_this, pos);
    __i_dlist_destroy_list_node(_this, pos);
    return n;
}

list_node_t* i_dlist_erase(i_list_t* _this, list_node_t* pos)
{
    if (unlikely(is_null(_this) || is_null(pos)))
        return NULL;
    return __i_dlist_erase(_this, pos);
}

static
list_node_t* __i_dlist_erase_range(i_list_t* _this, list_node_t* begin, list_node_t* end)
{
    list_node_t* pos = begin;

    if (unlikely(__i_dlist_end(_this) == begin))
        return begin == end ? end : NULL;

    while (__i_dlist_end(_this) != pos && end != pos)
        pos = __i_dlist_erase(_this, pos);

    JDSC_ASSERT(end == pos);
    return pos;
}

list_node_t* i_dlist_erase_range(i_list_t* _this, list_node_t* begin, list_node_t* end)
{
    if (unlikely(is_null(_this) || is_null(begin) || is_null(end)))
        return NULL;
    return __i_dlist_erase_range(_this, begin, end);
}

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
            if (__ds_ops_eq_diff_sso(data, (list_data_t)n->data))
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

list_node_t* i_dlist_find(const i_list_t* _this, list_data_t data)
{
    list_node_t* n;

    if (unlikely(is_null(_this)))
        return NULL;

    if (unlikely(!is_null(_this->ops) && !is_null(_this->ops->valid_data) && !_this->ops->valid_data(data)))
        return NULL;

    JDSC_ASSERT(!((_this->dtype & SEQ_CNTR_DTYPE_MASK_STUN) && (is_null(_this->ops) || is_null(_this->ops->__eq))));

    n = ___i_dlist_find(_this, data);
    return is_null(n) ? i_dlist_end(_this) : n;
}

static JDSC_ONLY_WRAPPER JDSC_NO_ITERATOR
list_count_t ___i_dlist_count(const i_list_t* _this, list_data_t data)
{
    list_size_t ret = 0;
    list_node_t* n;

    if (is_null(_this->ops) || is_null(_this->ops->__eq)) {
        list_for_each_entry(n, &_this->head, node) {
            if (__i_dlist_slot_equal(_this->step, data, n->data))
                ret++;
        }
    } else if (_this->sso) {
        list_for_each_entry(n, &_this->head, node) {
            if (__ds_ops_eq_diff_sso(data, (list_data_t)n->data))
                ret++;
        }
    } else {
        list_for_each_entry(n, &_this->head, node) {
            if (_this->ops->__eq(data, __i_dlist_slot_read(_this, n->data)))
                ret++;
        }
    }
    return ret;
}

list_count_t i_dlist_count(const i_list_t* _this, list_data_t data)
{
    if (unlikely(is_null(_this)))
        return -1;

    if (unlikely(!is_null(_this->ops) && !is_null(_this->ops->valid_data) && !_this->ops->valid_data(data)))
        return -1;

    JDSC_ASSERT(!((_this->dtype & SEQ_CNTR_DTYPE_MASK_STUN) && (is_null(_this->ops) || is_null(_this->ops->__eq))));

    return ___i_dlist_count(_this, data);
}

list_size_t i_dlist_remove(i_list_t* _this, list_data_t data)
{
    list_size_t ret = 0;
    list_node_t* t, *n;

    if (unlikely(is_null(_this)))
        return -1;

    if (unlikely(!is_null(_this->ops) && !is_null(_this->ops->valid_data) && !_this->ops->valid_data(data)))
        return -1;

    JDSC_ASSERT(!((_this->dtype & SEQ_CNTR_DTYPE_MASK_STUN) && (is_null(_this->ops) || is_null(_this->ops->__eq))));

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
            if (!__ds_ops_eq_diff_sso(data, (list_data_t)n->data))
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

list_size_t i_dlist_remove_if(i_list_t* _this, remove_if_condition cond)
{
    list_size_t ret = 0;
    list_node_t* t, *n;

    if (unlikely(is_null(_this) || is_null(cond)))
        return -1;

    if (_this->sso) {
        list_for_each_entry_safe(n, t, &_this->head, node) {
            if (!cond(__ds_ops_char_addr_sso((list_data_t)n->data)))
                continue;

            ___i_dlist_erase(_this, n);
            __i_dlist_destroy_list_node(_this, n);
            ret++;
        }
    } else {
        list_for_each_entry_safe(n, t, &_this->head, node) {
            if (!cond(__i_dlist_slot_read(_this, n->data)))
                continue;

            ___i_dlist_erase(_this, n);
            __i_dlist_destroy_list_node(_this, n);
            ret++;
        }
    }

    return ret;
}

list_size_t i_dlist_unique(i_list_t* _this)
{
    list_size_t ret = 0;
    list_node_t* t, *n;

    if (unlikely(is_null(_this)))
        return -1;

    JDSC_ASSERT(!((_this->dtype & SEQ_CNTR_DTYPE_MASK_STUN) && (is_null(_this->ops) || is_null(_this->ops->__eq))));

    if (__i_dlist_size(_this) < 2)
        return 0;

    if (is_null(_this->ops) || is_null(_this->ops->__eq)) {
        n = __i_dlist_begin(_this);
        uint8_t* slot = n->data;

        list_for_each_entry_safe_continue(n, t, &_this->head, node) {
            if (!__i_dlist_slot_equal_slot(_this->step, slot, n->data)) {
                slot = n->data;
                continue;
            }

            ___i_dlist_erase(_this, n);
            __i_dlist_destroy_list_node(_this, n);
            ret++;
        }
    } else if (_this->sso) {
        n = __i_dlist_begin(_this);
        uint8_t* slot = n->data;

        list_for_each_entry_safe_continue(n, t, &_this->head, node) {
            if (!__ds_ops_eq_same_sso((list_data_t)slot, (list_data_t)n->data)) {
                slot = n->data;
                continue;
            }

            ___i_dlist_erase(_this, n);
            __i_dlist_destroy_list_node(_this, n);
            ret++;
        }
    } else {
        n = __i_dlist_begin(_this);
        list_data_t slot = __i_dlist_slot_read(_this, n->data);

        list_for_each_entry_safe_continue(n, t, &_this->head, node) {
            if (!_this->ops->__eq(slot, __i_dlist_slot_read(_this, n->data))) {
                slot = __i_dlist_slot_read(_this, n->data);
                continue;
            }

            ___i_dlist_erase(_this, n);
            __i_dlist_destroy_list_node(_this, n);
            ret++;
        }
    }

    return ret;
}

static list_size_t __i_dlist_clear(i_list_t* _this);
static JDSC_INLINE
bool __i_dlist_assign(i_list_t* _this, list_size_t n, list_data_t data)
{
    list_node_t* first;

    if (unlikely(n < 0))
        return false;

    if (unlikely(0 == n)) {
        __i_dlist_clear(_this);
        return true;
    }

    first = __i_dlist_insert_n(_this, __i_dlist_end(_this), n, data);
    if (is_null(first))
        return false;

    __i_dlist_erase_range(_this, __i_dlist_begin(_this), first);
    return true;
}

bool i_dlist_assign(i_list_t* _this, list_size_t n, list_data_t data)
{
    if (unlikely(is_null(_this)))
        return false;
    return __i_dlist_assign(_this, n, data);
}

static JDSC_INLINE
bool __i_dlist_assign_range(i_list_t* _this, list_node_t* begin, list_node_t* end)
{
    list_node_t* first;

    if (unlikely(begin == end)) {
        __i_dlist_clear(_this);
        return true;
    }

    first = __i_dlist_insert_range(_this, __i_dlist_end(_this), begin, end);
    if (is_null(first))
        return false;

    __i_dlist_erase_range(_this, __i_dlist_begin(_this), first);
    return true;
}

bool i_dlist_assign_range(i_list_t* _this, list_node_t* begin, list_node_t* end)
{
    if (unlikely(is_null(_this) || is_null(begin) || is_null(end)))
        return false;
    return __i_dlist_assign_range(_this, begin, end);
}

static JDSC_INLINE
bool __i_dlist_resize(i_list_t* _this, list_size_t n, list_data_t default_data)
{
    list_size_t size = __i_dlist_size(_this);

    if (unlikely(n < 0))
        return false;

    if (unlikely(n == size))
        return true;

    if (n < size)
        return !is_null(__i_dlist_erase_range(_this, __i_dlist_it_list_node(_this, n), __i_dlist_end(_this)));
    return !is_null(__i_dlist_insert_n(_this, __i_dlist_end(_this), n - size, default_data));
}

bool i_dlist_resize(i_list_t* _this, list_size_t n, list_data_t default_data)
{
    if (unlikely(is_null(_this)))
        return false;
    return __i_dlist_resize(_this, n, default_data);
}

static JDSC_INLINE
void __i_dlist_sort_swap_head(struct list_head* a, struct list_head* b)
{
    if (!list_empty(a)) {
        if (!list_empty(b)) {
            struct list_head* tmp;

            tmp = a->next; a->next = b->next; b->next = tmp;
            tmp = a->prev; a->prev = b->prev; b->prev = tmp;
            a->next->prev = a;
            a->prev->next = a;
            b->next->prev = b;
            b->prev->next = b;
        } else {
            b->next = a->next;
            b->prev = a->prev;
            a->next = a->prev = a;
            b->next->prev = b;
            b->prev->next = b;
        }
    } else if (!list_empty(b)) {
        a->next = b->next;
        a->prev = b->prev;
        b->next = b->prev = b;
        a->next->prev = a;
        a->prev->next = a;
    }
}

void i_dlist_sort(i_list_t* _this, __list_lt_t __lt)
{
    struct list_head carry;
    struct list_head bins[64];
    struct list_head* fill;
    struct list_head* counter;
    int i;

    if (unlikely(is_null(_this) || is_null(__lt)))
        return ;

    if (unlikely(__i_dlist_size(_this) < 2))
        return ;

    INIT_LIST_HEAD(&carry);
    for (i = 0; i < 64; ++i)
        INIT_LIST_HEAD(&bins[i]);

    /* Keep the std algorithm. */
    fill = bins;
    do {
        ___i_dlist_splice_range(&carry, _this->head.next, _this->head.next->next);

        for (counter = bins; counter != fill && !list_empty(counter); ++counter) {
            ___i_dlist_merge(_this, counter, &carry, __lt);
            __i_dlist_sort_swap_head(&carry, counter);
        }
        __i_dlist_sort_swap_head(&carry, counter);
        if (counter == fill)
            ++fill;
    } while (!list_empty(&_this->head));

    for (counter = bins + 1; counter != fill; ++counter)
        ___i_dlist_merge(_this, counter, counter - 1, __lt);

    __i_dlist_sort_swap_head(&_this->head, fill - 1);
}

static JDSC_INLINE
void __i_dlist_swap(i_list_t* _this, i_list_t* other)
{
    struct list_head thead;
    list_size_t tsize;

    JDSC_ASSERT(   _this->ops   == other->ops /* Include sso */
                && __sequence_container_dtype_swap_compatible(_this->dtype, other->dtype)
                && _this->step  == other->step
                && _this->align == other->align
                && _this->pad   == other->pad);

    list_replace_init(&_this->head, &thead);
    list_replace_init(&other->head, &_this->head);
    list_replace_init(&thead, &other->head);

    tsize = _this->size;
    _this->size = other->size;
    other->size = tsize;
}

void i_dlist_swap(i_list_t* _this, i_list_t* other)
{
    if (unlikely(is_null(_this) || is_null(other) || _this == other))
        return ;
    __i_dlist_swap(_this, other);
}

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

    if (step <= 0) {
        JDSC_ASSERT_NEW(0, "%s: Invalid element step %zu (must be greater than zero)!",
                        __func__, step);
        return NULL;
    }

    if (!__sequence_container_dtype_valid(data_type)) {
        JDSC_ASSERT_NEW(0, "%s: Invalid data type %zu!", __func__, data_type);
        return NULL;
    }

    width = __sequence_container_dtype_width(data_type);
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

    if (g_class_list_ops_sso() == ops && (SEQ_CNTR_DTYPE_SSO != data_type || sizeof(ds_sso_t) != step)) {
        JDSC_ASSERT_NEW(0, "%s: SSO ops require the SSO data type and a step of sizeof(ds_sso_t) (got %zu)!",
                        __func__, step);
        return NULL;
    }

    list = (list_t*)p_calloc(1, sizeof(list_t));
    if (is_null(list)) {
        JDSC_ASSERT_NEW(0, "%s: Failed to allocate %zu bytes for list!",
                        __func__, sizeof(list_t));
        return NULL;
    }

    list->ops   = ops;
    if (g_class_list_ops_sso() == list->ops)
        list->sso = true;
    list->dtype = data_type;
    list->step  = step;
    list->align = align ? align : JDSC_ALIGN_DEFAULT;
#if JDSC_ALIGNMEM
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
        .__back       = __clist_back,
        .back         = clist_back,
        .__front      = __clist_front,
        .front        = clist_front,
        .find         = clist_find,
        .push_back    = clist_push_back,
        .push_front   = clist_push_front,
        .insert       = clist_insert,
        .insert_n     = clist_insert_n,
        .insert_range = clist_insert_range,
        .erase        = clist_erase,
        .erase_range  = clist_erase_range,
        .pop_back     = clist_pop_back,
        .pop_front    = clist_pop_front,
        .remove       = clist_remove,
        .remove_if    = clist_remove_if,
        .unique       = clist_unique,
        .assign       = clist_assign,
        .assign_range = clist_assign_range,
        .resize       = clist_resize,
        .splice       = clist_splice,
        .splice_range = clist_splice_range,
        .merge        = clist_merge,
        .sort         = clist_sort,
        .reverse      = clist_reverse,
        .swap         = clist_swap,
        .clear        = clist_clear,
    };
    return &ins;
}
