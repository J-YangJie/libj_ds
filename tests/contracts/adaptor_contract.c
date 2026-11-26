/*
  libj_ds permanent tests adaptor contract
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

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

#include "adaptor_desc.h"
#include "../core/test.h"

static const test_adaptor_desc_t* contract_desc;

static void contract_destroy(void* self)
{
    contract_desc->destroy(self);
}

static void* contract_open(const test_adaptor_desc_t* desc)
{
    void* self;

    contract_desc = desc;
    self = desc->create();
    TEST_ASSERT_NOT_NULL(self);
    test_cleanup_push(contract_destroy, self);
    return self;
}

static void contract_append(const test_adaptor_desc_t* desc, void* self,
                            const int32_t* values, size_t n)
{
    size_t i;

    for (i = 0; i < n; ++i)
        TEST_ASSERT_TRUE(desc->push(self, values[i]));
    TEST_ASSERT_EQ(n, desc->size(self));
    TEST_ASSERT_EQ(n == 0, desc->empty(self));
}

/*
  Return the value that the next `pop` must remove. Stack and queue preserve
  the pushed order in a known direction; a priority queue is ordered by value.
*/
static int32_t contract_expect_at(const test_adaptor_desc_t* desc,
                                  const int32_t* pushed, size_t n, size_t at)
{
    int32_t ordered[32];
    size_t i;
    size_t j;

    if (desc->kind == TEST_ADAPTOR_STACK)
        return pushed[n - 1 - at];
    if (desc->kind == TEST_ADAPTOR_QUEUE)
        return pushed[at];

    TEST_ASSERT_TRUE(n <= sizeof(ordered) / sizeof(ordered[0]));
    for (i = 0; i < n; ++i)
        ordered[i] = pushed[i];
    for (i = 1; i < n; ++i) {
        int32_t key = ordered[i];

        j = i;
        while (j > 0 && ordered[j - 1] > key) {
            ordered[j] = ordered[j - 1];
            --j;
        }
        ordered[j] = key;
    }
    if ((desc->caps & TEST_ADAPTOR_TOP_MAX) != 0)
        return ordered[n - 1 - at];
    return ordered[at];
}

static void contract_drain(const test_adaptor_desc_t* desc, void* self,
                           const int32_t* pushed, size_t n)
{
    size_t i;

    TEST_ASSERT_EQ(n, desc->size(self));
    for (i = 0; i < n; ++i) {
        int32_t value = 0;

        TEST_ASSERT_TRUE(desc->peek(self, &value));
        TEST_ASSERT_EQ(contract_expect_at(desc, pushed, n, i), value);
        desc->pop(self);
        TEST_ASSERT_EQ(n - 1 - i, desc->size(self));
    }
    TEST_ASSERT_TRUE(desc->empty(self));
}

static void register_case(const test_adaptor_desc_t* desc, const char* suffix,
                          test_case_fn fn)
{
    char name[96];

    snprintf(name, sizeof(name), "%s.%s", desc->name, suffix);
    test_register_case("contract", name, fn, desc, __FILE__, __LINE__);
}

static void case_lifecycle(const void* ctx)
{
    const test_adaptor_desc_t* desc = ctx;
    int32_t value = 0;
    void* self = contract_open(desc);

    TEST_ASSERT_EQ(0, desc->size(self));
    TEST_ASSERT_TRUE(desc->empty(self));
    TEST_ASSERT_EQ(0, desc->clear(self));

    TEST_ASSERT_TRUE(desc->push(self, 5));
    TEST_ASSERT_EQ(1, desc->size(self));
    TEST_ASSERT_FALSE(desc->empty(self));
    TEST_ASSERT_TRUE(desc->peek(self, &value));
    TEST_ASSERT_EQ(5, value);

    TEST_ASSERT_EQ(1, desc->clear(self));
    TEST_ASSERT_EQ(0, desc->size(self));
    TEST_ASSERT_TRUE(desc->empty(self));
    TEST_ASSERT_EQ(0, desc->clear(self));

    TEST_ASSERT_TRUE(desc->push(self, 6));
    TEST_ASSERT_EQ(1, desc->size(self));
    TEST_ASSERT_TRUE(desc->peek(self, &value));
    TEST_ASSERT_EQ(6, value);
}

static void case_push_pop_order(const void* ctx)
{
    const test_adaptor_desc_t* desc = ctx;
    int32_t values[10];
    void* self = contract_open(desc);
    size_t i;

    for (i = 0; i < 10; ++i)
        values[i] = (int32_t)i;
    contract_append(desc, self, values, 10);
    contract_drain(desc, self, values, 10);

    TEST_ASSERT_EQ(0, desc->size(self));
    TEST_ASSERT_TRUE(desc->push(self, 42));
    TEST_ASSERT_EQ(1, desc->size(self));
}

static void case_clear_count(const void* ctx)
{
    const test_adaptor_desc_t* desc = ctx;
    int32_t values[6];
    void* self = contract_open(desc);
    size_t i;

    for (i = 0; i < 6; ++i)
        values[i] = (int32_t)i;
    contract_append(desc, self, values, 6);
    desc->pop(self);
    TEST_ASSERT_EQ(5, desc->size(self));
    TEST_ASSERT_EQ(5, desc->clear(self));
    TEST_ASSERT_EQ(0, desc->size(self));
    TEST_ASSERT_TRUE(desc->empty(self));

    TEST_ASSERT_TRUE(desc->push(self, 7));
    TEST_ASSERT_EQ(1, desc->size(self));
    TEST_ASSERT_EQ(1, desc->clear(self));
}

static void case_swap(const void* ctx)
{
    const test_adaptor_desc_t* desc = ctx;
    int32_t first_values[3] = { 1, 2, 3 };
    int32_t second_values[2] = { 4, 5 };
    void* first = contract_open(desc);
    void* second = contract_open(desc);

    contract_append(desc, first, first_values, 3);
    contract_append(desc, second, second_values, 2);

    desc->swap(first, second);
    contract_drain(desc, first, second_values, 2);
    contract_drain(desc, second, first_values, 3);

    desc->swap(first, second);
    TEST_ASSERT_EQ(0, desc->size(first));
    TEST_ASSERT_EQ(0, desc->size(second));
    TEST_ASSERT_TRUE(desc->push(first, 9));
    TEST_ASSERT_TRUE(desc->push(second, 8));
    TEST_ASSERT_EQ(1, desc->clear(first));
    TEST_ASSERT_EQ(1, desc->clear(second));
}

static void case_queue_back(const void* ctx)
{
    const test_adaptor_desc_t* desc = ctx;
    int32_t front = 0;
    int32_t back = 0;
    void* self = contract_open(desc);

    TEST_ASSERT_TRUE(desc->push(self, 10));
    TEST_ASSERT_TRUE(desc->push(self, 20));
    TEST_ASSERT_TRUE(desc->push(self, 30));

    TEST_ASSERT_TRUE(desc->peek(self, &front));
    TEST_ASSERT_TRUE(desc->peek_back(self, &back));
    TEST_ASSERT_EQ(10, front);
    TEST_ASSERT_EQ(30, back);

    desc->pop(self);
    TEST_ASSERT_TRUE(desc->peek(self, &front));
    TEST_ASSERT_TRUE(desc->peek_back(self, &back));
    TEST_ASSERT_EQ(20, front);
    TEST_ASSERT_EQ(30, back);
    TEST_ASSERT_EQ(2, desc->clear(self));
}

void test_adaptor_contract_register(const test_adaptor_desc_t* desc)
{
    register_case(desc, "lifecycle", case_lifecycle);
    register_case(desc, "push_pop_order", case_push_pop_order);
    register_case(desc, "clear_count", case_clear_count);
    register_case(desc, "swap", case_swap);

    if (desc->kind == TEST_ADAPTOR_QUEUE)
        register_case(desc, "queue_back", case_queue_back);
}
