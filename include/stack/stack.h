/*
  Stack Interfaces
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

#ifndef __J_STACK_H
#define __J_STACK_H

#include <container_adaptor/container_adaptor.h>

typedef container_adaptor_value_t     stack_value_t;
typedef class_container_adaptor_ops_t class_stack_ops_t;

#include <stack/i_stack.h>

typedef i_stack_t stack_t;

/* Method 1 */
HEADER_STATIC_INLINE stack_size_t  cstack_size(const stack_t* _this)              { return i_stack_size(_this); }
HEADER_STATIC_INLINE bool          __cstack_empty(const stack_t* _this)           { return __i_stack_empty(_this); }
HEADER_STATIC_INLINE bool          cstack_push(stack_t* _this, stack_data_t data) { return i_stack_push(_this, data); }
HEADER_STATIC_INLINE stack_value_t __cstack_top(const stack_t* _this)             { return __i_stack_top(_this); }
HEADER_STATIC_INLINE stack_value_t cstack_top(const stack_t* _this)               { return i_stack_top(_this); }
HEADER_STATIC_INLINE void          cstack_pop(stack_t* _this)                     { i_stack_pop(_this); }
HEADER_STATIC_INLINE void          cstack_swap(stack_t* _this, stack_t* other)    { i_stack_swap(_this, other); }
HEADER_STATIC_INLINE stack_size_t  cstack_clear(stack_t* _this)                   { return i_stack_clear(_this); }

/* Method 2 */
typedef struct class_stack {
    stack_size_t (*size)(const stack_t* _this);
    bool (*__empty)(const stack_t* _this);
    bool (*push)(stack_t* _this, stack_data_t data);
    stack_value_t (*__top)(const stack_t* _this);
    stack_value_t (*top)(const stack_t* _this);
    void (*pop)(stack_t* _this);
    void (*swap)(stack_t* _this, stack_t* other);
    stack_size_t (*clear)(stack_t* _this);
} class_stack_t;

#define STACK_VECTOR_NEW_COMPLATE(_ops, _type_id, _type, _capacity_init) \
              VECTOR_NEW_COMPLATE((_ops), (_type_id), _type, (_capacity_init))
#define STACK_DEQUE_NEW_COMPLATE(_ops, _type_id, _type, _capacity_init) \
              DEQUE_NEW_COMPLATE((_ops), (_type_id), _type)
#define STACK_LIST_NEW_COMPLATE(_ops, _type_id, _type, _capacity_init) \
              LIST_NEW_COMPLATE((_ops), (_type_id), _type)
#define STACK_VECTOR_NEW_OPS_T(_ops, _type, _capacity_init) \
              VECTOR_NEW_OPS_T((_ops), _type)
#define STACK_DEQUE_NEW_OPS_T(_ops, _type, _capacity_init) \
              DEQUE_NEW_OPS_T((_ops), _type)
#define STACK_LIST_NEW_OPS_T(_ops, _type, _capacity_init) \
              LIST_NEW_OPS_T((_ops), _type)
#define _STACK_NEW_OPS_COMPLATE(_ops, _type_id, _type, _BASE_NAME, _capacity_init) \
        __stack_new((__CONTAINER_ADAPTOR_BASE_##_BASE_NAME), STACK_##_BASE_NAME##_NEW_COMPLATE((_ops), (_type_id), _type, (_capacity_init)))
#define _STACK_NEW_OPS_T(_ops, _type, _BASE_NAME, _capacity_init) \
        __stack_new((__CONTAINER_ADAPTOR_BASE_##_BASE_NAME), STACK_##_BASE_NAME##_NEW_OPS_T((_ops), _type, (_capacity_init)))
#define _STACK_NEW_T(_type, _BASE_NAME) \
        __stack_new((__CONTAINER_ADAPTOR_BASE_##_BASE_NAME), _BASE_NAME##_NEW_T(_type))

stack_t* __stack_new(container_adaptor_base_t id, void* base);
void     __stack_delete(stack_t** _this);
const class_stack_t* class_stack_ins(void);
#define g_class_stack()                           class_stack_ins()
#define cstack                                    g_class_stack()
#define g_class_stack_ops_chars()                 g_class_container_adaptor_ops_chars()
#define g_class_stack_ops_sso()                   g_class_container_adaptor_ops_sso()
#define STACK_NEW_OPS_COMPLATE(_ops, _type_id, _type, _BASE_NAME, _capacity_init) \
        _STACK_NEW_OPS_COMPLATE((_ops), (_type_id), _type, _BASE_NAME, (_capacity_init))
#define STACK_NEW_OPS_T(_ops, _type, _BASE_NAME)  _STACK_NEW_OPS_T((_ops), _type, _BASE_NAME, 0)
#define STACK_NEW()                               STACK_NEW_OPS_T(NULL,   intptr_t, VECTOR)
#define STACK_NEW_OPS(_ops)                       STACK_NEW_OPS_T((_ops), intptr_t, VECTOR)
#define STACK_DELETE(_pptr)                       do { __stack_delete((_pptr)); } while (0)

/*
    _type includes:
    1. uint8_t/uint16_t/uint32_t/uint64_t
    2. int8_t/int16_t/int32_t/int64_t
    3. uintptr_t/intptr_t/size_t/ssize_t
    4. float/double/ds_sso_t(string optimization)

    _BASE_NAME includes:
    1. VECTOR
    2. DEQUE
    3. LIST

    STACK_NEW_T(_type, _BASE_NAME) like std::stack<_type>
    Such as: stack_t* demo = STACK_NEW_T(ds_sso_t, VECTOR);
    Such as: stack_t* demo = STACK_NEW_T(int16_t, DEQUE);
    Such as: stack_t* demo = STACK_NEW_T(float, LIST);
 */
#define STACK_NEW_T(_type, _BASE_NAME)            _STACK_NEW_T(_type, _BASE_NAME)

/* About string */
#define STACK_NEW_CHAR_T(_BASE_NAME)              STACK_NEW_OPS_T(g_class_stack_ops_chars(), intptr_t, _BASE_NAME)
#define STACK_NEW_SSO_T(_BASE_NAME)               STACK_NEW_T(ds_sso_t, _BASE_NAME)
#define STACK_NEW_STRING_T(_BASE_NAME)            STACK_NEW_SSO_T(_BASE_NAME)
#define STACK_NEW_CHAR()                          STACK_NEW_OPS_T(g_class_stack_ops_chars(), intptr_t, VECTOR)
#define STACK_NEW_SSO()                           STACK_NEW_T(ds_sso_t, VECTOR)
#define STACK_NEW_STRING()                        STACK_NEW_SSO()

#endif /* __J_STACK_H */
