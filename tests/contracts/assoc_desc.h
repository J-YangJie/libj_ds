/*
  libj_ds permanent tests associative descriptor
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

#ifndef __J_ASSOC_DESC_H
#define __J_ASSOC_DESC_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define TEST_ASSOC_ORDERED        (1u << 0)
#define TEST_ASSOC_MULTI          (1u << 1)
#define TEST_ASSOC_HAS_VALUE      (1u << 2)
#define TEST_ASSOC_INSERT_REPLACE (1u << 3)
#define TEST_ASSOC_BOUNDS         (1u << 4)
#define TEST_ASSOC_HASHED         (1u << 5)

typedef bool (*test_assoc_visit_fn)(void* ctx, int32_t key, int32_t value);

typedef struct test_assoc_desc {
    const char* name;
    unsigned    caps;
    void* (*create)(void);
    void (*destroy)(void* self);
    size_t (*size)(const void* self);
    size_t (*count)(const void* self, int32_t key);
    bool (*insert)(void* self, int32_t key, int32_t value, bool* out_inserted);
    bool (*insert_replace)(void* self, int32_t key, int32_t value);
    bool (*erase_key)(void* self, int32_t key, int32_t* out_next_key,
                      bool* out_next_found);
    size_t (*remove)(void* self, int32_t key);
    bool (*find)(const void* self, int32_t key, int32_t* out_value);
    bool (*lower_bound)(const void* self, int32_t key, int32_t* out_key,
                        int32_t* out_value);
    bool (*upper_bound)(const void* self, int32_t key, int32_t* out_key,
                        int32_t* out_value);
    bool (*foreach)(const void* self, bool reverse,
                    test_assoc_visit_fn fn, void* ctx);
    size_t (*clear)(void* self);
    void (*swap)(void* self, void* other);
} test_assoc_desc_t;

void test_assoc_contract_register(const test_assoc_desc_t* desc);
void test_assoc_model_register(const test_assoc_desc_t* desc);
void test_assoc_model_run(const test_assoc_desc_t* desc);

#ifdef __cplusplus
}
#endif

#endif /* __J_ASSOC_DESC_H */
