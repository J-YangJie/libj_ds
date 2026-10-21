/*
  Priority Queue Implementations
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

#include <priority_queue/priority_queue.h>

#include <_log.h>
#include <_memory.h>
#include <linux/_compiler.h>

#define TAG "[priority_queue]"

priority_queue_t* __priority_queue_new(uint8_t id, void* pbase, const class_priority_queue_ops_t* ops)
{
    priority_queue_t* priority_queue;

    if (is_null(pbase))
        return NULL;

    priority_queue = (priority_queue_t*)p_calloc(1, sizeof(priority_queue_t));
    if (!is_null(priority_queue)) {
        priority_queue->p   = pbase;
        priority_queue->id  = id;
        priority_queue->ops = ops;
        if (g_class_priority_queue_ops_sso() == ops) {
            priority_queue->sso = true;
            switch (id) {
            case __SEQUENCE_CONTAINER_BASE_VECTOR: priority_queue->v->sso = true; break;
            case __SEQUENCE_CONTAINER_BASE_DEQUE:  priority_queue->d->sso = true; break;
            default: break;
            }
        }
        return priority_queue;
    }

    switch (id) {
    case __SEQUENCE_CONTAINER_BASE_VECTOR: VECTOR_DELETE((vector_t**)&pbase); break;
    case __SEQUENCE_CONTAINER_BASE_DEQUE:  DEQUE_DELETE((deque_t**)&pbase);   break;
    default: break;
    }
    return NULL;
}

void __priority_queue_delete(priority_queue_t** _this)
{
    if (unlikely(is_null(_this) || is_null(*_this)))
        return;

    switch ((*_this)->id) {
    case __SEQUENCE_CONTAINER_BASE_VECTOR: VECTOR_DELETE(&(*_this)->v); break;
    case __SEQUENCE_CONTAINER_BASE_DEQUE:  DEQUE_DELETE(&(*_this)->d);  break;
    default: break;
    }

    p_free(*_this);
}

const class_priority_queue_t* class_priority_queue_ins(void)
{
    static const class_priority_queue_t ins = {
        .size    = cpqueue_size,
        .__empty = __cpqueue_empty,
        .push    = cpqueue_push,
        .top     = cpqueue_top,
        .pop     = cpqueue_pop,
        .swap    = cpqueue_swap,
        .clear   = cpqueue_clear,
    };
    return &ins;
}
