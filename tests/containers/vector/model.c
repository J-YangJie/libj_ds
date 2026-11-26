/*
  libj_ds permanent tests vector models
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
#include <stdio.h>
#include <string.h>

#include "../../core/test.h"
#include "desc.h"

#define STR_MODEL_CAP 256
#define STR_MODEL_LEN 32
#define STR_BUFFER_SZ 64

typedef struct str_model {
    char   values[STR_MODEL_CAP][STR_MODEL_LEN + 1];
    size_t size;
} str_model_t;

static str_model_t str_model_a;
static str_model_t str_model_b;

static const test_seq_str_desc_t* str_model_desc;

static void str_model_destroy(void* self)
{
    str_model_desc->destroy(self);
}

static void* str_model_open(const test_seq_str_desc_t* desc)
{
    void* self;

    str_model_desc = desc;
    self = desc->create();
    TEST_ASSERT_NOT_NULL(self);
    test_cleanup_push(str_model_destroy, self);
    return self;
}

static void str_model_make(char* out)
{
    static const char alphabet[] = "abcdefghijklmnopqrstuvwxyz0123456789";
    size_t len = 1 + test_rng_bounded(STR_MODEL_LEN);
    size_t i;

    for (i = 0; i < len; ++i)
        out[i] = alphabet[test_rng_bounded(sizeof(alphabet) - 1)];
    out[len] = '\0';
}

static void str_model_insert(str_model_t* model, size_t index, const char* value)
{
    memmove(model->values[index + 1], model->values[index],
            (model->size - index) * sizeof(model->values[0]));
    snprintf(model->values[index], sizeof(model->values[0]), "%s", value);
    ++model->size;
}

static void str_model_erase(str_model_t* model, size_t index)
{
    memmove(model->values[index], model->values[index + 1],
            (model->size - index - 1) * sizeof(model->values[0]));
    --model->size;
}

typedef struct str_model_visit {
    const str_model_t* model;
    size_t             at;
    bool               reverse;
    bool               ok;
} str_model_visit_t;

static bool str_model_visit_value(void* opaque, const char* value)
{
    str_model_visit_t* visit = opaque;
    size_t index;

    if (visit->at >= visit->model->size) {
        visit->ok = false;
        return false;
    }
    index = visit->reverse ? visit->model->size - 1 - visit->at : visit->at;
    if (strcmp(visit->model->values[index], value) != 0) {
        visit->ok = false;
        return false;
    }
    ++visit->at;
    return true;
}

static void str_model_check(const test_seq_str_desc_t* desc, const void* self,
                            const str_model_t* model)
{
    str_model_visit_t forward = { model, 0, false, true };
    str_model_visit_t reverse = { model, 0, true, true };
    char buffer[STR_BUFFER_SZ];
    char small[1];
    bool completed;
    size_t i;

    TEST_ASSERT_EQ(model->size, desc->size(self));
    for (i = 0; i < model->size; ++i) {
        TEST_ASSERT_TRUE(desc->get(self, i, buffer, sizeof(buffer)));
        TEST_ASSERT_STR_EQ(model->values[i], buffer);
    }
    TEST_ASSERT_FALSE(desc->get(self, model->size, buffer, sizeof(buffer)));
    if (model->size > 0)
        TEST_ASSERT_FALSE(desc->get(self, 0, small, sizeof(small)));

    completed = desc->foreach(self, false, str_model_visit_value, &forward);
    TEST_ASSERT_TRUE(forward.ok);
    TEST_ASSERT_TRUE(completed);
    TEST_ASSERT_EQ(model->size, forward.at);

    completed = desc->foreach(self, true, str_model_visit_value, &reverse);
    TEST_ASSERT_TRUE(reverse.ok);
    TEST_ASSERT_TRUE(completed);
    TEST_ASSERT_EQ(model->size, reverse.at);
}

static void case_string_random(const void* ctx)
{
    const test_seq_str_desc_t* desc = ctx;
    void* first;
    void* second;
    size_t step;

    str_model_a.size = 0;
    str_model_b.size = 0;
    first = str_model_open(desc);
    second = str_model_open(desc);

    for (step = 0; step < test_model_iters; ++step) {
        uint32_t op = (uint32_t)test_rng_bounded(6);
        char value[STR_MODEL_LEN + 1];
        size_t index;

        str_model_make(value);
        if (str_model_a.size > STR_MODEL_CAP - 8)
            op = 4;

        switch (op) {
        case 0:
            test_trace("str_push_back", (long long)str_model_a.size, 0, 0);
            TEST_ASSERT_TRUE(desc->push_back(first, value));
            snprintf(str_model_a.values[str_model_a.size],
                     sizeof(str_model_a.values[0]), "%s", value);
            ++str_model_a.size;
            break;

        case 1:
            test_trace("str_push_front", (long long)str_model_a.size, 0, 0);
            TEST_ASSERT_TRUE(desc->push_front(first, value));
            str_model_insert(&str_model_a, 0, value);
            break;

        case 2:
            index = test_rng_bounded(str_model_a.size + 1);
            test_trace("str_insert", (long long)index, 0, 0);
            {
                size_t out = 0;

                TEST_ASSERT_TRUE(desc->insert(first, index, value, &out));
                TEST_ASSERT_EQ(index, out);
            }
            str_model_insert(&str_model_a, index, value);
            break;

        case 3:
            if (str_model_a.size > 0) {
                size_t out = 0;

                index = test_rng_bounded(str_model_a.size);
                test_trace("str_erase", (long long)index, 0, 0);
                TEST_ASSERT_TRUE(desc->erase(first, index, &out));
                TEST_ASSERT_EQ(index < str_model_a.size - 1 ? index
                                                            : str_model_a.size - 1,
                               out);
                str_model_erase(&str_model_a, index);
            }
            break;

        case 4:
            test_trace("str_clear", (long long)str_model_a.size, 0, 0);
            TEST_ASSERT_EQ(str_model_a.size, desc->clear(first));
            str_model_a.size = 0;
            break;

        case 5:
            if (test_rng_percent(30)) {
                str_model_t temporary = str_model_a;

                test_trace("str_swap", 0, 0, 0);
                desc->swap(first, second);
                str_model_a = str_model_b;
                str_model_b = temporary;
            }
            break;
        }

        str_model_check(desc, first, &str_model_a);
    }

    str_model_check(desc, first, &str_model_a);
    str_model_check(desc, second, &str_model_b);
}

static void vector_model_register(void)
{
    test_sequence_model_register(vector_desc_int32());
    test_register_case("model", "vector.string.random", case_string_random,
                       vector_desc_string(), __FILE__, __LINE__);
}

TEST_CTOR static void vector_register_models(void)
{
    vector_model_register();
}
