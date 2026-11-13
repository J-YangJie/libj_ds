/*
  Priority Queue Interfaces
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

#ifndef __J_PRIORITY_QUEUE_H
#define __J_PRIORITY_QUEUE_H

#include <container_adaptor/container_adaptor.h>
#include <priority_queue/priority_queue_ops.h>

typedef container_adaptor_value_t priority_queue_value_t;

#include <priority_queue/i_priority_queue.h>

typedef i_priority_queue_t priority_queue_t;

/* Method 1 */
HEADER_STATIC_INLINE priority_queue_size_t  cpqueue_size(const priority_queue_t* _this)                          { return i_priority_queue_size(_this); }
HEADER_STATIC_INLINE bool                   __cpqueue_empty(const priority_queue_t* _this)                       { return __i_priority_queue_empty(_this); }
HEADER_STATIC_INLINE bool                   cpqueue_push(priority_queue_t* _this, priority_queue_data_t data)    { return i_priority_queue_push(_this, data); }
HEADER_STATIC_INLINE priority_queue_value_t __cpqueue_top(const priority_queue_t* _this)                         { return __i_priority_queue_top(_this); }
HEADER_STATIC_INLINE priority_queue_value_t cpqueue_top(const priority_queue_t* _this)                           { return i_priority_queue_top(_this); }
HEADER_STATIC_INLINE void                   cpqueue_pop(priority_queue_t* _this)                                 { i_priority_queue_pop(_this); }
HEADER_STATIC_INLINE void                   cpqueue_swap(priority_queue_t* _this, priority_queue_t* other)       { i_priority_queue_swap(_this, other); }
HEADER_STATIC_INLINE priority_queue_size_t  cpqueue_clear(priority_queue_t* _this)                               { return i_priority_queue_clear(_this); }

/* Method 2 */
typedef struct class_priority_queue {
    priority_queue_size_t (*size)(const priority_queue_t* _this);
    bool (*__empty)(const priority_queue_t* _this);
    bool (*push)(priority_queue_t* _this, priority_queue_data_t data);
    priority_queue_value_t (*__top)(const priority_queue_t* _this);
    priority_queue_value_t (*top)(const priority_queue_t* _this);
    void (*pop)(priority_queue_t* _this);
    void (*swap)(priority_queue_t* _this, priority_queue_t* other);
    priority_queue_size_t (*clear)(priority_queue_t* _this);
} class_priority_queue_t;

#define PRIORITY_QUEUE_VECTOR_NEW_COMPLATE(_ops, _type_id, _type, _capacity_init) \
                       VECTOR_NEW_COMPLATE((const class_vector_ops_t*)(_ops), (_type_id), _type, (_capacity_init))
#define PRIORITY_QUEUE_DEQUE_NEW_COMPLATE(_ops, _type_id, _type, _capacity_init) \
                       DEQUE_NEW_COMPLATE((const class_deque_ops_t*)(_ops), (_type_id), _type)
#define PRIORITY_QUEUE_VECTOR_NEW_OPS_T(_ops, _type, _capacity_init) \
                       VECTOR_NEW_OPS_T((const class_vector_ops_t*)(_ops), _type)
#define PRIORITY_QUEUE_DEQUE_NEW_OPS_T(_ops, _type, _capacity_init) \
                       DEQUE_NEW_OPS_T((const class_deque_ops_t*)(_ops), _type)
#define _PRIORITY_QUEUE_NEW_OPS_COMPLATE(_ops, _type_id, _type, _BASE_NAME, _capacity_init) \
        __priority_queue_new((__CONTAINER_ADAPTOR_BASE_##_BASE_NAME), PRIORITY_QUEUE_##_BASE_NAME##_NEW_COMPLATE((_ops), (_type_id), _type, (_capacity_init)), (_ops))
#define _PRIORITY_QUEUE_NEW_OPS_T(_ops, _type, _BASE_NAME, _capacity_init) \
        __priority_queue_new((__CONTAINER_ADAPTOR_BASE_##_BASE_NAME), PRIORITY_QUEUE_##_BASE_NAME##_NEW_OPS_T((_ops), _type, (_capacity_init)), (_ops))
#define _PRIORITY_QUEUE_NEW_T(_ops, _type, _BASE_NAME) \
        __priority_queue_new((__CONTAINER_ADAPTOR_BASE_##_BASE_NAME), _BASE_NAME##_NEW_T(_type), (_ops))

priority_queue_t* __priority_queue_new(container_adaptor_base_t id, void* base, const class_priority_queue_ops_t* ops);
void              __priority_queue_delete(priority_queue_t** _this);
const class_priority_queue_t* class_priority_queue_ins(void);
#define g_class_priority_queue()                           class_priority_queue_ins()
#define cpriority_queue                                    g_class_priority_queue()
#define PRIORITY_QUEUE_NEW_OPS_COMPLATE(_ops, _type_id, _type, _BASE_NAME, _capacity_init) \
        _PRIORITY_QUEUE_NEW_OPS_COMPLATE((_ops), (_type_id), _type, _BASE_NAME, (_capacity_init))
#define PRIORITY_QUEUE_NEW_OPS_T(_ops, _type, _BASE_NAME)  _PRIORITY_QUEUE_NEW_OPS_T((_ops), _type, _BASE_NAME, 0)
#define PRIORITY_QUEUE_NEW()                               PRIORITY_QUEUE_NEW_OPS_T(NULL,   intptr_t, VECTOR)
#define PRIORITY_QUEUE_NEW_OPS(_ops)                       PRIORITY_QUEUE_NEW_OPS_T((_ops), intptr_t, VECTOR)
#define PRIORITY_QUEUE_DELETE(_pptr)                       do { __priority_queue_delete((_pptr)); } while (0)

/*
    The default comparator is `<`, so `top()` returns the greatest element.

    _type includes:
    1. uint8_t/uint16_t/uint32_t/uint64_t
    2. int8_t/int16_t/int32_t/int64_t
    3. uintptr_t/intptr_t/size_t/ssize_t
    4. float/double/ds_sso_t(string optimization)

    _BASE_NAME includes:
    1. VECTOR
    2. DEQUE

    PRIORITY_QUEUE_NEW_T(_type, _BASE_NAME) like std::priority_queue<_type>
    Such as: priority_queue_t* demo = PRIORITY_QUEUE_NEW_T(ds_sso_t, VECTOR);
    Such as: priority_queue_t* demo = PRIORITY_QUEUE_NEW_T(int16_t, DEQUE);
    Such as: priority_queue_t* demo = PRIORITY_QUEUE_NEW_T(float, VECTOR);
 */
#define PRIORITY_QUEUE_NEW_T(_type, _BASE_NAME)            _PRIORITY_QUEUE_NEW_T(g_class_priority_queue_ops_##_type(), _type, _BASE_NAME)

/* About string */
#define PRIORITY_QUEUE_NEW_CHAR_T(_BASE_NAME)              PRIORITY_QUEUE_NEW_OPS_T(g_class_priority_queue_ops_chars(), intptr_t, _BASE_NAME)
#define PRIORITY_QUEUE_NEW_SSO_T(_BASE_NAME)               PRIORITY_QUEUE_NEW_T(ds_sso_t, _BASE_NAME)
#define PRIORITY_QUEUE_NEW_STRING_T(_BASE_NAME)            PRIORITY_QUEUE_NEW_SSO_T(_BASE_NAME)
#define PRIORITY_QUEUE_NEW_CHAR()                          PRIORITY_QUEUE_NEW_OPS_T(g_class_priority_queue_ops_chars(), intptr_t, VECTOR)
#define PRIORITY_QUEUE_NEW_SSO()                           PRIORITY_QUEUE_NEW_T(ds_sso_t, VECTOR)
#define PRIORITY_QUEUE_NEW_STRING()                        PRIORITY_QUEUE_NEW_SSO()


/*
    `_GT` uses `>` as the comparator, so `top()` returns the smallest element.

    _type includes:
    1. uint8_t/uint16_t/uint32_t/uint64_t
    2. int8_t/int16_t/int32_t/int64_t
    3. uintptr_t/intptr_t/size_t/ssize_t
    4. float/double/ds_sso_t(string optimization)

    _BASE_NAME includes:
    1. VECTOR
    2. DEQUE

    PRIORITY_QUEUE_NEW_T_GT(_type, _BASE_NAME) like std::priority_queue<_type, Container, std::greater<_type>>
    Such as: priority_queue_t* demo = PRIORITY_QUEUE_NEW_T_GT(ds_sso_t, VECTOR);
    Such as: priority_queue_t* demo = PRIORITY_QUEUE_NEW_T_GT(int16_t, DEQUE);
    Such as: priority_queue_t* demo = PRIORITY_QUEUE_NEW_T_GT(float, VECTOR);
 */
#define PRIORITY_QUEUE_NEW_T_GT(_type, _BASE_NAME)         _PRIORITY_QUEUE_NEW_T(g_class_priority_queue_ops_##_type##_greater(), _type, _BASE_NAME)

/* About string */
#define PRIORITY_QUEUE_NEW_CHAR_T_GT(_BASE_NAME)           PRIORITY_QUEUE_NEW_OPS_T(g_class_priority_queue_ops_chars_greater(), intptr_t, _BASE_NAME)
#define PRIORITY_QUEUE_NEW_SSO_T_GT(_BASE_NAME)            PRIORITY_QUEUE_NEW_T_GT(ds_sso_t, _BASE_NAME)
#define PRIORITY_QUEUE_NEW_STRING_T_GT(_BASE_NAME)         PRIORITY_QUEUE_NEW_SSO_T_GT(_BASE_NAME)
#define PRIORITY_QUEUE_NEW_CHAR_GT()                       PRIORITY_QUEUE_NEW_OPS_T(g_class_priority_queue_ops_chars_greater(), intptr_t, VECTOR)
#define PRIORITY_QUEUE_NEW_SSO_GT()                        PRIORITY_QUEUE_NEW_T_GT(ds_sso_t, VECTOR)
#define PRIORITY_QUEUE_NEW_STRING_GT()                     PRIORITY_QUEUE_NEW_SSO_GT()

#endif /* __J_PRIORITY_QUEUE_H */
