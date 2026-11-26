/*
  libj_ds permanent tests vector allocation failures
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

#include <vector/vector.h>
#include <_memory.h>

#include "../../core/test.h"
#include "value.h"

typedef struct vector_alloc_state {
    vector_t* vector;
    vector_t* source;
    size_t    count;
} vector_alloc_state_t;

static void vector_alloc_teardown(void* opaque)
{
    vector_alloc_state_t* state = opaque;

    VECTOR_DELETE(&state->source);
    VECTOR_DELETE(&state->vector);
    p_free(state);
}

static void* vector_alloc_new_state(size_t initial)
{
    vector_alloc_state_t* state;
    size_t i;

    state = p_calloc(1, sizeof(*state));
    if (state == NULL)
        return NULL;
    state->vector = VECTOR_NEW_T(int32_t);
    if (state->vector == NULL) {
        p_free(state);
        return NULL;
    }
    for (i = 0; i < initial; ++i) {
        if (!cvector->push_back(state->vector, __ds_s32data((int32_t)i))) {
            VECTOR_DELETE(&state->vector);
            p_free(state);
            return NULL;
        }
        ++state->count;
    }
    return state;
}

static void* vector_alloc_push_setup(void)
{
    return vector_alloc_new_state(0);
}

static bool vector_alloc_push_operate(void* opaque)
{
    vector_alloc_state_t* state = opaque;
    size_t i;

    for (i = 0; i < 400; ++i) {
        if (!cvector->push_back(state->vector, __ds_s32data((int32_t)i)))
            return false;
        ++state->count;
    }
    return true;
}

static void vector_alloc_verify_usable(vector_alloc_state_t* state)
{
    vector_size_t size = cvector->size(state->vector);

    TEST_ASSERT_NULL(vector_check_invariants(state->vector));
    TEST_ASSERT_TRUE(cvector->push_back(state->vector, __ds_s32data(-1)));
    TEST_ASSERT_EQ(size + 1, cvector->size(state->vector));
    cvector->pop_back(state->vector);
    TEST_ASSERT_EQ(size, cvector->size(state->vector));
}

static void vector_alloc_push_verify(void* opaque, bool succeeded)
{
    vector_alloc_state_t* state = opaque;
    size_t i;

    TEST_ASSERT_EQ(state->count, (size_t)cvector->size(state->vector));
    for (i = 0; i < state->count; ++i) {
        TEST_ASSERT_EQ(i, *cvector->at(state->vector, i).s32);
    }
    if (succeeded)
        TEST_ASSERT_EQ(400, state->count);
    vector_alloc_verify_usable(state);
}

static void* vector_alloc_insert_setup(void)
{
    return vector_alloc_new_state(200);
}

static bool vector_alloc_insert_operate(void* opaque)
{
    vector_alloc_state_t* state = opaque;
    vector_iterator_t ret;

    ret = cvector->insert_n(state->vector, cvector->begin(state->vector),
                            150, __ds_s32data(7));
    return !i_vector_is_null_iterator(ret);
}

static void vector_alloc_insert_verify(void* opaque, bool succeeded)
{
    vector_alloc_state_t* state = opaque;
    size_t size = (size_t)cvector->size(state->vector);

    if (succeeded) {
        size_t i;

        TEST_ASSERT_EQ(350, size);
        for (i = 0; i < 150; ++i)
            TEST_ASSERT_EQ(7, *cvector->at(state->vector, i).s32);
        for (i = 0; i < 200; ++i)
            TEST_ASSERT_EQ(i, *cvector->at(state->vector, 150 + i).s32);
    } else {
        TEST_ASSERT_EQ(200, size);
        TEST_ASSERT_EQ(0, *cvector->at(state->vector, 0).s32);
        TEST_ASSERT_EQ(199, *cvector->at(state->vector, 199).s32);
    }
    vector_alloc_verify_usable(state);
}

static void* vector_alloc_assign_setup(void)
{
    return vector_alloc_new_state(50);
}

static bool vector_alloc_assign_operate(void* opaque)
{
    vector_alloc_state_t* state = opaque;

    return cvector->assign(state->vector, 600, __ds_s32data(3));
}

static void vector_alloc_assign_verify(void* opaque, bool succeeded)
{
    vector_alloc_state_t* state = opaque;
    size_t size = (size_t)cvector->size(state->vector);
    size_t i;

    if (succeeded) {
        TEST_ASSERT_EQ(600, size);
        for (i = 0; i < size; ++i)
            TEST_ASSERT_EQ(3, *cvector->at(state->vector, i).s32);
    } else {
        TEST_ASSERT_EQ(50, size);
        for (i = 0; i < size; ++i)
            TEST_ASSERT_EQ(i, *cvector->at(state->vector, i).s32);
    }
    vector_alloc_verify_usable(state);
}

static void* vector_alloc_resize_setup(void)
{
    return vector_alloc_new_state(50);
}

static bool vector_alloc_resize_operate(void* opaque)
{
    vector_alloc_state_t* state = opaque;

    return cvector->resize(state->vector, 1000, __ds_s32data(9));
}

static void vector_alloc_resize_verify(void* opaque, bool succeeded)
{
    vector_alloc_state_t* state = opaque;
    size_t size = (size_t)cvector->size(state->vector);
    size_t i;

    if (succeeded) {
        TEST_ASSERT_EQ(1000, size);
        for (i = 0; i < 50; ++i)
            TEST_ASSERT_EQ(i, *cvector->at(state->vector, i).s32);
        for (i = 50; i < size; ++i)
            TEST_ASSERT_EQ(9, *cvector->at(state->vector, i).s32);
    } else {
        TEST_ASSERT_EQ(50, size);
    }
    vector_alloc_verify_usable(state);
}

static bool vector_alloc_build_source(vector_t** source, size_t n, int32_t base)
{
    size_t i;

    *source = VECTOR_NEW_T(int32_t);
    if (*source == NULL)
        return false;
    for (i = 0; i < n; ++i) {
        if (!cvector->push_back(*source, __ds_s32data(base + (int32_t)i))) {
            VECTOR_DELETE(source);
            return false;
        }
    }
    return true;
}

static void* vector_alloc_range_state(size_t initial, size_t source_n,
                                      int32_t base)
{
    vector_alloc_state_t* state = vector_alloc_new_state(initial);

    if (state == NULL)
        return NULL;
    if (!vector_alloc_build_source(&state->source, source_n, base)) {
        VECTOR_DELETE(&state->vector);
        p_free(state);
        return NULL;
    }
    return state;
}

static void* vector_alloc_insert_range_setup(void)
{
    return vector_alloc_range_state(200, 150, 10000);
}

static bool vector_alloc_insert_range_operate(void* opaque)
{
    vector_alloc_state_t* state = opaque;
    vector_iterator_t ret;

    ret = cvector->insert_range(state->vector,
                                cvector->it(state->vector, 100),
                                cvector->begin(state->source),
                                cvector->end(state->source));
    return !i_vector_is_null_iterator(ret);
}

static void vector_alloc_insert_range_verify(void* opaque, bool succeeded)
{
    vector_alloc_state_t* state = opaque;
    size_t size = (size_t)cvector->size(state->vector);
    size_t i;

    if (succeeded) {
        TEST_ASSERT_EQ(350, size);
        for (i = 0; i < 100; ++i)
            TEST_ASSERT_EQ(i, *cvector->at(state->vector, i).s32);
        for (i = 0; i < 150; ++i)
            TEST_ASSERT_EQ(10000 + i,
                           *cvector->at(state->vector, 100 + i).s32);
        for (i = 0; i < 100; ++i)
            TEST_ASSERT_EQ(100 + i, *cvector->at(state->vector, 250 + i).s32);
    } else {
        TEST_ASSERT_EQ(200, size);
        for (i = 0; i < size; ++i)
            TEST_ASSERT_EQ(i, *cvector->at(state->vector, i).s32);
    }
    vector_alloc_verify_usable(state);
}

static void* vector_alloc_assign_range_setup(void)
{
    return vector_alloc_range_state(50, 600, 20000);
}

static bool vector_alloc_assign_range_operate(void* opaque)
{
    vector_alloc_state_t* state = opaque;

    return cvector->assign_range(state->vector,
                                 cvector->begin(state->source),
                                 cvector->end(state->source));
}

static void vector_alloc_assign_range_verify(void* opaque, bool succeeded)
{
    vector_alloc_state_t* state = opaque;
    size_t size = (size_t)cvector->size(state->vector);
    size_t i;

    if (succeeded) {
        TEST_ASSERT_EQ(600, size);
        for (i = 0; i < size; ++i)
            TEST_ASSERT_EQ(20000 + i, *cvector->at(state->vector, i).s32);
    } else {
        TEST_ASSERT_EQ(50, size);
        for (i = 0; i < size; ++i)
            TEST_ASSERT_EQ(i, *cvector->at(state->vector, i).s32);
    }
    vector_alloc_verify_usable(state);
}

static void* vector_alloc_reserve_setup(void)
{
    return vector_alloc_new_state(50);
}

static bool vector_alloc_reserve_operate(void* opaque)
{
    vector_alloc_state_t* state = opaque;

    return cvector->reserve(state->vector, 500);
}

static void vector_alloc_reserve_verify(void* opaque, bool succeeded)
{
    vector_alloc_state_t* state = opaque;
    size_t i;

    TEST_ASSERT_EQ(50, cvector->size(state->vector));
    for (i = 0; i < 50; ++i)
        TEST_ASSERT_EQ(i, *cvector->at(state->vector, i).s32);
    if (succeeded)
        TEST_ASSERT_TRUE(cvector->capacity(state->vector) >= 500);
    TEST_ASSERT_TRUE(cvector->capacity(state->vector)
                     >= cvector->size(state->vector));
    vector_alloc_verify_usable(state);
}

static void* vector_alloc_shrink_setup(void)
{
    return vector_alloc_new_state(1000);
}

static bool vector_alloc_shrink_operate(void* opaque)
{
    vector_alloc_state_t* state = opaque;

    cvector->shrink_to_fit(state->vector);
    return true;
}

static void vector_alloc_shrink_verify(void* opaque, bool succeeded)
{
    vector_alloc_state_t* state = opaque;
    size_t i;

    (void)succeeded;
    TEST_ASSERT_EQ(1000, cvector->size(state->vector));
    for (i = 0; i < 1000; ++i)
        TEST_ASSERT_EQ(i, *cvector->at(state->vector, i).s32);
    TEST_ASSERT_TRUE(cvector->capacity(state->vector) >= 1000);
    vector_alloc_verify_usable(state);
}

static const test_alloc_scenario_t push_scenario = {
    .name = "vector.push_back.growth",
    .setup = vector_alloc_push_setup,
    .operate = vector_alloc_push_operate,
    .verify = vector_alloc_push_verify,
    .teardown = vector_alloc_teardown,
};

static const test_alloc_scenario_t insert_scenario = {
    .name = "vector.insert_n.front",
    .setup = vector_alloc_insert_setup,
    .operate = vector_alloc_insert_operate,
    .verify = vector_alloc_insert_verify,
    .teardown = vector_alloc_teardown,
};

static const test_alloc_scenario_t assign_scenario = {
    .name = "vector.assign.growth",
    .setup = vector_alloc_assign_setup,
    .operate = vector_alloc_assign_operate,
    .verify = vector_alloc_assign_verify,
    .teardown = vector_alloc_teardown,
};

static const test_alloc_scenario_t resize_scenario = {
    .name = "vector.resize.growth",
    .setup = vector_alloc_resize_setup,
    .operate = vector_alloc_resize_operate,
    .verify = vector_alloc_resize_verify,
    .teardown = vector_alloc_teardown,
};

static const test_alloc_scenario_t insert_range_scenario = {
    .name = "vector.insert_range.middle",
    .setup = vector_alloc_insert_range_setup,
    .operate = vector_alloc_insert_range_operate,
    .verify = vector_alloc_insert_range_verify,
    .teardown = vector_alloc_teardown,
};

static const test_alloc_scenario_t assign_range_scenario = {
    .name = "vector.assign_range.growth",
    .setup = vector_alloc_assign_range_setup,
    .operate = vector_alloc_assign_range_operate,
    .verify = vector_alloc_assign_range_verify,
    .teardown = vector_alloc_teardown,
};

static const test_alloc_scenario_t reserve_scenario = {
    .name = "vector.reserve.growth",
    .setup = vector_alloc_reserve_setup,
    .operate = vector_alloc_reserve_operate,
    .verify = vector_alloc_reserve_verify,
    .teardown = vector_alloc_teardown,
};

static const test_alloc_scenario_t shrink_scenario = {
    .name = "vector.shrink_to_fit",
    .setup = vector_alloc_shrink_setup,
    .operate = vector_alloc_shrink_operate,
    .verify = vector_alloc_shrink_verify,
    .teardown = vector_alloc_teardown,
};

#ifndef JDSC_DEBUG
/* Debug builds abort in `JDSC_ASSERT_NEW` when construction allocation fails. */
TEST_CASE(alloc, vector_construction_failures)
{
    size_t attempt;

    for (attempt = 1; attempt <= 8; ++attempt) {
        vector_t* vector;

        test_alloc_fail_at(attempt);
        vector = VECTOR_NEW_T(int32_t);
        test_alloc_disarm();
        test_trace("construct", (long long)attempt, vector != NULL ? 1 : 0, 0);
        if (vector != NULL)
            VECTOR_DELETE(&vector);
        TEST_ASSERT_EQ(0, test_alloc_live_blocks());
    }
}
#endif /* !JDSC_DEBUG */

TEST_CASE(alloc, vector_push_growth)
{
    test_alloc_sweep(&push_scenario);
}

TEST_CASE(alloc, vector_insert_front)
{
    test_alloc_sweep(&insert_scenario);
}

TEST_CASE(alloc, vector_assign_growth)
{
    test_alloc_sweep(&assign_scenario);
}

TEST_CASE(alloc, vector_resize_growth)
{
    test_alloc_sweep(&resize_scenario);
}

TEST_CASE(alloc, vector_insert_range)
{
    test_alloc_sweep(&insert_range_scenario);
}

TEST_CASE(alloc, vector_assign_range)
{
    test_alloc_sweep(&assign_range_scenario);
}

TEST_CASE(alloc, vector_reserve_growth)
{
    test_alloc_sweep(&reserve_scenario);
}

TEST_CASE(alloc, vector_shrink_to_fit)
{
    test_alloc_sweep(&shrink_scenario);
}
