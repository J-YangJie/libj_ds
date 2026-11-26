/*
  libj_ds permanent tests sequence model
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
#include <string.h>

#include "sequence_desc.h"
#include "../core/test.h"

#define MODEL_CAP  4096

typedef struct model_array {
    int32_t values[MODEL_CAP];
    size_t  size;
} model_array_t;

static model_array_t model_a;
static model_array_t model_b;

static const test_seq_desc_t* model_desc;

static void model_destroy(void* self)
{
    model_desc->destroy(self);
}

static void* model_open(const test_seq_desc_t* desc)
{
    void* self;

    model_desc = desc;
    self = desc->create();
    TEST_ASSERT_NOT_NULL(self);
    test_cleanup_push(model_destroy, self);
    return self;
}

static void model_insert(model_array_t* model, size_t index, size_t n,
                         int32_t value)
{
    memmove(model->values + index + n, model->values + index,
            (model->size - index) * sizeof(model->values[0]));
    {
        size_t i;

        for (i = 0; i < n; ++i)
            model->values[index + i] = value;
    }
    model->size += n;
}

static void model_erase(model_array_t* model, size_t index, size_t count)
{
    memmove(model->values + index, model->values + index + count,
            (model->size - index - count) * sizeof(model->values[0]));
    model->size -= count;
}

static void model_assign(model_array_t* model, size_t n, int32_t value)
{
    size_t i;

    for (i = 0; i < n; ++i)
        model->values[i] = value;
    model->size = n;
}

static void model_resize(model_array_t* model, size_t n, int32_t value)
{
    size_t i;

    for (i = model->size; i < n; ++i)
        model->values[i] = value;
    model->size = n;
}

static size_t model_remove(model_array_t* model, int32_t value)
{
    size_t read;
    size_t write = 0;
    size_t removed = 0;

    for (read = 0; read < model->size; ++read) {
        if (model->values[read] == value) {
            ++removed;
        } else {
            model->values[write++] = model->values[read];
        }
    }
    model->size = write;
    return removed;
}

static size_t model_remove_even(model_array_t* model)
{
    size_t read;
    size_t write = 0;
    size_t removed = 0;

    for (read = 0; read < model->size; ++read) {
        if ((model->values[read] & 1) == 0) {
            ++removed;
        } else {
            model->values[write++] = model->values[read];
        }
    }
    model->size = write;
    return removed;
}

typedef struct model_visit {
    const model_array_t* model;
    size_t at;
    bool   reverse;
    bool   ok;
} model_visit_t;

static bool model_visit_value(void* opaque, int32_t value)
{
    model_visit_t* visit = opaque;
    size_t index;

    if (visit->at >= visit->model->size) {
        visit->ok = false;
        return false;
    }
    index = visit->reverse ? visit->model->size - 1 - visit->at : visit->at;
    if (visit->model->values[index] != value) {
        visit->ok = false;
        return false;
    }
    ++visit->at;
    return true;
}

static void model_check(const test_seq_desc_t* desc, const void* self,
                        const model_array_t* model)
{
    model_visit_t forward = { model, 0, false, true };
    model_visit_t reverse = { model, 0, true, true };
    int32_t value = 0;
    bool completed;

    TEST_ASSERT_EQ(model->size, desc->size(self));

    completed = desc->foreach(self, false, model_visit_value, &forward);
    TEST_ASSERT_TRUE(forward.ok);
    TEST_ASSERT_TRUE(completed);
    TEST_ASSERT_EQ(model->size, forward.at);

    completed = desc->foreach(self, true, model_visit_value, &reverse);
    TEST_ASSERT_TRUE(reverse.ok);
    TEST_ASSERT_TRUE(completed);
    TEST_ASSERT_EQ(model->size, reverse.at);

    if (model->size > 0) {
        size_t index = test_rng_bounded(model->size);

        TEST_ASSERT_TRUE(desc->get(self, index, &value));
        TEST_ASSERT_EQ(model->values[index], value);
    }
    TEST_ASSERT_FALSE(desc->get(self, model->size, &value));

    if (model->size > 32) {
        size_t index = test_rng_bounded(model->size);
        int32_t probe = model->values[index];

        TEST_ASSERT_TRUE(desc->find(self, probe, &index));
        TEST_ASSERT_TRUE(desc->get(self, index, &value));
        TEST_ASSERT_EQ(probe, value);
    }
}

static int32_t model_value(void)
{
    return (int32_t)test_rng_bounded(21) - 10;
}

static bool model_remove_even_pred(int32_t value, void* ctx)
{
    (void)ctx;
    return (value & 1) == 0;
}

static size_t model_count(const model_array_t* model, int32_t value)
{
    size_t i;
    size_t count = 0;

    for (i = 0; i < model->size; ++i) {
        if (model->values[i] == value)
            ++count;
    }
    return count;
}

void test_sequence_model_run(const test_seq_desc_t* desc, size_t iterations,
                             bool full_check_each_step)
{
    void* first;
    void* second = NULL;
    bool can_swap = (desc->caps & TEST_SEQ_SWAP) != 0;
    size_t step;

    model_a.size = 0;
    model_b.size = 0;
    first = model_open(desc);
    if (can_swap)
        second = model_open(desc);

    for (step = 0; step < iterations; ++step) {
        uint32_t op = (uint32_t)test_rng_bounded(20);
        int32_t value = model_value();
        size_t index;

        if (model_a.size > MODEL_CAP - 64)
            op = 12;

        switch (op) {
        case 0:
            test_trace("push_back", value, 0, 0);
            TEST_ASSERT_TRUE(desc->push_back(first, value));
            model_a.values[model_a.size++] = value;
            break;

        case 1:
            if ((desc->caps & TEST_SEQ_PUSH_FRONT) != 0) {
                test_trace("push_front", value, 0, 0);
                TEST_ASSERT_TRUE(desc->push_front(first, value));
                model_insert(&model_a, 0, 1, value);
            }
            break;

        case 2:
            if (model_a.size > 0) {
                test_trace("pop_back", 0, 0, 0);
                desc->pop_back(first);
                --model_a.size;
            }
            break;

        case 3:
            if ((desc->caps & TEST_SEQ_POP_FRONT) != 0 && model_a.size > 0) {
                test_trace("pop_front", 0, 0, 0);
                desc->pop_front(first);
                model_erase(&model_a, 0, 1);
            }
            break;

        case 4:
            index = test_rng_bounded(model_a.size + 1);
            test_trace("insert", (long long)index, value, 1);
            {
                size_t out = 0;

                TEST_ASSERT_TRUE(desc->insert(first, index, value, &out));
                TEST_ASSERT_EQ(index, out);
            }
            model_insert(&model_a, index, 1, value);
            break;

        case 5:
            index = test_rng_bounded(model_a.size + 1);
            {
                size_t n = test_rng_bounded(6);
                size_t out = 0;

                test_trace("insert_n", (long long)index, (long long)n, value);
                TEST_ASSERT_TRUE(desc->insert_n(first, index, n, value, &out));
                TEST_ASSERT_EQ(index, out);
                model_insert(&model_a, index, n, value);
            }
            break;

        case 6:
            if (model_a.size > 0) {
                size_t out = 0;

                index = test_rng_bounded(model_a.size);
                test_trace("erase", (long long)index, 0, 0);
                TEST_ASSERT_TRUE(desc->erase(first, index, &out));
                TEST_ASSERT_EQ(index < model_a.size - 1 ? index
                                                        : model_a.size - 1,
                               out);
                model_erase(&model_a, index, 1);
            }
            break;

        case 7:
            {
                size_t a = test_rng_bounded(model_a.size + 1);
                size_t b = a + test_rng_bounded(model_a.size - a + 1);
                size_t out = 0;
                size_t expected;

                test_trace("erase_range", (long long)a, (long long)b, 0);
                TEST_ASSERT_TRUE(desc->erase_range(first, a, b, &out));
                expected = b == model_a.size ? model_a.size - (b - a) : a;
                TEST_ASSERT_EQ(expected, out);
                model_erase(&model_a, a, b - a);
            }
            break;

        case 8:
            {
                size_t n = test_rng_bounded(33);

                test_trace("assign", (long long)n, value, 0);
                TEST_ASSERT_TRUE(desc->assign(first, n, value));
                model_assign(&model_a, n, value);
            }
            break;

        case 9:
            {
                size_t n = test_rng_bounded(65);

                test_trace("resize", (long long)n, value, 0);
                TEST_ASSERT_TRUE(desc->resize(first, n, value));
                model_resize(&model_a, n, value);
            }
            break;

        case 10:
            {
                size_t expected = model_count(&model_a, value);

                test_trace("remove", value, 0, 0);
                TEST_ASSERT_EQ(expected, desc->remove(first, value));
                TEST_ASSERT_EQ(expected, model_remove(&model_a, value));
            }
            break;

        case 11:
            if ((desc->caps & TEST_SEQ_REMOVE_IF) != 0) {
                size_t expected = model_remove_even(&model_a);

                test_trace("remove_if_even", 0, 0, 0);
                TEST_ASSERT_EQ(expected,
                               desc->remove_if(first, model_remove_even_pred,
                                               NULL));
            }
            break;

        case 12:
            if (test_rng_percent(25)) {
                test_trace("clear", 0, 0, 0);
                TEST_ASSERT_EQ(model_a.size, desc->clear(first));
                model_a.size = 0;
            } else if ((desc->caps & TEST_SEQ_RESERVE) != 0) {
                test_trace("reserve", 0, 0, 0);
                TEST_ASSERT_TRUE(desc->reserve(first, test_rng_bounded(96)));
            } else if ((desc->caps & TEST_SEQ_SHRINK) != 0) {
                test_trace("shrink_to_fit", 0, 0, 0);
                desc->shrink_to_fit(first);
            }
            break;

        case 13:
            {
                size_t found = 0;
                size_t expected = 0;
                bool present = false;
                size_t i;

                test_trace("find_count", value, 0, 0);
                for (i = 0; i < model_a.size; ++i) {
                    if (model_a.values[i] == value) {
                        if (!present) {
                            present = true;
                            expected = i;
                        }
                    }
                }
                if (present) {
                    TEST_ASSERT_TRUE(desc->find(first, value, &found));
                    TEST_ASSERT_EQ(expected, found);
                } else {
                    TEST_ASSERT_FALSE(desc->find(first, value, &found));
                }
                TEST_ASSERT_EQ(model_count(&model_a, value),
                               desc->count(first, value));
            }
            break;

        case 14:
            {
                size_t n = test_rng_bounded(9);

                test_trace("assign_range", (long long)n, value, 0);
                if ((desc->caps & TEST_SEQ_ASSIGN_RANGE) != 0) {
                    int32_t source[8];
                    size_t i;

                    for (i = 0; i < n; ++i)
                        source[i] = value + (int32_t)i;
                    TEST_ASSERT_TRUE(desc->assign_range_values(first, source,
                                                               n));
                    model_assign(&model_a, n, 0);
                    for (i = 0; i < n; ++i)
                        model_a.values[i] = source[i];
                } else if (model_a.size < MODEL_CAP) {
                    TEST_ASSERT_TRUE(desc->push_back(first, value));
                    model_a.values[model_a.size++] = value;
                }
            }
            break;

        case 15:
            if ((desc->caps & TEST_SEQ_INSERT_RANGE) != 0) {
                int32_t source[6];
                size_t n = test_rng_bounded(7);
                size_t i;
                size_t out = 0;

                index = test_rng_bounded(model_a.size + 1);
                for (i = 0; i < n; ++i)
                    source[i] = value + (int32_t)i;
                test_trace("insert_range", (long long)index, (long long)n, 0);
                TEST_ASSERT_TRUE(desc->insert_range_values(first, index,
                                                           source, n, &out));
                TEST_ASSERT_EQ(index, out);
                model_insert(&model_a, index, n, 0);
                for (i = 0; i < n; ++i)
                    model_a.values[index + i] = source[i];
            }
            break;

        case 16:
            if (can_swap && test_rng_percent(30)) {
                model_array_t temporary = model_a;

                test_trace("swap", 0, 0, 0);
                desc->swap(first, second);
                model_a = model_b;
                model_b = temporary;
            }
            break;

        case 17:
            if (model_a.size > 0) {
                index = test_rng_bounded(model_a.size);
                {
                    size_t out = 0;

                    test_trace("erase_first_half", (long long)index, 0, 0);
                    TEST_ASSERT_TRUE(desc->erase(first, index, &out));
                    model_erase(&model_a, index, 1);
                }
            }
            break;

        case 18:
            if (model_a.size < 128) {
                size_t n = test_rng_bounded(8);
                size_t i;

                test_trace("push_back_many", (long long)n, value, 0);
                for (i = 0; i < n; ++i) {
                    TEST_ASSERT_TRUE(desc->push_back(first, value));
                    model_a.values[model_a.size++] = value;
                }
            }
            break;

        case 19:
            if (model_a.size > 0) {
                size_t index2 = test_rng_bounded(model_a.size);
                int32_t probe = model_a.values[index2];

                TEST_ASSERT_EQ(probe, model_a.values[index2]);
            }
            break;
        }

        if (full_check_each_step || (step % 64) == 0)
            model_check(desc, first, &model_a);
    }

    model_check(desc, first, &model_a);
    if (can_swap)
        model_check(desc, second, &model_b);
}

static void case_random(const void* ctx)
{
    const test_seq_desc_t* desc = ctx;

    test_sequence_model_run(desc, test_model_iters, test_model_iters <= 4000);
}

void test_sequence_model_register(const test_seq_desc_t* desc)
{
    char name[96];

    snprintf(name, sizeof(name), "%s.random", desc->name);
    test_register_case("model", name, case_random, desc, __FILE__, __LINE__);
}
