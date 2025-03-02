/*
  Priority Queue Interfaces
  Copyright (C) 2021  YangJie <yangjie98765@yeah.net>

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

#include <linux/_types.h>
#include <priority_queue/priority_queue_ops.h>

typedef struct priority_queue_node {
    priority_queue_data_t data;
} priority_queue_node_t;

typedef struct priority_queue {
    const class_priority_queue_ops_t* ops;
    priority_queue_node_t* head;
    priority_queue_size_t size;
    priority_queue_size_t capacity;
} priority_queue_t;

typedef struct class_priority_queue {
    priority_queue_size_t (*size)(const priority_queue_t* _this);
    bool                  (*push)(priority_queue_t* _this, priority_queue_data_t data);
    priority_queue_data_t (*top)(priority_queue_t* _this, priority_queue_data_t default_data);
    priority_queue_data_t (*pop)(priority_queue_t* _this, priority_queue_data_t default_data); /* `return ptr` free manually */
    priority_queue_size_t (*clear)(priority_queue_t* _this);
} class_priority_queue_t;

void __priority_queue_init(priority_queue_t* priority_queue);
void __priority_queue_deinit(priority_queue_t* priority_queue);
const class_priority_queue_t* class_priority_queue_ins(void);
#define g_class_priority_queue()            class_priority_queue_ins()
#define cpriority_queue                     g_class_priority_queue()
#define PRIORITY_QUEUE_INIT(_ptr)           (priority_queue_t) { .ops = NULL, .size = 0, }; __priority_queue_init((_ptr))
#define PRIORITY_QUEUE_INIT_OPS(_ptr, _ops) (priority_queue_t) { .ops = _ops, .size = 0, }; __priority_queue_init((_ptr))
#define PRIORITY_QUEUE_INIT_STRING(_ptr)    PRIORITY_QUEUE_INIT_OPS((_ptr), g_class_priority_queue_ops_string())
#define PRIORITY_QUEUE_DEINIT(_ptr)         do { __priority_queue_deinit((_ptr)); } while(0)

#endif /* __J_PRIORITY_QUEUE_H */
