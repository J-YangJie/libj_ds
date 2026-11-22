/*
  libj_ds permanent tests deque allocation failures
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

#include <deque/deque.h>
#include <_memory.h>

#include "../../core/test.h"
#include "value.h"

typedef struct deque_alloc_state {
    deque_t* deque;
    deque_t* source;
    size_t   count;
} deque_alloc_state_t;

static void deque_alloc_teardown(void* opaque)
{
    deque_alloc_state_t* state = opaque;

    DEQUE_DELETE(&state->source);
    DEQUE_DELETE(&state->deque);
    p_free(state);
}

static void* deque_alloc_new_state(size_t initial)
{
    deque_alloc_state_t* state;
    size_t i;

    state = p_calloc(1, sizeof(*state));
    if (state == NULL)
        return NULL;
    state->deque = DEQUE_NEW_T(int32_t);
    if (state->deque == NULL) {
        p_free(state);
        return NULL;
    }
    for (i = 0; i < initial; ++i) {
        if (!cdeque->push_back(state->deque, __ds_s32data((int32_t)i))) {
            DEQUE_DELETE(&state->deque);
            p_free(state);
            return NULL;
        }
        ++state->count;
    }
    return state;
}

static void* deque_alloc_push_setup(void)
{
    return deque_alloc_new_state(0);
}

static bool deque_alloc_push_operate(void* opaque)
{
    deque_alloc_state_t* state = opaque;
    size_t i;

    for (i = 0; i < 400; ++i) {
        if (!cdeque->push_back(state->deque, __ds_s32data((int32_t)i)))
            return false;
        ++state->count;
    }
    return true;
}

static void deque_alloc_verify_usable(deque_alloc_state_t* state)
{
    deque_size_t size = cdeque->size(state->deque);

    TEST_ASSERT_NULL(deque_check_invariants(state->deque));
    TEST_ASSERT_TRUE(cdeque->push_back(state->deque, __ds_s32data(-1)));
    TEST_ASSERT_EQ(size + 1, cdeque->size(state->deque));
    cdeque->pop_back(state->deque);
    TEST_ASSERT_EQ(size, cdeque->size(state->deque));
}

static void deque_alloc_push_verify(void* opaque, bool succeeded)
{
    deque_alloc_state_t* state = opaque;
    size_t i;

    TEST_ASSERT_EQ(state->count, (size_t)cdeque->size(state->deque));
    for (i = 0; i < state->count; ++i) {
        TEST_ASSERT_EQ(i, *cdeque->at(state->deque, i).s32);
    }
    if (succeeded)
        TEST_ASSERT_EQ(400, state->count);
    deque_alloc_verify_usable(state);
}

static void* deque_alloc_insert_setup(void)
{
    return deque_alloc_new_state(200);
}

static bool deque_alloc_insert_operate(void* opaque)
{
    deque_alloc_state_t* state = opaque;
    deque_iterator_t ret;

    ret = cdeque->insert_n(state->deque, cdeque->begin(state->deque), 150,
                           __ds_s32data(7));
    return !i_deque_is_null_iterator(ret);
}

static void deque_alloc_insert_verify(void* opaque, bool succeeded)
{
    deque_alloc_state_t* state = opaque;
    size_t size = (size_t)cdeque->size(state->deque);

    if (succeeded) {
        size_t i;

        TEST_ASSERT_EQ(350, size);
        for (i = 0; i < 150; ++i)
            TEST_ASSERT_EQ(7, *cdeque->at(state->deque, i).s32);
        for (i = 0; i < 200; ++i)
            TEST_ASSERT_EQ(i, *cdeque->at(state->deque, 150 + i).s32);
    } else {
        TEST_ASSERT_EQ(200, size);
        TEST_ASSERT_EQ(0, *cdeque->at(state->deque, 0).s32);
        TEST_ASSERT_EQ(199, *cdeque->at(state->deque, 199).s32);
    }
    deque_alloc_verify_usable(state);
}

static void* deque_alloc_assign_setup(void)
{
    return deque_alloc_new_state(50);
}

static bool deque_alloc_assign_operate(void* opaque)
{
    deque_alloc_state_t* state = opaque;

    return cdeque->assign(state->deque, 600, __ds_s32data(3));
}

static void deque_alloc_assign_verify(void* opaque, bool succeeded)
{
    deque_alloc_state_t* state = opaque;
    size_t size = (size_t)cdeque->size(state->deque);
    size_t i;

    if (succeeded) {
        TEST_ASSERT_EQ(600, size);
        for (i = 0; i < size; ++i)
            TEST_ASSERT_EQ(3, *cdeque->at(state->deque, i).s32);
    } else {
        TEST_ASSERT_EQ(50, size);
        for (i = 0; i < size; ++i)
            TEST_ASSERT_EQ(i, *cdeque->at(state->deque, i).s32);
    }
    deque_alloc_verify_usable(state);
}

static void* deque_alloc_resize_setup(void)
{
    return deque_alloc_new_state(50);
}

static bool deque_alloc_resize_operate(void* opaque)
{
    deque_alloc_state_t* state = opaque;

    return cdeque->resize(state->deque, 1000, __ds_s32data(9));
}

static void deque_alloc_resize_verify(void* opaque, bool succeeded)
{
    deque_alloc_state_t* state = opaque;
    size_t size = (size_t)cdeque->size(state->deque);
    size_t i;

    if (succeeded) {
        TEST_ASSERT_EQ(1000, size);
        for (i = 0; i < 50; ++i)
            TEST_ASSERT_EQ(i, *cdeque->at(state->deque, i).s32);
        for (i = 50; i < size; ++i)
            TEST_ASSERT_EQ(9, *cdeque->at(state->deque, i).s32);
    } else {
        TEST_ASSERT_EQ(50, size);
    }
    deque_alloc_verify_usable(state);
}

static bool deque_alloc_build_source(deque_t** source, size_t n, int32_t base)
{
    size_t i;

    *source = DEQUE_NEW_T(int32_t);
    if (*source == NULL)
        return false;
    for (i = 0; i < n; ++i) {
        if (!cdeque->push_back(*source, __ds_s32data(base + (int32_t)i))) {
            DEQUE_DELETE(source);
            return false;
        }
    }
    return true;
}

static void* deque_alloc_range_state(size_t initial, size_t source_n,
                                     int32_t base)
{
    deque_alloc_state_t* state = deque_alloc_new_state(initial);

    if (state == NULL)
        return NULL;
    if (!deque_alloc_build_source(&state->source, source_n, base)) {
        DEQUE_DELETE(&state->deque);
        p_free(state);
        return NULL;
    }
    return state;
}

static void* deque_alloc_insert_range_setup(void)
{
    return deque_alloc_range_state(200, 150, 10000);
}

static bool deque_alloc_insert_range_operate(void* opaque)
{
    deque_alloc_state_t* state = opaque;
    deque_iterator_t ret;

    ret = cdeque->insert_range(state->deque, cdeque->it(state->deque, 100),
                               cdeque->begin(state->source),
                               cdeque->end(state->source));
    return !i_deque_is_null_iterator(ret);
}

static void deque_alloc_insert_range_verify(void* opaque, bool succeeded)
{
    deque_alloc_state_t* state = opaque;
    size_t size = (size_t)cdeque->size(state->deque);
    size_t i;

    if (succeeded) {
        TEST_ASSERT_EQ(350, size);
        for (i = 0; i < 100; ++i)
            TEST_ASSERT_EQ(i, *cdeque->at(state->deque, i).s32);
        for (i = 0; i < 150; ++i)
            TEST_ASSERT_EQ(10000 + i, *cdeque->at(state->deque, 100 + i).s32);
        for (i = 0; i < 100; ++i)
            TEST_ASSERT_EQ(100 + i, *cdeque->at(state->deque, 250 + i).s32);
    } else {
        TEST_ASSERT_EQ(200, size);
        for (i = 0; i < size; ++i)
            TEST_ASSERT_EQ(i, *cdeque->at(state->deque, i).s32);
    }
    deque_alloc_verify_usable(state);
}

static void* deque_alloc_assign_range_setup(void)
{
    return deque_alloc_range_state(50, 600, 20000);
}

static bool deque_alloc_assign_range_operate(void* opaque)
{
    deque_alloc_state_t* state = opaque;

    return cdeque->assign_range(state->deque, cdeque->begin(state->source),
                                cdeque->end(state->source));
}

static void deque_alloc_assign_range_verify(void* opaque, bool succeeded)
{
    deque_alloc_state_t* state = opaque;
    size_t size = (size_t)cdeque->size(state->deque);
    size_t i;

    if (succeeded) {
        TEST_ASSERT_EQ(600, size);
        for (i = 0; i < size; ++i)
            TEST_ASSERT_EQ(20000 + i, *cdeque->at(state->deque, i).s32);
    } else {
        TEST_ASSERT_EQ(50, size);
        for (i = 0; i < size; ++i)
            TEST_ASSERT_EQ(i, *cdeque->at(state->deque, i).s32);
    }
    deque_alloc_verify_usable(state);
}

static void* deque_alloc_shrink_setup(void)
{
    return deque_alloc_new_state(1000);
}

static bool deque_alloc_shrink_operate(void* opaque)
{
    deque_alloc_state_t* state = opaque;

    cdeque->shrink_to_fit(state->deque);
    return true;
}

static void deque_alloc_shrink_verify(void* opaque, bool succeeded)
{
    deque_alloc_state_t* state = opaque;
    size_t i;

    (void)succeeded;
    TEST_ASSERT_EQ(1000, cdeque->size(state->deque));
    for (i = 0; i < 1000; ++i)
        TEST_ASSERT_EQ(i, *cdeque->at(state->deque, i).s32);
    deque_alloc_verify_usable(state);
}

typedef struct deque_sso_alloc_state {
    deque_t*    deque;
    const char* alias;
} deque_sso_alloc_state_t;

static const char* const deque_sso_alloc_strings[6] = {
    "abcdefghijklmnop", "bbbb", "cccc", "dddd", "eeee", "ffff"
};

static void deque_sso_alloc_teardown(void* opaque)
{
    deque_sso_alloc_state_t* state = opaque;

    DEQUE_DELETE(&state->deque);
    p_free(state);
}

static void* deque_sso_alloc_setup(void)
{
    deque_sso_alloc_state_t* state = p_calloc(1, sizeof(*state));
    size_t i;

    if (state == NULL)
        return NULL;
    state->deque = DEQUE_NEW_SSO();
    if (state->deque == NULL) {
        p_free(state);
        return NULL;
    }
    for (i = 0; i < 6; ++i) {
        if (!cdeque->push_back(state->deque,
                               ds_sdata(deque_sso_alloc_strings[i]))) {
            DEQUE_DELETE(&state->deque);
            p_free(state);
            return NULL;
        }
    }
    state->alias = cdeque->at(state->deque, 0).ssd->p;
    return state;
}

static bool deque_sso_alloc_operate(void* opaque)
{
    deque_sso_alloc_state_t* state = opaque;
    deque_iterator_t ret;

    ret = cdeque->insert_n(state->deque, cdeque->it(state->deque, 2), 3,
                           ds_sdata(state->alias));
    return !i_deque_is_null_iterator(ret);
}

static void deque_sso_alloc_verify_usable(deque_sso_alloc_state_t* state)
{
    deque_size_t size = cdeque->size(state->deque);

    TEST_ASSERT_NULL(deque_check_invariants(state->deque));
    TEST_ASSERT_TRUE(cdeque->push_back(state->deque, ds_sdata("zzzz")));
    TEST_ASSERT_EQ(size + 1, cdeque->size(state->deque));
    cdeque->pop_back(state->deque);
    TEST_ASSERT_EQ(size, cdeque->size(state->deque));
}

static void deque_sso_alloc_verify(void* opaque, bool succeeded)
{
    static const char* const success[9] = {
        "abcdefghijklmnop", "bbbb", "abcdefghijklmnop",
        "abcdefghijklmnop", "abcdefghijklmnop", "cccc", "dddd", "eeee",
        "ffff"
    };
    static const char* const failure[6] = {
        "abcdefghijklmnop", "bbbb", "cccc", "dddd", "eeee", "ffff"
    };
    deque_sso_alloc_state_t* state = opaque;
    const char* const* values = succeeded ? success : failure;
    size_t count = succeeded ? 9 : 6;
    size_t i;

    TEST_ASSERT_EQ(count, (size_t)cdeque->size(state->deque));
    for (i = 0; i < count; ++i)
        TEST_ASSERT_STR_EQ(values[i], cdeque->at(state->deque, i).ssd->p);
    deque_sso_alloc_verify_usable(state);
}

static const test_alloc_scenario_t push_scenario = {
    .name = "deque.push_back.growth",
    .setup = deque_alloc_push_setup,
    .operate = deque_alloc_push_operate,
    .verify = deque_alloc_push_verify,
    .teardown = deque_alloc_teardown,
};

static const test_alloc_scenario_t insert_scenario = {
    .name = "deque.insert_n.front",
    .setup = deque_alloc_insert_setup,
    .operate = deque_alloc_insert_operate,
    .verify = deque_alloc_insert_verify,
    .teardown = deque_alloc_teardown,
};

static const test_alloc_scenario_t assign_scenario = {
    .name = "deque.assign.growth",
    .setup = deque_alloc_assign_setup,
    .operate = deque_alloc_assign_operate,
    .verify = deque_alloc_assign_verify,
    .teardown = deque_alloc_teardown,
};

static const test_alloc_scenario_t resize_scenario = {
    .name = "deque.resize.growth",
    .setup = deque_alloc_resize_setup,
    .operate = deque_alloc_resize_operate,
    .verify = deque_alloc_resize_verify,
    .teardown = deque_alloc_teardown,
};

static const test_alloc_scenario_t insert_range_scenario = {
    .name = "deque.insert_range.middle",
    .setup = deque_alloc_insert_range_setup,
    .operate = deque_alloc_insert_range_operate,
    .verify = deque_alloc_insert_range_verify,
    .teardown = deque_alloc_teardown,
};

static const test_alloc_scenario_t assign_range_scenario = {
    .name = "deque.assign_range.growth",
    .setup = deque_alloc_assign_range_setup,
    .operate = deque_alloc_assign_range_operate,
    .verify = deque_alloc_assign_range_verify,
    .teardown = deque_alloc_teardown,
};

static const test_alloc_scenario_t self_reference_scenario = {
    .name = "deque.insert_n.self_reference",
    .setup = deque_sso_alloc_setup,
    .operate = deque_sso_alloc_operate,
    .verify = deque_sso_alloc_verify,
    .teardown = deque_sso_alloc_teardown,
};

static const test_alloc_scenario_t shrink_scenario = {
    .name = "deque.shrink_to_fit",
    .setup = deque_alloc_shrink_setup,
    .operate = deque_alloc_shrink_operate,
    .verify = deque_alloc_shrink_verify,
    .teardown = deque_alloc_teardown,
};

#ifndef JDSC_DEBUG
/* Debug builds abort in `JDSC_ASSERT_NEW` when construction allocation fails. */
TEST_CASE(alloc, deque_construction_failures)
{
    size_t attempt;

    for (attempt = 1; attempt <= 8; ++attempt) {
        deque_t* deque;

        test_alloc_fail_at(attempt);
        deque = DEQUE_NEW_T(int32_t);
        test_alloc_disarm();
        test_trace("construct", (long long)attempt, deque != NULL ? 1 : 0, 0);
        if (deque != NULL)
            DEQUE_DELETE(&deque);
        TEST_ASSERT_EQ(0, test_alloc_live_blocks());
    }
}
#endif /* !JDSC_DEBUG */

TEST_CASE(alloc, deque_push_growth)
{
    test_alloc_sweep(&push_scenario);
}

TEST_CASE(alloc, deque_insert_front)
{
    test_alloc_sweep(&insert_scenario);
}

TEST_CASE(alloc, deque_assign_growth)
{
    test_alloc_sweep(&assign_scenario);
}

TEST_CASE(alloc, deque_resize_growth)
{
    test_alloc_sweep(&resize_scenario);
}

TEST_CASE(alloc, deque_insert_range)
{
    test_alloc_sweep(&insert_range_scenario);
}

TEST_CASE(alloc, deque_assign_range)
{
    test_alloc_sweep(&assign_range_scenario);
}

TEST_CASE(alloc, deque_shrink_to_fit)
{
    test_alloc_sweep(&shrink_scenario);
}

TEST_CASE(alloc, deque_self_reference)
{
    test_alloc_sweep(&self_reference_scenario);
}
