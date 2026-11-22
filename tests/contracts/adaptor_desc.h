/*
  libj_ds permanent tests adaptor descriptor
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

#ifndef __J_ADAPTOR_DESC_H
#define __J_ADAPTOR_DESC_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum test_adaptor_kind {
    TEST_ADAPTOR_STACK,
    TEST_ADAPTOR_QUEUE,
    TEST_ADAPTOR_PRIORITY_QUEUE
} test_adaptor_kind_t;

/*
  Test-only capability bits.
*/
#define TEST_ADAPTOR_TOP_MAX (1u << 0) /* Priority queue returns the greatest value */

/*
  Each backend variant is a separate descriptor. `peek` is `top` for a stack
  or a priority queue, and `front` for a queue. `peek_back` is only provided
  by a queue descriptor.
*/
typedef struct test_adaptor_desc {
    const char* name;
    test_adaptor_kind_t kind;
    unsigned caps;

    void*  (*create)(void);
    void   (*destroy)(void* self);
    size_t (*size)(const void* self);
    bool   (*empty)(const void* self);
    bool   (*push)(void* self, int32_t value);
    void   (*pop)(void* self);
    bool   (*peek)(const void* self, int32_t* out);
    bool   (*peek_back)(const void* self, int32_t* out);
    size_t (*clear)(void* self);
    void   (*swap)(void* self, void* other);
} test_adaptor_desc_t;

void test_adaptor_contract_register(const test_adaptor_desc_t* desc);
void test_adaptor_model_register(const test_adaptor_desc_t* desc);
void test_adaptor_model_run(const test_adaptor_desc_t* desc, size_t iterations);

#ifdef __cplusplus
}
#endif

#endif /* __J_ADAPTOR_DESC_H */
