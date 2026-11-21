/*
  Sorting Algorithms Implementations
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

#include "test_adapter_stack.h"

#include <stack/stack.h>

#define cds cstack

static ds_size_t size(void* _this)
{
    stack_t* this = (stack_t*)_this;
    return cds->size(this);
}

static bool empty(void* _this)
{
    stack_t* this = (stack_t*)_this;
    return cds->__empty(this);
}

static bool push(void* _this, ds_data_t data)
{
    stack_t* this = (stack_t*)_this;
    return cds->push(this, data);
}

static ds_data_t top(void* _this, ds_data_t default_data)
{
    stack_t* this = (stack_t*)_this;
    stack_value_t value = cds->top(this);
    return value.d ? *(ds_data_t*)value.d : default_data;
}

static void pop(void* _this)
{
    stack_t* this = (stack_t*)_this;
    cds->pop(this);
}

static void swap(void* _this, void* other)
{
    stack_t* this = (stack_t*)_this;
    cds->swap(this, (stack_t*)other);
}

static ds_size_t clear(void* _this)
{
    stack_t* this = (stack_t*)_this;
    return cds->clear(this);
}

const class_stress_adapter_stack_interface_t* class_stress_adapter_stack_ins(void)
{
    static class_stress_adapter_stack_interface_t ins = {
        .size  = size,
        .empty = empty,
        .push  = push,
        .top   = top,
        .pop   = pop,
        .swap  = swap,
        .clear = clear,
    };
    return &ins;
}
