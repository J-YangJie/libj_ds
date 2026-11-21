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

#include "test_adapter_queue.h"

#include <queue/queue.h>

#define cds cqueue

static ds_size_t size(void* _this)
{
    queue_t* this = (queue_t*)_this;
    return cds->size(this);
}

static bool empty(void* _this)
{
    queue_t* this = (queue_t*)_this;
    return cds->__empty(this);
}

static bool push(void* _this, ds_data_t data)
{
    queue_t* this = (queue_t*)_this;
    return cds->push(this, data);
}

static ds_data_t front(void* _this, ds_data_t default_data)
{
    queue_t* this = (queue_t*)_this;
    queue_value_t value = cds->front(this);
    return value.d ? *(ds_data_t*)value.d : default_data;
}

static ds_data_t back(void* _this, ds_data_t default_data)
{
    queue_t* this = (queue_t*)_this;
    queue_value_t value = cds->back(this);
    return value.d ? *(ds_data_t*)value.d : default_data;
}

static void pop(void* _this)
{
    queue_t* this = (queue_t*)_this;
    cds->pop(this);
}

static void swap(void* _this, void* other)
{
    queue_t* this = (queue_t*)_this;
    cds->swap(this, (queue_t*)other);
}

static ds_size_t clear(void* _this)
{
    queue_t* this = (queue_t*)_this;
    return cds->clear(this);
}

const class_stress_adapter_queue_interface_t* class_stress_adapter_queue_ins(void)
{
    static class_stress_adapter_queue_interface_t ins = {
        .size  = size,
        .empty = empty,
        .push  = push,
        .front = front,
        .back  = back,
        .pop   = pop,
        .swap  = swap,
        .clear = clear,
    };
    return &ins;
}
