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
    case 8: *(uint64_t*)slot = (uint64_t)data; break;
    default: return false;
    }
    return true;
}

/* checked */
static inline
list_data_t __i_dlist_slot_read(const i_list_t* _this, const uint8_t* slot)
{
    switch (_this->step)
    {
    case 1: return *(const uint8_t*)slot;
    case 2: return *(const uint16_t*)slot;
    case 4: return *(const uint32_t*)slot;
    case 8: return *(const uint64_t*)slot;
    default: return (list_data_t)(uintptr_t)slot;
    }
}

/* checked */
static inline
list_node_t* __i_dlist_make_list_node(const list_t* _this, list_data_t data)
{
    list_node_t* n;

    n = (list_node_t*)p_malloc(sizeof(list_node_t) + _this->step);
    if (is_null(n))
        return NULL;

    if (!__i_dlist_slot_write(_this, n->data, data)) {
        p_free(n);
        return NULL;
    }

    return n;
}

/* checked */
static inline
void __i_dlist_destroy_list_node(const i_list_t* _this, list_node_t* node)
{
    if (!is_null(_this->ops) && !is_null(_this->ops->free_data))
        _this->ops->free_data((list_data_t*)node->data);

    p_free(node);
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
list_node_t* ___i_dlist_insert(list_t* _this, list_node_t* pos, list_node_t* node)
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
void ___i_dlist_erase(list_t* _this, list_node_t* pos)
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
list_node_t* ___i_dlist_find(const list_t* _this, list_data_t data)
{
    list_node_t* n;

    if (is_null(_this->ops) || is_null(_this->ops->__eq)) {
        list_for_each_entry(n, &_this->head, node) {
            if (__i_dlist_slot_read(_this, n->data) == data)
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
list_node_t* i_dlist_find(const list_t* _this, list_data_t data)
{
    list_node_t* n;

    if (unlikely(is_null(_this)))
        return NULL;

    if (unlikely(!is_null(_this->ops) && !is_null(_this->ops->valid_data) && !_this->ops->valid_data(data)))
        return NULL;

    n = ___i_dlist_find(_this, data);
    return is_null(n) ? i_dlist_end(_this) : n;
}

/* checked */
static JDSC_ONLY_WRAPPER JDSC_NO_ITERATOR
list_count_t ___i_dlist_count(const list_t* _this, list_data_t data)
{
    list_size_t ret = 0;
    list_node_t* t, * n;

    if (is_null(_this->ops) || is_null(_this->ops->__eq)) {
        list_for_each_entry_safe(n, t, &_this->head, node) {
            if (__i_dlist_slot_read(_this, n->data) == data)
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
list_count_t i_dlist_count(const list_t* _this, list_data_t data)
{
    if (unlikely(is_null(_this)))
        return -1;

    if (unlikely(!is_null(_this->ops) && !is_null(_this->ops->valid_data) && !_this->ops->valid_data(data)))
        return -1;

    return ___i_dlist_count(_this, data);
}

/* checked */
list_size_t i_dlist_remove(list_t* _this, list_data_t data)
{
    list_size_t ret = 0;
    list_node_t* t, *n;

    if (unlikely(is_null(_this)))
        return -1;

    if (!is_null(_this->ops) && !is_null(_this->ops->valid_data) && !_this->ops->valid_data(data))
        return -1;

    if (is_null(_this->ops) || is_null(_this->ops->__eq)) {
        list_for_each_entry_safe(n, t, &_this->head, node) {
            if (__i_dlist_slot_read(_this, n->data) != data)
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
list_size_t i_dlist_remove_if(list_t* _this, remove_if_condition cond)
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
list_size_t i_dlist_clear(list_t* _this)
{
    if (unlikely(is_null(_this)))
        return -1;
    return __i_dlist_clear(_this);
}

/* ============================================================================
 * sort —— STL: list::sort()，carry + tmp[] 的 64 路归并
 *
 * 每次从原链摘一个节点进 carry，然后跟 tmp[] 里长度递增的已序段两两归并，
 * 像二进制加法一样往上进位。全程只改指针，元素一次都不搬。
 * i_clist_sort（i_list.h）把 cmp 的魔术数字归一化之后再调到这里。
 * ========================================================================== */
#define _I_DLIST_SORT_TMP  64

/* 由 struct list_head* 找回 list_node_t*（node 恒在 offset 0） */
#define __i_dlist_node_entry(_ptr)  ((list_node_t*)(void*)(_ptr))

/* checked —— 纯改指针，不碰 size，也允许跨容器（first/last 属于源链、pos 属于目标链） */
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

/* checked —— 两个节点里存的值做比较，只看调用方给的 cmp。
   不经 ops->__lt：i_clist_sort 已经把 NULL / 1 / 2 归一化成具体比较器了，走不到这里；
   留着那条分支只会让人以为「传 NULL 会用容器自己的次序」，其实不会。 */
static inline
bool __i_dlist_value_lt(const i_list_t* _this, list_cmp_t cmp, const list_node_t* l, const list_node_t* r)
{
    list_data_t lv = __i_dlist_slot_read(_this, l->data);
    list_data_t rv = __i_dlist_slot_read(_this, r->data);

    return is_null(cmp) ? (lv < rv) : cmp(lv, rv);
}

/* checked —— 让 shell 变成「和 proto 同配置的空链表」，给 sort 的 tmp[] 用 */
static inline
void __i_dlist_init_shell(i_list_t* shell, const i_list_t* proto)
{
    shell->ops  = proto->ops;
    shell->step = proto->step;
    shell->size = 0;
    INIT_LIST_HEAD(&shell->head);
}

/* ---------------------------------------------------------------------------
 * STL: _List_node_base::swap(__x, __y) —— 换的是两条链的「头」，元素一个都不动
 * ------------------------------------------------------------------------- */
/* checked */
void __i_dlist_swap_head(i_list_t* _this, i_list_t* other)
{
    struct list_head* a = &_this->head;
    struct list_head* b = &other->head;
    list_size_t       s;

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

/* ---------------------------------------------------------------------------
 * STL: list::merge(list& __x) —— 纯搬链，不碰 size，size 由调用方负责
 * ------------------------------------------------------------------------- */
/* checked */
void __i_dlist_merge_run(i_list_t* _this, i_list_t* other, list_cmp_t cmp)
{
    struct list_head* first1 = _this->head.next;
    struct list_head* last1  = &_this->head;
    struct list_head* first2 = other->head.next;
    struct list_head* last2  = &other->head;

    while (first1 != last1 && first2 != last2) {
        if (__i_dlist_value_lt(_this, cmp, __i_dlist_node_entry(first2), __i_dlist_node_entry(first1))) {
            struct list_head* next = first2->next;

            __i_dlist_transfer(first1, first2, next);
            first2 = next;
        } else {
            first1 = first1->next;
        }
    }

    if (first2 != last2)
        __i_dlist_transfer(last1, first2, last2);
}

/* ---------------------------------------------------------------------------
 * STL: list::sort()
 * ------------------------------------------------------------------------- */
/* checked */
void __i_dlist_sort(i_list_t* _this, list_cmp_t cmp)
{
    i_list_t    carry;
    i_list_t    tmp[_I_DLIST_SORT_TMP];
    i_list_t*   fill;
    i_list_t*   counter;
    list_size_t n = _this->size;
    int         i;

    if (n < 2)
        return ;

    __i_dlist_init_shell(&carry, _this);
    for (i = 0; i < _I_DLIST_SORT_TMP; ++i)
        __i_dlist_init_shell(&tmp[i], _this);

    fill = tmp;
    do {
        /* carry.splice(carry.begin(), *this, this->begin()) */
        __i_dlist_transfer(&carry.head, _this->head.next, _this->head.next->next);

        for (counter = tmp; counter != fill && !list_empty(&counter->head); ++counter) {
            __i_dlist_merge_run(counter, &carry, cmp);
            __i_dlist_swap_head(counter, &carry);
        }
        __i_dlist_swap_head(counter, &carry);
        if (counter == fill)
            ++fill;
    } while (!list_empty(&_this->head));

    for (counter = tmp + 1; counter != fill; ++counter)
        __i_dlist_merge_run(counter, counter - 1, cmp);

    __i_dlist_swap_head(_this, fill - 1);
    _this->size = n; /* swap_head 只换「头」，size 得自己找回来 */
}

/* checked */
list_t* __list_new(const class_list_ops_t* ops, list_step_t step)
{
    list_t* list;

    if (0 == step)
        return NULL;

    list = (list_t*)p_calloc(1, sizeof(list_t));
    if (is_null(list))
        return NULL;

    list->ops  = ops;
    list->step = step;
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
        .sort         = clist_sort,
        /* TODO: unique / reverse / merge / splice / splice_range / swap 尚未移植到新的节点 API，
                 暂时从 vtable 里摘掉，等补上实现再加回来（旧实现在 HEAD 的 include/list/i_list.h 里）。 */
        .resize       = clist_resize,
        .assign       = clist_assign,
        .clear        = clist_clear,
    };
    return &ins;
}
