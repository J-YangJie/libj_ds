/*
  libj_ds permanent tests vector stress
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

#include <string.h>

#include "../../core/test.h"
#include "desc.h"
#include "value.h"

#define STRESS_CAP 2048

static void vector_free(void* self)
{
    vector_t* vector = self;

    VECTOR_DELETE(&vector);
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

static void stress_check(const vector_t* vector, const int32_t* values,
                         size_t size)
{
    vector_iterator_t it;
    vector_iterator_t end;
    size_t i;

    TEST_ASSERT_NULL(vector_check_invariants(vector));
    TEST_ASSERT_EQ(size, cvector->size(vector));
    TEST_ASSERT_TRUE(cvector->capacity(vector) >= cvector->size(vector));
    end = cvector->end(vector);
    i = 0;
    for (it = cvector->begin(vector); it.cur != end.cur;
         it = cvector->next(it)) {
        TEST_ASSERT_TRUE(i < size);
        TEST_ASSERT_EQ(values[i], it_s32data(it));
        ++i;
    }
    TEST_ASSERT_EQ(size, i);
}

TEST_CASE(stress, vector_int32_model)
{
    test_sequence_model_run(vector_desc_int32(), test_model_iters, false);
}

TEST_CASE(stress, vector_int32_invariants)
{
    vector_t* vector = VECTOR_NEW_T(int32_t);
    int32_t values[STRESS_CAP];
    size_t size = 0;
    size_t step;

    TEST_ASSERT_NOT_NULL(vector);
    test_cleanup_push(vector_free, vector);

    for (step = 0; step < test_model_iters; ++step) {
        uint32_t op = (uint32_t)test_rng_bounded(8);
        int32_t value = (int32_t)test_rng_bounded(101) - 50;
        size_t index;

        if (size > STRESS_CAP - 64)
            op = 7;

        switch (op) {
        case 0:
            test_trace("push_back", value, 0, 0);
            TEST_ASSERT_TRUE(cvector->push_back(vector, __ds_s32data(value)));
            values[size++] = value;
            break;

        case 1:
            if (size > 0) {
                test_trace("pop_back", 0, 0, 0);
                cvector->pop_back(vector);
                --size;
            }
            break;

        case 2:
            index = test_rng_bounded(size + 1);
            test_trace("insert", (long long)index, value, 0);
            TEST_ASSERT_TRUE(cvector->insert(vector,
                                             index == size
                                                 ? cvector->end(vector)
                                                 : cvector->it(vector, index),
                                             __ds_s32data(value)).cur
                             != NULL);
            stress_model_insert(values, &size, index, value);
            break;

        case 3:
            if (size > 0) {
                index = test_rng_bounded(size);
                test_trace("erase", (long long)index, 0, 0);
                TEST_ASSERT_TRUE(cvector->erase(vector,
                                                cvector->it(vector, index)).cur
                                 != NULL);
                stress_model_erase(values, &size, index);
            }
            break;

        case 4:
            index = test_rng_bounded(STRESS_CAP + 512);
            test_trace("reserve", (long long)index, 0, 0);
            TEST_ASSERT_TRUE(cvector->reserve(vector, (vector_size_t)index));
            TEST_ASSERT_TRUE(cvector->capacity(vector) >= (vector_size_t)index);
            break;

        case 5:
            if (test_rng_percent(10)) {
                test_trace("shrink_to_fit", 0, 0, 0);
                cvector->shrink_to_fit(vector);
            }
            break;

        case 6:
            {
                size_t n = test_rng_bounded(size + 32);

                test_trace("resize", (long long)n, value, 0);
                TEST_ASSERT_TRUE(cvector->resize(vector, (vector_size_t)n,
                                                 __ds_s32data(value)));
                if (n < size) {
                    size = n;
                } else {
                    while (size < n)
                        values[size++] = value;
                }
            }
            break;

        case 7:
            if (test_rng_percent(20)) {
                test_trace("clear", 0, 0, 0);
                TEST_ASSERT_EQ(size, cvector->clear(vector));
                size = 0;
            }
            break;
        }

        if ((step % 256) == 0)
            stress_check(vector, values, size);
    }

    stress_check(vector, values, size);
}
