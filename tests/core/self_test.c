/*
  libj_ds permanent tests self tests
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

#include <stdint.h>
#include <string.h>

#include "test_internal.h"

void* _p_malloc(size_t size);
void* _p_calloc(size_t nmemb, size_t size);
void* _p_realloc(void* ptr, size_t size);
void  _p_free(void** ptr);

TEST_CASE(selftest, rng_is_reproducible)
{
    uint64_t first[8];
    int i;

    test_rng_init(0x1234567890abcdefULL);
    for (i = 0; i < 8; ++i)
        first[i] = test_rng_u64();
    test_rng_init(0x1234567890abcdefULL);
    for (i = 0; i < 8; ++i)
        TEST_ASSERT_EQ(first[i], test_rng_u64());
}

TEST_CASE(selftest, rng_ranges_are_bounded)
{
    int i;

    test_rng_init(0x0f0f0f0f0f0f0f0fULL);
    for (i = 0; i < 1000; ++i) {
        size_t value = test_rng_bounded(7);
        size_t ranged = test_rng_range(10, 20);

        TEST_ASSERT_TRUE(value < 7);
        TEST_ASSERT_TRUE(ranged >= 10 && ranged <= 20);
        TEST_ASSERT_EQ(0, test_rng_bounded(0));
    }
}

TEST_CASE(selftest, trace_wraps)
{
    int i;

    test_trace_reset();
    for (i = 0; i < 300; ++i)
        test_trace("op", i, i + 1, i + 2);
    TEST_ASSERT_EQ(32, test_trace_limit());
}

TEST_CASE(selftest, allocator_accounts_blocks)
{
    void* a;
    void* b;
    size_t base_blocks = test_alloc_live_blocks();
    size_t base_bytes = test_alloc_live_bytes();

    test_alloc_reset_stats();
    a = _p_malloc(16);
    b = _p_malloc(32);
    TEST_ASSERT_NOT_NULL(a);
    TEST_ASSERT_NOT_NULL(b);
    TEST_ASSERT_EQ(2, test_alloc_live_blocks());
    TEST_ASSERT_EQ(48, test_alloc_live_bytes());
    TEST_ASSERT_EQ(2, test_alloc_peak_blocks());

    _p_free(&a);
    _p_free(&b);
    TEST_ASSERT_NULL(a);
    TEST_ASSERT_NULL(b);
    TEST_ASSERT_EQ(0, test_alloc_live_blocks());
    TEST_ASSERT_EQ(0, test_alloc_live_bytes());

    test_alloc_reset_stats();
    TEST_ASSERT_EQ(base_blocks, test_alloc_live_blocks());
    TEST_ASSERT_EQ(base_bytes, test_alloc_live_bytes());
}

TEST_CASE(selftest, allocator_fail_at_n)
{
    void* ptr;

    test_alloc_fail_at(1);
    ptr = _p_malloc(8);
    TEST_ASSERT_NULL(ptr);

    ptr = _p_malloc(8);
    TEST_ASSERT_NOT_NULL(ptr);
    test_alloc_disarm();
    _p_free(&ptr);
}

TEST_CASE(selftest, realloc_preserves_on_failure)
{
    unsigned char* ptr = _p_malloc(8);
    unsigned char* grown;

    TEST_ASSERT_NOT_NULL(ptr);
    memset(ptr, 0x5a, 8);

    test_alloc_fail_at(1);
    grown = _p_realloc(ptr, 64);
    TEST_ASSERT_NULL(grown);
    test_alloc_disarm();
    TEST_ASSERT_EQ(0x5a, ptr[0]);
    TEST_ASSERT_EQ(0x5a, ptr[7]);

    grown = _p_realloc(ptr, 64);
    TEST_ASSERT_NOT_NULL(grown);
    TEST_ASSERT_EQ(0x5a, grown[0]);
    TEST_ASSERT_EQ(0x5a, grown[7]);
    {
        void* raw = grown;

        _p_free(&raw);
    }
}

TEST_CASE(selftest, calloc_zeroes_and_checks_overflow)
{
    void* ptr = _p_calloc(4, 8);
    const unsigned char* bytes = ptr;
    size_t i;

    TEST_ASSERT_NOT_NULL(ptr);
    for (i = 0; i < 32; ++i)
        TEST_ASSERT_EQ(0, bytes[i]);
    _p_free(&ptr);

    TEST_ASSERT_NULL(_p_calloc(SIZE_MAX, 2));
}

typedef struct sweep_state {
    void* base;
    void* first;
    void* second;
} sweep_state_t;

static void* sweep_setup(void)
{
    sweep_state_t* state = _p_malloc(sizeof(*state));

    if (state == NULL)
        return NULL;
    state->base = state;
    state->first = NULL;
    state->second = NULL;
    return state;
}

static bool sweep_operate(void* opaque)
{
    sweep_state_t* state = opaque;

    state->first = _p_malloc(16);
    if (state->first == NULL)
        return false;
    state->second = _p_malloc(16);
    if (state->second == NULL) {
        _p_free(&state->first);
        return false;
    }
    return true;
}

static void sweep_verify(void* opaque, bool succeeded)
{
    sweep_state_t* state = opaque;

    if (succeeded) {
        TEST_ASSERT_NOT_NULL(state->first);
        TEST_ASSERT_NOT_NULL(state->second);
    } else {
        TEST_ASSERT_TRUE(state->first == NULL || state->second == NULL);
    }
}

static void sweep_teardown(void* opaque)
{
    sweep_state_t* state = opaque;
    void* base = state->base;

    _p_free(&state->first);
    _p_free(&state->second);
    _p_free(&base);
}

TEST_CASE(selftest, allocator_sweep_machinery)
{
    static const test_alloc_scenario_t scenario = {
        .name = "self_check",
        .setup = sweep_setup,
        .operate = sweep_operate,
        .verify = sweep_verify,
        .teardown = sweep_teardown,
    };

    test_alloc_sweep(&scenario);
}

TEST_CASE(selftest, assertions_accept_matching_values)
{
    const char* text = "same";

    TEST_ASSERT_TRUE(true);
    TEST_ASSERT_FALSE(false);
    TEST_ASSERT_EQ(42, 42);
    TEST_ASSERT_NE(1, 2);
    TEST_ASSERT_NULL(NULL);
    TEST_ASSERT_NOT_NULL(text);
    TEST_ASSERT_PTR_EQ(text, text);
    TEST_ASSERT_STR_EQ("text", "text");
    TEST_ASSERT_MEM_EQ("abcd", "abcd", 4);
    TEST_ASSERT_OK(1 == 1);
}
