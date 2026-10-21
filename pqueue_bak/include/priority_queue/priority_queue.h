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

#include <priority_queue/priority_queue_ops.h>

typedef struct priority_queue priority_queue_t;

/* Method 1 */
priority_queue_size_t cpqueue_size(const priority_queue_t* _this);
bool                  cpqueue_push(priority_queue_t* _this, priority_queue_data_t data);
priority_queue_data_t cpqueue_top(priority_queue_t* _this, priority_queue_data_t default_data);
priority_queue_data_t cpqueue_pop(priority_queue_t* _this, priority_queue_data_t default_data);
priority_queue_size_t cpqueue_clear(priority_queue_t* _this);

/* Method 2 */
typedef struct class_priority_queue {
    priority_queue_size_t (*size)(const priority_queue_t* _this);
    bool                  (*push)(priority_queue_t* _this, priority_queue_data_t data);
    priority_queue_data_t (*top)(priority_queue_t* _this, priority_queue_data_t default_data);
    priority_queue_data_t (*pop)(priority_queue_t* _this, priority_queue_data_t default_data); /* `return ptr` free manually */
    priority_queue_size_t (*clear)(priority_queue_t* _this);
} class_priority_queue_t;

priority_queue_t* __priority_queue_new(const class_priority_queue_ops_t* ops);
void              __priority_queue_delete(priority_queue_t** _this);
const class_priority_queue_t* class_priority_queue_ins(void);
#define g_class_priority_queue()      class_priority_queue_ins()
#define cpriority_queue               g_class_priority_queue()
#define PRIORITY_QUEUE_NEW()          __priority_queue_new(NULL)
#define PRIORITY_QUEUE_NEW_OPS(_ops)  __priority_queue_new((_ops))
#define PRIORITY_QUEUE_NEW_STRING()   __priority_queue_new(g_class_priority_queue_ops_string())
#define PRIORITY_QUEUE_DELETE(_pptr)  do { __priority_queue_delete((_pptr)); } while(0)

#endif /* __J_PRIORITY_QUEUE_H */
