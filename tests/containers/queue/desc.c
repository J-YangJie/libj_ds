/*
  libj_ds permanent tests queue descriptors
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

#include "desc.h"

#define QUEUE_DESC_DEFINE(_suffix, _base, _name)                               \
static void* queue_##_suffix##_create(void)                                    \
{                                                                              \
    return QUEUE_NEW_T(int32_t, _base);                                        \
}                                                                              \
static void queue_##_suffix##_destroy(void* self)                              \
{                                                                              \
    queue_t* queue = self;                                                     \
                                                                               \
    QUEUE_DELETE(&queue);                                                      \
}                                                                              \
static size_t queue_##_suffix##_size(const void* self)                         \
{                                                                              \
    queue_size_t size = cqueue->size(self);                                    \
                                                                               \
    return size < 0 ? 0 : (size_t)size;                                        \
}                                                                              \
static bool queue_##_suffix##_empty(const void* self)                          \
{                                                                              \
    return __cqueue_empty(self);                                               \
}                                                                              \
static bool queue_##_suffix##_push(void* self, int32_t value)                  \
{                                                                              \
    return cqueue->push(self, __ds_s32data(value));                            \
}                                                                              \
static void queue_##_suffix##_pop(void* self)                                  \
{                                                                              \
    cqueue->pop(self);                                                         \
}                                                                              \
static bool queue_##_suffix##_peek(const void* self, int32_t* out)             \
{                                                                              \
    queue_value_t value = cqueue->front(self);                                 \
                                                                               \
    if (value.u8 == NULL)                                                      \
        return false;                                                          \
    *out = *value.s32;                                                         \
    return true;                                                               \
}                                                                              \
static bool queue_##_suffix##_peek_back(const void* self, int32_t* out)        \
{                                                                              \
    queue_value_t value = cqueue->back(self);                                  \
                                                                               \
    if (value.u8 == NULL)                                                      \
        return false;                                                          \
    *out = *value.s32;                                                         \
    return true;                                                               \
}                                                                              \
static size_t queue_##_suffix##_clear(void* self)                              \
{                                                                              \
    queue_size_t removed = cqueue->clear(self);                                \
                                                                               \
    return removed < 0 ? 0 : (size_t)removed;                                  \
}                                                                              \
static void queue_##_suffix##_swap(void* self, void* other)                    \
{                                                                              \
    cqueue->swap(self, other);                                                 \
}                                                                              \
static const test_adaptor_desc_t queue_adaptor_##_suffix = {                   \
    .name      = _name,                                                        \
    .kind      = TEST_ADAPTOR_QUEUE,                                           \
    .caps      = 0,                                                            \
    .create    = queue_##_suffix##_create,                                     \
    .destroy   = queue_##_suffix##_destroy,                                    \
    .size      = queue_##_suffix##_size,                                       \
    .empty     = queue_##_suffix##_empty,                                      \
    .push      = queue_##_suffix##_push,                                       \
    .pop       = queue_##_suffix##_pop,                                        \
    .peek      = queue_##_suffix##_peek,                                       \
    .peek_back = queue_##_suffix##_peek_back,                                  \
    .clear     = queue_##_suffix##_clear,                                      \
    .swap      = queue_##_suffix##_swap,                                       \
};

QUEUE_DESC_DEFINE(deque, DEQUE, "adaptor.queue.deque")
QUEUE_DESC_DEFINE(list, LIST, "adaptor.queue.list")

const test_adaptor_desc_t* queue_desc_deque(void)
{
    return &queue_adaptor_deque;
}

const test_adaptor_desc_t* queue_desc_list(void)
{
    return &queue_adaptor_list;
}
