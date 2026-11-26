/*
  libj_ds permanent tests list allocation failures
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

#include <list/list.h>
#include <_memory.h>

#include <stdio.h>
#include <string.h>

#include "../../core/test.h"
#include "value.h"

typedef struct list_alloc_state {
    list_t* list;
    list_t* source;
    size_t  count;
} list_alloc_state_t;

static void list_alloc_teardown(void* opaque)
{
    list_alloc_state_t* state = opaque;

    LIST_DELETE(&state->source);
    LIST_DELETE(&state->list);
    p_free(state);
}

static list_alloc_state_t* list_alloc_state_new(list_t* list, size_t initial,
                                                int32_t base)
{
    list_alloc_state_t* state;
    size_t i;

    if (list == NULL)
        return NULL;
    state = p_calloc(1, sizeof(*state));
    if (state == NULL) {
        LIST_DELETE(&list);
        return NULL;
    }
    state->list = list;
    for (i = 0; i < initial; ++i) {
        if (!clist->push_back(state->list,
                              __ds_s32data(base + (int32_t)i))) {
            LIST_DELETE(&state->list);
            p_free(state);
            return NULL;
        }
        ++state->count;
    }
    return state;
}

static void* list_alloc_push_setup(void)
{
    return list_alloc_state_new(LIST_NEW_T(int32_t), 0, 0);
}

static bool list_alloc_push_operate(void* opaque)
{
    list_alloc_state_t* state = opaque;
    size_t i;

    for (i = 0; i < 200; ++i) {
        if (!clist->push_back(state->list, __ds_s32data((int32_t)i)))
            return false;
        ++state->count;
    }
    return true;
}

static void list_alloc_verify_usable(list_alloc_state_t* state)
{
    list_size_t size = clist->size(state->list);
    int32_t tail = -2;

    TEST_ASSERT_NULL(list_check_invariants(state->list));
    TEST_ASSERT_TRUE(clist->push_back(state->list, __ds_s32data(-1)));
    TEST_ASSERT_EQ(size + 1, clist->size(state->list));
    TEST_ASSERT_EQ(-1, *clist->back(state->list).s32);
    clist->pop_back(state->list);
    TEST_ASSERT_EQ(size, clist->size(state->list));
    if (size > 0) {
        tail = *clist->back(state->list).s32;
        TEST_ASSERT_TRUE(tail != -1);
    }
}

static void list_alloc_push_verify(void* opaque, bool succeeded)
{
    list_alloc_state_t* state = opaque;
    size_t i;

    TEST_ASSERT_EQ(state->count, (size_t)clist->size(state->list));
    for (i = 0; i < state->count; ++i)
        TEST_ASSERT_EQ(i, *clist->at(state->list, i).s32);
    if (succeeded)
        TEST_ASSERT_EQ(200, state->count);
    list_alloc_verify_usable(state);
}

static void* list_alloc_push_front_setup(void)
{
    return list_alloc_state_new(LIST_NEW_T(int32_t), 0, 0);
}

static bool list_alloc_push_front_operate(void* opaque)
{
    list_alloc_state_t* state = opaque;
    size_t i;

    for (i = 0; i < 200; ++i) {
        if (!clist->push_front(state->list, __ds_s32data((int32_t)i)))
            return false;
        ++state->count;
    }
    return true;
}

static void list_alloc_push_front_verify(void* opaque, bool succeeded)
{
    list_alloc_state_t* state = opaque;
    size_t i;

    TEST_ASSERT_EQ(state->count, (size_t)clist->size(state->list));
    for (i = 0; i < state->count; ++i)
        TEST_ASSERT_EQ((int32_t)(state->count - 1 - i),
                       *clist->at(state->list, i).s32);
    if (succeeded)
        TEST_ASSERT_EQ(200, state->count);
    list_alloc_verify_usable(state);
}

static void* list_alloc_insert_setup(void)
{
    return list_alloc_state_new(LIST_NEW_T(int32_t), 120, 0);
}

static bool list_alloc_insert_operate(void* opaque)
{
    list_alloc_state_t* state = opaque;

    return clist->insert_n(state->list, clist->it(state->list, 60), 60,
                           __ds_s32data(7)).d != NULL;
}

static void list_alloc_insert_verify(void* opaque, bool succeeded)
{
    list_alloc_state_t* state = opaque;
    size_t size = (size_t)clist->size(state->list);
    size_t i;

    if (succeeded) {
        TEST_ASSERT_EQ(180, size);
        for (i = 0; i < 60; ++i)
            TEST_ASSERT_EQ(i, *clist->at(state->list, i).s32);
        for (i = 0; i < 60; ++i)
            TEST_ASSERT_EQ(7, *clist->at(state->list, 60 + i).s32);
        for (i = 0; i < 60; ++i)
            TEST_ASSERT_EQ(60 + i, *clist->at(state->list, 120 + i).s32);
    } else {
        TEST_ASSERT_EQ(120, size);
        for (i = 0; i < size; ++i)
            TEST_ASSERT_EQ(i, *clist->at(state->list, i).s32);
    }
    list_alloc_verify_usable(state);
}

static list_alloc_state_t* list_alloc_range_setup_state(size_t initial,
                                                        size_t source_n,
                                                        int32_t base)
{
    list_alloc_state_t* state =
        list_alloc_state_new(LIST_NEW_T(int32_t), initial, 0);
    size_t i;

    if (state == NULL)
        return NULL;
    state->source = LIST_NEW_T(int32_t);
    if (state->source == NULL) {
        LIST_DELETE(&state->list);
        p_free(state);
        return NULL;
    }
    for (i = 0; i < source_n; ++i) {
        if (!clist->push_back(state->source,
                              __ds_s32data(base + (int32_t)i))) {
            LIST_DELETE(&state->source);
            LIST_DELETE(&state->list);
            p_free(state);
            return NULL;
        }
    }
    return state;
}

static void* list_alloc_insert_range_setup(void)
{
    return list_alloc_range_setup_state(120, 80, 1000);
}

static bool list_alloc_insert_range_operate(void* opaque)
{
    list_alloc_state_t* state = opaque;

    return clist->insert_range(state->list, clist->it(state->list, 60),
                               clist->begin(state->source),
                               clist->end(state->source)).d != NULL;
}

static void list_alloc_check_source(const list_alloc_state_t* state,
                                    size_t count, int32_t base)
{
    size_t i;

    TEST_ASSERT_EQ(count, (size_t)clist->size(state->source));
    for (i = 0; i < count; ++i)
        TEST_ASSERT_EQ(base + (int32_t)i,
                       *clist->at(state->source, i).s32);
    TEST_ASSERT_NULL(list_check_invariants(state->source));
}

static void list_alloc_insert_range_verify(void* opaque, bool succeeded)
{
    list_alloc_state_t* state = opaque;
    size_t size = (size_t)clist->size(state->list);
    size_t i;

    if (succeeded) {
        TEST_ASSERT_EQ(200, size);
        for (i = 0; i < 60; ++i)
            TEST_ASSERT_EQ(i, *clist->at(state->list, i).s32);
        for (i = 0; i < 80; ++i)
            TEST_ASSERT_EQ(1000 + (int32_t)i,
                           *clist->at(state->list, 60 + i).s32);
        for (i = 0; i < 60; ++i)
            TEST_ASSERT_EQ(60 + (int32_t)i,
                           *clist->at(state->list, 140 + i).s32);
    } else {
        TEST_ASSERT_EQ(120, size);
        for (i = 0; i < size; ++i)
            TEST_ASSERT_EQ(i, *clist->at(state->list, i).s32);
    }
    list_alloc_check_source(state, 80, 1000);
    list_alloc_verify_usable(state);
}

static void* list_alloc_assign_setup(void)
{
    return list_alloc_state_new(LIST_NEW_T(int32_t), 40, 0);
}

static bool list_alloc_assign_operate(void* opaque)
{
    list_alloc_state_t* state = opaque;

    return clist->assign(state->list, 300, __ds_s32data(3));
}

static void list_alloc_assign_verify(void* opaque, bool succeeded)
{
    list_alloc_state_t* state = opaque;
    size_t size = (size_t)clist->size(state->list);
    size_t i;

    if (succeeded) {
        TEST_ASSERT_EQ(300, size);
        for (i = 0; i < size; ++i)
            TEST_ASSERT_EQ(3, *clist->at(state->list, i).s32);
    } else {
        TEST_ASSERT_EQ(40, size);
        for (i = 0; i < size; ++i)
            TEST_ASSERT_EQ(i, *clist->at(state->list, i).s32);
    }
    list_alloc_verify_usable(state);
}

static void* list_alloc_assign_range_setup(void)
{
    return list_alloc_range_setup_state(40, 300, 2000);
}

static bool list_alloc_assign_range_operate(void* opaque)
{
    list_alloc_state_t* state = opaque;

    return clist->assign_range(state->list, clist->begin(state->source),
                               clist->end(state->source));
}

static void list_alloc_assign_range_verify(void* opaque, bool succeeded)
{
    list_alloc_state_t* state = opaque;
    size_t size = (size_t)clist->size(state->list);
    size_t i;

    if (succeeded) {
        TEST_ASSERT_EQ(300, size);
        for (i = 0; i < size; ++i)
            TEST_ASSERT_EQ(2000 + (int32_t)i,
                           *clist->at(state->list, i).s32);
    } else {
        TEST_ASSERT_EQ(40, size);
        for (i = 0; i < size; ++i)
            TEST_ASSERT_EQ(i, *clist->at(state->list, i).s32);
    }
    list_alloc_check_source(state, 300, 2000);
    list_alloc_verify_usable(state);
}

static void* list_alloc_resize_setup(void)
{
    return list_alloc_state_new(LIST_NEW_T(int32_t), 40, 0);
}

static bool list_alloc_resize_operate(void* opaque)
{
    list_alloc_state_t* state = opaque;

    return clist->resize(state->list, 500, __ds_s32data(9));
}

static void list_alloc_resize_verify(void* opaque, bool succeeded)
{
    list_alloc_state_t* state = opaque;
    size_t size = (size_t)clist->size(state->list);
    size_t i;

    if (succeeded) {
        TEST_ASSERT_EQ(500, size);
        for (i = 0; i < 40; ++i)
            TEST_ASSERT_EQ(i, *clist->at(state->list, i).s32);
        for (i = 40; i < size; ++i)
            TEST_ASSERT_EQ(9, *clist->at(state->list, i).s32);
    } else {
        TEST_ASSERT_EQ(40, size);
        for (i = 0; i < size; ++i)
            TEST_ASSERT_EQ(i, *clist->at(state->list, i).s32);
    }
    list_alloc_verify_usable(state);
}

static void list_alloc_make_string(char* buffer, size_t size, size_t index)
{
    snprintf(buffer, size, "sso-value-%03zu-0123456789abcdef", index);
}

static void* list_alloc_sso_setup(void)
{
    return list_alloc_state_new(LIST_NEW_SSO(), 0, 0);
}

static bool list_alloc_sso_operate(void* opaque)
{
    list_alloc_state_t* state = opaque;
    size_t i;

    for (i = 0; i < 120; ++i) {
        char buffer[64];

        list_alloc_make_string(buffer, sizeof(buffer), i);
        if (!clist->push_back(state->list, ds_sdata(buffer)))
            return false;
        ++state->count;
    }
    return true;
}

static void list_alloc_sso_verify(void* opaque, bool succeeded)
{
    list_alloc_state_t* state = opaque;
    size_t i;

    TEST_ASSERT_EQ(state->count, (size_t)clist->size(state->list));
    for (i = 0; i < state->count; ++i) {
        char buffer[64];

        list_alloc_make_string(buffer, sizeof(buffer), i);
        TEST_ASSERT_STR_EQ(buffer, clist->at(state->list, i).ssd->p);
        TEST_ASSERT_EQ(strlen(buffer), clist->at(state->list, i).ssd->len);
    }
    if (succeeded)
        TEST_ASSERT_EQ(120, state->count);
    TEST_ASSERT_NULL(list_check_invariants(state->list));
    TEST_ASSERT_TRUE(clist->push_back(state->list, ds_sdata("tail")));
    TEST_ASSERT_EQ(state->count + 1, (size_t)clist->size(state->list));
    clist->pop_back(state->list);
    TEST_ASSERT_EQ(state->count, (size_t)clist->size(state->list));
}

static const test_alloc_scenario_t push_scenario = {
    .name = "list.push_back.growth",
    .setup = list_alloc_push_setup,
    .operate = list_alloc_push_operate,
    .verify = list_alloc_push_verify,
    .teardown = list_alloc_teardown,
};

static const test_alloc_scenario_t push_front_scenario = {
    .name = "list.push_front.growth",
    .setup = list_alloc_push_front_setup,
    .operate = list_alloc_push_front_operate,
    .verify = list_alloc_push_front_verify,
    .teardown = list_alloc_teardown,
};

static const test_alloc_scenario_t insert_scenario = {
    .name = "list.insert_n.middle",
    .setup = list_alloc_insert_setup,
    .operate = list_alloc_insert_operate,
    .verify = list_alloc_insert_verify,
    .teardown = list_alloc_teardown,
};

static const test_alloc_scenario_t insert_range_scenario = {
    .name = "list.insert_range.middle",
    .setup = list_alloc_insert_range_setup,
    .operate = list_alloc_insert_range_operate,
    .verify = list_alloc_insert_range_verify,
    .teardown = list_alloc_teardown,
};

static const test_alloc_scenario_t assign_scenario = {
    .name = "list.assign.growth",
    .setup = list_alloc_assign_setup,
    .operate = list_alloc_assign_operate,
    .verify = list_alloc_assign_verify,
    .teardown = list_alloc_teardown,
};

static const test_alloc_scenario_t assign_range_scenario = {
    .name = "list.assign_range.growth",
    .setup = list_alloc_assign_range_setup,
    .operate = list_alloc_assign_range_operate,
    .verify = list_alloc_assign_range_verify,
    .teardown = list_alloc_teardown,
};

static const test_alloc_scenario_t resize_scenario = {
    .name = "list.resize.growth",
    .setup = list_alloc_resize_setup,
    .operate = list_alloc_resize_operate,
    .verify = list_alloc_resize_verify,
    .teardown = list_alloc_teardown,
};

static const test_alloc_scenario_t sso_scenario = {
    .name = "list.sso_push.growth",
    .setup = list_alloc_sso_setup,
    .operate = list_alloc_sso_operate,
    .verify = list_alloc_sso_verify,
    .teardown = list_alloc_teardown,
};

#ifndef JDSC_DEBUG
/* Debug builds abort in `JDSC_ASSERT_NEW` when construction allocation fails. */
TEST_CASE(alloc, list_construction_failures)
{
    size_t attempt;

    for (attempt = 1; attempt <= 8; ++attempt) {
        list_t* list;

        test_alloc_fail_at(attempt);
        list = LIST_NEW_T(int32_t);
        test_alloc_disarm();
        test_trace("construct", (long long)attempt, list != NULL ? 1 : 0, 0);
        if (list != NULL)
            LIST_DELETE(&list);
        TEST_ASSERT_EQ(0, test_alloc_live_blocks());
    }
}
#endif /* !JDSC_DEBUG */

TEST_CASE(alloc, list_push_back_growth)
{
    test_alloc_sweep(&push_scenario);
}

TEST_CASE(alloc, list_push_front_growth)
{
    test_alloc_sweep(&push_front_scenario);
}

TEST_CASE(alloc, list_insert_n_middle)
{
    test_alloc_sweep(&insert_scenario);
}

TEST_CASE(alloc, list_insert_range_middle)
{
    test_alloc_sweep(&insert_range_scenario);
}

TEST_CASE(alloc, list_assign_growth)
{
    test_alloc_sweep(&assign_scenario);
}

TEST_CASE(alloc, list_assign_range_growth)
{
    test_alloc_sweep(&assign_range_scenario);
}

TEST_CASE(alloc, list_resize_growth)
{
    test_alloc_sweep(&resize_scenario);
}

TEST_CASE(alloc, list_sso_push_growth)
{
    test_alloc_sweep(&sso_scenario);
}
