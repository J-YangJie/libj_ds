/*
  Queue Implementations
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

#include <queue/queue.h>

#include <_log.h>
#include <_memory.h>
#include <linux/_compiler.h>

#define TAG "[queue]"

queue_t* __queue_new(container_adaptor_base_t id, void* pbase)
{
    queue_t* queue;

    if (is_null(pbase)) {
        JDSC_ASSERT_NEW(0, "%s: The `pbase` must not be NULL!", __func__);
        return NULL;
    }

    switch (id) {
    case __CONTAINER_ADAPTOR_BASE_DEQUE:
    case __CONTAINER_ADAPTOR_BASE_LIST:
        break;
    default:
        JDSC_ASSERT_NEW(0, "%s: Unsupported container base id %u (expected DEQUE or LIST)!",
                        __func__, (unsigned)id);
        goto free_pbase;
    }

    queue = (queue_t*)p_calloc(1, sizeof(queue_t));
    if (is_null(queue)) {
        JDSC_ASSERT_NEW(0, "%s: Failed to allocate %zu bytes for queue!",
                        __func__, sizeof(queue_t));
        goto free_pbase;
    }

    queue->p  = pbase;
    queue->id = id;
    return queue;

free_pbase:
    switch (id) {
    case __CONTAINER_ADAPTOR_BASE_DEQUE: DEQUE_DELETE((deque_t**)&pbase); break;
    case __CONTAINER_ADAPTOR_BASE_LIST:  LIST_DELETE((list_t**)&pbase);   break;
    default: break;
    }
    return NULL;
}

void __queue_delete(queue_t** _this)
{
    if (unlikely(is_null(_this) || is_null(*_this)))
        return;

    switch ((*_this)->id) {
    case __CONTAINER_ADAPTOR_BASE_DEQUE: DEQUE_DELETE(&(*_this)->d); break;
    case __CONTAINER_ADAPTOR_BASE_LIST:  LIST_DELETE(&(*_this)->l);  break;
    default: break;
    }

    p_free(*_this);
}

const class_queue_t* class_queue_ins(void)
{
    static const class_queue_t ins = {
        .size    = cqueue_size,
        .__empty = __cqueue_empty,
        .front   = cqueue_front,
        .back    = cqueue_back,
        .push    = cqueue_push,
        .pop     = cqueue_pop,
        .swap    = cqueue_swap,
        .clear   = cqueue_clear,
    };
    return &ins;
}
