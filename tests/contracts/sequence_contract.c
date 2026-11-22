/*
  libj_ds permanent tests sequence contract
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

#include "sequence_desc.h"
#include "../core/test.h"

static const test_seq_desc_t* contract_desc;

static void contract_destroy(void* self)
{
    contract_desc->destroy(self);
}

static void* contract_open(const test_seq_desc_t* desc)
{
    void* self;

    contract_desc = desc;
    self = desc->create();
    TEST_ASSERT_NOT_NULL(self);
    test_cleanup_push(contract_destroy, self);
    return self;
}

static void contract_append(const test_seq_desc_t* desc, void* self,
                            int32_t first, size_t n)
{
    size_t i;

    for (i = 0; i < n; ++i)
        TEST_ASSERT_TRUE(desc->push_back(self, first + (int32_t)i));
}

typedef struct visit_check {
    const int32_t* want;
    size_t         n;
    size_t         at;
    bool           ok;
} visit_check_t;

static bool visit_forward(void* opaque, int32_t value)
{
    visit_check_t* check = opaque;

    if (check->at >= check->n || check->want[check->at] != value) {
        check->ok = false;
        return false;
    }
    ++check->at;
    return true;
}

static bool visit_reverse(void* opaque, int32_t value)
{
    visit_check_t* check = opaque;

    if (check->at >= check->n || check->want[check->n - 1 - check->at] != value) {
        check->ok = false;
        return false;
    }
    ++check->at;
    return true;
}

static void contract_check(const test_seq_desc_t* desc, const void* self,
                           const int32_t* want, size_t n)
{
    visit_check_t forward = { want, n, 0, true };
    visit_check_t reverse = { want, n, 0, true };
    bool completed;
    size_t i;

    TEST_ASSERT_EQ(n, desc->size(self));
    for (i = 0; i < n; ++i) {
        int32_t value = 0;

        TEST_ASSERT_TRUE(desc->get(self, i, &value));
        TEST_ASSERT_EQ(want[i], value);
    }
    {
        int32_t value = 0;

        TEST_ASSERT_FALSE(desc->get(self, n, &value));
    }

    completed = desc->foreach(self, false, visit_forward, &forward);
    TEST_ASSERT_TRUE(forward.ok);
    TEST_ASSERT_TRUE(completed);
    TEST_ASSERT_EQ(n, forward.at);

    completed = desc->foreach(self, true, visit_reverse, &reverse);
    TEST_ASSERT_TRUE(reverse.ok);
    TEST_ASSERT_TRUE(completed);
    TEST_ASSERT_EQ(n, reverse.at);
}

static void register_case(const test_seq_desc_t* desc, const char* suffix,
                          test_case_fn fn)
{
    char name[96];

    snprintf(name, sizeof(name), "%s.%s", desc->name, suffix);
    test_register_case("contract", name, fn, desc, __FILE__, __LINE__);
}

static void case_lifecycle(const void* ctx)
{
    const test_seq_desc_t* desc = ctx;
    int32_t value = 0;
    void* self = contract_open(desc);

    TEST_ASSERT_EQ(0, desc->size(self));
    TEST_ASSERT_FALSE(desc->get(self, 0, &value));
    TEST_ASSERT_EQ(0, desc->clear(self));
    TEST_ASSERT_TRUE(desc->push_back(self, 5));
    TEST_ASSERT_EQ(1, desc->size(self));
    TEST_ASSERT_TRUE(desc->get(self, 0, &value));
    TEST_ASSERT_EQ(5, value);
    TEST_ASSERT_EQ(1, desc->clear(self));
    TEST_ASSERT_EQ(0, desc->size(self));
    TEST_ASSERT_EQ(0, desc->clear(self));
    TEST_ASSERT_TRUE(desc->push_back(self, 6));
    TEST_ASSERT_EQ(1, desc->size(self));
}

static void case_push_pop_back(const void* ctx)
{
    const test_seq_desc_t* desc = ctx;
    int32_t want[10];
    void* self = contract_open(desc);
    size_t i;

    for (i = 0; i < 10; ++i) {
        int32_t value = (int32_t)i;

        TEST_ASSERT_TRUE(desc->push_back(self, value));
        want[i] = value;
    }
    contract_check(desc, self, want, 10);

    for (i = 0; i < 3; ++i)
        desc->pop_back(self);
    contract_check(desc, self, want, 7);

    while (desc->size(self) > 0)
        desc->pop_back(self);
    TEST_ASSERT_EQ(0, desc->size(self));
    TEST_ASSERT_TRUE(desc->push_back(self, 42));
    TEST_ASSERT_EQ(1, desc->size(self));
}

static void case_push_pop_front(const void* ctx)
{
    const test_seq_desc_t* desc = ctx;
    int32_t want[10];
    void* self = contract_open(desc);
    size_t i;

    for (i = 0; i < 10; ++i)
        TEST_ASSERT_TRUE(desc->push_front(self, (int32_t)i));
    for (i = 0; i < 10; ++i)
        want[i] = (int32_t)(9 - i);
    contract_check(desc, self, want, 10);

    for (i = 0; i < 3; ++i)
        desc->pop_front(self);
    contract_check(desc, self, want + 3, 7);

    desc->pop_back(self);
    contract_check(desc, self, want + 3, 6);

    TEST_ASSERT_TRUE(desc->push_front(self, 100));
    TEST_ASSERT_TRUE(desc->push_back(self, 101));
    TEST_ASSERT_TRUE(desc->get(self, 0, &want[0]));
    TEST_ASSERT_EQ(100, want[0]);
    TEST_ASSERT_TRUE(desc->get(self, 7, &want[0]));
    TEST_ASSERT_EQ(101, want[0]);

    while (desc->size(self) > 0)
        desc->pop_front(self);
    TEST_ASSERT_EQ(0, desc->size(self));
}

static void case_insert(const void* ctx)
{
    const test_seq_desc_t* desc = ctx;
    void* self = contract_open(desc);
    size_t out = 0;
    int32_t want1[] = { 7 };
    int32_t want2[] = { 6, 7 };
    int32_t want3[] = { 6, 7, 9 };
    int32_t want4[] = { 6, 8, 7, 9 };
    int32_t want5[] = { 6, 8, 5, 5, 5, 7, 9 };
    int32_t want6[] = { 6, 8, 5, 5, 5, 7, 9, 4, 4 };

    TEST_ASSERT_TRUE(desc->insert(self, 0, 7, &out));
    TEST_ASSERT_EQ(0, out);
    contract_check(desc, self, want1, 1);

    TEST_ASSERT_TRUE(desc->insert(self, 0, 6, &out));
    TEST_ASSERT_EQ(0, out);
    contract_check(desc, self, want2, 2);

    TEST_ASSERT_TRUE(desc->insert(self, 2, 9, &out));
    TEST_ASSERT_EQ(2, out);
    contract_check(desc, self, want3, 3);

    TEST_ASSERT_TRUE(desc->insert(self, 1, 8, &out));
    TEST_ASSERT_EQ(1, out);
    contract_check(desc, self, want4, 4);

    TEST_ASSERT_TRUE(desc->insert_n(self, 2, 3, 5, &out));
    TEST_ASSERT_EQ(2, out);
    contract_check(desc, self, want5, 7);

    TEST_ASSERT_TRUE(desc->insert_n(self, 0, 0, 1, &out));
    TEST_ASSERT_EQ(0, out);
    contract_check(desc, self, want5, 7);

    TEST_ASSERT_TRUE(desc->insert_n(self, 7, 2, 4, &out));
    TEST_ASSERT_EQ(7, out);
    contract_check(desc, self, want6, 9);
}

static void case_insert_range(const void* ctx)
{
    const test_seq_desc_t* desc = ctx;
    int32_t source[] = { 10, 11, 12 };
    int32_t tail[] = { 13 };
    int32_t want_a[] = { 1, 10, 11, 12, 2 };
    int32_t want_b[] = { 1, 10, 11, 12, 2, 13 };
    void* self = contract_open(desc);
    size_t out = 0;

    TEST_ASSERT_TRUE(desc->push_back(self, 1));
    TEST_ASSERT_TRUE(desc->push_back(self, 2));
    TEST_ASSERT_TRUE(desc->insert_range_values(self, 1, source, 3, &out));
    TEST_ASSERT_EQ(1, out);
    contract_check(desc, self, want_a, 5);

    TEST_ASSERT_TRUE(desc->insert_range_values(self, 2, source, 0, &out));
    TEST_ASSERT_EQ(2, out);
    contract_check(desc, self, want_a, 5);

    TEST_ASSERT_TRUE(desc->insert_range_values(self, 5, tail, 1, &out));
    TEST_ASSERT_EQ(5, out);
    contract_check(desc, self, want_b, 6);
}

static void case_erase(const void* ctx)
{
    const test_seq_desc_t* desc = ctx;
    int32_t want0[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
    int32_t want1[] = { 0, 1, 2, 4, 5, 6, 7, 8, 9 };
    int32_t want2[] = { 1, 2, 4, 5, 6, 7, 8, 9 };
    int32_t want3[] = { 1, 2, 4, 5, 6, 7, 8 };
    void* self = contract_open(desc);
    size_t out = 0;

    contract_append(desc, self, 0, 10);
    contract_check(desc, self, want0, 10);

    TEST_ASSERT_TRUE(desc->erase(self, 3, &out));
    TEST_ASSERT_EQ(3, out);
    contract_check(desc, self, want1, 9);

    TEST_ASSERT_TRUE(desc->erase(self, 0, &out));
    TEST_ASSERT_EQ(0, out);
    contract_check(desc, self, want2, 8);

    TEST_ASSERT_TRUE(desc->erase(self, 7, &out));
    TEST_ASSERT_EQ(7, out);
    contract_check(desc, self, want3, 7);

    while (desc->size(self) > 0)
        TEST_ASSERT_TRUE(desc->erase(self, 0, &out));
    TEST_ASSERT_EQ(0, desc->size(self));

    TEST_ASSERT_TRUE(desc->push_back(self, 3));
    TEST_ASSERT_TRUE(desc->insert(self, 0, 4, &out));
    TEST_ASSERT_TRUE(desc->erase(self, 1, &out));
    TEST_ASSERT_EQ(1, out);
    TEST_ASSERT_EQ(1, desc->size(self));
}

static void case_erase_range(const void* ctx)
{
    const test_seq_desc_t* desc = ctx;
    int32_t want1[] = { 0, 1, 5, 6, 7, 8, 9 };
    int32_t want2[] = { 0, 1, 5, 6, 7 };
    void* self = contract_open(desc);
    size_t out = 0;

    contract_append(desc, self, 0, 10);
    TEST_ASSERT_TRUE(desc->erase_range(self, 2, 5, &out));
    TEST_ASSERT_EQ(2, out);
    contract_check(desc, self, want1, 7);

    TEST_ASSERT_TRUE(desc->erase_range(self, 1, 1, &out));
    TEST_ASSERT_EQ(1, out);
    contract_check(desc, self, want1, 7);

    TEST_ASSERT_TRUE(desc->erase_range(self, 5, 7, &out));
    TEST_ASSERT_EQ(5, out);
    contract_check(desc, self, want2, 5);

    TEST_ASSERT_TRUE(desc->erase_range(self, 0, 5, &out));
    TEST_ASSERT_EQ(0, out);
    TEST_ASSERT_EQ(0, desc->size(self));
    TEST_ASSERT_TRUE(desc->push_back(self, 1));
    TEST_ASSERT_EQ(1, desc->size(self));
}

static void case_assign_resize(const void* ctx)
{
    const test_seq_desc_t* desc = ctx;
    int32_t same[] = { 3, 3, 3, 3, 3 };
    int32_t grown[] = { 3, 3, 3, 3, 3, 7, 7 };
    void* self = contract_open(desc);

    TEST_ASSERT_TRUE(desc->assign(self, 4, 9));
    {
        int32_t want[] = { 9, 9, 9, 9 };

        contract_check(desc, self, want, 4);
    }
    TEST_ASSERT_TRUE(desc->assign(self, 0, 9));
    TEST_ASSERT_EQ(0, desc->size(self));

    TEST_ASSERT_TRUE(desc->assign(self, 5, 3));
    contract_check(desc, self, same, 5);

    TEST_ASSERT_TRUE(desc->resize(self, 7, 7));
    contract_check(desc, self, grown, 7);

    TEST_ASSERT_TRUE(desc->resize(self, 2, 7));
    contract_check(desc, self, grown, 2);

    TEST_ASSERT_TRUE(desc->resize(self, 2, 9));
    contract_check(desc, self, grown, 2);

    TEST_ASSERT_TRUE(desc->resize(self, 0, 9));
    TEST_ASSERT_EQ(0, desc->size(self));
    TEST_ASSERT_TRUE(desc->push_back(self, 1));
}

static void case_assign_range(const void* ctx)
{
    const test_seq_desc_t* desc = ctx;
    int32_t source[] = { 1, 2, 3 };
    int32_t want[] = { 1, 2, 3 };
    void* self = contract_open(desc);

    TEST_ASSERT_TRUE(desc->push_back(self, 9));
    TEST_ASSERT_TRUE(desc->assign_range_values(self, source, 3));
    contract_check(desc, self, want, 3);

    TEST_ASSERT_TRUE(desc->assign_range_values(self, source, 0));
    TEST_ASSERT_EQ(0, desc->size(self));
    TEST_ASSERT_TRUE(desc->push_back(self, 1));
}

static void case_find_count_remove(const void* ctx)
{
    const test_seq_desc_t* desc = ctx;
    int32_t values[] = { 1, 2, 2, 3, 2, 4 };
    int32_t after[] = { 1, 3, 4 };
    void* self = contract_open(desc);
    size_t out = 0;

    contract_append(desc, self, 1, 1);
    {
        size_t i;

        for (i = 1; i < sizeof(values) / sizeof(values[0]); ++i)
            TEST_ASSERT_TRUE(desc->push_back(self, values[i]));
    }

    TEST_ASSERT_TRUE(desc->find(self, 2, &out));
    TEST_ASSERT_EQ(1, out);
    TEST_ASSERT_FALSE(desc->find(self, 9, &out));
    TEST_ASSERT_EQ(3, desc->count(self, 2));
    TEST_ASSERT_EQ(0, desc->count(self, 9));
    TEST_ASSERT_EQ(3, desc->remove(self, 2));
    contract_check(desc, self, after, 3);
    TEST_ASSERT_EQ(0, desc->remove(self, 2));
    contract_check(desc, self, after, 3);
}

static bool remove_even(int32_t value, void* ctx)
{
    (void)ctx;
    return (value & 1) == 0;
}

static void case_remove_if(const void* ctx)
{
    const test_seq_desc_t* desc = ctx;
    void* self = contract_open(desc);
    int32_t values[] = { 2, 4, 5, 6, 7 };
    int32_t left[] = { 5, 7 };

    {
        size_t i;

        for (i = 0; i < sizeof(values) / sizeof(values[0]); ++i)
            TEST_ASSERT_TRUE(desc->push_back(self, values[i]));
    }
    TEST_ASSERT_EQ(3, desc->remove_if(self, remove_even, NULL));
    contract_check(desc, self, left, 2);
    TEST_ASSERT_EQ(0, desc->remove_if(self, remove_even, NULL));
}

static void case_clear_swap(const void* ctx)
{
    const test_seq_desc_t* desc = ctx;
    int32_t want_a[] = { 1, 2, 3 };
    int32_t want_b[] = { 4, 5 };
    void* first = contract_open(desc);
    void* second = contract_open(desc);

    contract_append(desc, first, 1, 3);
    contract_append(desc, second, 4, 2);

    TEST_ASSERT_EQ(2, desc->clear(second));
    TEST_ASSERT_EQ(0, desc->size(second));
    contract_check(desc, first, want_a, 3);

    contract_append(desc, second, 4, 2);
    desc->swap(first, second);
    contract_check(desc, first, want_b, 2);
    contract_check(desc, second, want_a, 3);

    TEST_ASSERT_EQ(2, desc->clear(first));
    desc->swap(first, second);
    TEST_ASSERT_EQ(3, desc->size(first));
    contract_check(desc, first, want_a, 3);
    TEST_ASSERT_EQ(0, desc->size(second));
    TEST_ASSERT_TRUE(desc->push_back(first, 9));
    TEST_ASSERT_EQ(4, desc->size(first));
    desc->swap(first, second);
    TEST_ASSERT_EQ(0, desc->size(first));
    TEST_ASSERT_EQ(4, desc->clear(second));
}

static void case_reserve_shrink(const void* ctx)
{
    const test_seq_desc_t* desc = ctx;
    int32_t want[16];
    void* self = contract_open(desc);
    size_t i;

    for (i = 0; i < 16; ++i) {
        want[i] = (int32_t)i;
        TEST_ASSERT_TRUE(desc->push_back(self, want[i]));
    }
    if ((desc->caps & TEST_SEQ_RESERVE) != 0) {
        TEST_ASSERT_TRUE(desc->reserve(self, 64));
        contract_check(desc, self, want, 16);
        TEST_ASSERT_TRUE(desc->reserve(self, 0));
        contract_check(desc, self, want, 16);
    }
    if ((desc->caps & TEST_SEQ_SHRINK) != 0) {
        desc->shrink_to_fit(self);
        contract_check(desc, self, want, 16);
        TEST_ASSERT_EQ(16, desc->clear(self));
        desc->shrink_to_fit(self);
        TEST_ASSERT_EQ(0, desc->size(self));
        TEST_ASSERT_TRUE(desc->push_back(self, 1));
    }
}

void test_sequence_contract_register(const test_seq_desc_t* desc)
{
    register_case(desc, "lifecycle", case_lifecycle);
    register_case(desc, "push_pop_back", case_push_pop_back);
    register_case(desc, "insert", case_insert);
    register_case(desc, "erase", case_erase);
    register_case(desc, "erase_range", case_erase_range);
    register_case(desc, "assign_resize", case_assign_resize);
    register_case(desc, "find_count_remove", case_find_count_remove);
    register_case(desc, "reserve_shrink", case_reserve_shrink);

    if ((desc->caps & TEST_SEQ_PUSH_FRONT) != 0)
        register_case(desc, "push_pop_front", case_push_pop_front);
    if ((desc->caps & TEST_SEQ_INSERT_RANGE) != 0)
        register_case(desc, "insert_range", case_insert_range);
    if ((desc->caps & TEST_SEQ_ASSIGN_RANGE) != 0)
        register_case(desc, "assign_range", case_assign_range);
    if ((desc->caps & TEST_SEQ_REMOVE_IF) != 0)
        register_case(desc, "remove_if", case_remove_if);
    if ((desc->caps & TEST_SEQ_SWAP) != 0)
        register_case(desc, "clear_swap", case_clear_swap);
}
