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

/* ============================================================================
 * 节点布局 —— 对标 bits/stl_list.h 的 __detail::_List_node_base + _List_node
 *
 *     struct list_node {
 *         struct list_head node;     <-- offset 0，对应 _List_node_base
 *         uint8_t           value[]; <-- offset 16，对应 _M_storage
 *     };
 *
 * 为什么 node 放前面（这里是和 vector / deque 最大的不同，务必留意）：
 *   next / prev 是一对指针，必须落在指针对齐的地址上。若把 value 放前面、
 *   由用户传进来的 `step` 决定 node 的偏移，那么 step 为 1 / 2 / 4 时 node 就会
 *   落在非指针对齐的地址上 —— 在 ARM 上直接是 UB。放前面之后 node 恒在 offset 0，
 *   永远和 struct list_head 一样对齐。
 *
 *   代价：value 被顶到 offset 16，所以元素实际拿到的对齐就是 p_malloc 给的那一档
 *   （64 位 glibc 是 16，32 位是 8），用户 alignas(32) 之类的超对齐诉求满足不了。
 *   这一点和 vector / deque 不同 —— 那边元素直接铺在 malloc 出来的连续槽里，
 *   偏移只由 step 决定，容器侧不掺指针。
 *
 * 迭代器（list_iterator_t / list_r_iterator_t）里的 .d / .cur 指向「值」本身，
 * 也就是 node + offsetof(list_node_t, value)，这样：
 *   - iterator/iterator.h 的 it_data(it) == *(it).d 继续成立；
 *   - stress/stress_slow.h 的 stress_it_data(it) == *(ds_data_t*)((it).d) 继续成立；
 *   - ds_sso_t 的 p 恰好在 offset 0，所以 it_sdata(it) == *(it).sd 对 SSO 也成立。
 *
 * end() 和 rend() 是同一个哨兵：&_this->head。底层本来就是内核双向环形链表，
 * 首尾天然相接，所以不需要老版本那套 iterator_end() / iterator_rend() 魔数。
 * ========================================================================== */
typedef struct list_node {
    struct list_head node;
    uint8_t          value[];
} list_node_t;

#define __I_LIST_NODE2VALUE(_n)    ((uint8_t*)(_n) + offsetof(list_node_t, value))
#define __I_LIST_VALUE2NODE(_v)    ((list_node_t*)((uint8_t*)(_v) - offsetof(list_node_t, value)))

typedef struct i_list {
    const class_list_ops_t* ops;
    list_step_t             step;
    struct list_head        head;
    list_size_t             size;
    bool                    sso;   /* 仅作标记：list 的节点永不搬迁，所以用不到 __ds_ops_fix_sso */
} i_list_t;

/* ---------------------------------------------------------------------------
 * 基础
 * ------------------------------------------------------------------------- */
/* checked */
static inline
bool __i_list_empty(const i_list_t* _this)
{
    return list_empty(&_this->head);
}

/* checked(空表时返回哨兵自身) */
static inline
struct list_head* __i_list_base(const i_list_t* _this)
{
    return (struct list_head*)(uintptr_t)&_this->head;
}

/* checked */
static inline
list_node_t* __i_list_first_node(const i_list_t* _this)
{
    if (__i_list_empty(_this))
        return NULL;
    return (list_node_t*)__i_list_base(_this)->next;
}

/* checked */
static inline
list_node_t* __i_list_last_node(const i_list_t* _this)
{
    if (__i_list_empty(_this))
        return NULL;
    return (list_node_t*)__i_list_base(_this)->prev;
}

/* checked */
static inline
list_size_t __i_list_size(const i_list_t* _this)
{
    return _this->size;
}

/* checked */
static inline
list_size_t i_list_size(const i_list_t* _this)
{
    return is_null(_this) ? -1 : __i_list_size(_this);
}

/* ---------------------------------------------------------------------------
 * 迭代器构造 / 判定
 * ------------------------------------------------------------------------- */
/* checked */
static inline
bool i_list_is_null_iterator(list_iterator_t it)
{
    return is_null(it.cur);
}

/* checked */
static inline
bool i_list_is_null_r_iterator(list_r_iterator_t it)
{
    return is_null(it.cur);
}

/* checked */
static inline
list_iterator_t i_list_null_iterator(void)
{
    list_iterator_t it;
    it.cur = NULL;
    return it;
}

/* checked */
static inline
list_r_iterator_t i_list_null_r_iterator(void)
{
    list_r_iterator_t it;
    it.cur = NULL;
    return it;
}

/* checked */
static inline
list_iterator_t __i_list_make_iterator(const uint8_t* cur)
{
    list_iterator_t it;
    it.cur = (uint8_t*)cur;
    return it;
}

/* checked */
static inline
list_iterator_t __i_list_end(const i_list_t* _this)
{
    return __i_list_make_iterator((const uint8_t*)&_this->head);
}

/* checked —— end() 和 rend() 是同一个哨兵（内核环形链表首尾相接） */
static inline
list_iterator_t __i_list_rend(const i_list_t* _this)
{
    return __i_list_make_iterator((const uint8_t*)&_this->head);
}

/* checked */
static inline
bool __i_list_is_end(const i_list_t* _this, const list_iterator_t it)
{
    return (const uint8_t*)it.cur == (const uint8_t*)&_this->head;
}

/* 把迭代器还原成节点。调用前必须先用 __i_list_is_end() 排掉哨兵 */
static inline
list_node_t* __i_list_it_node(const list_iterator_t it)
{
    return __I_LIST_VALUE2NODE(it.cur);
}

/* 迭代器落点的 list_head：哨兵时就是 head 本身（等价于「插到尾部」） */
static inline
struct list_head* __i_list_it_head(const i_list_t* _this, const list_iterator_t it)
{
    return __i_list_is_end(_this, it) ? __i_list_base(_this) : &__i_list_it_node(it)->node;
}

/* ---------------------------------------------------------------------------
 * 迭代器推进 —— 四个方向的语义和旧版完全一致
 *
 *   next  : 尾元素 -> end ；end -> end（含空表，此时 begin == end）
 *   prev  : 首元素 -> end ；end -> 尾元素
 *   rnext : 首元素 -> rend；rend -> rend
 *   rprev : 尾元素 -> rend；rend -> 首元素
 *
 * 因为 end 与 rend 同为 &head，所以 next/prev 与 rprev/rnext 只差哨兵那一步。
 * ------------------------------------------------------------------------- */
/* checked */
static inline
list_iterator_t __i_list_begin(const i_list_t* _this)
{
    list_node_t* n = __i_list_first_node(_this);

    if (is_null(n))
        return __i_list_end(_this);
    return __i_list_make_iterator(__I_LIST_NODE2VALUE(n));
}

/* checked */
static inline
list_iterator_t __i_list_next(const i_list_t* _this, const list_iterator_t it)
{
    struct list_head* t;

    if (__i_list_is_end(_this, it))
        return __i_list_end(_this);

    t = __i_list_it_node(it)->node.next;
    if (t == __i_list_base(_this))
        return __i_list_end(_this);
    return __i_list_make_iterator(__I_LIST_NODE2VALUE(t));
}

/* checked */
static inline
list_iterator_t __i_list_prev(const i_list_t* _this, const list_iterator_t it)
{
    struct list_head* t;

    if (__i_list_empty(_this))
        return __i_list_end(_this);

    if (__i_list_is_end(_this, it)) /* --end() == 尾元素 */
        return __i_list_make_iterator(__I_LIST_NODE2VALUE(__i_list_last_node(_this)));

    t = __i_list_it_node(it)->node.prev;
    if (t == __i_list_base(_this))
        return __i_list_end(_this);
    return __i_list_make_iterator(__I_LIST_NODE2VALUE(t));
}

/* checked */
static inline
list_iterator_t __i_list_rbegin(const i_list_t* _this)
{
    list_node_t* n = __i_list_last_node(_this);

    if (is_null(n))
        return __i_list_end(_this);
    return __i_list_make_iterator(__I_LIST_NODE2VALUE(n));
}

/* checked */
static inline
list_iterator_t __i_list_rnext(const i_list_t* _this, const list_iterator_t it)
{
    struct list_head* t;

    if (__i_list_is_end(_this, it))
        return __i_list_end(_this);

    t = __i_list_it_node(it)->node.prev;
    if (t == __i_list_base(_this))
        return __i_list_end(_this);
    return __i_list_make_iterator(__I_LIST_NODE2VALUE(t));
}

/* checked */
static inline
list_iterator_t __i_list_rprev(const i_list_t* _this, const list_iterator_t it)
{
    struct list_head* t;

    if (__i_list_empty(_this))
        return __i_list_end(_this);

    if (__i_list_is_end(_this, it)) /* ++rend() 语义上是首元素，不是尾元素 */
        return __i_list_make_iterator(__I_LIST_NODE2VALUE(__i_list_first_node(_this)));

    t = __i_list_it_node(it)->node.next;
    if (t == __i_list_base(_this))
        return __i_list_end(_this);
    return __i_list_make_iterator(__I_LIST_NODE2VALUE(t));
}

/* checked */
static inline
list_iterator_t i_list_end(const i_list_t* _this)
{
    JDSC_ASSERT(!is_null(_this));
#if JDSC_ITERATOR_ERR_NULL
    if (is_null(_this))
        return i_list_null_iterator();
#endif /* JDSC_ITERATOR_ERR_NULL */
    return __i_list_end(_this);
}

/* checked */
static inline
list_iterator_t i_list_begin(const i_list_t* _this)
{
    JDSC_ASSERT(!is_null(_this));
#if JDSC_ITERATOR_ERR_NULL
    if (is_null(_this))
        return i_list_null_iterator();
#endif /* JDSC_ITERATOR_ERR_NULL */
    return __i_list_begin(_this);
}

/* checked */
static inline
list_iterator_t i_list_next(const i_list_t* _this, const list_iterator_t it)
{
    JDSC_ASSERT(!is_null(_this) && !i_list_is_null_iterator(it));
#if JDSC_ITERATOR_ERR_NULL
    if (is_null(_this) || i_list_is_null_iterator(it))
        return i_list_null_iterator();
#endif /* JDSC_ITERATOR_ERR_NULL */
    return __i_list_next(_this, it);
}

/* checked */
static inline
list_iterator_t i_list_prev(const i_list_t* _this, const list_iterator_t it)
{
    JDSC_ASSERT(!is_null(_this) && !i_list_is_null_iterator(it));
#if JDSC_ITERATOR_ERR_NULL
    if (is_null(_this) || i_list_is_null_iterator(it))
        return i_list_null_iterator();
#endif /* JDSC_ITERATOR_ERR_NULL */
    return __i_list_prev(_this, it);
}

/* checked */
static inline
list_r_iterator_t i_list_rend(const i_list_t* _this)
{
    list_r_iterator_t rit;

    JDSC_ASSERT(!is_null(_this));
#if JDSC_ITERATOR_ERR_NULL
    if (is_null(_this)) {
        return i_list_null_r_iterator();
    }
#endif /* JDSC_ITERATOR_ERR_NULL */

    rit.cur = (uint8_t*)&_this->head;
    return rit;
}

/* checked */
static inline
list_r_iterator_t i_list_rbegin(const i_list_t* _this)
{
    list_r_iterator_t rit;
    list_iterator_t   it;

    JDSC_ASSERT(!is_null(_this));
#if JDSC_ITERATOR_ERR_NULL
    if (is_null(_this)) {
        return i_list_null_r_iterator();
    }
#endif /* JDSC_ITERATOR_ERR_NULL */

    it = __i_list_rbegin(_this);
    rit.cur = it.cur;
    return rit;
}

/* checked */
static inline
list_r_iterator_t i_list_rnext(const i_list_t* _this, const list_r_iterator_t rit)
{
    list_r_iterator_t r;
    list_iterator_t   it;

    JDSC_ASSERT(!is_null(_this) && !i_list_is_null_r_iterator(rit));
#if JDSC_ITERATOR_ERR_NULL
    if (is_null(_this) || i_list_is_null_r_iterator(rit)) {
        return i_list_null_r_iterator();
    }
#endif /* JDSC_ITERATOR_ERR_NULL */

    it.cur = rit.cur;
    r.cur  = __i_list_rnext(_this, it).cur;
    return r;
}

/* checked */
static inline
list_r_iterator_t i_list_rprev(const i_list_t* _this, const list_r_iterator_t rit)
{
    list_r_iterator_t r;
    list_iterator_t   it;

    JDSC_ASSERT(!is_null(_this) && !i_list_is_null_r_iterator(rit));
#if JDSC_ITERATOR_ERR_NULL
    if (is_null(_this) || i_list_is_null_r_iterator(rit)) {
        return i_list_null_r_iterator();
    }
#endif /* JDSC_ITERATOR_ERR_NULL */

    it.cur = rit.cur;
    r.cur  = __i_list_rprev(_this, it).cur;
    return r;
}

/* ---------------------------------------------------------------------------
 * 值访问
 * ------------------------------------------------------------------------- */
/* checked —— 判据只有一个：step 是不是常规字宽 {1,2,4,8}。
 * 用户给了 copy_data 就永远用用户的（它定义的是"槽里那个值怎么来的、归谁所有"，
 * 不等于把这个字直接塞进去）；没给时只有常规字宽能直接写，其余明确失败。 */
static inline
bool __i_list_value_write(const i_list_t* _this, uint8_t* slot, list_data_t data)
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

/* checked —— 同一个判据：{1,2,4,8} 按宽度把那个字取出来；其余（3/5/6/7 以及 > 8，
 * 例如 16/24/32 的 ds_sso_t）一律把槽位地址交出去，配合 copy_data 使用。 */
static inline
list_data_t __i_list_value_read(const i_list_t* _this, const uint8_t* slot)
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

/* checked —— 取样语义与 it_data(it) == *(it).d 保持一致 */
static inline
list_data_t __i_list_it_data(const i_list_t* _this, const list_iterator_t it)
{
    return __i_list_value_read(_this, it.cur);
}

/* checked */
static inline
list_data_t __i_list_first(const i_list_t* _this)
{
    list_node_t* n = __i_list_first_node(_this);

    return is_null(n) ? 0 : __i_list_value_read(_this, n->value);
}

/* checked */
static inline
list_data_t __i_list_last(const i_list_t* _this)
{
    list_node_t* n = __i_list_last_node(_this);

    return is_null(n) ? 0 : __i_list_value_read(_this, n->value);
}

/* checked */
static inline
list_value_t __i_list_front(const i_list_t* _this)
{
    list_node_t* n = __i_list_first_node(_this);

    return (list_value_t){ .u8 = is_null(n) ? NULL : n->value, };
}

/* checked */
static inline
list_value_t __i_list_back(const i_list_t* _this)
{
    list_node_t* n = __i_list_last_node(_this);

    return (list_value_t){ .u8 = is_null(n) ? NULL : n->value, };
}

/* checked —— list 没有下标，这里是 O(n) 的爬链 */
static inline
list_iterator_t __i_list_it(const i_list_t* _this, list_size_t n)
{
    list_iterator_t it = __i_list_begin(_this);

    while (n-- > 0 && !__i_list_is_end(_this, it))
        it = __i_list_next(_this, it);
    return it;
}

/* checked */
static inline
list_value_t __i_list_at(const i_list_t* _this, list_size_t n)
{
    return (list_value_t){ .u8 = __i_list_it(_this, n).cur, };
}

/* checked */
static inline
list_value_t i_list_front(const i_list_t* _this)
{
    return (!is_null(_this) && !__i_list_empty(_this)) ? __i_list_front(_this) : (list_value_t){ .u8 = NULL, };
}

/* checked */
static inline
list_value_t i_list_back(const i_list_t* _this)
{
    return (!is_null(_this) && !__i_list_empty(_this)) ? __i_list_back(_this) : (list_value_t){ .u8 = NULL, };
}

/* checked */
static inline
list_iterator_t i_list_it(const i_list_t* _this, list_size_t n)
{
    if (!is_null(_this) && n >= 0 && n < __i_list_size(_this))
        return __i_list_it(_this, n);
    return i_list_null_iterator();
}

/* checked */
static inline
list_value_t i_list_at(const i_list_t* _this, list_size_t n)
{
    if (!is_null(_this) && n >= 0 && n < __i_list_size(_this))
        return __i_list_at(_this, n);
    return (list_value_t){ .u8 = NULL, };
}

/* ---------------------------------------------------------------------------
 * 比较 —— __eq 的 left 是调用方要找的值、right 是节点里存的值（和旧实现一致）；
 *        __lt 两侧都经过 __i_list_value_read，也就是「节点里存的值」本身
 *        （step > 8 时是槽位地址，例如 ds_sso_t*）。merge / unique 走它，
 *        sort 只在调用方没给 cmp 时才回落过来。
 * ------------------------------------------------------------------------- */
/* checked */
static inline
bool __i_list_value_lt(const i_list_t* _this, const uint8_t* l, const uint8_t* r)
{
    list_data_t lv = __i_list_value_read(_this, l);
    list_data_t rv = __i_list_value_read(_this, r);

    if (!is_null(_this->ops) && !is_null(_this->ops->__lt))
        return _this->ops->__lt(lv, rv);
    return lv < rv;
}

/* sort 的魔术数字 1 / 2：不看 ops，直接对 list_data_t 做数值比较。
   注意在 step > 8 的链上（例如 SSO），读出来的是槽位地址，所以这就是「按指针数值排」——
   那正是「对普通数字排序」的字面含义。 */
/* checked */
static inline
bool __i_list_lt_num_asc(list_data_t left, list_data_t right)
{
    return left < right;
}

/* checked */
static inline
bool __i_list_lt_num_desc(list_data_t left, list_data_t right)
{
    return left > right;
}

/* merge_run 的统一入口：cmp 非空就用它，否则回落到 __i_list_value_lt（ops->__lt，再没有就数值比）。
   sort 把归一化后的 cmp 一路传下来；merge / unique 传 NULL，行为和以前完全一样。 */
/* checked */
static inline
bool __i_list_value_lt_cmp(const i_list_t* _this, list_cmp_t cmp, const uint8_t* l, const uint8_t* r)
{
    if (!is_null(cmp))
        return cmp(__i_list_value_read(_this, l), __i_list_value_read(_this, r));
    return __i_list_value_lt(_this, l, r);
}

/* ---------------------------------------------------------------------------
 * 节点分配 / 归还 / 挂接 —— 对应 STL 的 _M_create_node / _M_erase / _M_hook / _M_unhook
 * ------------------------------------------------------------------------- */
list_node_t* __i_list_alloc_node(i_list_t* _this, list_data_t data);
void         __i_list_free_node(i_list_t* _this, list_node_t* node);

/* STL: _List_node_base::_M_hook(__position) —— 把 n 挂到 pos 之前 */
static inline
void __i_list_hook(list_node_t* n, struct list_head* pos)
{
    n->node.next = pos;
    n->node.prev = pos->prev;
    pos->prev->next = &n->node;
    pos->prev = &n->node;
}

/* STL: _List_node_base::_M_unhook() */
static inline
void __i_list_unhook(list_node_t* n)
{
    struct list_head* next = n->node.next;
    struct list_head* prev = n->node.prev;

    prev->next = next;
    next->prev = prev;
}

/* ---------------------------------------------------------------------------
 * STL: _List_node_base::_M_transfer(__first, __last) —— 把 [first, last) 整段搬到 pos 之前
 *
 * 纯改指针，不碰 size，也允许跨容器（first/last 属于源链表、pos 属于目标链表）。
 * 所有 splice / merge / sort 都建立在它之上。
 * ------------------------------------------------------------------------- */
static inline
void __i_list_transfer(struct list_head* pos, struct list_head* first, struct list_head* last)
{
    struct list_head* tmp;

    if (pos == last)
        return ;

    /* Remove [first, last) from its old position. */
    last->prev->next  = pos;
    first->prev->next = last;
    pos->prev->next   = first;

    /* Splice [first, last) into its new position. */
    tmp        = pos->prev;
    pos->prev  = last->prev;
    last->prev = first->prev;
    first->prev = tmp;
}

/* ---------------------------------------------------------------------------
 * STL: _List_node_base::_M_reverse()
 * ------------------------------------------------------------------------- */
static inline
void __i_list_reverse(struct list_head* head)
{
    struct list_head* p = head;

    do {
        struct list_head* t = p->next;

        p->next = p->prev;
        p->prev = t;
        p = p->prev;
    } while (p != head);
}

/* ---------------------------------------------------------------------------
 * 增删
 * ------------------------------------------------------------------------- */
/* checked */
static inline
list_iterator_t __i_list_push_back(i_list_t* _this, list_data_t data)
{
    list_node_t* t = __i_list_alloc_node(_this, data);

    if (is_null(t))
        return i_list_null_iterator();

    __i_list_hook(t, __i_list_base(_this)); /* 挂到哨兵之前 == 尾部 */
    _this->size++;
    return __i_list_make_iterator(t->value);
}

/* checked */
static inline
list_iterator_t __i_list_push_front(i_list_t* _this, list_data_t data)
{
    list_node_t* t = __i_list_alloc_node(_this, data);

    if (is_null(t))
        return i_list_null_iterator();

    __i_list_hook(t, __i_list_base(_this)->next); /* 挂到首元素之前 == 头部 */
    _this->size++;
    return __i_list_make_iterator(t->value);
}

/* checked */
static inline
list_iterator_t __i_list_insert(i_list_t* _this, const list_iterator_t pos, list_data_t data)
{
    list_node_t* t;

    if (!is_null(_this->ops) && !is_null(_this->ops->valid_data) && !_this->ops->valid_data(data))
        return i_list_null_iterator();

    t = __i_list_alloc_node(_this, data);
    if (is_null(t))
        return i_list_null_iterator();

    __i_list_hook(t, __i_list_it_head(_this, pos));
    _this->size++;
    return __i_list_make_iterator(t->value);
}

/* checked */
static inline
list_iterator_t __i_list_erase(i_list_t* _this, const list_iterator_t pos)
{
    list_iterator_t next;
    list_node_t*    t;

    if (__i_list_empty(_this) || __i_list_is_end(_this, pos))
        return i_list_null_iterator();

    next = __i_list_next(_this, pos);
    t    = __i_list_it_node(pos);

    __i_list_unhook(t);
    _this->size--;
    __i_list_free_node(_this, t);
    return next;
}

/* checked —— 返回指向 [begin, end) 之后那个元素的迭代器 */
static inline
list_iterator_t __i_list_erase_range(i_list_t* _this, list_iterator_t begin, list_iterator_t end)
{
    list_iterator_t pos = begin;

    while (!i_list_is_null_iterator(pos) && !__i_list_is_end(_this, pos) && pos.cur != end.cur)
        pos = __i_list_erase(_this, pos);

    return pos;
}

/* checked */
static inline
void __i_list_pop_back(i_list_t* _this)
{
    list_node_t* t;

    if (__i_list_empty(_this))
        return ;

    t = __i_list_last_node(_this);
    __i_list_unhook(t);
    _this->size--;
    __i_list_free_node(_this, t);
}

/* checked */
static inline
void __i_list_pop_front(i_list_t* _this)
{
    list_node_t* t;

    if (__i_list_empty(_this))
        return ;

    t = __i_list_first_node(_this);
    __i_list_unhook(t);
    _this->size--;
    __i_list_free_node(_this, t);
}

/* checked */
static inline
list_iterator_t i_list_push_back(i_list_t* _this, list_data_t data)
{
    if (is_null(_this))
        return i_list_null_iterator();
    return __i_list_push_back(_this, data);
}

/* checked */
static inline
list_iterator_t i_list_push_front(i_list_t* _this, list_data_t data)
{
    if (is_null(_this))
        return i_list_null_iterator();
    return __i_list_push_front(_this, data);
}

/* checked */
static inline
list_iterator_t i_list_insert(i_list_t* _this, list_iterator_t pos, list_data_t data)
{
    if (is_null(_this) || i_list_is_null_iterator(pos))
        return i_list_null_iterator();
    return __i_list_insert(_this, pos, data);
}

/* checked */
static inline
list_iterator_t i_list_erase(i_list_t* _this, list_iterator_t pos)
{
    if (is_null(_this) || i_list_is_null_iterator(pos))
        return i_list_null_iterator();
    return __i_list_erase(_this, pos);
}

/* checked */
static inline
list_iterator_t i_list_erase_range(i_list_t* _this, list_iterator_t begin, list_iterator_t end)
{
    if (is_null(_this) || i_list_is_null_iterator(begin) || i_list_is_null_iterator(end))
        return i_list_null_iterator();
    return __i_list_erase_range(_this, begin, end);
}

/* checked */
static inline
void i_list_pop_back(i_list_t* _this)
{
    if (!is_null(_this))
        __i_list_pop_back(_this);
}

/* checked */
static inline
void i_list_pop_front(i_list_t* _this)
{
    if (!is_null(_this))
        __i_list_pop_front(_this);
}

/* checked */
static inline
list_size_t __i_list_count(const i_list_t* _this, list_data_t data)
{
    list_size_t ret = 0;
    list_iterator_t it = __i_list_begin(_this);

    for ( ; !__i_list_is_end(_this, it); it = __i_list_next(_this, it)) {
        if ((!is_null(_this->ops) && !is_null(_this->ops->__eq))
            ? _this->ops->__eq(data, __i_list_it_data(_this, it))
            : (data == __i_list_it_data(_this, it)))
            ret++;
    }
    return ret;
}

/* checked */
static inline
list_size_t i_list_count(const i_list_t* _this, list_data_t data)
{
    if (is_null(_this))
        return -1;

    if (!is_null(_this->ops) && !is_null(_this->ops->valid_data) && !_this->ops->valid_data(data))
        return -1;

    return __i_list_count(_this, data);
}

/* checked —— 找不到时返回 end() */
static inline
list_iterator_t __i_list_find(const i_list_t* _this, list_data_t data)
{
    list_iterator_t it = __i_list_begin(_this);

    for ( ; !__i_list_is_end(_this, it); it = __i_list_next(_this, it)) {
        if ((!is_null(_this->ops) && !is_null(_this->ops->__eq))
            ? _this->ops->__eq(data, __i_list_it_data(_this, it))
            : (data == __i_list_it_data(_this, it)))
            return it;
    }
    return __i_list_end(_this);
}

/* checked */
static inline
list_iterator_t i_list_find(const i_list_t* _this, list_data_t data)
{
    if (is_null(_this))
        return i_list_null_iterator();

    if (!is_null(_this->ops) && !is_null(_this->ops->valid_data) && !_this->ops->valid_data(data))
        return i_list_null_iterator();

    return __i_list_find(_this, data);
}

/* checked */
static inline
list_size_t __i_list_remove(i_list_t* _this, list_data_t data)
{
    list_size_t ret = 0;
    list_iterator_t it = __i_list_begin(_this);

    while (!__i_list_is_end(_this, it)) {
        if ((!is_null(_this->ops) && !is_null(_this->ops->__eq))
            ? _this->ops->__eq(data, __i_list_it_data(_this, it))
            : (data == __i_list_it_data(_this, it))) {
            it = __i_list_erase(_this, it);
            ret++;
        } else {
            it = __i_list_next(_this, it);
        }
    }
    return ret;
}

/* checked */
static inline
list_size_t i_list_remove(i_list_t* _this, list_data_t data)
{
    if (is_null(_this))
        return -1;

    if (!is_null(_this->ops) && !is_null(_this->ops->valid_data) && !_this->ops->valid_data(data))
        return -1;

    return __i_list_remove(_this, data);
}

/* checked */
static inline
list_size_t __i_list_remove_if(i_list_t* _this, remove_if_condition cond)
{
    list_size_t ret = 0;
    list_iterator_t it = __i_list_begin(_this);

    while (!__i_list_is_end(_this, it)) {
        if (cond(__i_list_it_data(_this, it))) {
            it = __i_list_erase(_this, it);
            ret++;
        } else {
            it = __i_list_next(_this, it);
        }
    }
    return ret;
}

/* checked */
static inline
list_size_t i_list_remove_if(i_list_t* _this, remove_if_condition cond)
{
    if (is_null(_this) || is_null(cond))
        return -1;
    return __i_list_remove_if(_this, cond);
}

/* ---------------------------------------------------------------------------
 * STL: list::unique() —— 去掉连续重复（用 __lt 两侧取反判等，避免依赖 __eq 的方向性）
 * ------------------------------------------------------------------------- */
/* checked */
static inline
list_size_t __i_list_unique(i_list_t* _this)
{
    list_size_t ret = 0;
    list_iterator_t first;
    list_iterator_t next;

    if (__i_list_size(_this) < 2)
        return 0;

    first = __i_list_begin(_this);
    next  = __i_list_next(_this, first);

    while (!__i_list_is_end(_this, next)) {
        if (!__i_list_value_lt(_this, first.cur, next.cur)
            && !__i_list_value_lt(_this, next.cur, first.cur)) {
            next = __i_list_erase(_this, next);
            ret++;
        } else {
            first = next;
            next  = __i_list_next(_this, next);
        }
    }
    return ret;
}

/* checked */
static inline
list_size_t i_list_unique(i_list_t* _this)
{
    if (is_null(_this))
        return -1;
    return __i_list_unique(_this);
}

/* ---------------------------------------------------------------------------
 * STL: list::reverse()
 * ------------------------------------------------------------------------- */
/* checked */
static inline
void __i_list_reverse_self(i_list_t* _this)
{
    if (__i_list_size(_this) < 2)
        return ;
    __i_list_reverse(__i_list_base(_this));
}

/* checked */
static inline
void i_list_reverse(i_list_t* _this)
{
    if (!is_null(_this))
        __i_list_reverse_self(_this);
}

/* ---------------------------------------------------------------------------
 * STL: splice / merge / sort / swap / resize / assign —— 见 list/list.c
 * ------------------------------------------------------------------------- */
/* 让 shell 变成「和 proto 同配置的空链表」，给 sort 的 tmp[64] 用 */
static inline
void __i_list_init_shell(i_list_t* shell, const i_list_t* proto)
{
    shell->ops  = proto->ops;
    shell->step = proto->step;
    shell->sso  = proto->sso;
    shell->size = 0;
    INIT_LIST_HEAD(&shell->head);
}

void __i_list_swap_head(i_list_t* _this, i_list_t* other);
void __i_list_merge_run(i_list_t* _this, i_list_t* other, list_cmp_t cmp);
void __i_list_sort(i_list_t* _this, list_cmp_t cmp);
void __i_list_insert_n(i_list_t* _this, struct list_head* pos, list_size_t n, list_data_t data, list_iterator_t* out);
list_size_t __i_list_resize(i_list_t* _this, list_size_t n, list_data_t default_data);
list_size_t __i_list_assign(i_list_t* _this, list_size_t n, list_data_t data);
list_size_t __i_list_clear(i_list_t* _this);

/* checked —— 接续 list[i] 的 O(n) 计数 */
static inline
list_size_t __i_list_range_count(const i_list_t* _this, const list_iterator_t first, const list_iterator_t last)
{
    list_size_t n = 0;
    list_iterator_t it = first;

    while (!__i_list_is_end(_this, it) && it.cur != last.cur) {
        it = __i_list_next(_this, it);
        n++;
    }
    return n;
}

/* checked —— STL: splice(pos, other) 整条链搬到 pos 之前 */
static inline
void i_list_splice(i_list_t* _this, list_iterator_t pos, i_list_t* other)
{
    if (is_null(_this) || is_null(other) || i_list_is_null_iterator(pos))
        return ;

    if (_this == other || __i_list_empty(other))
        return ;

    __i_list_transfer(__i_list_it_head(_this, pos), __i_list_base(other)->next, __i_list_base(other));

    _this->size += other->size;
    other->size = 0;
}

/* checked —— STL: splice(pos, other, first, last)，[first, last) 必须来自 other */
static inline
void i_list_splice_range(i_list_t* _this, list_iterator_t pos, i_list_t* other,
                         list_iterator_t first, list_iterator_t last)
{
    list_size_t     n = 0;
    list_iterator_t it;

    if (is_null(_this) || is_null(other) || i_list_is_null_iterator(pos)
        || i_list_is_null_iterator(first) || i_list_is_null_iterator(last))
        return ;

    if (__i_list_empty(other) || first.cur == last.cur)
        return ;

    if (_this == other) {
        /* 同链自搬。数长度的同时校验 pos 不在 [first, last) 里 —— STL 对这种
           情形也是 UB，这里直接放弃，免得把链环搞断。 */
        for (it = first; !__i_list_is_end(other, it) && it.cur != last.cur; it = __i_list_next(other, it)) {
            if (it.cur == pos.cur)
                return ;
            n++;
        }
    } else {
        n = __i_list_range_count(other, first, last);
    }

    __i_list_transfer(__i_list_it_head(_this, pos),
                      __i_list_it_head(other, first), __i_list_it_head(other, last));

    _this->size += n;
    other->size -= n;
}

/* checked —— STL: list::merge(other)，要求两条链都已按同一规则升序 */
static inline
void i_list_merge(i_list_t* _this, i_list_t* other)
{
    list_size_t n;

    if (is_null(_this) || is_null(other) || _this == other || __i_list_empty(other))
        return ;

    n = other->size;
    __i_list_merge_run(_this, other, NULL); /* 走容器自己的 ops->__lt */
    _this->size += n;
    other->size = 0;
}

/* checked —— cmp 的约定：
 *   NULL  或者 (list_cmp_t)1  -> 对普通数字升序（不看 ops）
 *   (list_cmp_t)2             -> 对普通数字降序（不看 ops）
 *   其它                      -> 直接当比较器调用
 * 所以这里先把前三种归一化成具体函数指针再往下传；便捷宏见 list/list.h 的
 * LIST_SORT_ASC / LIST_SORT_DESC。 */
static inline
void i_list_sort(i_list_t* _this, list_cmp_t cmp)
{
    if (is_null(_this))
        return ;

    if (is_null(cmp) || (uintptr_t)cmp == 1)
        cmp = __i_list_lt_num_asc;
    else if ((uintptr_t)cmp == 2)
        cmp = __i_list_lt_num_desc;

    __i_list_sort(_this, cmp);
}

/* checked */
static inline
void i_list_swap(i_list_t* _this, i_list_t* other)
{
    if (is_null(_this) || is_null(other) || _this == other)
        return ;

    if (_this->step != other->step || _this->ops != other->ops)
        return ;

    __i_list_swap_head(_this, other);
}

/* checked */
static inline
list_iterator_t i_list_insert_n(i_list_t* _this, list_iterator_t pos, list_size_t n, list_data_t data)
{
    list_iterator_t out = i_list_null_iterator();

    if (is_null(_this) || i_list_is_null_iterator(pos) || n < 0)
        return i_list_null_iterator();

    if (0 == n)
        return pos;

    if (!is_null(_this->ops) && !is_null(_this->ops->valid_data) && !_this->ops->valid_data(data))
        return i_list_null_iterator();

    __i_list_insert_n(_this, __i_list_it_head(_this, pos), n, data, &out);
    return out;
}

/* checked */
static inline
bool i_list_resize(i_list_t* _this, list_size_t n, list_data_t default_data)
{
    if (is_null(_this) || n < 0)
        return false;
    return __i_list_resize(_this, n, default_data) >= 0;
}

/* checked */
static inline
list_size_t i_list_assign(i_list_t* _this, list_size_t n, list_data_t data)
{
    if (is_null(_this) || n < 0)
        return -1;
    return __i_list_assign(_this, n, data);
}

/* checked */
static inline
list_size_t i_list_clear(i_list_t* _this)
{
    if (is_null(_this))
        return -1;
    return __i_list_clear(_this);
}

#endif /* __J_I_LIST_H */
