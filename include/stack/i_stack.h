/*
  Stack Inline Implementations
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

#ifndef __J_I_STACK_H
#define __J_I_STACK_H

#include <vector/vector.h>
#include <deque/deque.h>
#include <list/list.h>

typedef struct i_stack {
    uint8_t id;
    union {
        vector_t* v;
        deque_t*  d;
        list_t*   l;
        void*     p;
    };
} i_stack_t;

static inline
stack_size_t i_stack_size(const i_stack_t* _this)
{
    if (!is_null(_this)) {
        switch (_this->id) {
        case __SEQUENCE_CONTAINER_BASE_VECTOR: return cvector_size(_this->v);
        case __SEQUENCE_CONTAINER_BASE_DEQUE:  return cdeque_size(_this->d);
        case __SEQUENCE_CONTAINER_BASE_LIST:   return clist_size(_this->l);
        default:                               break;
        }
    }
    return -1;
}

static inline
bool __i_stack_empty(const i_stack_t* _this)
{
    switch (_this->id) {
    case __SEQUENCE_CONTAINER_BASE_VECTOR: return __cvector_empty(_this->v);
    case __SEQUENCE_CONTAINER_BASE_DEQUE:  return __cdeque_empty(_this->d);
    case __SEQUENCE_CONTAINER_BASE_LIST:   return __clist_empty(_this->l);
    default:                               return false;
    }
}

static inline
stack_value_t i_stack_top(const i_stack_t* _this)
{
    if (!is_null(_this)) {
        switch (_this->id) {
        case __SEQUENCE_CONTAINER_BASE_VECTOR: return cvector_back(_this->v);
        case __SEQUENCE_CONTAINER_BASE_DEQUE:  return cdeque_back(_this->d);
        case __SEQUENCE_CONTAINER_BASE_LIST:   return clist_back(_this->l);
        default:                               break;
        }
    }
    return (stack_value_t){ .u8 = NULL };
}

static inline
bool i_stack_push(i_stack_t* _this, stack_data_t data)
{
    if (!is_null(_this)) {
        switch (_this->id) {
        case __SEQUENCE_CONTAINER_BASE_VECTOR: return cvector_push_back(_this->v, data);
        case __SEQUENCE_CONTAINER_BASE_DEQUE:  return cdeque_push_back(_this->d, data);
        case __SEQUENCE_CONTAINER_BASE_LIST:   return clist_push_back(_this->l, data);
        default:                               break;
        }
    }
    return false;
}

static inline
void i_stack_pop(i_stack_t* _this)
{
    if (!is_null(_this)) {
        switch (_this->id) {
        case __SEQUENCE_CONTAINER_BASE_VECTOR: cvector_pop_back(_this->v); break;
        case __SEQUENCE_CONTAINER_BASE_DEQUE:  cdeque_pop_back(_this->d);  break;
        case __SEQUENCE_CONTAINER_BASE_LIST:   clist_pop_back(_this->l);   break;
        default:                               break;
        }
    }
}

static inline
void i_stack_swap(i_stack_t* _this, i_stack_t* other)
{
    void* p;

    if (!is_null(_this) && !is_null(other) && _this != other && _this->id == other->id) {
        p = _this->p;
        _this->p = other->p;
        other->p = p;
        return ;
    }
    JDSC_ASSERT(!is_null(_this) && !is_null(other) && _this->id == other.id);
}

static inline
stack_size_t i_stack_clear(i_stack_t* _this)
{
    if (!is_null(_this)) {
        switch (_this->id) {
        case __SEQUENCE_CONTAINER_BASE_VECTOR: return cvector_clear(_this->v);
        case __SEQUENCE_CONTAINER_BASE_DEQUE:  return cdeque_clear(_this->d);
        case __SEQUENCE_CONTAINER_BASE_LIST:   return clist_clear(_this->l);
        default:                               break;
        }
    }
    return -1;
}

#endif /* __J_I_STACK_H */
