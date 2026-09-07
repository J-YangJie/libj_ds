/*
  Deque Implementations
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

#include <deque/deque.h>

#include <string.h>
#include <_memory.h>
#include <_compiler.h>

#define DEQUE_CAPACITY_INIT 8

/* checked */
static
bool __i_deque_reserve(deque_t* _this)
{
    deque_bcount_t bcnt_o = _this->bkt_count;
    deque_bcount_t bcnt_n = bcnt_o > 0 ? bcnt_o * 2 : DEQUE_CAPACITY_INIT;
    deque_bcount_t off_b  = _this->begin.bkt - _this->bkt;
    deque_bcount_t off_e  = _this->end.bkt   - _this->bkt;
    deque_bcount_t n      = off_e - off_b + 1;
    deque_bcount_t off_n  = (bcnt_n - n) / 2;
    deque_data_t** bkt_n;

    bkt_n = (deque_data_t**)p_calloc(bcnt_n, sizeof(deque_data_t*));
    if (is_null(bkt_n))
        return false;

    memcpy(bkt_n + off_n, _this->begin.bkt, n * sizeof(deque_data_t*));
    p_free(_this->bkt);

    _this->bkt = bkt_n;
    _this->bkt_count = bcnt_n;
    _this->begin.bkt = _this->bkt + off_n;
    _this->end.bkt   = _this->bkt + off_n + n - 1;
    return true;
}

/* checked */
static JDSC_INLINE_FORCE
deque_data_t* i_deque_alloc_bkt(void)
{
    return (deque_data_t*)p_malloc(_I_DEQUE_BKT_SIZE * sizeof(deque_data_t));
}

/* checked */
static JDSC_INLINE_FORCE_POLICY
deque_iterator_t __i_deque_make_iterator(deque_data_t** bkt, deque_data_t* cur)
{
    deque_iterator_t it;
    __i_deque_set_bkt(&it, bkt);
    it.cur = cur;
    return it;
}

/* checked */
bool __i_deque_push_back_alloc(deque_t* _this, deque_data_t data)
{
    deque_data_t** bkt;

    if (_this->end.bkt == _this->bkt + _this->bkt_count - 1) {
        if (!__i_deque_reserve(_this))
            return false;
    }

    bkt = _this->end.bkt + 1;
    *bkt = i_deque_alloc_bkt();
    if (is_null(*bkt))
        return false;

    *_this->end.cur = data;
    _this->end = __i_deque_make_iterator(bkt, *bkt);
    return true;
}

/* checked */
bool __i_deque_push_front_alloc(deque_t* _this, deque_data_t data)
{
    deque_data_t** bkt;

    if (_this->begin.bkt == _this->bkt) {
        if (!__i_deque_reserve(_this))
            return false;
    }

    bkt = _this->begin.bkt - 1;
    *bkt = i_deque_alloc_bkt();
    if (is_null(*bkt))
        return false;

    _this->begin = __i_deque_make_iterator(bkt, *bkt + _I_DEQUE_BKT_SIZE - 1);
    *_this->begin.cur = data;
    return true;
}

/* checked */
JDSC_INLINE
deque_data_t __i_deque_pop_back_free(deque_t* _this)
{
    p_free(_this->end.begin);
    _this->end = __i_deque_make_iterator(_this->end.bkt - 1, 
                                            (_this->end.bkt - 1)[0] + _I_DEQUE_BKT_SIZE - 1);
    return *_this->end.cur;
}

/* checked */
JDSC_INLINE
deque_data_t __i_deque_pop_front_free(deque_t* _this)
{
    deque_data_t ret = *_this->begin.cur;
    p_free(_this->begin.begin);
    _this->begin = __i_deque_make_iterator(_this->begin.bkt + 1, 
                                            (_this->begin.bkt + 1)[0]);
    return ret;
}

static
bool __deque_init(deque_t* deque)
{
    deque_bcount_t m = DEQUE_CAPACITY_INIT / 2;

    deque->bkt_count_init = DEQUE_CAPACITY_INIT;
    deque->bkt_count = deque->bkt_count_init;
    deque->bkt = (deque_data_t**)p_calloc(deque->bkt_count, sizeof(deque_data_t*));
    if (is_null(deque->bkt))
        return false;

    deque->bkt[m] = i_deque_alloc_bkt();
    if (is_null(deque->bkt[m]))
        goto err;

    deque->begin = __i_deque_make_iterator(&deque->bkt[m], deque->bkt[m]);
    deque->end = deque->begin;
    return true;

err:
    p_free(deque->bkt);
    return false;
}

static
bool __deque_deinit(deque_t* deque)
{
    deque_data_t** i = deque->begin.bkt;
    for (; i <= deque->end.bkt; ++i)
        p_free(*i);
    p_free(deque->bkt);
    return true;
}

deque_t* __deque_new(const class_deque_ops_t* ops)
{
    deque_t* deque = (deque_t*)p_calloc(1, sizeof(deque_t));
    if (is_null(deque))
        return NULL;

    if (!__deque_init(deque))
        goto err;

    deque->ops = ops;
    return deque;

err:
    p_free(deque);
    return NULL;
}

void __deque_delete(deque_t** _this)
{
    if (is_null(_this) || is_null(*_this))
        return ;

    __deque_deinit(*_this);
    p_free(*_this);
}

static inline
bool         i_deque_iter_eq(deque_iterator_t a, deque_iterator_t b)  { return a.cur == b.cur; }

static inline
deque_data_t i_deque_iter_get(deque_iterator_t it)                    { return *it.cur; }

static inline
void         i_deque_iter_set(deque_iterator_t it, deque_data_t x)    { *it.cur = x; }

static inline
bool _deque_eq(const deque_t* _this, deque_data_t left, deque_data_t right)
{
    if (!is_null(_this->ops) && !is_null(_this->ops->__eq))
        return _this->ops->__eq(left, right);
    return left == right;
}

static inline
void _deque_free_data(const deque_t* _this, deque_data_t* slot)
{
    if (!is_null(_this->ops) && !is_null(_this->ops->free_data))
        _this->ops->free_data(slot);
}

/* 跨缓冲区推进迭代器 n 步（n 可正可负），相当于 STL 迭代器 operator+= */
static inline
deque_iterator_t _deque_advance(deque_iterator_t it, long n)
{
    long off = n + (long)(it.cur - it.begin);
    long bs  = (long)_I_DEQUE_BKT_SIZE;

    if (off >= 0 && off < bs) {
        it.cur += n;
        return it;
    }

    {
        long node_off = off > 0 ? off / bs : -((-off - 1) / bs) - 1;
        __i_deque_set_bkt(&it, it.bkt + node_off);
        it.cur = it.begin + (off - node_off * bs);
    }
    return it;
}

/* 把"应删除"判据包成统一结构，供单遍压实共用 */
typedef struct {
    const deque_t*      d;
    deque_data_t        data;    /* remove 的比对数据 */
    remove_if_condition cond;    /* remove_if 的条件 */
    int                 mode;    /* 0 = remove(data)，1 = remove_if(cond) */
} _deque_match_t;

static inline
bool _deque_keep(deque_data_t cur, void* ctxp)   /* return true 表示保留该元素 */
{
    _deque_match_t* m = (_deque_match_t*)ctxp;

    if (0 == m->mode)
        return !_deque_eq(m->d, m->data, cur);
    return is_null(m->cond) ? true : !m->cond(cur);
}

/* 单遍压实删除：不保留的元素 free + 源槽清 0，保留元素就地左移，尾部空出的 0 槽用 pop 裁剪。O(n)。 */
static
deque_size_t _deque_compact(deque_t* _this, _deque_match_t* m)
{
    deque_iterator_t w, r, e;
    deque_size_t org, removed = 0;

    w = i_deque_begin(_this);
    e = i_deque_end(_this);
    r = w;

    while (!i_deque_iter_eq(r, e)) {
        if (_deque_keep(*r.cur, m)) {
            if (w.cur != r.cur) {
                *w.cur = *r.cur;
                *r.cur = 0;
            }
            w = i_deque_next(w);
        } else {
            _deque_free_data(_this, r.cur);
            *r.cur = 0;
            removed++;
        }
        r = i_deque_next(r);
    }

    org = removed;
    while (removed-- > 0)
        i_deque_pop_back(_this);
    return org;
}

/* ---------------- insert：在 pos 前插入，返回新元素迭代器 ---------------- */
deque_iterator_t i_deque_insert(deque_t* _this, deque_iterator_t pos, deque_data_t data)
{
    deque_iterator_t b, e;
    long n, k;

    if (is_null(_this))
        return i_deque_null_iterator();

    /* 数据合法性前置检查（与 push 一致） */
    if (!is_null(_this->ops) && !is_null(_this->ops->valid_data) && !_this->ops->valid_data(data))
        return i_deque_null_iterator();

    b = i_deque_begin(_this);
    e = i_deque_end(_this);
    n = (long)__i_deque_iterator_distance(b, e);

    if (0 == n) {                                 /* 空容器：只允许插在 begin==end */
        if (pos.cur != e.cur)
            return i_deque_null_iterator();
        if (!i_deque_push_back(_this, data))
            return i_deque_null_iterator();
        return i_deque_begin(_this);
    }

    if (pos.cur == e.cur) {                       /* 尾插 == push_back */
        if (!i_deque_push_back(_this, data))
            return i_deque_null_iterator();
        return i_deque_prev(i_deque_end(_this));
    }
    if (pos.cur == b.cur) {                       /* 头插 == push_front */
        if (!i_deque_push_front(_this, data))
            return i_deque_null_iterator();
        return i_deque_begin(_this);
    }

    k = (long)__i_deque_iterator_distance(b, pos);
    if (k <= 0 || k >= n)                         /* 越界迭代器 */
        return i_deque_null_iterator();

    if (k < n / 2) {                              /* 离头近：头端补副本开槽，前缀左移腾出 slot k */
        deque_data_t front_v = *b.cur;

        if (!i_deque_push_front(_this, front_v))
            return i_deque_null_iterator();

        b = i_deque_begin(_this);
        _deque_free_data(_this, b.cur);           /* 丢弃刚补的无用深拷贝（无 ops 则空操作） */

        {
            deque_iterator_t d = b;               /* 目标从 slot 0 开始左移一格 */
            deque_iterator_t s = i_deque_next(d);
            for (long j = 0; j < k; ++j) {        /* slot[j] = slot[j+1]，源清零 */
                *d.cur = *s.cur;
                *s.cur = 0;
                d = s;
                s = i_deque_next(s);
            }
            deque_data_t tdata = data;            /* d == slot k（已置 0），写入 data 副本 */
            if (!is_null(_this->ops) && !is_null(_this->ops->copy_data) && !_this->ops->copy_data(data, &tdata))
                return i_deque_null_iterator();
            *d.cur = tdata;
            return d;
        }
    } else {                                      /* 离尾近：尾端补副本开槽，尾段右移腾出 slot k */
        deque_data_t back_v = *i_deque_prev(e).cur;

        if (!i_deque_push_back(_this, back_v))
            return i_deque_null_iterator();

        e = i_deque_end(_this);
        {
            deque_iterator_t d = i_deque_prev(e); /* 尾补副本所在槽（新最后） */
            _deque_free_data(_this, d.cur);       /* 丢弃该无用深拷贝 */

            for (long j = n; j > k; --j) {        /* slot[j] = slot[j-1]，源清零 */
                deque_iterator_t s = i_deque_prev(d);
                *d.cur = *s.cur;
                *s.cur = 0;
                d = s;
            }
            deque_data_t tdata = data;            /* d == slot k，写入 data 副本 */
            if (!is_null(_this->ops) && !is_null(_this->ops->copy_data) && !_this->ops->copy_data(data, &tdata))
                return i_deque_null_iterator();
            *d.cur = tdata;
            return d;
        }
    }
}

/* ---------------- erase：删除 pos 处元素，返回其后元素迭代器 ---------------- */
deque_iterator_t i_deque_erase(deque_t* _this, deque_iterator_t pos)
{
    deque_iterator_t b, e;
    long n, k;

    if (is_null(_this) || is_null(pos.cur))
        return i_deque_null_iterator();

    b = i_deque_begin(_this);
    e = i_deque_end(_this);
    n = (long)__i_deque_iterator_distance(b, e);

    if (0 == n || pos.cur == e.cur)               /* 空表 / 删 end：非法 */
        return i_deque_null_iterator();

    if (pos.cur == b.cur) {                       /* 删首元素 */
        i_deque_pop_front(_this);
        return i_deque_begin(_this);
    }
    if (pos.cur == i_deque_prev(e).cur) {         /* 删末元素 */
        i_deque_pop_back(_this);
        return i_deque_end(_this);
    }

    k = (long)__i_deque_iterator_distance(b, pos);
    if (k <= 0 || k >= n - 1)                     /* pos 必须是 [begin+1, end-1] */
        return i_deque_null_iterator();

    _deque_free_data(_this, pos.cur);             /* 释放被删元素（深拷贝容器） */

    if (k < n / 2) {                              /* 离头近：前缀右移覆盖 pos，再弹头 */
        deque_iterator_t d = pos;
        while (!i_deque_iter_eq(d, b)) {
            deque_iterator_t s = i_deque_prev(d);
            *d.cur = *s.cur;
            *s.cur = 0;
            d = s;
        }
        i_deque_pop_front(_this);                 /* 头部槽已清 0，pop 只丢弃不误释放 */
    } else {                                      /* 离尾近：后缀左移覆盖 pos，再弹尾 */
        deque_iterator_t d = pos;
        deque_iterator_t s = i_deque_next(d);
        while (!i_deque_iter_eq(s, e)) {
            *d.cur = *s.cur;
            *s.cur = 0;
            d = s;
            s = i_deque_next(s);
        }
        i_deque_pop_back(_this);
    }

    return _deque_advance(i_deque_begin(_this), k);
}

/* ---------------- erase_range：删除 [first, last) ---------------- */
deque_iterator_t i_deque_erase_range(deque_t* _this, deque_iterator_t first, deque_iterator_t last)
{
    deque_iterator_t b, e, dst, src;
    long k1, k2, cnt;

    if (is_null(_this))
        return i_deque_null_iterator();

    b = i_deque_begin(_this);
    e = i_deque_end(_this);

    k1 = (long)__i_deque_iterator_distance(b, first);
    k2 = (long)__i_deque_iterator_distance(b, last);
    if (k1 < 0 || k2 < k1 || k2 > (long)__i_deque_iterator_distance(b, e))
        return i_deque_null_iterator();

    cnt = k2 - k1;
    if (0 == cnt)                                 /* 空区间，原样返回 */
        return first;

    /* 释放并清零被删区间 [first, last) */
    for (dst = first; !i_deque_iter_eq(dst, last); dst = i_deque_next(dst)) {
        _deque_free_data(_this, dst.cur);
        *dst.cur = 0;
    }

    /* 把 [last, e) 尾段整体左移 cnt 格（移动语义，源清零），尾部空出 cnt 个 0 槽 */
    dst = first;
    src = last;
    while (!i_deque_iter_eq(src, e)) {
        *dst.cur = *src.cur;
        *src.cur = 0;
        dst = i_deque_next(dst);
        src = i_deque_next(src);
    }

    while (cnt-- > 0)                             /* 裁剪尾部 cnt 个 0 槽 */
        i_deque_pop_back(_this);

    return _deque_advance(i_deque_begin(_this), k1);
}

/* ---------------- find / count ---------------- */
deque_iterator_t i_deque_find(const deque_t* _this, deque_data_t data)
{
    deque_iterator_t it, b, e;

    if (is_null(_this))
        return i_deque_null_iterator();
    if (!is_null(_this->ops) && !is_null(_this->ops->valid_data) && !_this->ops->valid_data(data))
        return i_deque_null_iterator();

    b = i_deque_begin(_this);
    e = i_deque_end(_this);
    for (it = b; !i_deque_iter_eq(it, e); it = i_deque_next(it))
        if (_deque_eq(_this, data, *it.cur))
            return it;
    return e;
}

deque_count_t i_deque_count(const deque_t* _this, deque_data_t data)
{
    deque_iterator_t it, b, e;
    deque_count_t ret = 0;

    if (is_null(_this))
        return (deque_count_t)-1;
    if (!is_null(_this->ops) && !is_null(_this->ops->valid_data) && !_this->ops->valid_data(data))
        return (deque_count_t)-1;

    b = i_deque_begin(_this);
    e = i_deque_end(_this);
    for (it = b; !i_deque_iter_eq(it, e); it = i_deque_next(it))
        if (_deque_eq(_this, data, *it.cur))
            ret++;
    return ret;
}

/* ---------------- remove / remove_if：单遍压实 ---------------- */
deque_size_t i_deque_remove(deque_t* _this, deque_data_t data)
{
    _deque_match_t m;

    if (is_null(_this))
        return (deque_size_t)-1;
    if (!is_null(_this->ops) && !is_null(_this->ops->valid_data) && !_this->ops->valid_data(data))
        return (deque_size_t)-1;

    m.d = _this; m.data = data; m.cond = NULL; m.mode = 0;
    return _deque_compact(_this, &m);
}

deque_size_t i_deque_remove_if(deque_t* _this, remove_if_condition cond)
{
    _deque_match_t m;

    if (is_null(_this) || is_null(cond))
        return (deque_size_t)-1;

    m.d = _this; m.data = 0; m.cond = cond; m.mode = 1;
    return _deque_compact(_this, &m);
}

/* ---------------- clear ---------------- */
deque_size_t i_deque_clear(deque_t* _this)
{
    deque_size_t old;

    if (is_null(_this))
        return 0;

    old = i_deque_size(_this);
    while (!__i_deque_empty(_this))
        i_deque_pop_back(_this);
    return old;
}
