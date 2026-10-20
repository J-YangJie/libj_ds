/*
  Stack Implementations
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

#include <stack/stack.h>

#include <_log.h>
#include <_memory.h>
#include <linux/_compiler.h>

#define TAG "[stack]"

stack_t* __stack_new(uint8_t id, void* pbase)
{
    stack_t* stack;

    if (is_null(pbase))
        return NULL;

    stack = (stack_t*)p_calloc(1, sizeof(stack_t));
    if (!is_null(stack)) {
        stack->p  = pbase;
        stack->id = id;
        return stack;
    }

    switch (id) {
    case __SEQUENCE_CONTAINER_BASE_VECTOR: VECTOR_DELETE((vector_t**)&pbase); break;
    case __SEQUENCE_CONTAINER_BASE_DEQUE:  DEQUE_DELETE((deque_t**)&pbase);  break;
    case __SEQUENCE_CONTAINER_BASE_LIST:   LIST_DELETE((list_t**)&pbase);   break;
    default: break;
    }
    return NULL;
}

void __stack_delete(stack_t** _this)
{
    if (unlikely(is_null(_this) || is_null(*_this)))
        return;

    switch ((*_this)->id) {
    case __SEQUENCE_CONTAINER_BASE_VECTOR: VECTOR_DELETE(&(*_this)->v); break;
    case __SEQUENCE_CONTAINER_BASE_DEQUE:  DEQUE_DELETE(&(*_this)->d);  break;
    case __SEQUENCE_CONTAINER_BASE_LIST:   LIST_DELETE(&(*_this)->l);   break;
    default: break;
    }

    p_free(*_this);
}

const class_stack_t* class_stack_ins(void)
{
    static const class_stack_t ins = {
        .size    = cstack_size,
        .__empty = __cstack_empty,
        .top     = cstack_top,
        .push    = cstack_push,
        .pop     = cstack_pop,
        .swap    = cstack_swap,
        .clear   = cstack_clear,
    };
    return &ins;
}

