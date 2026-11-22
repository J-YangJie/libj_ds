/*
  libj_ds permanent tests associative contract
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

#include "assoc_desc.h"
#include "../core/test.h"

#define CONTRACT_COLLECT_MAX 64

static const test_assoc_desc_t* contract_desc;

static void contract_destroy(void* self)
{
    contract_desc->destroy(self);
}

static void* contract_open(const test_assoc_desc_t* desc)
{
    void* self;

    contract_desc = desc;
    self = desc->create();
    TEST_ASSERT_NOT_NULL(self);
    test_cleanup_push(contract_destroy, self);
    return self;
}

static int32_t contract_value_at(const test_assoc_desc_t* desc, int32_t key)
{
    if ((desc->caps & TEST_ASSOC_HAS_VALUE) != 0)
        return key * 10 + 7;
    return key;
}

static void contract_insert(const test_assoc_desc_t* desc, void* self,
                            int32_t key)
{
    bool inserted = false;

    TEST_ASSERT_TRUE(desc->insert(self, key, contract_value_at(desc, key),
                                  &inserted));
    TEST_ASSERT_TRUE(inserted);
}

static void contract_check_find(const test_assoc_desc_t* desc,
                                const void* self, int32_t key,
                                int32_t expected, bool found)
{
    int32_t value = 0;

    TEST_ASSERT_EQ(found, desc->find(self, key, &value));
    if (found)
        TEST_ASSERT_EQ(expected, value);
}

typedef struct contract_collect {
    int32_t keys[CONTRACT_COLLECT_MAX];
    int32_t values[CONTRACT_COLLECT_MAX];
    size_t  count;
    bool    overflow;
} contract_collect_t;

static bool contract_collect_visit(void* ctx, int32_t key, int32_t value)
{
    contract_collect_t* collect = ctx;

    if (collect->count >= CONTRACT_COLLECT_MAX) {
        collect->overflow = true;
        return false;
    }
    collect->keys[collect->count] = key;
    collect->values[collect->count] = value;
    ++collect->count;
    return true;
}

static void contract_sort(int32_t* keys, size_t n)
{
    size_t i;

    for (i = 1; i < n; ++i) {
        int32_t value = keys[i];
        size_t j = i;

        while (j > 0 && keys[j - 1] > value) {
            keys[j] = keys[j - 1];
            --j;
        }
        keys[j] = value;
    }
}

static void contract_check_traversal(const test_assoc_desc_t* desc,
                                     const void* self)
{
    contract_collect_t forward;
    contract_collect_t reverse;

    forward.count = 0;
    forward.overflow = false;
    reverse.count = 0;
    reverse.overflow = false;

    TEST_ASSERT_TRUE(desc->foreach(self, false, contract_collect_visit,
                                   &forward));
    TEST_ASSERT_TRUE(desc->foreach(self, true, contract_collect_visit,
                                   &reverse));
    TEST_ASSERT_FALSE(forward.overflow);
    TEST_ASSERT_FALSE(reverse.overflow);
    TEST_ASSERT_EQ(desc->size(self), forward.count);
    TEST_ASSERT_EQ(forward.count, reverse.count);

    if ((desc->caps & TEST_ASSOC_ORDERED) != 0) {
        size_t i;

        for (i = 1; i < forward.count; ++i) {
            if ((desc->caps & TEST_ASSOC_MULTI) != 0)
                TEST_ASSERT_TRUE(forward.keys[i - 1] <= forward.keys[i]);
            else
                TEST_ASSERT_TRUE(forward.keys[i - 1] < forward.keys[i]);
        }
        for (i = 1; i < reverse.count; ++i) {
            if ((desc->caps & TEST_ASSOC_MULTI) != 0)
                TEST_ASSERT_TRUE(reverse.keys[i - 1] >= reverse.keys[i]);
            else
                TEST_ASSERT_TRUE(reverse.keys[i - 1] > reverse.keys[i]);
        }
        for (i = 0; i < forward.count; ++i)
            TEST_ASSERT_EQ(forward.keys[i], reverse.keys[reverse.count - 1 - i]);
    }
}

static void contract_check_sorted_sets(const int32_t* expected, size_t n,
                                       const int32_t* actual, size_t m)
{
    size_t i;

    TEST_ASSERT_EQ(n, m);
    for (i = 0; i < n; ++i)
        TEST_ASSERT_EQ(expected[i], actual[i]);
}

static void case_lifecycle(const void* ctx)
{
    const test_assoc_desc_t* desc = ctx;
    void* self = contract_open(desc);
    int32_t value = 0;
    int32_t next_key = 0;
    bool next_found = true;

    TEST_ASSERT_EQ(0, desc->size(self));
    TEST_ASSERT_EQ(0, desc->count(self, 1));
    TEST_ASSERT_FALSE(desc->find(self, 1, &value));
    if ((desc->caps & TEST_ASSOC_BOUNDS) != 0) {
        TEST_ASSERT_FALSE(desc->lower_bound(self, 1, &next_key, &value));
        TEST_ASSERT_FALSE(desc->upper_bound(self, 1, &next_key, &value));
    }
    TEST_ASSERT_TRUE(desc->erase_key(self, 1, &next_key, &next_found));
    TEST_ASSERT_FALSE(next_found);
    TEST_ASSERT_EQ(0, desc->remove(self, 1));
    TEST_ASSERT_EQ(0, desc->clear(self));

    contract_insert(desc, self, 1);
    TEST_ASSERT_EQ(1, desc->size(self));
    TEST_ASSERT_EQ(1, desc->count(self, 1));
    contract_check_find(desc, self, 1, contract_value_at(desc, 1), true);

    TEST_ASSERT_EQ(1, desc->clear(self));
    TEST_ASSERT_EQ(0, desc->size(self));
    TEST_ASSERT_EQ(0, desc->count(self, 1));
    TEST_ASSERT_FALSE(desc->find(self, 1, &value));
    TEST_ASSERT_EQ(0, desc->clear(self));
}

static void case_unique_insert(const void* ctx)
{
    const test_assoc_desc_t* desc = ctx;
    void* self = contract_open(desc);
    bool inserted = true;
    int32_t first = contract_value_at(desc, 5);
    int32_t second = first + 1;

    contract_insert(desc, self, 5);
    TEST_ASSERT_EQ(1, desc->size(self));
    TEST_ASSERT_EQ(1, desc->count(self, 5));

    TEST_ASSERT_TRUE(desc->insert(self, 5, second, &inserted));
    TEST_ASSERT_FALSE(inserted);
    TEST_ASSERT_EQ(1, desc->size(self));
    TEST_ASSERT_EQ(1, desc->count(self, 5));
    contract_check_find(desc, self, 5, first, true);

    if ((desc->caps & TEST_ASSOC_INSERT_REPLACE) != 0) {
        TEST_ASSERT_TRUE(desc->insert_replace(self, 5, second));
        TEST_ASSERT_EQ(1, desc->size(self));
        contract_check_find(desc, self, 5, second, true);

        TEST_ASSERT_TRUE(desc->insert_replace(self, 6,
                                              contract_value_at(desc, 6)));
        TEST_ASSERT_EQ(2, desc->size(self));
        TEST_ASSERT_EQ(1, desc->count(self, 6));
        contract_check_find(desc, self, 6, contract_value_at(desc, 6), true);
    }

    {
        size_t size = desc->size(self);

        TEST_ASSERT_EQ(size, desc->clear(self));
    }
    TEST_ASSERT_EQ(0, desc->size(self));
}

static void case_multi_insert(const void* ctx)
{
    const test_assoc_desc_t* desc = ctx;
    void* self = contract_open(desc);
    bool inserted = false;
    int32_t value = 0;
    int32_t next_key = 0;
    bool next_found = false;

    TEST_ASSERT_TRUE(desc->insert(self, 5, contract_value_at(desc, 5),
                                  &inserted));
    TEST_ASSERT_TRUE(inserted);
    TEST_ASSERT_TRUE(desc->insert(self, 5, contract_value_at(desc, 5),
                                  &inserted));
    TEST_ASSERT_TRUE(inserted);
    TEST_ASSERT_EQ(2, desc->size(self));
    TEST_ASSERT_EQ(2, desc->count(self, 5));
    TEST_ASSERT_TRUE(desc->find(self, 5, &value));
    TEST_ASSERT_EQ(contract_value_at(desc, 5), value);

    TEST_ASSERT_TRUE(desc->erase_key(self, 5, &next_key, &next_found));
    TEST_ASSERT_EQ(1, desc->size(self));
    TEST_ASSERT_EQ(1, desc->count(self, 5));

    contract_insert(desc, self, 5);
    TEST_ASSERT_EQ(2, desc->size(self));
    TEST_ASSERT_EQ(2, desc->remove(self, 5));
    TEST_ASSERT_EQ(0, desc->size(self));
    TEST_ASSERT_EQ(0, desc->count(self, 5));
    TEST_ASSERT_FALSE(desc->find(self, 5, &value));
    TEST_ASSERT_EQ(0, desc->clear(self));
}

static void case_bounds(const void* ctx)
{
    const test_assoc_desc_t* desc = ctx;
    void* self = contract_open(desc);
    int32_t key = 0;
    int32_t value = 0;

    contract_insert(desc, self, 20);
    contract_insert(desc, self, 10);
    contract_insert(desc, self, 30);

    TEST_ASSERT_TRUE(desc->lower_bound(self, 5, &key, &value));
    TEST_ASSERT_EQ(10, key);
    TEST_ASSERT_EQ(contract_value_at(desc, 10), value);
    TEST_ASSERT_TRUE(desc->lower_bound(self, 20, &key, &value));
    TEST_ASSERT_EQ(20, key);
    TEST_ASSERT_TRUE(desc->lower_bound(self, 25, &key, &value));
    TEST_ASSERT_EQ(30, key);
    TEST_ASSERT_FALSE(desc->lower_bound(self, 31, &key, &value));

    TEST_ASSERT_TRUE(desc->upper_bound(self, 5, &key, &value));
    TEST_ASSERT_EQ(10, key);
    TEST_ASSERT_TRUE(desc->upper_bound(self, 20, &key, &value));
    TEST_ASSERT_EQ(30, key);
    TEST_ASSERT_FALSE(desc->upper_bound(self, 30, &key, &value));

    TEST_ASSERT_EQ(3, desc->clear(self));
}

static void case_traversal(const void* ctx)
{
    const test_assoc_desc_t* desc = ctx;
    void* self = contract_open(desc);
    static const int32_t inserted[] = { 4, 2, 5, 1, 3 };
    static const int32_t expected[] = { 1, 2, 3, 4, 5 };
    contract_collect_t forward;
    contract_collect_t reverse;
    size_t i;

    for (i = 0; i < sizeof(inserted) / sizeof(inserted[0]); ++i)
        contract_insert(desc, self, inserted[i]);

    contract_check_traversal(desc, self);

    forward.count = 0;
    forward.overflow = false;
    reverse.count = 0;
    reverse.overflow = false;
    TEST_ASSERT_TRUE(desc->foreach(self, false, contract_collect_visit,
                                   &forward));
    TEST_ASSERT_TRUE(desc->foreach(self, true, contract_collect_visit,
                                   &reverse));
    TEST_ASSERT_FALSE(forward.overflow);
    TEST_ASSERT_FALSE(reverse.overflow);
    TEST_ASSERT_EQ(5, forward.count);
    TEST_ASSERT_EQ(5, reverse.count);

    for (i = 0; i < forward.count; ++i) {
        TEST_ASSERT_EQ(contract_value_at(desc, forward.keys[i]),
                       forward.values[i]);
        TEST_ASSERT_EQ(contract_value_at(desc, reverse.keys[i]),
                       reverse.values[i]);
    }
    contract_sort(forward.keys, forward.count);
    contract_sort(reverse.keys, reverse.count);
    contract_check_sorted_sets(expected, 5, forward.keys, forward.count);
    contract_check_sorted_sets(expected, 5, reverse.keys, reverse.count);

    TEST_ASSERT_EQ(5, desc->clear(self));
}

static bool contract_has_key(const test_assoc_desc_t* desc,
                             const void* self, int32_t key)
{
    int32_t value = 0;

    return desc->find(self, key, &value);
}

static void case_erase_successor(const void* ctx)
{
    const test_assoc_desc_t* desc = ctx;
    void* self = contract_open(desc);
    bool ordered = (desc->caps & TEST_ASSOC_ORDERED) != 0;
    int32_t next_key = 0;
    bool next_found = false;

    contract_insert(desc, self, 20);
    contract_insert(desc, self, 10);
    contract_insert(desc, self, 30);

    TEST_ASSERT_TRUE(desc->erase_key(self, 20, &next_key, &next_found));
    TEST_ASSERT_EQ(2, desc->size(self));
    TEST_ASSERT_EQ(0, contract_has_key(desc, self, 20));
    if (ordered) {
        TEST_ASSERT_TRUE(next_found);
        TEST_ASSERT_EQ(30, next_key);
    } else if (next_found) {
        TEST_ASSERT_TRUE(contract_has_key(desc, self, next_key));
    }

    TEST_ASSERT_TRUE(desc->erase_key(self, 30, &next_key, &next_found));
    TEST_ASSERT_EQ(1, desc->size(self));
    TEST_ASSERT_EQ(0, contract_has_key(desc, self, 30));
    if (ordered)
        TEST_ASSERT_FALSE(next_found);
    else if (next_found)
        TEST_ASSERT_TRUE(contract_has_key(desc, self, next_key));

    TEST_ASSERT_TRUE(desc->erase_key(self, 99, &next_key, &next_found));
    TEST_ASSERT_FALSE(next_found);
    TEST_ASSERT_EQ(1, desc->size(self));

    TEST_ASSERT_TRUE(desc->erase_key(self, 10, &next_key, &next_found));
    TEST_ASSERT_FALSE(next_found);
    TEST_ASSERT_EQ(0, desc->size(self));
}

static void register_case(const test_assoc_desc_t* desc, const char* suffix,
                          test_case_fn fn)
{
    char name[96];

    snprintf(name, sizeof(name), "%s.%s", desc->name, suffix);
    test_register_case("contract", name, fn, desc, __FILE__, __LINE__);
}

void test_assoc_contract_register(const test_assoc_desc_t* desc)
{
    register_case(desc, "lifecycle", case_lifecycle);
    if ((desc->caps & TEST_ASSOC_MULTI) == 0)
        register_case(desc, "unique_insert", case_unique_insert);
    else
        register_case(desc, "multi_insert", case_multi_insert);
    if ((desc->caps & TEST_ASSOC_BOUNDS) != 0)
        register_case(desc, "bounds", case_bounds);
    register_case(desc, "traversal", case_traversal);
    register_case(desc, "erase_successor", case_erase_successor);
}
