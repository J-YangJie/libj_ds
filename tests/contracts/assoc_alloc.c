/*
  libj_ds permanent tests associative allocation failures
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

#include <_memory.h>

#include "assoc_alloc.h"
#include "../core/test.h"

#define ASSOC_ALLOC_SEED 8
#define ASSOC_ALLOC_GROW 64
#define ASSOC_ALLOC_PROBE 100000

typedef struct assoc_alloc_state {
    void*  self;
    size_t changed;
} assoc_alloc_state_t;

static const test_assoc_desc_t* alloc_desc;

static int32_t alloc_value_at(int32_t key)
{
    if ((alloc_desc->caps & TEST_ASSOC_HAS_VALUE) != 0)
        return key * 10 + 7;
    return key;
}

static int32_t alloc_replaced_value_at(int32_t key)
{
    return alloc_value_at(key) + 5000;
}

static void alloc_teardown(void* opaque)
{
    assoc_alloc_state_t* state = opaque;

    alloc_desc->destroy(state->self);
    p_free(state);
}

static void* alloc_setup(void)
{
    assoc_alloc_state_t* state;
    size_t i;

    state = p_calloc(1, sizeof(*state));
    if (state == NULL)
        return NULL;
    state->self = alloc_desc->create();
    if (state->self == NULL) {
        p_free(state);
        return NULL;
    }
    for (i = 0; i < ASSOC_ALLOC_SEED; ++i) {
        int32_t key = (int32_t)i;
        bool inserted = false;

        if (!alloc_desc->insert(state->self, key, alloc_value_at(key),
                                &inserted)
            || !inserted) {
            alloc_desc->destroy(state->self);
            p_free(state);
            return NULL;
        }
    }
    return state;
}

static void alloc_check_entry(const void* self, int32_t key, int32_t expected)
{
    int32_t value = 0;

    TEST_ASSERT_TRUE(alloc_desc->find(self, key, &value));
    TEST_ASSERT_EQ(expected, value);
}

static void alloc_check_usable(void* self)
{
    int32_t value = 0;
    bool inserted = false;

    TEST_ASSERT_TRUE(alloc_desc->insert(self, ASSOC_ALLOC_PROBE,
                                        alloc_value_at(ASSOC_ALLOC_PROBE),
                                        &inserted));
    TEST_ASSERT_TRUE(inserted);
    alloc_check_entry(self, ASSOC_ALLOC_PROBE,
                      alloc_value_at(ASSOC_ALLOC_PROBE));
    TEST_ASSERT_EQ(1, alloc_desc->remove(self, ASSOC_ALLOC_PROBE));
    TEST_ASSERT_FALSE(alloc_desc->find(self, ASSOC_ALLOC_PROBE, &value));
}

static bool alloc_insert_operate(void* opaque)
{
    assoc_alloc_state_t* state = opaque;
    size_t i;

    for (i = 0; i < ASSOC_ALLOC_GROW; ++i) {
        int32_t key = (int32_t)(ASSOC_ALLOC_SEED + i);
        bool inserted = false;

        if (!alloc_desc->insert(state->self, key, alloc_value_at(key),
                                &inserted)
            || !inserted)
            return false;
        ++state->changed;
    }
    return true;
}

static void alloc_insert_verify(void* opaque, bool succeeded)
{
    assoc_alloc_state_t* state = opaque;
    size_t i;

    (void)succeeded;
    TEST_ASSERT_EQ(ASSOC_ALLOC_SEED + state->changed,
                   alloc_desc->size(state->self));
    for (i = 0; i < ASSOC_ALLOC_SEED; ++i)
        alloc_check_entry(state->self, (int32_t)i,
                          alloc_value_at((int32_t)i));
    for (i = 0; i < state->changed; ++i) {
        int32_t key = (int32_t)(ASSOC_ALLOC_SEED + i);

        alloc_check_entry(state->self, key, alloc_value_at(key));
    }
    alloc_check_usable(state->self);
}

static bool alloc_replace_operate(void* opaque)
{
    assoc_alloc_state_t* state = opaque;
    size_t i;

    for (i = 0; i < ASSOC_ALLOC_GROW; ++i) {
        int32_t key = (int32_t)(ASSOC_ALLOC_SEED + i);

        if (!alloc_desc->insert_replace(state->self, key,
                                        alloc_replaced_value_at(key)))
            return false;
        ++state->changed;
    }
    return true;
}

static void alloc_replace_verify(void* opaque, bool succeeded)
{
    assoc_alloc_state_t* state = opaque;
    size_t i;

    (void)succeeded;
    TEST_ASSERT_EQ(ASSOC_ALLOC_SEED + state->changed,
                   alloc_desc->size(state->self));
    for (i = 0; i < ASSOC_ALLOC_SEED; ++i)
        alloc_check_entry(state->self, (int32_t)i,
                          alloc_value_at((int32_t)i));
    for (i = 0; i < state->changed; ++i) {
        int32_t key = (int32_t)(ASSOC_ALLOC_SEED + i);

        alloc_check_entry(state->self, key, alloc_replaced_value_at(key));
    }
    alloc_check_usable(state->self);
}

static const test_alloc_scenario_t alloc_insert_scenario = {
    .name = "assoc.insert.growth",
    .setup = alloc_setup,
    .operate = alloc_insert_operate,
    .verify = alloc_insert_verify,
    .teardown = alloc_teardown,
};

static const test_alloc_scenario_t alloc_replace_scenario = {
    .name = "assoc.insert_replace.growth",
    .setup = alloc_setup,
    .operate = alloc_replace_operate,
    .verify = alloc_replace_verify,
    .teardown = alloc_teardown,
};

static void case_construction_failures(const void* ctx)
{
    const test_assoc_desc_t* desc = ctx;
    size_t attempt;

    for (attempt = 1; attempt <= 2; ++attempt) {
        void* self;

        test_alloc_fail_at(attempt);
        self = desc->create();
        test_alloc_disarm();
        test_trace("construct", (long long)attempt, self != NULL ? 1 : 0, 0);
        if (self != NULL)
            desc->destroy(self);
        TEST_ASSERT_EQ(0, test_alloc_live_blocks());
    }
}

static void case_insert_growth(const void* ctx)
{
    alloc_desc = ctx;
    test_alloc_sweep(&alloc_insert_scenario);
}

static void case_insert_replace_growth(const void* ctx)
{
    alloc_desc = ctx;
    test_alloc_sweep(&alloc_replace_scenario);
}

static void register_case(const test_assoc_desc_t* desc, const char* suffix,
                          test_case_fn fn)
{
    char name[96];

    snprintf(name, sizeof(name), "%s.%s", desc->name, suffix);
    test_register_case("alloc", name, fn, desc, __FILE__, __LINE__);
}

void test_assoc_alloc_register(const test_assoc_desc_t* desc)
{
    register_case(desc, "construction_failures", case_construction_failures);
    register_case(desc, "insert_growth", case_insert_growth);
    if ((desc->caps & TEST_ASSOC_INSERT_REPLACE) != 0)
        register_case(desc, "insert_replace_growth",
                      case_insert_replace_growth);
}
