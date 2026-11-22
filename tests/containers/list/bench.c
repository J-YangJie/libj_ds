/*
  libj_ds permanent tests list benchmark
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

#include <list/list.h>

#include <stdio.h>
#include <time.h>

#include "../../core/test.h"

#define BENCH_ITERS 200000
#define BENCH_FRONT_ITERS 20000
#define BENCH_MIDDLE_ITERS 10000
#define BENCH_SORT_ITERS 100000

static void list_free(void* self)
{
    list_t* list = self;

    LIST_DELETE(&list);
}

static double monotonic_ms(void)
{
    struct timespec ts;

    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (double)ts.tv_sec * 1000.0 + (double)ts.tv_nsec / 1000000.0;
}

TEST_CASE(bench, list_push_pop_back)
{
    list_t* list = LIST_NEW_T(int32_t);
    double start;
    double elapsed;
    size_t i;

    TEST_ASSERT_NOT_NULL(list);
    test_cleanup_push(list_free, list);

    start = monotonic_ms();
    for (i = 0; i < BENCH_ITERS; ++i) {
        if (!clist->push_back(list, __ds_s32data((int32_t)i)))
            TEST_FAIL("Push failed at iteration %zu.", i);
    }
    for (i = 0; i < BENCH_ITERS; ++i)
        clist->pop_back(list);
    elapsed = monotonic_ms() - start;

    TEST_ASSERT_EQ(0, clist->size(list));
    printf("bench list push_back/pop_back: %d pairs in %.2f ms\n",
           BENCH_ITERS, elapsed);
}

TEST_CASE(bench, list_push_pop_front)
{
    list_t* list = LIST_NEW_T(int32_t);
    double start;
    double elapsed;
    size_t i;

    TEST_ASSERT_NOT_NULL(list);
    test_cleanup_push(list_free, list);

    start = monotonic_ms();
    for (i = 0; i < BENCH_ITERS; ++i) {
        if (!clist->push_front(list, __ds_s32data((int32_t)i)))
            TEST_FAIL("Push failed at iteration %zu.", i);
    }
    for (i = 0; i < BENCH_ITERS; ++i)
        clist->pop_front(list);
    elapsed = monotonic_ms() - start;

    TEST_ASSERT_EQ(0, clist->size(list));
    printf("bench list push_front/pop_front: %d pairs in %.2f ms\n",
           BENCH_ITERS, elapsed);
}

TEST_CASE(bench, list_insert_erase_middle)
{
    list_t* list = LIST_NEW_T(int32_t);
    double start;
    double elapsed;
    size_t i;

    TEST_ASSERT_NOT_NULL(list);
    test_cleanup_push(list_free, list);

    for (i = 0; i < BENCH_MIDDLE_ITERS; ++i) {
        if (!clist->push_back(list, __ds_s32data((int32_t)i)))
            TEST_FAIL("Push failed at iteration %zu.", i);
    }

    start = monotonic_ms();
    for (i = 0; i < BENCH_MIDDLE_ITERS; ++i) {
        list_iterator_t middle =
            clist->it(list, (list_index_t)(clist->size(list) / 2));

        if (clist->insert(list, middle, __ds_s32data((int32_t)i)).d == NULL)
            TEST_FAIL("Insert failed at iteration %zu.", i);
    }
    for (i = 0; i < BENCH_MIDDLE_ITERS; ++i) {
        list_iterator_t middle =
            clist->it(list, (list_index_t)(clist->size(list) / 2));

        if (clist->erase(list, middle).d == NULL)
            TEST_FAIL("Erase failed at iteration %zu.", i);
    }
    elapsed = monotonic_ms() - start;

    TEST_ASSERT_EQ(BENCH_MIDDLE_ITERS, clist->size(list));
    printf("bench list insert/erase middle: %d pairs in %.2f ms\n",
           BENCH_MIDDLE_ITERS, elapsed);
}

TEST_CASE(bench, list_sort)
{
    list_t* list = LIST_NEW_T(int32_t);
    double start;
    double elapsed;
    size_t i;

    TEST_ASSERT_NOT_NULL(list);
    test_cleanup_push(list_free, list);

    for (i = 0; i < BENCH_SORT_ITERS; ++i) {
        if (!clist->push_back(list, __ds_s32data((int32_t)test_rng_bounded(
                                    1000000))))
            TEST_FAIL("Push failed at iteration %zu.", i);
    }

    start = monotonic_ms();
    clist->sort(list, __list_lt_default);
    elapsed = monotonic_ms() - start;

    TEST_ASSERT_EQ(BENCH_SORT_ITERS, clist->size(list));
    TEST_ASSERT_TRUE(clist_sorted_default(list, true));
    printf("bench list sort: %d elements in %.2f ms\n", BENCH_SORT_ITERS,
           elapsed);
}
