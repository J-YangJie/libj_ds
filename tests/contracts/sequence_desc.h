/*
  libj_ds permanent tests sequence descriptor
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

#ifndef __J_SEQUENCE_DESC_H
#define __J_SEQUENCE_DESC_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define TEST_SEQ_PUSH_FRONT    (1u << 0)
#define TEST_SEQ_POP_FRONT     (1u << 1)
#define TEST_SEQ_RESERVE       (1u << 2)
#define TEST_SEQ_SHRINK        (1u << 3)
#define TEST_SEQ_ASSIGN_RANGE  (1u << 4)
#define TEST_SEQ_INSERT_RANGE  (1u << 5)
#define TEST_SEQ_REMOVE_IF     (1u << 6)
#define TEST_SEQ_SWAP          (1u << 7)

typedef bool (*test_seq_visit_fn)(void* ctx, int32_t value);
typedef bool (*test_seq_pred_fn)(int32_t value, void* ctx);

/*
  Indexes are used instead of iterators so generic code never depends on
  a container iterator layout. A callback for an absent capability is NULL.
*/
typedef struct test_seq_desc {
    const char* name;
    unsigned    caps;

    void*  (*create)(void);
    void   (*destroy)(void* self);
    size_t (*size)(const void* self);
    bool   (*get)(const void* self, size_t index, int32_t* out);
    bool   (*foreach)(const void* self, bool reverse,
                      test_seq_visit_fn fn, void* ctx);

    bool   (*push_back)(void* self, int32_t value);
    bool   (*push_front)(void* self, int32_t value);
    void   (*pop_back)(void* self);
    void   (*pop_front)(void* self);

    bool   (*insert)(void* self, size_t index, int32_t value,
                     size_t* out_index);
    bool   (*insert_n)(void* self, size_t index, size_t n, int32_t value,
                       size_t* out_index);
    bool   (*insert_range_values)(void* self, size_t index,
                                  const int32_t* values, size_t n,
                                  size_t* out_index);
    bool   (*erase)(void* self, size_t index, size_t* out_next);
    bool   (*erase_range)(void* self, size_t first, size_t last,
                          size_t* out_next);

    bool   (*find)(const void* self, int32_t value, size_t* out_index);
    size_t (*count)(const void* self, int32_t value);
    size_t (*remove)(void* self, int32_t value);
    size_t (*remove_if)(void* self, test_seq_pred_fn pred, void* ctx);

    bool   (*assign)(void* self, size_t n, int32_t value);
    bool   (*assign_range_values)(void* self, const int32_t* values, size_t n);
    bool   (*resize)(void* self, size_t n, int32_t value);
    bool   (*reserve)(void* self, size_t n);
    void   (*shrink_to_fit)(void* self);
    size_t (*clear)(void* self);
    void   (*swap)(void* self, void* other);
} test_seq_desc_t;

void test_sequence_contract_register(const test_seq_desc_t* desc);
void test_sequence_model_register(const test_seq_desc_t* desc);
void test_sequence_model_run(const test_seq_desc_t* desc, size_t iterations,
                             bool full_check_each_step);

#ifdef __cplusplus
}
#endif

#endif /* __J_SEQUENCE_DESC_H */
