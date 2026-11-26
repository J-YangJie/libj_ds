/*
  libj_ds permanent tests priority queue descriptors
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

#include "desc.h"

#define PQ_DESC_DEFINE(_suffix, _ctor, _name, _caps)                           \
static void* pq_##_suffix##_create(void)                                       \
{                                                                              \
    return _ctor;                                                              \
}                                                                              \
static void pq_##_suffix##_destroy(void* self)                                 \
{                                                                              \
    priority_queue_t* pq = self;                                               \
                                                                               \
    PRIORITY_QUEUE_DELETE(&pq);                                                \
}                                                                              \
static size_t pq_##_suffix##_size(const void* self)                            \
{                                                                              \
    priority_queue_size_t size = cpriority_queue->size(self);                  \
                                                                               \
    return size < 0 ? 0 : (size_t)size;                                        \
}                                                                              \
static bool pq_##_suffix##_empty(const void* self)                             \
{                                                                              \
    return __cpqueue_empty(self);                                              \
}                                                                              \
static bool pq_##_suffix##_push(void* self, int32_t value)                     \
{                                                                              \
    return cpriority_queue->push(self, __ds_s32data(value));                   \
}                                                                              \
static void pq_##_suffix##_pop(void* self)                                     \
{                                                                              \
    cpriority_queue->pop(self);                                                \
}                                                                              \
static bool pq_##_suffix##_peek(const void* self, int32_t* out)                \
{                                                                              \
    priority_queue_value_t value = cpriority_queue->top(self);                 \
                                                                               \
    if (value.u8 == NULL)                                                      \
        return false;                                                          \
    *out = *value.s32;                                                         \
    return true;                                                               \
}                                                                              \
static size_t pq_##_suffix##_clear(void* self)                                 \
{                                                                              \
    priority_queue_size_t removed = cpriority_queue->clear(self);              \
                                                                               \
    return removed < 0 ? 0 : (size_t)removed;                                  \
}                                                                              \
static void pq_##_suffix##_swap(void* self, void* other)                       \
{                                                                              \
    cpriority_queue->swap(self, other);                                        \
}                                                                              \
static const test_adaptor_desc_t pq_adaptor_##_suffix = {                      \
    .name      = _name,                                                        \
    .kind      = TEST_ADAPTOR_PRIORITY_QUEUE,                                  \
    .caps      = _caps,                                                        \
    .create    = pq_##_suffix##_create,                                        \
    .destroy   = pq_##_suffix##_destroy,                                       \
    .size      = pq_##_suffix##_size,                                          \
    .empty     = pq_##_suffix##_empty,                                         \
    .push      = pq_##_suffix##_push,                                          \
    .pop       = pq_##_suffix##_pop,                                           \
    .peek      = pq_##_suffix##_peek,                                          \
    .peek_back = NULL,                                                         \
    .clear     = pq_##_suffix##_clear,                                         \
    .swap      = pq_##_suffix##_swap,                                          \
};

PQ_DESC_DEFINE(vector_max, PRIORITY_QUEUE_NEW_T(int32_t, VECTOR),
               "adaptor.pq.vector.max", TEST_ADAPTOR_TOP_MAX)
PQ_DESC_DEFINE(vector_min, PRIORITY_QUEUE_NEW_T_GT(int32_t, VECTOR),
               "adaptor.pq.vector.min", 0)
PQ_DESC_DEFINE(deque_max, PRIORITY_QUEUE_NEW_T(int32_t, DEQUE),
               "adaptor.pq.deque.max", TEST_ADAPTOR_TOP_MAX)
PQ_DESC_DEFINE(deque_min, PRIORITY_QUEUE_NEW_T_GT(int32_t, DEQUE),
               "adaptor.pq.deque.min", 0)

const test_adaptor_desc_t* priority_queue_desc_vector_max(void)
{
    return &pq_adaptor_vector_max;
}

const test_adaptor_desc_t* priority_queue_desc_vector_min(void)
{
    return &pq_adaptor_vector_min;
}

const test_adaptor_desc_t* priority_queue_desc_deque_max(void)
{
    return &pq_adaptor_deque_max;
}

const test_adaptor_desc_t* priority_queue_desc_deque_min(void)
{
    return &pq_adaptor_deque_min;
}
