/*
  Queue Inline Implementations
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

#ifndef __J_I_QUEUE_H
#define __J_I_QUEUE_H

#include <deque/deque.h>
#include <list/list.h>

typedef struct i_queue {
    container_adaptor_base_t id;
    union {
        deque_t* d;
        list_t*  l;
        void*    p;
    };
} i_queue_t;

static inline
queue_size_t i_queue_size(const i_queue_t* _this)
{
    if (!is_null(_this)) {
        switch (_this->id) {
        case __CONTAINER_ADAPTOR_BASE_DEQUE: return cdeque_size(_this->d);
        case __CONTAINER_ADAPTOR_BASE_LIST:  return clist_size(_this->l);
        default:                              break;
        }
    }
    return -1;
}

static inline
bool __i_queue_empty(const i_queue_t* _this)
{
    switch (_this->id) {
    case __CONTAINER_ADAPTOR_BASE_DEQUE: return __cdeque_empty(_this->d);
    case __CONTAINER_ADAPTOR_BASE_LIST:  return __clist_empty(_this->l);
    default:                              return false;
    }
}

static inline
bool i_queue_push(i_queue_t* _this, queue_data_t data)
{
    if (!is_null(_this)) {
        switch (_this->id) {
        case __CONTAINER_ADAPTOR_BASE_DEQUE: return cdeque_push_back(_this->d, data);
        case __CONTAINER_ADAPTOR_BASE_LIST:  return clist_push_back(_this->l, data);
        default:                              break;
        }
    }
    return false;
}

static inline
queue_value_t __i_queue_back(const i_queue_t* _this)
{
    switch (_this->id) {
    case __CONTAINER_ADAPTOR_BASE_DEQUE: return __cdeque_back(_this->d);
    case __CONTAINER_ADAPTOR_BASE_LIST:  return __clist_back(_this->l);
    default:                              return (queue_value_t){ .u8 = NULL };
    }
}

static inline
queue_value_t i_queue_back(const i_queue_t* _this)
{
    if (!is_null(_this)) {
        switch (_this->id) {
        case __CONTAINER_ADAPTOR_BASE_DEQUE: return cdeque_back(_this->d);
        case __CONTAINER_ADAPTOR_BASE_LIST:  return clist_back(_this->l);
        default:                              break;
        }
    }
    return (queue_value_t){ .u8 = NULL };
}

static inline
queue_value_t __i_queue_front(const i_queue_t* _this)
{
    switch (_this->id) {
    case __CONTAINER_ADAPTOR_BASE_DEQUE: return __cdeque_front(_this->d);
    case __CONTAINER_ADAPTOR_BASE_LIST:  return __clist_front(_this->l);
    default:                              return (queue_value_t){ .u8 = NULL };
    }
}

static inline
queue_value_t i_queue_front(const i_queue_t* _this)
{
    if (!is_null(_this)) {
        switch (_this->id) {
        case __CONTAINER_ADAPTOR_BASE_DEQUE: return cdeque_front(_this->d);
        case __CONTAINER_ADAPTOR_BASE_LIST:  return clist_front(_this->l);
        default:                              break;
        }
    }
    return (queue_value_t){ .u8 = NULL };
}

static inline
void i_queue_pop(i_queue_t* _this)
{
    if (!is_null(_this)) {
        switch (_this->id) {
        case __CONTAINER_ADAPTOR_BASE_DEQUE: cdeque_pop_front(_this->d); break;
        case __CONTAINER_ADAPTOR_BASE_LIST:  clist_pop_front(_this->l);  break;
        default:                              break;
        }
    }
}

static inline
void i_queue_swap(i_queue_t* _this, i_queue_t* other)
{
    void* p;

    if (!is_null(_this) && !is_null(other) && _this != other && _this->id == other->id) {
        switch (_this->id) {
        case __CONTAINER_ADAPTOR_BASE_DEQUE:
            JDSC_ASSERT(   _this->d->ops   == other->d->ops /* Include sso */
                        && __sequence_container_dtype_swap_compatible(_this->d->dtype, other->d->dtype)
                        && _this->d->step  == other->d->step 
                        && _this->d->align == other->d->align);
            break;
        case __CONTAINER_ADAPTOR_BASE_LIST:
            JDSC_ASSERT(   _this->l->ops   == other->l->ops /* Include sso */
                        && __sequence_container_dtype_swap_compatible(_this->l->dtype, other->l->dtype)
                        && _this->l->step  == other->l->step 
                        && _this->l->align == other->l->align 
                        && _this->l->pad   == other->l->pad);
            break;
        default:
            break;
        }

        p = _this->p;
        _this->p = other->p;
        other->p = p;
    }
}

static inline
queue_size_t i_queue_clear(i_queue_t* _this)
{
    if (!is_null(_this)) {
        switch (_this->id) {
        case __CONTAINER_ADAPTOR_BASE_DEQUE: return cdeque_clear(_this->d);
        case __CONTAINER_ADAPTOR_BASE_LIST:  return clist_clear(_this->l);
        default:                              break;
        }
    }
    return -1;
}

#endif /* __J_I_QUEUE_H */
