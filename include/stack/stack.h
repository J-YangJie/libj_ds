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

typedef sequence_container_value_t     stack_value_t;
typedef class_sequence_container_ops_t class_stack_ops_t;

#include <stack/i_stack.h>

typedef i_stack_t stack_t;

/* Method 1 */
static inline stack_size_t  cstack_size(const stack_t* _this)              { return i_stack_size(_this); }
static inline bool          __cstack_empty(const stack_t* _this)           { return __i_stack_empty(_this); }
static inline bool          cstack_push(stack_t* _this, stack_data_t data) { return i_stack_push(_this, data); }
static inline stack_value_t __cstack_top(const stack_t* _this)             { return __i_stack_top(_this); }
static inline stack_value_t cstack_top(const stack_t* _this)               { return i_stack_top(_this); }
static inline void          cstack_pop(stack_t* _this)                     { i_stack_pop(_this); }
static inline void          cstack_swap(stack_t* _this, stack_t* other)    { i_stack_swap(_this, other); }
static inline stack_size_t  cstack_clear(stack_t* _this)                   { return i_stack_clear(_this); }

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

#define STACK_VECTOR_NEW_OPS_COMPLATE(_ops, _type_id, _type, _capacity_init)  VECTOR_NEW_OPS_COMPLATE((_ops), (_type_id), _type, (_capacity_init))
#define STACK_DEQUE_NEW_OPS_COMPLATE(_ops, _type_id, _type, _capacity_init)   DEQUE_NEW_OPS_COMPLATE((_ops), (_type_id), _type)
#define STACK_LIST_NEW_OPS_COMPLATE(_ops, _type_id, _type, _capacity_init)    LIST_NEW_OPS_COMPLATE((_ops), (_type_id), _type)
#define STACK_VECTOR_NEW_OPS_T(_ops, _type, _capacity_init)                   VECTOR_NEW_OPS_T((_ops), _type)
#define STACK_DEQUE_NEW_OPS_T(_ops, _type, _capacity_init)                    DEQUE_NEW_OPS_T((_ops), _type)
#define STACK_LIST_NEW_OPS_T(_ops, _type, _capacity_init)                     LIST_NEW_OPS_T((_ops), _type)
#define _STACK_NEW_OPS_COMPLATE(_ops, _type_id, _type, _BASE_NAME, _capacity_init) \
        __stack_new((__CONTAINER_ADAPTOR_BASE_##_BASE_NAME), STACK_##_BASE_NAME##_NEW_OPS_COMPLATE((_ops), (_type_id), _type, (_capacity_init)))
#define _STACK_NEW_OPS_T(_ops, _type, _BASE_NAME, _capacity_init) \
        __stack_new((__CONTAINER_ADAPTOR_BASE_##_BASE_NAME), STACK_##_BASE_NAME##_NEW_OPS_T((_ops), _type, (_capacity_init)))

stack_t* __stack_new(container_adaptor_base_t id, void* base);
void     __stack_delete(stack_t** _this);
const class_stack_t* class_stack_ins(void);
#define g_class_stack()                           class_stack_ins()
#define cstack                                    g_class_stack()
#define g_class_stack_ops_string()                g_class_sequence_container_ops_string()
#define g_class_stack_ops_sso()                   g_class_sequence_container_ops_sso()
#define STACK_NEW_OPS_COMPLATE(_ops, _type_id, _type, _BASE_NAME, _capacity_init) \
        _STACK_NEW_OPS_COMPLATE((_ops), (_type_id), _type, _BASE_NAME, (_capacity_init))
#define STACK_NEW_T(_type, _BASE_NAME)            _STACK_NEW_OPS_T(NULL,   _type, _BASE_NAME, 0)
#define STACK_NEW_OPS_T(_ops, _type, _BASE_NAME)  _STACK_NEW_OPS_T((_ops), _type, _BASE_NAME, 0)
#define STACK_NEW()                               _STACK_NEW_OPS_T(NULL,   stack_data_t, VECTOR, 0)
#define STACK_NEW_OPS(_ops)                       _STACK_NEW_OPS_T((_ops), stack_data_t, VECTOR, 0)
#define STACK_NEW_CHAR()                          _STACK_NEW_OPS_T(g_class_stack_ops_string(), stack_data_t, VECTOR, 0)
#define STACK_NEW_SSO()                           _STACK_NEW_OPS_T(g_class_stack_ops_sso(),    ds_sso_t,     VECTOR, 0)
#define STACK_NEW_STRING()                        STACK_NEW_SSO()
#define STACK_DELETE(_pptr)                       do { __stack_delete((_pptr)); } while (0)

#endif /* __J_STACK_H */
