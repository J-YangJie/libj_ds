/*
  libj_ds permanent tests adaptor model
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

#include "adaptor_desc.h"
#include "../core/test.h"

#define MODEL_CAP 4096

typedef struct adaptor_model {
    int32_t values[MODEL_CAP];
    size_t  size;
} adaptor_model_t;

static adaptor_model_t model_a;
static adaptor_model_t model_b;

static const test_adaptor_desc_t* model_desc;

static void model_destroy(void* self)
{
    model_desc->destroy(self);
}

static void* model_open(const test_adaptor_desc_t* desc)
{
    void* self;

    model_desc = desc;
    self = desc->create();
    TEST_ASSERT_NOT_NULL(self);
    test_cleanup_push(model_destroy, self);
    return self;
}

static size_t model_extreme(const test_adaptor_desc_t* desc,
                            const adaptor_model_t* model)
{
    size_t best = 0;
    size_t i;

    for (i = 1; i < model->size; ++i) {
        if ((desc->caps & TEST_ADAPTOR_TOP_MAX) != 0) {
            if (model->values[i] > model->values[best])
                best = i;
        } else {
            if (model->values[i] < model->values[best])
                best = i;
        }
    }
    return best;
}

static int32_t model_peek(const test_adaptor_desc_t* desc,
                          const adaptor_model_t* model)
{
    TEST_ASSERT_TRUE(model->size > 0);
    if (desc->kind == TEST_ADAPTOR_STACK)
        return model->values[model->size - 1];
    if (desc->kind == TEST_ADAPTOR_QUEUE)
        return model->values[0];
    return model->values[model_extreme(desc, model)];
}

static void model_pop(const test_adaptor_desc_t* desc,
                      adaptor_model_t* model)
{
    TEST_ASSERT_TRUE(model->size > 0);
    if (desc->kind == TEST_ADAPTOR_STACK) {
        --model->size;
    } else if (desc->kind == TEST_ADAPTOR_QUEUE) {
        memmove(model->values, model->values + 1,
                (model->size - 1) * sizeof(model->values[0]));
        --model->size;
    } else {
        size_t index = model_extreme(desc, model);

        memmove(model->values + index, model->values + index + 1,
                (model->size - index - 1) * sizeof(model->values[0]));
        --model->size;
    }
}

static void model_check(const test_adaptor_desc_t* desc, const void* self,
                        const adaptor_model_t* model)
{
    int32_t value = 0;

    TEST_ASSERT_EQ(model->size, desc->size(self));
    TEST_ASSERT_EQ(model->size == 0, desc->empty(self));
    if (model->size > 0) {
        TEST_ASSERT_TRUE(desc->peek(self, &value));
        TEST_ASSERT_EQ(model_peek(desc, model), value);
    }
    if (desc->kind == TEST_ADAPTOR_QUEUE && desc->peek_back != NULL &&
        model->size > 0) {
        TEST_ASSERT_TRUE(desc->peek_back(self, &value));
        TEST_ASSERT_EQ(model->values[model->size - 1], value);
    }
}

static void model_drain_check(const test_adaptor_desc_t* desc, void* self,
                              adaptor_model_t* model)
{
    while (model->size > 0) {
        int32_t expected = model_peek(desc, model);
        int32_t value = 0;

        TEST_ASSERT_TRUE(desc->peek(self, &value));
        TEST_ASSERT_EQ(expected, value);
        desc->pop(self);
        model_pop(desc, model);
        model_check(desc, self, model);
    }
    TEST_ASSERT_EQ(0, desc->size(self));
    TEST_ASSERT_TRUE(desc->empty(self));
}

void test_adaptor_model_run(const test_adaptor_desc_t* desc, size_t iterations)
{
    void* first;
    void* second;
    size_t step;

    model_a.size = 0;
    model_b.size = 0;
    first = model_open(desc);
    second = model_open(desc);

    for (step = 0; step < iterations; ++step) {
        uint32_t op = (uint32_t)test_rng_bounded(9);
        int32_t value = (int32_t)test_rng_bounded(41) - 20;

        if (model_a.size > MODEL_CAP - 64)
            op = 5;

        switch (op) {
        case 0:
            test_trace("push", value, 0, 0);
            TEST_ASSERT_TRUE(desc->push(first, value));
            model_a.values[model_a.size++] = value;
            break;

        case 1:
            test_trace("push_many", value, 0, 0);
            {
                size_t n = 1 + test_rng_bounded(4);
                size_t i;

                for (i = 0; i < n; ++i) {
                    TEST_ASSERT_TRUE(desc->push(first, value));
                    model_a.values[model_a.size++] = value;
                }
            }
            break;

        case 2:
            if (model_a.size > 0) {
                test_trace("pop", 0, 0, 0);
                desc->pop(first);
                model_pop(desc, &model_a);
            }
            break;

        case 3:
            if (model_a.size > 0) {
                int32_t peeked = 0;

                test_trace("peek", value, 0, 0);
                TEST_ASSERT_TRUE(desc->peek(first, &peeked));
                TEST_ASSERT_EQ(model_peek(desc, &model_a), peeked);
            }
            break;

        case 4:
            if (test_rng_percent(30)) {
                adaptor_model_t temporary = model_a;

                test_trace("swap", 0, 0, 0);
                desc->swap(first, second);
                model_a = model_b;
                model_b = temporary;
            }
            break;

        case 5:
            if (model_a.size > 0) {
                test_trace("clear", (long long)model_a.size, 0, 0);
                TEST_ASSERT_EQ(model_a.size, desc->clear(first));
                model_a.size = 0;
            }
            break;

        case 6:
            if (desc->kind == TEST_ADAPTOR_QUEUE && desc->peek_back != NULL &&
                model_a.size > 0) {
                int32_t back = 0;

                test_trace("peek_back", 0, 0, 0);
                TEST_ASSERT_TRUE(desc->peek_back(first, &back));
                TEST_ASSERT_EQ(model_a.values[model_a.size - 1], back);
            }
            break;

        case 7:
            test_trace("size_empty", 0, 0, 0);
            model_check(desc, first, &model_a);
            model_check(desc, second, &model_b);
            break;

        case 8:
            if (test_rng_percent(10)) {
                test_trace("drain", (long long)model_a.size, 0, 0);
                model_drain_check(desc, first, &model_a);
            }
            break;
        }

        model_check(desc, first, &model_a);
        if ((step % 512) == 0) {
            model_drain_check(desc, first, &model_a);
            model_drain_check(desc, second, &model_b);
        }
    }

    model_check(desc, first, &model_a);
    model_check(desc, second, &model_b);
    model_drain_check(desc, first, &model_a);
    model_drain_check(desc, second, &model_b);
}

static void case_random(const void* ctx)
{
    const test_adaptor_desc_t* desc = ctx;

    test_adaptor_model_run(desc, test_model_iters);
}

void test_adaptor_model_register(const test_adaptor_desc_t* desc)
{
    char name[96];

    snprintf(name, sizeof(name), "%s.random", desc->name);
    test_register_case("model", name, case_random, desc, __FILE__, __LINE__);
}
