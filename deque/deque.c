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

/* checked2 */
static
bool __i_deque_reserve(i_deque_t* _this)
{
    deque_bcount_t bcnt_o = _this->bkt_count;
    deque_bcount_t bcnt_n = bcnt_o > 0 ? bcnt_o * 2 : DEQUE_CAPACITY_INIT;
    deque_bcount_t off_b  = _this->begin.bkt - _this->bkt;
    deque_bcount_t off_e  = _this->end.bkt   - _this->bkt;
    deque_bcount_t n      = off_e - off_b + 1;
    deque_bcount_t off_n  = (bcnt_n - n) / 2;
    uint8_t**      bkt_n;

    bkt_n = (uint8_t**)p_calloc(bcnt_n, sizeof(uint8_t*));
    if (is_null(bkt_n))
        return false;

    memcpy(bkt_n + off_n, _this->begin.bkt, n * sizeof(uint8_t*));
    p_free(_this->bkt);

    _this->bkt = bkt_n;
    _this->bkt_count = bcnt_n;
    _this->begin.bkt = _this->bkt + off_n;
    _this->end.bkt   = _this->bkt + off_n + n - 1;
    return true;
}

/* checked2 */
static JDSC_INLINE_FORCE
uint8_t* i_deque_alloc_bkt(const i_deque_t* _this)
{
    return (uint8_t*)p_malloc(_this->bkt_size * _this->step);
}

/* checked2 */
uint8_t* __i_deque_push_back_bkt_alloc(i_deque_t* _this)
{
    uint8_t** bkt;
    uint8_t*  slot;

    if (_this->end.bkt == _this->bkt + _this->bkt_count - 1) {
        if (!__i_deque_reserve(_this))
            return NULL;
    }

    bkt = _this->end.bkt + 1;
    *bkt = i_deque_alloc_bkt(_this);
    if (is_null(*bkt))
        return NULL;

    slot = _this->end.cur;
    __i_deque_set_bkt_n(&_this->end, bkt, _this->bkt_size, _this->step);
    _this->end.cur = _this->end.begin;
    return slot;
}

/* checked2 */
uint8_t* __i_deque_push_front_bkt_alloc(i_deque_t* _this)
{
    uint8_t** bkt;

    if (_this->begin.bkt == _this->bkt) {
        if (!__i_deque_reserve(_this))
            return NULL;
    }

    bkt = _this->begin.bkt - 1;
    *bkt = i_deque_alloc_bkt(_this);
    if (is_null(*bkt))
        return NULL;

    __i_deque_set_bkt_n(&_this->begin, bkt, _this->bkt_size, _this->step);
    _this->begin.cur = __i_deque_ptr_sub(_this->begin.end, 1, _this->step);
    return _this->begin.cur;
}

/* checked2 */
JDSC_INLINE
void __i_deque_pop_back_bkt_free(i_deque_t* _this)
{
    p_free(_this->end.begin);
    __i_deque_set_bkt_n(&_this->end, _this->end.bkt - 1, _this->bkt_size, _this->step);
    _this->end.cur = __i_deque_ptr_sub(_this->end.end, 1, _this->step);
}

/* checked2 */
JDSC_INLINE
void __i_deque_pop_front_bkt_free(i_deque_t* _this)
{
    p_free(_this->begin.begin);
    __i_deque_set_bkt_n(&_this->begin, _this->begin.bkt + 1, _this->bkt_size, _this->step);
    _this->begin.cur = _this->begin.begin;
}

/* checked2 */
static JDSC_INLINE
void __i_deque_slot_move(const i_deque_t* _this, uint8_t* dst, const uint8_t* src)
{
    switch (_this->step) {
    case 1:  memmove(dst, src, 1);  return ;
    case 2:  memmove(dst, src, 2);  return ;
    case 4:  memmove(dst, src, 4);  return ;
    case 8:  memmove(dst, src, 8);  return ;
    case 16: memmove(dst, src, 16); return ;
    case 32: memmove(dst, src, 32); return ;
    default: memmove(dst, src, _this->step); return ;
    }
}

/* checked2 */
static JDSC_INLINE
void __i_deque_memmove_forward(const i_deque_t* _this, deque_iterator_t dest, deque_iterator_t src, deque_size_t n)
{
    for (deque_size_t i = 0; i < n; ++i) {
        __i_deque_slot_move(_this, dest.cur, src.cur);
        dest = __i_deque_next(dest);
        src  = __i_deque_next(src);
    }
}

/* checked2 */
static JDSC_INLINE
void __i_deque_memmove_backward(const i_deque_t* _this, deque_iterator_t dest, deque_iterator_t src, deque_size_t n)
{
    if (n <= 0)
        return ;

    dest = __i_deque_advance(dest, n - 1);
    src  = __i_deque_advance(src,  n - 1);
    for (deque_size_t i = 0; i < n; ++i) {
        __i_deque_slot_move(_this, dest.cur, src.cur);
        dest = __i_deque_prev(dest);
        src  = __i_deque_prev(src);
    }
}

/* checked2 */
static JDSC_INLINE_FORCE_POLICY
bool __i_deque_push_back_null(i_deque_t* _this)
{
    if (_this->end.cur != __i_deque_ptr_sub(_this->end.end, 1, _this->step)) {
        _this->end.cur = __i_deque_ptr_add(_this->end.cur, 1, _this->step);
        return true;
    }
    return !is_null(__i_deque_push_back_bkt_alloc(_this));
}

/* checked2 */
static JDSC_INLINE_FORCE_POLICY
bool __i_deque_push_front_null(i_deque_t* _this)
{
    if (_this->begin.cur != _this->begin.begin) {
        _this->begin.cur = __i_deque_ptr_sub(_this->begin.cur, 1, _this->step);
        return true;
    }
    return !is_null(__i_deque_push_front_bkt_alloc(_this));
}

/* checked2 */
static JDSC_INLINE
void __i_deque_pop_back_without_free_data(i_deque_t* _this)
{
    if (__i_deque_empty(_this))
        return ;

    if (_this->end.cur != _this->end.begin)
        _this->end.cur = __i_deque_ptr_sub(_this->end.cur, 1, _this->step);
    else
        __i_deque_pop_back_bkt_free(_this);
}

/* checked2 */
static JDSC_INLINE
void __i_deque_pop_front_without_free_data(i_deque_t* _this)
{
    if (__i_deque_empty(_this))
        return ;

    if (_this->begin.cur != __i_deque_ptr_sub(_this->begin.end, 1, _this->step))
        _this->begin.cur = __i_deque_ptr_add(_this->begin.cur, 1, _this->step);
    else
        __i_deque_pop_front_bkt_free(_this);
}

/* checked2 */
static JDSC_INLINE
bool __i_deque_slot_write_n(const i_deque_t* _this, deque_iterator_t it, deque_data_t data, deque_size_t n)
{
    deque_size_t succ = 0;

    if (!is_null(_this->ops) && !is_null(_this->ops->copy_data)) {
        while (n-- > 0) {
            if (_this->ops->copy_data(data, (deque_data_t*)it.cur))
                succ++;
            else
                goto err;
            it = __i_deque_next(it);
        }

        return true;
    } else {
        if (_this->step > sizeof(deque_data_t))
            return false;

        while (n-- > 0) {
            switch (_this->step)
            {
            case 1: *(uint8_t*)it.cur  = (uint8_t)data;  break;
            case 2: *(uint16_t*)it.cur = (uint16_t)data; break;
            case 4: *(uint32_t*)it.cur = (uint32_t)data; break;
            case 8: *(uint64_t*)it.cur = (uint64_t)data; break;
            default: break;
            }

            succ++;
            it = __i_deque_next(it);
        }

        return true;
    }

err:
    if (!is_null(_this->ops) && !is_null(_this->ops->free_data)) {
        for (deque_size_t i = 0; i < succ; ++i) {
            it = __i_deque_prev(it);
            _this->ops->free_data((deque_data_t*)it.cur);
        }
    }
    return false;
}

/* checked2 */
deque_iterator_t __i_deque_insert_run(i_deque_t* _this, deque_iterator_t pos, deque_data_t data, deque_size_t n)
{
    deque_size_t i;
    deque_iterator_t ret;

    if (n < 0)
        return i_deque_null_iterator();

    deque_size_t fn = __i_deque_iterator_distance(_this->begin, pos);
    deque_size_t bn = __i_deque_iterator_distance(pos, _this->end);
    if (fn < 0 || bn < 0)
        return i_deque_null_iterator();

    if (fn < bn) {
        for (i = 0; i < n; ++i) {
            if (!__i_deque_push_front_null(_this))
                goto err;
        }

        __i_deque_memmove_forward(_this, _this->begin, __i_deque_advance(_this->begin, n), fn);
        ret = __i_deque_advance(_this->begin, fn);
        if (!__i_deque_slot_write_n(_this, ret, data, n))
            goto err_slot;
        return ret;
    } else {
        for (i = 0; i < n; ++i) {
            if (!__i_deque_push_back_null(_this))
                goto err;
        }

        ret = __i_deque_advance(_this->begin, fn);
        __i_deque_memmove_backward(_this, __i_deque_advance(ret, n), ret, bn);
        if (!__i_deque_slot_write_n(_this, ret, data, n))
            goto err_slot;
        return ret;
    }

err_slot:
    if (fn < bn) {
        __i_deque_memmove_backward(_this, __i_deque_advance(_this->begin, n), _this->begin, fn);
    } else {
        __i_deque_memmove_forward(_this, ret, __i_deque_advance(ret, n), bn);
    }

err:
    if (fn < bn) {
        for (; i > 0; --i)
            __i_deque_pop_front_without_free_data(_this);
    } else {
        for (; i > 0; --i)
            __i_deque_pop_back_without_free_data(_this);
    }
    return i_deque_null_iterator();
}

/* checked2 */
deque_iterator_t __i_deque_insert(i_deque_t* _this, deque_iterator_t pos, deque_data_t data)
{
    if (!is_null(_this->ops) && !is_null(_this->ops->valid_data) && !_this->ops->valid_data(data))
        return i_deque_null_iterator();
    return __i_deque_insert_run(_this, pos, data, 1);
}

/* checked2 */
deque_iterator_t i_deque_insert_n(i_deque_t* _this, deque_iterator_t pos, deque_size_t n, deque_data_t data)
{
    if (is_null(_this) || i_deque_is_null_iterator(pos) || n < 0)
        return i_deque_null_iterator();

    if (0 == n)
        return pos;

    if (!is_null(_this->ops) && !is_null(_this->ops->valid_data) && !_this->ops->valid_data(data))
        return i_deque_null_iterator();
    return __i_deque_insert_run(_this, pos, data, n);
}

/* checked2 */
deque_iterator_t __i_deque_erase(i_deque_t* _this, deque_iterator_t pos)
{
    deque_size_t fn = __i_deque_iterator_distance(_this->begin, pos);
    deque_size_t bn = __i_deque_iterator_distance(pos, _this->end);
    if (fn < 0 || bn < 0)
        return i_deque_null_iterator();

    deque_iterator_t pnext = __i_deque_next(pos);

    if (!is_null(_this->ops) && !is_null(_this->ops->free_data))
        _this->ops->free_data((deque_data_t*)pos.cur);

    if (fn < bn - 1) {
        __i_deque_memmove_backward(_this, __i_deque_next(_this->begin), _this->begin, fn);
        __i_deque_pop_front_without_free_data(_this);
        return pnext;
    } else {
        __i_deque_memmove_forward(_this, pos, pnext, bn - 1);
        __i_deque_pop_back_without_free_data(_this);
        return pos;
    }
}

/* checked2 */
deque_iterator_t __i_deque_erase_range(i_deque_t* _this, deque_iterator_t iterator_begin, deque_iterator_t iterator_end)
{
    deque_size_t fn = __i_deque_iterator_distance(_this->begin, iterator_begin);
    deque_size_t bn = __i_deque_iterator_distance(iterator_end, _this->end);
    deque_size_t en = __i_deque_iterator_distance(iterator_begin, iterator_end);

    if (fn < 0 || bn < 0 || en < 0)
        return i_deque_null_iterator();

    if (0 == en)
        return iterator_begin;

    if (!is_null(_this->ops) && !is_null(_this->ops->free_data)) {
        for (deque_iterator_t it = iterator_begin; iterator_end.cur != it.cur; it = __i_deque_next(it))
            _this->ops->free_data((deque_data_t*)it.cur);
    }

    if (fn < bn) {
        deque_iterator_t nbegin = __i_deque_advance(_this->begin, en);
        __i_deque_memmove_backward(_this, nbegin, _this->begin, fn);
        for (deque_size_t i = 0; i < en; ++i)
            __i_deque_pop_front_without_free_data(_this);
        return iterator_end;
    } else {
        __i_deque_memmove_forward(_this, iterator_begin, iterator_end, bn);
        for (deque_size_t i = 0; i < en; ++i)
            __i_deque_pop_back_without_free_data(_this);
        return iterator_begin;
    }
}

/* checked2 */
static JDSC_INLINE_FORCE
uint8_t* __i_deque_bkt_first(const i_deque_t* _this, uint8_t** bkt)
{
    return bkt == _this->begin.bkt ? _this->begin.cur : *bkt;
}

/* checked2 */
static JDSC_INLINE_FORCE
uint8_t* __i_deque_bkt_last(const i_deque_t* _this, uint8_t** bkt)
{
    return bkt == _this->end.bkt ? _this->end.cur
                                 : __i_deque_ptr_add(*bkt, _this->bkt_size, _this->step);
}

/* checked2 */
static JDSC_INLINE_FORCE
deque_iterator_t __i_deque_make_iterator(const i_deque_t* _this, uint8_t** bkt, uint8_t* cur)
{
    deque_iterator_t it;
    __i_deque_set_bkt_n(&it, bkt, _this->bkt_size, _this->step);
    it.cur = cur;
    return it;
}

/* checked2 */
static JDSC_INLINE
deque_data_t __i_deque_slot_read(const i_deque_t* _this, const uint8_t* slot)
{
    deque_data_t data = 0;

    if (_this->step > sizeof(deque_data_t))
        return (deque_data_t)(uintptr_t)slot;

    switch (_this->step)
    {
    case 1: data = *(const uint8_t*)slot;  break;
    case 2: data = *(const uint16_t*)slot; break;
    case 4: data = *(const uint32_t*)slot; break;
    case 8: data = *(const uint64_t*)slot; break;
    default: break;
    }
    return data;
}

/* checked2 */
deque_iterator_t i_deque_find(const i_deque_t* _this, deque_data_t data)
{
    uint8_t** bkt;

    if (is_null(_this))
        return i_deque_null_iterator();

    if (!is_null(_this->ops) && !is_null(_this->ops->valid_data) && !_this->ops->valid_data(data))
        return i_deque_null_iterator();

    if (is_null(_this->ops) || is_null(_this->ops->__eq)) {
        for (bkt = _this->begin.bkt; bkt <= _this->end.bkt; ++bkt) {
            uint8_t* i = __i_deque_bkt_first(_this, bkt);
            uint8_t* l = __i_deque_bkt_last(_this, bkt);

            for ( ; i < l; i = __i_deque_ptr_add(i, 1, _this->step)) {
                if (__i_deque_slot_read(_this, i) == data)
                    return __i_deque_make_iterator(_this, bkt, i);
            }
        }
    } else {
        for (bkt = _this->begin.bkt; bkt <= _this->end.bkt; ++bkt) {
            uint8_t* i = __i_deque_bkt_first(_this, bkt);
            uint8_t* l = __i_deque_bkt_last(_this, bkt);

            for ( ; i < l; i = __i_deque_ptr_add(i, 1, _this->step)) {
                if (_this->ops->__eq(__i_deque_slot_read(_this, i), data))
                    return __i_deque_make_iterator(_this, bkt, i);
            }
        }
    }
    return _this->end;
}

/* checked2 */
deque_count_t i_deque_count(const i_deque_t* _this, deque_data_t data)
{
    uint8_t** bkt;
    deque_count_t ret = 0;

    if (is_null(_this))
        return -1;

    if (!is_null(_this->ops) && !is_null(_this->ops->valid_data) && !_this->ops->valid_data(data))
        return -1;

    if (is_null(_this->ops) || is_null(_this->ops->__eq)) {
        for (bkt = _this->begin.bkt; bkt <= _this->end.bkt; ++bkt) {
            uint8_t* i = __i_deque_bkt_first(_this, bkt);
            uint8_t* l = __i_deque_bkt_last(_this, bkt);

            for ( ; i < l; i = __i_deque_ptr_add(i, 1, _this->step)) {
                if (__i_deque_slot_read(_this, i) == data)
                    ret++;
            }
        }
    } else {
        for (bkt = _this->begin.bkt; bkt <= _this->end.bkt; ++bkt) {
            uint8_t* i = __i_deque_bkt_first(_this, bkt);
            uint8_t* l = __i_deque_bkt_last(_this, bkt);

            for ( ; i < l; i = __i_deque_ptr_add(i, 1, _this->step)) {
                if (_this->ops->__eq(__i_deque_slot_read(_this, i), data))
                    ret++;
            }
        }
    }
    return ret;
}

/* checked2 */
deque_size_t i_deque_clear(i_deque_t* _this)
{
    uint8_t** bkt;
    deque_count_t ret = 0;

    if (is_null(_this))
        return -1;

    if (!is_null(_this->ops) && !is_null(_this->ops->free_data)) {
        for (bkt = _this->begin.bkt; bkt <= _this->end.bkt; ++bkt) {
            uint8_t* i = __i_deque_bkt_first(_this, bkt);
            uint8_t* l = __i_deque_bkt_last(_this, bkt);

            for ( ; i < l; i = __i_deque_ptr_add(i, 1, _this->step)) {
                _this->ops->free_data((deque_data_t*)i);
                ret++;
            }

            if (_this->begin.bkt != bkt)
                p_free(*bkt);
        }
    }

    deque_bcount_t m = _this->bkt_count / 2;
    _this->bkt[m] = *_this->begin.bkt;
    *_this->begin.bkt = NULL;

    __i_deque_set_bkt_n(&_this->begin, &_this->bkt[m], _this->bkt_size, _this->step);
    _this->begin.cur = _this->begin.begin;
    _this->end       = _this->begin;
    return ret;
}

/* checked2 */
static
bool __deque_init(deque_t* deque)
{
    deque_bcount_t m = DEQUE_CAPACITY_INIT / 2;

    deque->bkt_count_init = DEQUE_CAPACITY_INIT;
    deque->bkt_count      = deque->bkt_count_init;
    deque->bkt            = (uint8_t**)p_calloc(deque->bkt_count, sizeof(uint8_t*));
    if (is_null(deque->bkt))
        return false;

    deque->bkt[m] = i_deque_alloc_bkt(deque);
    if (is_null(deque->bkt[m]))
        goto err;

    __i_deque_set_bkt_n(&deque->begin, &deque->bkt[m], deque->bkt_size, deque->step);
    deque->begin.cur = deque->begin.begin;
    deque->end       = deque->begin;
    return true;

err:
    p_free(deque->bkt);
    return false;
}

/* checked2 */
static
void __deque_deinit(deque_t* deque)
{
    i_deque_clear(deque);
    p_free(*deque->begin.bkt);
    p_free(deque->bkt);
}

/* checked2 */
deque_t* __deque_new(const class_deque_ops_t* ops, deque_step_t step)
{
    deque_t* deque;

    /* TODO: 要制定规则，有限制 */
    if (0 == step)
        return NULL;

    deque = (deque_t*)p_calloc(1, sizeof(deque_t));
    if (is_null(deque))
        return NULL;

    /* TODO: 要制定规则，有限制 */
    deque->step     = step;
    deque->bkt_size = step > _I_DEQUE_BKT_BYTES ? step * 16 : _I_DEQUE_BKT_BYTES / step;

    if (!__deque_init(deque))
        goto err;

    deque->ops = ops;
    return deque;

err:
    p_free(deque);
    return NULL;
}

/* checked2 */
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

static JDSC_INLINE
void _deque_free_run(const deque_t* _this, const deque_data_t* run, deque_size_t n)
{
    while (n-- > 0)
        _deque_free_data(_this, (deque_data_t*)run + n);
}

/* ============================================================================
 * remove / remove_if —— 照 bits/stl_algo.h 的 __remove_if 来
 *
 *     first = __find_if(first, last, pred);   // 先找第一个要删的
 *     if (first == last) return first;        // 一个都没有，原样返回
 *     result = first;
 *     ++first;
 *     for (; first != last; ++first)
 *         if (!pred(first)) { *result = MOVE(*first); ++result; }
 *     return result;                          // 新的逻辑尾，真删交给 erase
 *
 * 保留的元素往前覆盖，被删的原地留着，最后 [result, last) 一起 erase 掉。
 * C 里没有"移动赋值顺带析构"，所以三件事得显式做：
 *   1) 覆盖前先 free_data(result 槽) —— 那个槽此刻装的正是被跳过的待删元素
 *   2) 搬走后把源槽清 0 —— 否则尾部 erase 时会对同一份内容重复 free
 *   3) 尾部 [result, last) 交给 erase_range；槽已清零，free_data 自动 no-op
 * ========================================================================== */
typedef struct {
    const deque_t*      d;
    deque_data_t        data;    /* remove 的比对数据 */
    remove_if_condition cond;    /* remove_if 的条件 */
    int                 mode;    /* 0 = remove(data)，1 = remove_if(cond) */
} _deque_match_t;

static inline
bool _deque_hit(const _deque_match_t* m, const uint8_t* slot)   /* true 表示该删 */
{
    // if (0 == m->mode)
        // return _deque_slot_eq(m->d, m->data, slot);
    return true;
    //return is_null(m->cond) ? false : m->cond(__i_deque_slot_read(m->d, slot));
}

static
deque_size_t _deque_remove_run(deque_t* _this, _deque_match_t* m)
{
    deque_iterator_t it, last, result;
    deque_size_t     removed;

    it   = i_deque_begin(_this);
    last = i_deque_end(_this);

    while (!i_deque_iter_eq(it, last) && !_deque_hit(m, it.cur))   /* __find_if */
        it = i_deque_next(it);

    if (i_deque_iter_eq(it, last))
        return 0;                                                  /* 一个都没命中 */

    result = it;
    it     = i_deque_next(it);

    for (; !i_deque_iter_eq(it, last); it = i_deque_next(it)) {
        if (!_deque_hit(m, it.cur)) {
            _deque_free_data(_this, (deque_data_t*)result.cur);    /* 1) 销毁 result 槽里的待删元素 */
            __i_deque_slot_move(_this, result.cur, it.cur);        /*    *result = MOVE(*it) */
            //__i_deque_slot_zero(_this, it.cur);                    /* 2) 源槽清 0 */
            result = i_deque_next(result);
        }
    }

    removed = __i_deque_iterator_distance(result, last);
    i_deque_erase_range(_this, result, last);                      /* 3) 真删 */
    return removed;
}

/* 数据校验 / 拷贝助手，语义与 push 系列一致 */
static inline
bool _deque_valid_one(const deque_t* _this, deque_data_t data)
{
    if (is_null(_this->ops) || is_null(_this->ops->valid_data))
        return true;
    return _this->ops->valid_data(data);
}

static inline
bool _deque_copy_one(const deque_t* _this, deque_data_t in, deque_data_t* out)
{
    *out = in;
    if (!is_null(_this->ops) && !is_null(_this->ops->copy_data))
        return _this->ops->copy_data(in, out);
    return true;
}

/* ---------------- remove / remove_if ---------------- */
deque_size_t i_deque_remove(deque_t* _this, deque_data_t data)
{
    _deque_match_t m;

    if (is_null(_this))
        return (deque_size_t)-1;
    if (!is_null(_this->ops) && !is_null(_this->ops->valid_data) && !_this->ops->valid_data(data))
        return (deque_size_t)-1;

    m.d = _this; m.data = data; m.cond = NULL; m.mode = 0;
    return _deque_remove_run(_this, &m);
}

deque_size_t i_deque_remove_if(deque_t* _this, remove_if_condition cond)
{
    _deque_match_t m;

    if (is_null(_this) || is_null(cond))
        return (deque_size_t)-1;

    m.d = _this; m.data = 0; m.cond = cond; m.mode = 1;
    return _deque_remove_run(_this, &m);
}
