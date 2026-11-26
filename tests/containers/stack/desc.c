/*
  libj_ds permanent tests stack descriptors
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

#include "desc.h"

#define STACK_DESC_DEFINE(_suffix, _base, _name)                               \
static void* stack_##_suffix##_create(void)                                    \
{                                                                              \
    return STACK_NEW_T(int32_t, _base);                                        \
}                                                                              \
static void stack_##_suffix##_destroy(void* self)                              \
{                                                                              \
    stack_t* stack = self;                                                     \
                                                                               \
    STACK_DELETE(&stack);                                                      \
}                                                                              \
static size_t stack_##_suffix##_size(const void* self)                         \
{                                                                              \
    stack_size_t size = cstack->size(self);                                    \
                                                                               \
    return size < 0 ? 0 : (size_t)size;                                        \
}                                                                              \
static bool stack_##_suffix##_empty(const void* self)                          \
{                                                                              \
    return __cstack_empty(self);                                               \
}                                                                              \
static bool stack_##_suffix##_push(void* self, int32_t value)                  \
{                                                                              \
    return cstack->push(self, __ds_s32data(value));                            \
}                                                                              \
static void stack_##_suffix##_pop(void* self)                                  \
{                                                                              \
    cstack->pop(self);                                                         \
}                                                                              \
static bool stack_##_suffix##_peek(const void* self, int32_t* out)             \
{                                                                              \
    stack_value_t value = cstack->top(self);                                   \
                                                                               \
    if (value.u8 == NULL)                                                      \
        return false;                                                          \
    *out = *value.s32;                                                         \
    return true;                                                               \
}                                                                              \
static size_t stack_##_suffix##_clear(void* self)                              \
{                                                                              \
    stack_size_t removed = cstack->clear(self);                                \
                                                                               \
    return removed < 0 ? 0 : (size_t)removed;                                  \
}                                                                              \
static void stack_##_suffix##_swap(void* self, void* other)                    \
{                                                                              \
    cstack->swap(self, other);                                                 \
}                                                                              \
static const test_adaptor_desc_t stack_adaptor_##_suffix = {                   \
    .name      = _name,                                                        \
    .kind      = TEST_ADAPTOR_STACK,                                           \
    .caps      = 0,                                                            \
    .create    = stack_##_suffix##_create,                                     \
    .destroy   = stack_##_suffix##_destroy,                                    \
    .size      = stack_##_suffix##_size,                                       \
    .empty     = stack_##_suffix##_empty,                                      \
    .push      = stack_##_suffix##_push,                                       \
    .pop       = stack_##_suffix##_pop,                                        \
    .peek      = stack_##_suffix##_peek,                                       \
    .peek_back = NULL,                                                         \
    .clear     = stack_##_suffix##_clear,                                      \
    .swap      = stack_##_suffix##_swap,                                       \
};

STACK_DESC_DEFINE(vector, VECTOR, "adaptor.stack.vector")
STACK_DESC_DEFINE(deque, DEQUE, "adaptor.stack.deque")
STACK_DESC_DEFINE(list, LIST, "adaptor.stack.list")

const test_adaptor_desc_t* stack_desc_vector(void)
{
    return &stack_adaptor_vector;
}

const test_adaptor_desc_t* stack_desc_deque(void)
{
    return &stack_adaptor_deque;
}

const test_adaptor_desc_t* stack_desc_list(void)
{
    return &stack_adaptor_list;
}
