/*
  Priority Queue Inline Implementations
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

#ifndef __J_I_PRIORITY_QUEUE_H
#define __J_I_PRIORITY_QUEUE_H

#include <vector/vector.h>
#include <deque/deque.h>

typedef struct i_priority_queue {
    const class_priority_queue_ops_t* ops;
    container_adaptor_base_t id;
    bool sso;
    union {
        vector_t* v;
        deque_t*  d;
        void*     p;
    };
    uint8_t* swap;
} i_priority_queue_t;

HEADER_STATIC_INLINE
priority_queue_size_t i_priority_queue_size(const i_priority_queue_t* _this)
{
    if (!is_null(_this)) {
        switch (_this->id) {
        case __CONTAINER_ADAPTOR_BASE_VECTOR: return cvector_size(_this->v);
        case __CONTAINER_ADAPTOR_BASE_DEQUE:  return cdeque_size(_this->d);
        default:                               break;
        }
    }
    return -1;
}

HEADER_STATIC_INLINE
bool __i_priority_queue_empty(const i_priority_queue_t* _this)
{
    switch (_this->id) {
    case __CONTAINER_ADAPTOR_BASE_VECTOR: return __cvector_empty(_this->v);
    case __CONTAINER_ADAPTOR_BASE_DEQUE:  return __cdeque_empty(_this->d);
    default:                               return false;
    }
}

bool i_priority_queue_push(i_priority_queue_t* _this, priority_queue_data_t data);
void i_priority_queue_pop(i_priority_queue_t* _this);

HEADER_STATIC_INLINE
priority_queue_value_t __i_priority_queue_top(const i_priority_queue_t* _this)
{
    switch (_this->id) {
    case __CONTAINER_ADAPTOR_BASE_VECTOR: return __cvector_front(_this->v);
    case __CONTAINER_ADAPTOR_BASE_DEQUE:  return __cdeque_front(_this->d);
    default:                               return (priority_queue_value_t){ .u8 = NULL };
    }
}

HEADER_STATIC_INLINE
priority_queue_value_t i_priority_queue_top(const i_priority_queue_t* _this)
{
    if (!is_null(_this)) {
        switch (_this->id) {
        case __CONTAINER_ADAPTOR_BASE_VECTOR:
            if (!__cvector_empty(_this->v))
                return cvector_front(_this->v);
            break;
        case __CONTAINER_ADAPTOR_BASE_DEQUE:
            if (!__cdeque_empty(_this->d))
                return cdeque_front(_this->d);
            break;
        default:
            break;
        }
    }
    return (priority_queue_value_t){ .u8 = NULL };
}

HEADER_STATIC_INLINE
void i_priority_queue_swap(i_priority_queue_t* _this, i_priority_queue_t* other)
{
    void* p;

    if (!is_null(_this) && !is_null(other) && _this != other && _this->id == other->id) {
        JDSC_ASSERT(_this->ops == other->ops); /* Include sso */
        switch (_this->id) {
        case __CONTAINER_ADAPTOR_BASE_VECTOR:
            JDSC_ASSERT(__sequence_container_dtype_swap_compatible(_this->v->dtype, other->v->dtype)
                        && _this->v->step  == other->v->step
                        && _this->v->align == other->v->align);
            break;
        case __CONTAINER_ADAPTOR_BASE_DEQUE:
            JDSC_ASSERT(__sequence_container_dtype_swap_compatible(_this->d->dtype, other->d->dtype)
                        && _this->d->step  == other->d->step
                        && _this->d->align == other->d->align);
            break;
        default:
            break;
        }

        p = _this->p;
        _this->p = other->p;
        other->p = p;
    }
}

HEADER_STATIC_INLINE
priority_queue_size_t i_priority_queue_clear(i_priority_queue_t* _this)
{
    if (!is_null(_this)) {
        switch (_this->id) {
        case __CONTAINER_ADAPTOR_BASE_VECTOR: return cvector_clear(_this->v);
        case __CONTAINER_ADAPTOR_BASE_DEQUE:  return cdeque_clear(_this->d);
        default:                               break;
        }
    }
    return -1;
}

#endif /* __J_I_PRIORITY_QUEUE_H */
