/*
  libj_ds permanent tests vector benchmark
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

#include <vector/vector.h>

#include <stdio.h>
#include <time.h>

#include "../../core/test.h"

#define BENCH_ITERS 200000
#define BENCH_FRONT_ITERS 20000

static void vector_free(void* self)
{
    vector_t* vector = self;

    VECTOR_DELETE(&vector);
}

static double monotonic_ms(void)
{
    struct timespec ts;

    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (double)ts.tv_sec * 1000.0 + (double)ts.tv_nsec / 1000000.0;
}

TEST_CASE(bench, vector_push_pop)
{
    vector_t* vector = VECTOR_NEW_T(int32_t);
    double start;
    double elapsed;
    size_t i;

    TEST_ASSERT_NOT_NULL(vector);
    test_cleanup_push(vector_free, vector);

    start = monotonic_ms();
    for (i = 0; i < BENCH_ITERS; ++i) {
        if (!cvector->push_back(vector, __ds_s32data((int32_t)i)))
            TEST_FAIL("Push failed at iteration %zu.", i);
    }
    for (i = 0; i < BENCH_ITERS; ++i)
        cvector->pop_back(vector);
    elapsed = monotonic_ms() - start;

    TEST_ASSERT_EQ(0, cvector->size(vector));
    printf("bench vector push_back/pop_back: %d pairs in %.2f ms\n",
           BENCH_ITERS, elapsed);
}

TEST_CASE(bench, vector_insert_front)
{
    vector_t* vector = VECTOR_NEW_T(int32_t);
    double start;
    double elapsed;
    size_t i;

    TEST_ASSERT_NOT_NULL(vector);
    test_cleanup_push(vector_free, vector);

    start = monotonic_ms();
    for (i = 0; i < BENCH_FRONT_ITERS; ++i) {
        if (cvector->insert(vector, cvector->begin(vector),
                            __ds_s32data((int32_t)i)).cur == NULL)
            TEST_FAIL("Insert failed at iteration %zu.", i);
    }
    for (i = 0; i < BENCH_FRONT_ITERS; ++i)
        TEST_ASSERT_TRUE(cvector->erase(vector, cvector->begin(vector)).cur
                         != NULL);
    elapsed = monotonic_ms() - start;

    TEST_ASSERT_EQ(0, cvector->size(vector));
    printf("bench vector insert(begin)/erase(begin): %d pairs in %.2f ms\n",
           BENCH_FRONT_ITERS, elapsed);
}

TEST_CASE(bench, vector_reserved_push)
{
    vector_t* vector = VECTOR_NEW_T(int32_t);
    double start;
    double elapsed;
    size_t i;

    TEST_ASSERT_NOT_NULL(vector);
    test_cleanup_push(vector_free, vector);
    TEST_ASSERT_TRUE(cvector->reserve(vector, BENCH_ITERS));

    start = monotonic_ms();
    for (i = 0; i < BENCH_ITERS; ++i) {
        if (!cvector->push_back(vector, __ds_s32data((int32_t)i)))
            TEST_FAIL("Push failed at iteration %zu.", i);
    }
    for (i = 0; i < BENCH_ITERS; ++i)
        cvector->pop_back(vector);
    elapsed = monotonic_ms() - start;

    TEST_ASSERT_EQ(0, cvector->size(vector));
    printf("bench vector reserved push_back/pop_back: %d pairs in %.2f ms\n",
           BENCH_ITERS, elapsed);
}
