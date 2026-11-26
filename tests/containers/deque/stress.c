/*
  libj_ds permanent tests deque stress
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

#include <string.h>

#include "../../core/test.h"
#include "desc.h"
#include "value.h"

#define STRESS_CAP 2048

static void deque_free(void* self)
{
    deque_t* deque = self;

    DEQUE_DELETE(&deque);
}

static void stress_model_insert(int32_t* values, size_t* size, size_t index,
                                int32_t value)
{
    memmove(values + index + 1, values + index,
            (*size - index) * sizeof(values[0]));
    values[index] = value;
    ++*size;
}

static void stress_model_erase(int32_t* values, size_t* size, size_t index)
{
    memmove(values + index, values + index + 1,
            (*size - index - 1) * sizeof(values[0]));
    --*size;
}

static void stress_check(const deque_t* deque, const int32_t* values,
                         size_t size)
{
    deque_iterator_t it;
    deque_iterator_t end;
    size_t i;

    TEST_ASSERT_NULL(deque_check_invariants(deque));
    TEST_ASSERT_EQ(size, cdeque->size(deque));
    end = cdeque->end(deque);
    i = 0;
    for (it = cdeque->begin(deque); it.cur != end.cur; it = cdeque->next(it)) {
        TEST_ASSERT_TRUE(i < size);
        TEST_ASSERT_EQ(values[i], it_s32data(it));
        ++i;
    }
    TEST_ASSERT_EQ(size, i);
}

TEST_CASE(stress, deque_int32_model)
{
    test_sequence_model_run(deque_desc_int32(), test_model_iters, false);
}

TEST_CASE(stress, deque_int32_invariants)
{
    deque_t* deque = DEQUE_NEW_T(int32_t);
    int32_t values[STRESS_CAP];
    size_t size = 0;
    size_t step;

    TEST_ASSERT_NOT_NULL(deque);
    test_cleanup_push(deque_free, deque);

    for (step = 0; step < test_model_iters; ++step) {
        uint32_t op = (uint32_t)test_rng_bounded(8);
        int32_t value = (int32_t)test_rng_bounded(101) - 50;
        size_t index;

        if (size > STRESS_CAP - 64)
            op = 7;

        switch (op) {
        case 0:
            test_trace("push_back", value, 0, 0);
            TEST_ASSERT_TRUE(cdeque->push_back(deque, __ds_s32data(value)));
            values[size++] = value;
            break;

        case 1:
            test_trace("push_front", value, 0, 0);
            TEST_ASSERT_TRUE(cdeque->push_front(deque, __ds_s32data(value)));
            memmove(values + 1, values, size * sizeof(values[0]));
            values[0] = value;
            ++size;
            break;

        case 2:
            if (size > 0) {
                test_trace("pop_back", 0, 0, 0);
                cdeque->pop_back(deque);
                --size;
            }
            break;

        case 3:
            if (size > 0) {
                test_trace("pop_front", 0, 0, 0);
                cdeque->pop_front(deque);
                stress_model_erase(values, &size, 0);
            }
            break;

        case 4:
            index = test_rng_bounded(size + 1);
            test_trace("insert", (long long)index, value, 0);
            TEST_ASSERT_TRUE(cdeque->insert(deque,
                                            index == size
                                                ? cdeque->end(deque)
                                                : cdeque->it(deque, index),
                                            __ds_s32data(value)).cur != NULL);
            stress_model_insert(values, &size, index, value);
            break;

        case 5:
            if (size > 0) {
                index = test_rng_bounded(size);
                test_trace("erase", (long long)index, 0, 0);
                TEST_ASSERT_TRUE(cdeque->erase(deque, cdeque->it(deque, index))
                                     .cur != NULL);
                stress_model_erase(values, &size, index);
            }
            break;

        case 6:
            if (test_rng_percent(5)) {
                test_trace("shrink_to_fit", 0, 0, 0);
                cdeque->shrink_to_fit(deque);
            }
            break;

        case 7:
            if (test_rng_percent(20)) {
                test_trace("clear", 0, 0, 0);
                TEST_ASSERT_EQ(size, cdeque->clear(deque));
                size = 0;
            }
            break;
        }

        if ((step % 256) == 0)
            stress_check(deque, values, size);
    }

    stress_check(deque, values, size);
}
