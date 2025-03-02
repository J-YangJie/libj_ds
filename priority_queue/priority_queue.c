/*
  Priority Queue Implementations
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

#include <priority_queue/priority_queue.h>

#include <_log.h>
#include <_memory.h>
#include <linux/_compiler.h>

#define TAG "[priority_queue]"

static __always_inline priority_queue_size_t __priority_queue_size(const priority_queue_t* _this)
{
    return _this->size;
}

static __always_inline priority_queue_size_t _priority_queue_size(const priority_queue_t* _this)
{
    if (unlikely(is_null(_this)))
        return -1;
    return __priority_queue_size(_this);
}

static __always_inline priority_queue_size_t __priority_queue_capacity(const priority_queue_t* _this)
{
    return _this->capacity;
}

static __always_inline void __priority_queue_exchange_data(priority_queue_data_t* a, priority_queue_data_t* b)
{
    priority_queue_data_t tdata = *a;
    *a = *b;
    *b = tdata;
}

static inline void __priority_queue_rise(priority_queue_t* _this, priority_queue_size_t index)
{
    priority_queue_size_t i = index;
    priority_queue_size_t p;

    if (is_null(_this->ops) || is_null(_this->ops->__lt)) {
        for (; i > 0 && (p = (i - 1) >> 1) >= 0;) {
            if (_this->head[p].data >= _this->head[i].data)
                break;

            __priority_queue_exchange_data(&_this->head[p].data, &_this->head[i].data);
            i = p;
        }
    } else {
        for (; i > 0 && (p = (i - 1) >> 1) >= 0;) {
            if (!_this->ops->__lt(_this->head[p].data, _this->head[i].data))
                break;

            __priority_queue_exchange_data(&_this->head[p].data, &_this->head[i].data);
            i = p;
        }
    }
}

static inline void __priority_queue_heapify(priority_queue_t* _this, priority_queue_size_t index)
{
    priority_queue_size_t i = index;
    priority_queue_size_t len = __priority_queue_size(_this) - 1;
    priority_queue_size_t m;

    if (index >= __priority_queue_size(_this))
        return ;

    if (is_null(_this->ops) || is_null(_this->ops->__lt)) {
        for (; (i << 1) + 1 <= len;) {
            priority_queue_size_t ls = (i << 1) + 1;
            priority_queue_size_t rs = (i << 1) + 2;

            m = ls <= len && _this->head[i].data < _this->head[ls].data ? ls : i;
            m = rs <= len && _this->head[m].data < _this->head[rs].data ? rs : m;

            if (i == m)
                break;

            __priority_queue_exchange_data(&_this->head[m].data, &_this->head[i].data);
            i = m;
        }
    } else {
        for (; (i << 1) + 1 <= len;) {
            priority_queue_size_t ls = (i << 1) + 1;
            priority_queue_size_t rs = (i << 1) + 2;

            m = ls <= len && _this->ops->__lt(_this->head[i].data, _this->head[ls].data) ? ls : i;
            m = rs <= len && _this->ops->__lt(_this->head[m].data, _this->head[rs].data) ? rs : m;

            if (i == m)
                break;

            __priority_queue_exchange_data(&_this->head[m].data, &_this->head[i].data);
            i = m;
        }
    }
}

static inline bool __priority_queue_push(priority_queue_t* _this, priority_queue_data_t data)
{
    priority_queue_node_t* n = NULL;
    priority_queue_data_t tdata = data;

    if (!is_null(_this->ops) && !is_null(_this->ops->copy_data) && !_this->ops->copy_data(data, &tdata))
        return false;

    if (__priority_queue_capacity(_this) == __priority_queue_size(_this)) {
        n = p_realloc(_this->head, sizeof(priority_queue_node_t) * (0 == _this->capacity ? 1 : _this->capacity << 1));
        if (is_null(n))
            goto err;

        if (_this->head != n) {
            pr_notice("Head [ %p -> %p ], capacity [ %zd -> %zd ]", 
                        _this->head, n, _this->capacity, (0 == _this->capacity ? 1 : _this->capacity << 1));
            _this->head = n;
        } else {
            pr_notice("Head [ %p ], capacity [ %zd -> %zd ]", 
                        n, _this->capacity, (0 == _this->capacity ? 1 : _this->capacity << 1));
        }

        _this->capacity = 0 == _this->capacity ? 1 : _this->capacity << 1;
    }

    _this->head[__priority_queue_size(_this)].data = tdata;
    __priority_queue_rise(_this, __priority_queue_size(_this));
    _this->size++;
    return true;

err:
    if (!is_null(_this->ops) && !is_null(_this->ops->free_data) && !is_null(_this->ops->copy_data))
        _this->ops->free_data(&tdata);
    return false;
}

static __always_inline bool priority_queue_push(priority_queue_t* _this, priority_queue_data_t data)
{
    if (unlikely(is_null(_this)))
        return false;

    if (!is_null(_this->ops) && !is_null(_this->ops->valid_data) && !_this->ops->valid_data(data))
        return false;

    return __priority_queue_push(_this, data);
}

static __always_inline priority_queue_data_t __priority_queue_top(priority_queue_t* _this, priority_queue_data_t default_data)
{
    if (__priority_queue_size(_this) <= 0)
        return default_data;
    return _this->head[0].data;
}

static __always_inline priority_queue_data_t priority_queue_top(priority_queue_t* _this, priority_queue_data_t default_data)
{
    if (unlikely(is_null(_this)))
        return default_data;
    return __priority_queue_top(_this, default_data);
}

static __always_inline priority_queue_data_t __priority_queue_pop(priority_queue_t* _this, priority_queue_data_t default_data)
{
    if (__priority_queue_size(_this) <= 0)
        return default_data;

    if (1 == __priority_queue_size(_this)) {
        _this->size = 0;
        return _this->head[0].data;
    }

    __priority_queue_exchange_data(&_this->head[__priority_queue_size(_this) - 1].data, &_this->head[0].data);
    _this->size--;
    __priority_queue_heapify(_this, 0);

    return _this->head[__priority_queue_size(_this)].data;
}

static __always_inline priority_queue_data_t priority_queue_pop(priority_queue_t* _this, priority_queue_data_t default_data)
{
    if (unlikely(is_null(_this)))
        return default_data;
    return __priority_queue_pop(_this, default_data);
}

static inline priority_queue_size_t priority_queue_clear(priority_queue_t* _this)
{
    priority_queue_size_t ret = 0;

    if (unlikely(is_null(_this)))
        return -1;

    ret = __priority_queue_size(_this);
    if (ret <= 0)
        return ret;

    if (is_null(_this->ops) || is_null(_this->ops->free_data)) {
        _this->size = 0;
        return ret;
    }

    for (priority_queue_size_t i = 0; i < ret; ++i) {
        _this->ops->free_data(&_this->head[i].data);
        _this->size--;
    }

    return ret - __priority_queue_size(_this);
}

__always_inline void __priority_queue_init(priority_queue_t* priority_queue)
{
    priority_queue->head = NULL;
    priority_queue->capacity = 0;
}

__always_inline void __priority_queue_deinit(priority_queue_t* priority_queue)
{
    priority_queue_clear(priority_queue);
    p_free(priority_queue->head);

    priority_queue->ops = NULL;
    priority_queue->head = NULL;
    priority_queue->size = 0;
    priority_queue->capacity = 0;
}

__always_inline const class_priority_queue_t* class_priority_queue_ins(void)
{
    static const class_priority_queue_t ins = {
        .size        = _priority_queue_size,
        .push        = priority_queue_push,
        .top         = priority_queue_top,
        .pop         = priority_queue_pop,
        .clear       = priority_queue_clear,
    };
    return &ins;
}
