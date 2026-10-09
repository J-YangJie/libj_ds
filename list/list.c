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

/* STL list::sort() 里那个 __tmp[64]：长度够 2^64 个元素，够用了 */
#define _I_LIST_SORT_TMP  64

/* ---------------------------------------------------------------------------
 * 节点分配 —— 对应 STL 的 _List_impl::_M_create_node 与 _Alloc_traits::construct
 *
 * 节点一次分配 sizeof(struct list_head) + step 字节：前半是指针域（offset 0），
 * 后半是元素槽（offset 16）。所以元素拿到的是 p_malloc 那一档对齐，用户
 * alignas 超过这一档的诉求在这里满足不了 —— 详见 include/list/i_list.h 顶部注释。
 * ------------------------------------------------------------------------- */
/* checked */
list_node_t* __i_list_alloc_node(i_list_t* _this, list_data_t data)
{
    list_node_t* t;

    if (is_null(_this))
        return NULL;

    if (!is_null(_this->ops) && !is_null(_this->ops->valid_data) && !_this->ops->valid_data(data))
        return NULL;

    t = (list_node_t*)p_malloc(sizeof(list_node_t) + _this->step);
    if (is_null(t))
        return NULL;

    if (!__i_list_value_write(_this, t->value, data)) {
        p_free(t);
        return NULL;
    }

    return t;
}

/* checked */
void __i_list_free_node(i_list_t* _this, list_node_t* node)
{
    if (!is_null(_this->ops) && !is_null(_this->ops->free_data))
        _this->ops->free_data((list_data_t*)node->value);

    p_free(node);
}

/* checked */
list_size_t __i_list_clear(i_list_t* _this)
{
    list_size_t       ret = _this->size;
    struct list_head* p   = _this->head.next;

    while (p != &_this->head) {
        struct list_head* n = p->next;
        list_node_t*      t = (list_node_t*)p;

        if (!is_null(_this->ops) && !is_null(_this->ops->free_data))
            _this->ops->free_data((list_data_t*)t->value);

        p_free(t);
        p = n;
    }

    INIT_LIST_HEAD(&_this->head);
    _this->size = 0;
    return ret;
}

/* ---------------------------------------------------------------------------
 * STL: _List_node_base::swap(__x, __y) —— 换的是两条链的「头」，元素一个都不动
 * ------------------------------------------------------------------------- */
/* checked */
void __i_list_swap_head(i_list_t* _this, i_list_t* other)
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
void __i_list_merge_run(i_list_t* _this, i_list_t* other, list_cmp_t cmp)
{
    struct list_head* first1 = _this->head.next;
    struct list_head* last1  = &_this->head;
    struct list_head* first2 = other->head.next;
    struct list_head* last2  = &other->head;

    while (first1 != last1 && first2 != last2) {
        if (__i_list_value_lt_cmp(_this, cmp, __I_LIST_NODE2VALUE(first2), __I_LIST_NODE2VALUE(first1))) {
            struct list_head* next = first2->next;

            __i_list_transfer(first1, first2, next);
            first2 = next;
        } else {
            first1 = first1->next;
        }
    }

    if (first2 != last2)
        __i_list_transfer(last1, first2, last2);
}

/* ---------------------------------------------------------------------------
 * STL: list::sort() —— carry + __tmp[64] 的 64 路归并
 *
 * 每次从原链摘一个节点进 carry，然后跟 tmp[] 里长度递增的已序段两两归并，
 * 像二进制加法一样往上进位。全程只改指针，元素一次都不搬。
 * ------------------------------------------------------------------------- */
/* checked */
void __i_list_sort(i_list_t* _this, list_cmp_t cmp)
{
    i_list_t   carry;
    i_list_t   tmp[_I_LIST_SORT_TMP];
    i_list_t*  fill;
    i_list_t*  counter;
    list_size_t n = _this->size;
    int        i;

    if (n < 2)
        return ;

    __i_list_init_shell(&carry, _this);
    for (i = 0; i < _I_LIST_SORT_TMP; ++i)
        __i_list_init_shell(&tmp[i], _this);

    fill = tmp;
    do {
        /* carry.splice(carry.begin(), *this, this->begin()) */
        __i_list_transfer(__i_list_base(&carry), _this->head.next, _this->head.next->next);

        for (counter = tmp; counter != fill && !list_empty(&counter->head); ++counter) {
            __i_list_merge_run(counter, &carry, cmp);
            __i_list_swap_head(counter, &carry);
        }
        __i_list_swap_head(counter, &carry);
        if (counter == fill)
            ++fill;
    } while (!list_empty(&_this->head));

    for (counter = tmp + 1; counter != fill; ++counter)
        __i_list_merge_run(counter, counter - 1, cmp);

    __i_list_swap_head(_this, fill - 1);
    _this->size = n; /* swap_head 只换「头」，size 得自己找回来 */
}

/* ---------------------------------------------------------------------------
 * STL: list::insert(pos, n, data) —— 往 pos 之前塞 n 份；失败时把已塞的撤回来
 * ------------------------------------------------------------------------- */
/* checked */
void __i_list_insert_n(i_list_t* _this, struct list_head* pos, list_size_t n, list_data_t data, list_iterator_t* out)
{
    list_iterator_t at;
    list_iterator_t first = i_list_null_iterator();
    list_node_t*    t;
    list_size_t     i;

    for (i = 0; i < n; ++i) {
        t = __i_list_alloc_node(_this, data);
        if (is_null(t))
            goto err;

        __i_list_hook(t, pos);
        _this->size++;

        if (0 == i)
            first = __i_list_make_iterator(t->value);
    }

    *out = first;
    return ;

err:
    if (!i_list_is_null_iterator(first)) {
        at = (pos == __i_list_base(_this)) ? __i_list_end(_this)
                                           : __i_list_make_iterator(__I_LIST_NODE2VALUE(pos));
        __i_list_erase_range(_this, first, at);
    }
    *out = i_list_null_iterator();
}

/* checked */
list_size_t __i_list_resize(i_list_t* _this, list_size_t n, list_data_t default_data)
{
    list_iterator_t ret;

    if (n == _this->size)
        return n;

    if (n < _this->size) {
        ret = __i_list_erase_range(_this, __i_list_it(_this, n), __i_list_end(_this));
        return i_list_is_null_iterator(ret) ? -1 : n;
    }

    if (!is_null(_this->ops) && !is_null(_this->ops->valid_data) && !_this->ops->valid_data(default_data))
        return -1;

    __i_list_insert_n(_this, __i_list_base(_this), n - _this->size, default_data, &ret);
    return i_list_is_null_iterator(ret) ? -1 : n;
}

/* checked */
list_size_t __i_list_assign(i_list_t* _this, list_size_t n, list_data_t data)
{
    list_iterator_t ret;

    __i_list_clear(_this);

    if (0 == n)
        return 0;

    if (!is_null(_this->ops) && !is_null(_this->ops->valid_data) && !_this->ops->valid_data(data))
        return -1;

    __i_list_insert_n(_this, __i_list_base(_this), n, data, &ret);
    return i_list_is_null_iterator(ret) ? -1 : n;
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
    if (g_class_list_ops_sso() == list->ops)
        list->sso = true;
    return list;
}

/* checked */
void __list_delete(list_t** _this)
{
    if (is_null(_this) || is_null(*_this))
        return ;

    __i_list_clear(*_this);
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
        .front        = clist_front,
        .back         = clist_back,
        .first        = clist_first,
        .last         = clist_last,
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
        .sort         = clist_sort,
        .merge        = clist_merge,
        .splice       = clist_splice,
        .splice_range = clist_splice_range,
        .resize       = clist_resize,
        .swap         = clist_swap,
        .assign       = clist_assign,
        .clear        = clist_clear,
    };
    return &ins;
}
