/*
  libj_ds permanent tests deque benchmark
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

#ifndef _POSIX_C_SOURCE
#define _POSIX_C_SOURCE 200809L
#endif

#include <deque/deque.h>

#include <stdio.h>
#include <time.h>

#include "../../core/test.h"

#define BENCH_ITERS 200000

static void deque_free(void* self)
{
    deque_t* deque = self;

    DEQUE_DELETE(&deque);
}

static double monotonic_ms(void)
{
    struct timespec ts;

    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (double)ts.tv_sec * 1000.0 + (double)ts.tv_nsec / 1000000.0;
}

TEST_CASE(bench, deque_push_pop)
{
    deque_t* deque = DEQUE_NEW_T(int32_t);
    double start;
    double elapsed;
    size_t i;

    TEST_ASSERT_NOT_NULL(deque);
    test_cleanup_push(deque_free, deque);

    start = monotonic_ms();
    for (i = 0; i < BENCH_ITERS; ++i) {
        if (!cdeque->push_back(deque, __ds_s32data((int32_t)i)))
            TEST_FAIL("Push failed at iteration %zu.", i);
    }
    for (i = 0; i < BENCH_ITERS; ++i)
        cdeque->pop_back(deque);
    elapsed = monotonic_ms() - start;

    TEST_ASSERT_EQ(0, cdeque->size(deque));
    printf("bench deque push_back/pop_back: %d pairs in %.2f ms\n",
           BENCH_ITERS, elapsed);
}

TEST_CASE(bench, deque_push_front)
{
    deque_t* deque = DEQUE_NEW_T(int32_t);
    double start;
    double elapsed;
    size_t i;

    TEST_ASSERT_NOT_NULL(deque);
    test_cleanup_push(deque_free, deque);

    start = monotonic_ms();
    for (i = 0; i < BENCH_ITERS; ++i) {
        if (!cdeque->push_front(deque, __ds_s32data((int32_t)i)))
            TEST_FAIL("Push failed at iteration %zu.", i);
    }
    for (i = 0; i < BENCH_ITERS; ++i)
        cdeque->pop_front(deque);
    elapsed = monotonic_ms() - start;

    TEST_ASSERT_EQ(0, cdeque->size(deque));
    printf("bench deque push_front/pop_front: %d pairs in %.2f ms\n",
           BENCH_ITERS, elapsed);
}
