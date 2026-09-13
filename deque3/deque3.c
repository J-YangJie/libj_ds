/*
  Deque3 Implementations

  参照 /usr/include/c++/11/bits/stl_deque.h + stl_deque.tcc 翻译成 C：
    _M_initialize_map / _M_reallocate_map   ->  __deque3_init / __i_deque3_reserve
    _M_allocate_node / _M_deallocate_node   ->  i_deque3_alloc_bkt / p_free
    _M_push_back_aux / _M_push_front_aux    ->  __i_deque3_push_back_open / __i_deque3_push_front_open
    _M_pop_back_aux / _M_pop_front_aux      ->  __i_deque3_pop_back_free / __i_deque3_pop_front_free
    _M_insert_aux                           ->  __i_deque3_insert_fill
    _M_erase                                ->  __i_deque3_erase / __i_deque3_erase_range
  C++ 里 sizeof(_Tp) 是编译期常量，这里全部换成运行期的 step；
  元素的搬移就是 step 字节的 memmove（_Tp 为 trivially copyable 时的语义）。

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

#include <deque3/deque3.h>

#include <string.h>
#include <_memory.h>
#include <_compiler.h>

/* checked */
static
bool __i_deque3_reserve(deque3_t* _this)   /* stl_deque.h:_M_reallocate_map */
{
    deque3_bcount_t bcnt_o = _this->bkt_count;
    deque3_bcount_t bcnt_n = bcnt_o > 0 ? bcnt_o * 2 : _I_DEQUE3_INITIAL_MAP_SIZE;
    deque3_bcount_t off_b  = _this->begin.bkt - _this->bkt;
    deque3_bcount_t off_e  = _this->end.bkt   - _this->bkt;
    deque3_bcount_t n      = off_e - off_b + 1;
    deque3_bcount_t off_n  = (bcnt_n - n) / 2;
    uint8_t** bkt_n;

    bkt_n = (uint8_t**)p_calloc(bcnt_n, sizeof(uint8_t*));
    if (is_null(bkt_n))
        return false;

    memcpy(bkt_n + off_n, _this->begin.bkt, n * sizeof(uint8_t*));
    p_free(_this->bkt);

    _this->bkt       = bkt_n;
    _this->bkt_count = bcnt_n;
    _this->begin.bkt = _this->bkt + off_n;
    _this->end.bkt   = _this->bkt + off_n + n - 1;
    return true;
}

/* checked —— stl_deque.h:_M_allocate_node，一个 node 装 buf_size 个 step 字节的元素 */
static JDSC_INLINE_FORCE
uint8_t* i_deque3_alloc_bkt(const deque3_t* _this)
{
    return (uint8_t*)p_malloc(_this->buf_size * _this->step);
}

/* checked */
uint8_t* __i_deque3_push_back_open(deque3_t* _this)
{
    uint8_t** bkt;
    uint8_t*  slot;

    if (_this->end.bkt == _this->bkt + _this->bkt_count - 1) {
        if (!__i_deque3_reserve(_this))
            return NULL;
    }

    bkt = _this->end.bkt + 1;
    *bkt = i_deque3_alloc_bkt(_this);
    if (is_null(*bkt))
        return NULL;

    slot = _this->end.cur;                 /* 新元素落在旧 node 的末槽 */
    __i_deque3_iter_set_node_n(&_this->end, _this->step, _this->buf_size, bkt);
    _this->end.cur = _this->end.begin;
    return slot;
}

/* checked */
uint8_t* __i_deque3_push_front_open(deque3_t* _this)
{
    uint8_t** bkt;

    if (_this->begin.bkt == _this->bkt) {
        if (!__i_deque3_reserve(_this))
            return NULL;
    }

    bkt = _this->begin.bkt - 1;
    *bkt = i_deque3_alloc_bkt(_this);
    if (is_null(*bkt))
        return NULL;

    __i_deque3_iter_set_node_n(&_this->begin, _this->step, _this->buf_size, bkt);
    _this->begin.cur = I_DEQUE3_PTR_SUB(_this->begin.end, _this->step, 1);  /* 新元素落在新 node 末槽 */
    return _this->begin.cur;
}

/* checked —— 只回收空 node；元素数据由 __i_deque3_pop_*_impl 在放 node 之前销毁 */
void __i_deque3_pop_back_free(deque3_t* _this)
{
    p_free(_this->end.begin);
    __i_deque3_iter_set_node_n(&_this->end, _this->step, _this->buf_size, _this->end.bkt - 1);
    _this->end.cur = I_DEQUE3_PTR_SUB(_this->end.end, _this->step, 1);
}

/* checked */
void __i_deque3_pop_front_free(deque3_t* _this)
{
    p_free(_this->begin.begin);
    __i_deque3_iter_set_node_n(&_this->begin, _this->step, _this->buf_size, _this->begin.bkt + 1);
    _this->begin.cur = _this->begin.begin;
}

/* checked —— 区间平移：对应 STL 的 std::copy / std::copy_backward（POD 时为 memmove） */
static JDSC_INLINE
void __i_deque3_move_forward(deque3_t* _this, deque3_iterator_t dest, deque3_iterator_t src, deque3_size_t n)
{
    for (deque3_size_t i = 0; i < n; ++i) {
        __i_deque3_slot_move(_this, dest.cur, src.cur);
        dest = __i_deque3_iter_next(dest);
        src  = __i_deque3_iter_next(src);
    }
}

/* checked */
static JDSC_INLINE
void __i_deque3_move_backward(deque3_t* _this, deque3_iterator_t dest, deque3_iterator_t src, deque3_size_t n)
{
    if (n <= 0)
        return ;

    dest = __i_deque3_iter_add(dest, n - 1);
    src  = __i_deque3_iter_add(src,  n - 1);
    for (deque3_size_t i = 0; i < n; ++i) {
        __i_deque3_slot_move(_this, dest.cur, src.cur);
        dest = __i_deque3_iter_prev(dest);
        src  = __i_deque3_iter_prev(src);
    }
}

/* checked */
static inline
deque3_iterator_t __i_deque3_make_iterator(const deque3_t* _this, uint8_t** bkt, uint8_t* cur)
{
    deque3_iterator_t it;

    __i_deque3_iter_set_node_n(&it, _this->step, _this->buf_size, bkt);
    it.cur = cur;
    return it;
}

static inline
uint8_t* __i_deque3_bkt_first(const deque3_t* _this, uint8_t** b)
{
    return (b == _this->begin.bkt) ? _this->begin.cur : *b;
}

static inline
uint8_t* __i_deque3_bkt_last(const deque3_t* _this, uint8_t** b)
{
    return (b == _this->end.bkt) ? _this->end.cur
                                 : I_DEQUE3_PTR_ADD(*b, _this->step, (ptrdiff_t)_this->buf_size);
}

/* ============================================================================
 * insert —— stl_deque.h:_M_insert_aux：往离 pos 近的那一端挪，腾出 n 个洞再填值。
 * 先 push n 个全 0 的洞（新 node 里是"空"元素），平移后用 data 把洞填满。
 * copy_data 存在时填洞可能失败，此时撤销平移并把洞丢掉。
 * ========================================================================== */
static
deque3_iterator_t __i_deque3_insert_fill(deque3_t* _this, deque3_iterator_t pos, deque3_size_t n, deque_data_t data)
{
    deque3_size_t i, fn, bn;
    deque3_iterator_t ret, slot;

    if (is_null(_this) || i_deque3_is_null_iterator(pos) || n < 0)
        return i_deque3_null_iterator();

    if (0 == n)
        return pos;

    fn = __i_deque3_iter_distance(_this->begin, pos);
    bn = __i_deque3_iter_distance(pos, _this->end);
    if (fn < 0 || bn < 0)
        return i_deque3_null_iterator();

    if (fn < bn) {                                     /* 离头近：头部开洞，前缀右移 */
        for (i = 0; i < n; ++i) {
            if (!__i_deque3_push_front_zero(_this))
                goto err_front;
        }

        /* push 可能触发 __i_deque3_reserve 重建 map：按偏移重算，勿用旧迭代器 */
        __i_deque3_move_forward(_this, _this->begin, __i_deque3_iter_add(_this->begin, n), fn);
        ret = __i_deque3_iter_add(_this->begin, fn);
    } else {                                           /* 离尾近：尾部开洞，后缀左移 */
        for (i = 0; i < n; ++i) {
            if (!__i_deque3_push_back_zero(_this))
                goto err_back;
        }

        ret = __i_deque3_iter_add(_this->begin, fn);
        __i_deque3_move_backward(_this, __i_deque3_iter_add(ret, n), ret, bn);
    }

    for (i = 0, slot = ret; i < n; ++i, slot = __i_deque3_iter_next(slot)) {
        if (!__i_deque3_slot_write(_this, data, slot.cur))
            goto err_fill;
    }
    return ret;

err_fill:                                              /* 已填好的 i 个洞各自持有一份拷贝，先销毁 */
    for (slot = ret; i > 0; --i, slot = __i_deque3_iter_next(slot))
        __i_deque3_slot_free(_this, slot.cur);

    if (fn < bn) {                                     /* 撤销平移，再把洞从头部丢掉 */
        __i_deque3_move_backward(_this, __i_deque3_iter_add(_this->begin, n), _this->begin, fn);
        for (i = 0; i < n; ++i)
            __i_deque3_pop_front_raw(_this);
    } else {
        __i_deque3_move_forward(_this, ret, __i_deque3_iter_add(ret, n), bn);
        for (i = 0; i < n; ++i)
            __i_deque3_pop_back_raw(_this);
    }
    return i_deque3_null_iterator();

err_front:                                             /* 洞没开完：把已开的全 0 洞丢掉即可 */
    while (i-- > 0)
        __i_deque3_pop_front_raw(_this);
    return i_deque3_null_iterator();

err_back:
    while (i-- > 0)
        __i_deque3_pop_back_raw(_this);
    return i_deque3_null_iterator();
}

/* checked */
deque3_iterator_t __i_deque3_insert(deque3_t* _this, deque3_iterator_t pos, deque_data_t data)
{
    return __i_deque3_insert_fill(_this, pos, 1, data);
}

/* checked */
deque3_iterator_t i_deque3_insert_n(deque3_t* _this, deque3_iterator_t pos, deque3_size_t n, deque_data_t data)
{
    if (is_null(_this) || i_deque3_is_null_iterator(pos) || n < 0)
        return i_deque3_null_iterator();

    if (0 == n)
        return pos;

    if (!__i_deque3_valid(_this, data))
        return i_deque3_null_iterator();

    return __i_deque3_insert_fill(_this, pos, n, data);
}

/* ============================================================================
 * erase —— stl_deque.h:_M_erase：同样是往短的一侧挪。
 * 平移只搬 step 字节，被删元素先就地销毁；搬完之后那个"空出来"的槽是
 * 别的活元素的影子，弹它时必须走 *_raw（不碰元素数据），否则会重复释放。
 * ========================================================================== */
/* checked */
deque3_iterator_t __i_deque3_erase(deque3_t* _this, deque3_iterator_t pos)
{
    deque3_size_t     fn, bn;
    deque3_iterator_t pnext;

    fn = __i_deque3_iter_distance(_this->begin, pos);
    bn = __i_deque3_iter_distance(pos, _this->end);
    if (fn < 0 || bn < 0)
        return i_deque3_null_iterator();

    pnext = __i_deque3_iter_next(pos);

    __i_deque3_slot_free(_this, pos.cur);              /* 先销毁，后面平移不会再读它 */

    if (fn < bn - 1) {                                 /* 离头近：前缀右移补位，再弹头 */
        __i_deque3_move_backward(_this, __i_deque3_iter_next(_this->begin), _this->begin, fn);
        __i_deque3_pop_front_raw(_this);
        return pnext;
    }

    __i_deque3_move_forward(_this, pos, pnext, bn - 1); /* 离尾近：后缀左移补位，再弹尾 */
    __i_deque3_pop_back_raw(_this);
    return pos;
}

/* checked */
deque3_iterator_t __i_deque3_erase_range(deque3_t* _this, deque3_iterator_t iterator_begin, deque3_iterator_t iterator_end)
{
    deque3_size_t     fn, bn, en, i;
    deque3_iterator_t it;

    fn = __i_deque3_iter_distance(_this->begin, iterator_begin);
    bn = __i_deque3_iter_distance(iterator_end, _this->end);
    en = __i_deque3_iter_distance(iterator_begin, iterator_end);

    if (fn < 0 || bn < 0 || en < 0)
        return i_deque3_null_iterator();

    if (0 == en)
        return iterator_begin;

    for (it = iterator_begin; it.cur != iterator_end.cur; it = __i_deque3_iter_next(it))
        __i_deque3_slot_free(_this, it.cur);

    if (fn < bn) {
        __i_deque3_move_backward(_this, __i_deque3_iter_add(_this->begin, en), _this->begin, fn);
        for (i = 0; i < en; ++i)
            __i_deque3_pop_front_raw(_this);
        return iterator_end;
    }

    __i_deque3_move_forward(_this, iterator_begin, iterator_end, bn);
    for (i = 0; i < en; ++i)
        __i_deque3_pop_back_raw(_this);
    return iterator_begin;
}

/* ============================================================================
 * find / count —— 直接按"桶 + 槽指针"递进。
 * 相等判定：ops->__eq 优先，实现者自己解释信封；没提供就按 step 字节 memcmp。
 * ========================================================================== */
deque3_iterator_t i_deque3_find(const deque3_t* _this, deque_data_t data)
{
    uint8_t** b;
    deque_data_t key;

    if (is_null(_this))
        return i_deque3_null_iterator();

    if (!__i_deque3_valid(_this, data))
        return i_deque3_null_iterator();

    key = __i_deque3_key_to_data(_this, data);

    for (b = _this->begin.bkt; b <= _this->end.bkt; ++b) {
        uint8_t* p = __i_deque3_bkt_first(_this, b);
        uint8_t* q = __i_deque3_bkt_last(_this, b);

        for (; p < q; p = I_DEQUE3_PTR_ADD(p, _this->step, 1)) {
            if (__i_deque3_slot_eq(_this, key, p))
                return __i_deque3_make_iterator(_this, b, p);
        }
    }

    return _this->end;                                 /* 未找到：与 std::find 一致，返回 end */
}

deque3_count_t i_deque3_count(const deque3_t* _this, deque_data_t data)
{
    uint8_t** b;
    deque3_count_t ret = 0;
    deque_data_t key;

    if (is_null(_this))
        return -1;

    if (!__i_deque3_valid(_this, data))
        return -1;

    key = __i_deque3_key_to_data(_this, data);

    for (b = _this->begin.bkt; b <= _this->end.bkt; ++b) {
        uint8_t* p = __i_deque3_bkt_first(_this, b);
        uint8_t* q = __i_deque3_bkt_last(_this, b);

        for (; p < q; p = I_DEQUE3_PTR_ADD(p, _this->step, 1)) {
            if (__i_deque3_slot_eq(_this, key, p))
                ++ret;
        }
    }

    return ret;
}

/* ============================================================================
 * remove / remove_if —— 单遍压实
 * ========================================================================== */
/* 把"应删除"判据包成统一结构，供单遍压实共用 */
typedef struct {
    const deque3_t*     d;
    deque_data_t        data;    /* remove 的比对数据 */
    remove_if_condition cond;    /* remove_if 的条件 */
    int                 mode;    /* 0 = remove(data)，1 = remove_if(cond) */
} __i_deque3_match_t;

static inline
bool __i_deque3_keep(const deque3_t* _this, __i_deque3_match_t* m, uint8_t* slot)   /* true 表示保留 */
{
    if (0 == m->mode)
        return !__i_deque3_slot_eq(_this, m->data, slot);
    return is_null(m->cond) ? true : !m->cond(__i_deque3_slot_to_data(_this, slot));
}

/* 单遍压实：不保留的元素就地销毁 + 槽清 0，保留元素左移覆盖，尾部空出的槽靠 pop 裁剪。O(n)。 */
static
deque3_size_t __i_deque3_compact(deque3_t* _this, __i_deque3_match_t* m)
{
    deque3_iterator_t w, r, e;
    deque3_size_t org, removed = 0;

    w = i_deque3_begin(_this);
    e = i_deque3_end(_this);
    r = w;

    while (r.cur != e.cur) {
        if (__i_deque3_keep(_this, m, r.cur)) {
            if (w.cur != r.cur) {
                __i_deque3_slot_move(_this, w.cur, r.cur);
                __i_deque3_slot_zero(_this, r.cur);    /* 源槽清 0：尾部裁剪时不能再销毁它 */
            }
            w = __i_deque3_iter_next(w);
        } else {
            __i_deque3_slot_free(_this, r.cur);
            __i_deque3_slot_zero(_this, r.cur);
            removed++;
        }
        r = __i_deque3_iter_next(r);
    }

    org = removed;
    while (removed-- > 0)
        i_deque3_pop_back(_this);
    return org;
}

deque3_size_t i_deque3_remove(deque3_t* _this, deque_data_t data)
{
    __i_deque3_match_t m;

    if (is_null(_this))
        return (deque3_size_t)-1;
    if (!__i_deque3_valid(_this, data))
        return (deque3_size_t)-1;

    m.d = _this; m.data = __i_deque3_key_to_data(_this, data); m.cond = NULL; m.mode = 0;
    return __i_deque3_compact(_this, &m);
}

deque3_size_t i_deque3_remove_if(deque3_t* _this, remove_if_condition cond)
{
    __i_deque3_match_t m;

    if (is_null(_this) || is_null(cond))
        return (deque3_size_t)-1;

    m.d = _this; m.data = 0; m.cond = cond; m.mode = 1;
    return __i_deque3_compact(_this, &m);
}

/* checked */
deque3_size_t i_deque3_clear(deque3_t* _this)
{
    deque3_size_t old;

    if (is_null(_this))
        return 0;

    old = i_deque3_size(_this);
    while (!__i_deque3_empty(_this))
        i_deque3_pop_back(_this);
    return old;
}

/* ============================================================================
 * new / delete —— stl_deque.h:_M_initialize_map
 * ========================================================================== */
static
bool __deque3_init(deque3_t* deque)
{
    deque3_bcount_t m = _I_DEQUE3_INITIAL_MAP_SIZE / 2;

    deque->bkt_count_init = _I_DEQUE3_INITIAL_MAP_SIZE;
    deque->bkt_count      = deque->bkt_count_init;
    deque->bkt            = (uint8_t**)p_calloc(deque->bkt_count, sizeof(uint8_t*));
    if (is_null(deque->bkt))
        return false;

    deque->bkt[m] = i_deque3_alloc_bkt(deque);
    if (is_null(deque->bkt[m]))
        goto err;

    /* map 从中间往外长，begin/end 都落在中间那个 node 的首槽上 */
    __i_deque3_iter_set_node_n(&deque->begin, deque->step, deque->buf_size, &deque->bkt[m]);
    deque->begin.cur = deque->begin.begin;
    deque->end       = deque->begin;
    return true;

err:
    p_free(deque->bkt);
    return false;
}

static
void __deque3_deinit(deque3_t* deque)
{
    uint8_t** i = deque->begin.bkt;

    for (; i <= deque->end.bkt; ++i)
        p_free(*i);
    p_free(deque->bkt);
}

deque3_t* __deque3_new(const class_deque_ops_t* ops, deque3_step_t step)
{
    deque3_t* deque;

    if (0 == step)                                     /* step 决定 node 切分，不能为 0 */
        return NULL;

    deque = (deque3_t*)p_calloc(1, sizeof(deque3_t));
    if (is_null(deque))
        return NULL;

    deque->step     = step;
    deque->buf_size = i_deque3_buf_size(step);

    if (!__deque3_init(deque))
        goto err;

    deque->ops = ops;
    return deque;

err:
    p_free(deque);
    return NULL;
}

void __deque3_delete(deque3_t** _this)
{
    if (is_null(_this) || is_null(*_this))
        return ;

    /* 先把存活元素交给 ops->free_data 销毁，再回收 node —— 与 pop 的契约一致 */
    while (!__i_deque3_empty(*_this))
        i_deque3_pop_back(*_this);

    __deque3_deinit(*_this);
    p_free(*_this);
}
