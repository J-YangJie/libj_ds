/*
  Queue Interfaces
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

#ifndef __J_QUEUE_H
#define __J_QUEUE_H

#include <container_adaptor/container_adaptor.h>

typedef sequence_container_value_t     queue_value_t;
typedef class_sequence_container_ops_t class_queue_ops_t;

#include <queue/i_queue.h>

typedef i_queue_t queue_t;

/* Method 1 */
static inline queue_size_t  cqueue_size(const queue_t* _this)               { return i_queue_size(_this); }
static inline bool          __cqueue_empty(const queue_t* _this)            { return __i_queue_empty(_this); }
static inline bool          cqueue_push(queue_t* _this, queue_data_t data)  { return i_queue_push(_this, data); }
static inline queue_value_t __cqueue_back(const queue_t* _this)             { return __i_queue_back(_this); }
static inline queue_value_t cqueue_back(const queue_t* _this)               { return i_queue_back(_this); }
static inline queue_value_t __cqueue_front(const queue_t* _this)            { return __i_queue_front(_this); }
static inline queue_value_t cqueue_front(const queue_t* _this)              { return i_queue_front(_this); }
static inline void          cqueue_pop(queue_t* _this)                      { i_queue_pop(_this); }
static inline void          cqueue_swap(queue_t* _this, queue_t* other)     { i_queue_swap(_this, other); }
static inline queue_size_t  cqueue_clear(queue_t* _this)                    { return i_queue_clear(_this); }

/* Method 2 */
typedef struct class_queue {
    queue_size_t (*size)(const queue_t* _this);
    bool (*__empty)(const queue_t* _this);
    bool (*push)(queue_t* _this, queue_data_t data);
    queue_value_t (*__back)(const queue_t* _this);
    queue_value_t (*back)(const queue_t* _this);
    queue_value_t (*__front)(const queue_t* _this);
    queue_value_t (*front)(const queue_t* _this);
    void (*pop)(queue_t* _this);
    void (*swap)(queue_t* _this, queue_t* other);
    queue_size_t (*clear)(queue_t* _this);
} class_queue_t;

#define QUEUE_DEQUE_NEW_OPS_COMPLATE(_ops, _type_id, _type)  DEQUE_NEW_OPS_COMPLATE((_ops), (_type_id), _type)
#define QUEUE_LIST_NEW_OPS_COMPLATE(_ops, _type_id, _type)   LIST_NEW_OPS_COMPLATE((_ops), (_type_id), _type)
#define QUEUE_DEQUE_NEW_OPS_T(_ops, _type)                   DEQUE_NEW_OPS_T((_ops), _type)
#define QUEUE_LIST_NEW_OPS_T(_ops, _type)                    LIST_NEW_OPS_T((_ops), _type)
#define _QUEUE_NEW_OPS_COMPLATE(_ops, _type_id, _type, _BASE_NAME) \
        __queue_new((__CONTAINER_ADAPTOR_BASE_##_BASE_NAME), QUEUE_##_BASE_NAME##_NEW_OPS_COMPLATE((_ops), (_type_id), _type))
#define _QUEUE_NEW_OPS_T(_ops, _type, _BASE_NAME) \
        __queue_new((__CONTAINER_ADAPTOR_BASE_##_BASE_NAME), QUEUE_##_BASE_NAME##_NEW_OPS_T((_ops), _type))

queue_t* __queue_new(container_adaptor_base_t id, void* base);
void     __queue_delete(queue_t** _this);
const class_queue_t* class_queue_ins(void);
#define g_class_queue()                           class_queue_ins()
#define cqueue                                    g_class_queue()
#define g_class_queue_ops_string()                g_class_sequence_container_ops_string()
#define g_class_queue_ops_sso()                   g_class_sequence_container_ops_sso()
#define QUEUE_NEW_OPS_COMPLATE(_ops, _type_id, _type, _BASE_NAME) \
        _QUEUE_NEW_OPS_COMPLATE((_ops), (_type_id), _type, _BASE_NAME)
#define QUEUE_NEW_T(_type, _BASE_NAME)            _QUEUE_NEW_OPS_T(NULL,   _type, _BASE_NAME)
#define QUEUE_NEW_OPS_T(_ops, _type, _BASE_NAME)  _QUEUE_NEW_OPS_T((_ops), _type, _BASE_NAME)
#define QUEUE_NEW()                               _QUEUE_NEW_OPS_T(NULL,   queue_data_t, DEQUE)
#define QUEUE_NEW_OPS(_ops)                       _QUEUE_NEW_OPS_T((_ops), queue_data_t, DEQUE)
#define QUEUE_NEW_CHAR()                          _QUEUE_NEW_OPS_T(g_class_queue_ops_string(), queue_data_t, DEQUE)
#define QUEUE_NEW_SSO()                           _QUEUE_NEW_OPS_T(g_class_queue_ops_sso(),    ds_sso_t,     DEQUE)
#define QUEUE_NEW_STRING()                        QUEUE_NEW_SSO()
#define QUEUE_DELETE(_pptr)                       do { __queue_delete((_pptr)); } while (0)

#endif /* __J_QUEUE_H */
