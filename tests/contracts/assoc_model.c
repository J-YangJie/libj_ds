/*
  libj_ds permanent tests associative model
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

#include "assoc_desc.h"
#include "../core/test.h"

#define ASSOC_MODEL_CAP 256

typedef struct assoc_model_pair {
    int32_t key;
    int32_t value;
} assoc_model_pair_t;

typedef struct assoc_model {
    assoc_model_pair_t pairs[ASSOC_MODEL_CAP];
    size_t             size;
} assoc_model_t;

static assoc_model_t assoc_model;
static const test_assoc_desc_t* assoc_model_desc;

static void assoc_model_destroy(void* self)
{
    assoc_model_desc->destroy(self);
}

static void* assoc_model_open(const test_assoc_desc_t* desc)
{
    void* self;

    assoc_model_desc = desc;
    self = desc->create();
    TEST_ASSERT_NOT_NULL(self);
    test_cleanup_push(assoc_model_destroy, self);
    return self;
}

static size_t assoc_model_lower(const assoc_model_t* model, int32_t key)
{
    size_t lo = 0;
    size_t hi = model->size;

    while (lo < hi) {
        size_t mid = lo + (hi - lo) / 2;

        if (model->pairs[mid].key < key)
            lo = mid + 1;
        else
            hi = mid;
    }
    return lo;
}

static size_t assoc_model_upper(const assoc_model_t* model, int32_t key)
{
    size_t lo = 0;
    size_t hi = model->size;

    while (lo < hi) {
        size_t mid = lo + (hi - lo) / 2;

        if (model->pairs[mid].key <= key)
            lo = mid + 1;
        else
            hi = mid;
    }
    return lo;
}

static int32_t assoc_model_value(const test_assoc_desc_t* desc, int32_t key,
                                 int32_t value)
{
    if ((desc->caps & TEST_ASSOC_HAS_VALUE) == 0)
        return key;
    return value;
}

static bool assoc_model_insert(const test_assoc_desc_t* desc,
                               assoc_model_t* model, int32_t key,
                               int32_t value)
{
    bool multi = (desc->caps & TEST_ASSOC_MULTI) != 0;
    size_t at = multi ? assoc_model_upper(model, key)
                      : assoc_model_lower(model, key);

    value = assoc_model_value(desc, key, value);
    if (!multi && at < model->size && model->pairs[at].key == key)
        return false;
    memmove(&model->pairs[at + 1], &model->pairs[at],
            (model->size - at) * sizeof(model->pairs[0]));
    model->pairs[at].key = key;
    model->pairs[at].value = value;
    ++model->size;
    return true;
}

static void assoc_model_insert_replace(const test_assoc_desc_t* desc,
                                       assoc_model_t* model, int32_t key,
                                       int32_t value)
{
    size_t at = assoc_model_lower(model, key);

    value = assoc_model_value(desc, key, value);
    if (at < model->size && model->pairs[at].key == key) {
        model->pairs[at].value = value;
        return;
    }
    assoc_model_insert(desc, model, key, value);
}

static bool assoc_model_erase(assoc_model_t* model, int32_t key,
                              int32_t* out_next_key, bool* out_next_found)
{
    size_t at = assoc_model_lower(model, key);

    if (at >= model->size || model->pairs[at].key != key) {
        *out_next_found = false;
        return false;
    }
    memmove(&model->pairs[at], &model->pairs[at + 1],
            (model->size - at - 1) * sizeof(model->pairs[0]));
    --model->size;
    if (at < model->size) {
        *out_next_key = model->pairs[at].key;
        *out_next_found = true;
    } else {
        *out_next_found = false;
    }
    return true;
}

static size_t assoc_model_count(const assoc_model_t* model, int32_t key)
{
    return assoc_model_upper(model, key) - assoc_model_lower(model, key);
}

static size_t assoc_model_remove(assoc_model_t* model, int32_t key)
{
    size_t at = assoc_model_lower(model, key);
    size_t count = assoc_model_count(model, key);

    if (count == 0)
        return 0;
    memmove(&model->pairs[at], &model->pairs[at + count],
            (model->size - at - count) * sizeof(model->pairs[0]));
    model->size -= count;
    return count;
}

static bool assoc_model_find(const assoc_model_t* model, int32_t key,
                             int32_t* out_value)
{
    size_t at = assoc_model_lower(model, key);

    if (at >= model->size || model->pairs[at].key != key)
        return false;
    *out_value = model->pairs[at].value;
    return true;
}

static bool assoc_model_value_matches(const assoc_model_t* model, int32_t key,
                                      int32_t value)
{
    size_t at = assoc_model_lower(model, key);
    size_t end = assoc_model_upper(model, key);

    for (; at < end; ++at) {
        if (model->pairs[at].value == value)
            return true;
    }
    return false;
}

typedef struct assoc_model_visit {
    assoc_model_pair_t pairs[ASSOC_MODEL_CAP];
    size_t             count;
    bool               overflow;
} assoc_model_visit_t;

static bool assoc_model_visit_pair(void* ctx, int32_t key, int32_t value)
{
    assoc_model_visit_t* visit = ctx;

    if (visit->count >= ASSOC_MODEL_CAP) {
        visit->overflow = true;
        return false;
    }
    visit->pairs[visit->count].key = key;
    visit->pairs[visit->count].value = value;
    ++visit->count;
    return true;
}

static void assoc_model_sort(int32_t* keys, size_t n)
{
    size_t i;

    for (i = 1; i < n; ++i) {
        int32_t key = keys[i];
        size_t j = i;

        while (j > 0 && keys[j - 1] > key) {
            keys[j] = keys[j - 1];
            --j;
        }
        keys[j] = key;
    }
}

static void assoc_model_check_pairs(const test_assoc_desc_t* desc,
                                    const assoc_model_t* model,
                                    const assoc_model_visit_t* visit)
{
    bool used[ASSOC_MODEL_CAP];
    size_t i;

    memset(used, 0, sizeof(used));
    for (i = 0; i < visit->count; ++i) {
        size_t j;
        bool found = false;

        for (j = 0; j < model->size; ++j) {
            if (!used[j]
                && model->pairs[j].key == visit->pairs[i].key
                && model->pairs[j].value == visit->pairs[i].value) {
                used[j] = true;
                found = true;
                break;
            }
        }
        if (!found)
            TEST_FAIL("`%s` visited pair (%d, %d) is not in the model",
                      desc->name, (int)visit->pairs[i].key,
                      (int)visit->pairs[i].value);
    }
}

static void assoc_model_check_traversal(const test_assoc_desc_t* desc,
                                        const assoc_model_t* model,
                                        const assoc_model_visit_t* visit,
                                        bool reverse)
{
    size_t i;

    TEST_ASSERT_EQ(model->size, visit->count);
    if ((desc->caps & TEST_ASSOC_ORDERED) != 0) {
        for (i = 0; i < visit->count; ++i) {
            size_t at = reverse ? model->size - 1 - i : i;

            TEST_ASSERT_EQ(model->pairs[at].key, visit->pairs[i].key);
        }
    } else {
        int32_t actual[ASSOC_MODEL_CAP];
        int32_t expected[ASSOC_MODEL_CAP];

        for (i = 0; i < visit->count; ++i) {
            actual[i] = visit->pairs[i].key;
            expected[i] = model->pairs[i].key;
        }
        assoc_model_sort(actual, visit->count);
        assoc_model_sort(expected, model->size);
        for (i = 0; i < visit->count; ++i)
            TEST_ASSERT_EQ(expected[i], actual[i]);
    }
    assoc_model_check_pairs(desc, model, visit);
}

static void assoc_model_check_bounds(const test_assoc_desc_t* desc,
                                     const void* self,
                                     const assoc_model_t* model)
{
    int32_t probe = (int32_t)test_rng_bounded(96) - 16;
    int32_t key = 0;
    int32_t value = 0;
    size_t at = assoc_model_lower(model, probe);

    if (at < model->size) {
        TEST_ASSERT_TRUE(desc->lower_bound(self, probe, &key, &value));
        TEST_ASSERT_EQ(model->pairs[at].key, key);
        if ((desc->caps & TEST_ASSOC_MULTI) == 0)
            TEST_ASSERT_EQ(model->pairs[at].value, value);
    } else {
        TEST_ASSERT_FALSE(desc->lower_bound(self, probe, &key, &value));
    }

    at = assoc_model_upper(model, probe);
    if (at < model->size) {
        TEST_ASSERT_TRUE(desc->upper_bound(self, probe, &key, &value));
        TEST_ASSERT_EQ(model->pairs[at].key, key);
        if ((desc->caps & TEST_ASSOC_MULTI) == 0)
            TEST_ASSERT_EQ(model->pairs[at].value, value);
    } else {
        TEST_ASSERT_FALSE(desc->upper_bound(self, probe, &key, &value));
    }
}

static void assoc_model_check(const test_assoc_desc_t* desc, const void* self)
{
    assoc_model_visit_t forward;
    assoc_model_visit_t reverse;
    int32_t value = 0;
    int32_t probe;
    size_t i;

    forward.count = 0;
    forward.overflow = false;
    reverse.count = 0;
    reverse.overflow = false;

    TEST_ASSERT_EQ(assoc_model.size, desc->size(self));
    TEST_ASSERT_TRUE(desc->foreach(self, false, assoc_model_visit_pair,
                                   &forward));
    TEST_ASSERT_TRUE(desc->foreach(self, true, assoc_model_visit_pair,
                                   &reverse));
    TEST_ASSERT_FALSE(forward.overflow);
    TEST_ASSERT_FALSE(reverse.overflow);
    assoc_model_check_traversal(desc, &assoc_model, &forward, false);
    assoc_model_check_traversal(desc, &assoc_model, &reverse, true);

    for (i = 0; i < 8; ++i) {
        probe = (int32_t)test_rng_bounded(96) - 16;
        TEST_ASSERT_EQ(assoc_model_count(&assoc_model, probe),
                       desc->count(self, probe));
        if (assoc_model_find(&assoc_model, probe, &value)) {
            int32_t actual = 0;

            TEST_ASSERT_TRUE(desc->find(self, probe, &actual));
            TEST_ASSERT_TRUE(assoc_model_value_matches(&assoc_model, probe,
                                                       actual));
        } else {
            TEST_ASSERT_FALSE(desc->find(self, probe, &value));
        }
    }

    if ((desc->caps & TEST_ASSOC_BOUNDS) != 0)
        assoc_model_check_bounds(desc, self, &assoc_model);
}

static bool assoc_model_has_key(const assoc_model_t* model, int32_t key)
{
    size_t at = assoc_model_lower(model, key);

    return at < model->size && model->pairs[at].key == key;
}

void test_assoc_model_run(const test_assoc_desc_t* desc)
{
    void* self;
    size_t step;

    assoc_model.size = 0;
    self = assoc_model_open(desc);

    for (step = 0; step < test_model_iters; ++step) {
        uint32_t op = (uint32_t)test_rng_bounded(8);
        int32_t key = (int32_t)test_rng_bounded(64) - 16;
        int32_t value = (int32_t)test_rng_bounded(2000) - 1000;

        if (assoc_model.size > ASSOC_MODEL_CAP - 16)
            op = 6;

        switch (op) {
        case 0:
            {
                bool inserted = false;
                bool expected = assoc_model_insert(desc, &assoc_model, key,
                                                   value);

                test_trace("assoc_insert", key, value, 0);
                TEST_ASSERT_TRUE(desc->insert(self, key, value, &inserted));
                TEST_ASSERT_EQ(expected ? 1 : 0, inserted ? 1 : 0);
            }
            break;

        case 1:
            if ((desc->caps & TEST_ASSOC_INSERT_REPLACE) != 0) {
                test_trace("assoc_insert_replace", key, value, 0);
                TEST_ASSERT_TRUE(desc->insert_replace(self, key, value));
                assoc_model_insert_replace(desc, &assoc_model, key, value);
            }
            break;

        case 2:
            {
                int32_t expected_next = 0;
                int32_t next_key = 0;
                bool expected_found = false;
                bool next_found = false;
                bool existed;

                if (test_rng_percent(25))
                    key = 500 + (int32_t)test_rng_bounded(64);
                test_trace("assoc_erase", key, 0, 0);
                existed = assoc_model_erase(&assoc_model, key, &expected_next,
                                            &expected_found);
                TEST_ASSERT_TRUE(desc->erase_key(self, key, &next_key,
                                                 &next_found));
                TEST_ASSERT_EQ(assoc_model.size, desc->size(self));
                if (!existed) {
                    TEST_ASSERT_FALSE(next_found);
                } else if ((desc->caps & TEST_ASSOC_ORDERED) != 0) {
                    TEST_ASSERT_EQ(expected_found ? 1 : 0,
                                   next_found ? 1 : 0);
                    if (next_found)
                        TEST_ASSERT_EQ(expected_next, next_key);
                } else if (assoc_model.size == 0) {
                    TEST_ASSERT_FALSE(next_found);
                } else if (next_found) {
                    TEST_ASSERT_TRUE(assoc_model_has_key(&assoc_model,
                                                         next_key));
                }
            }
            break;

        case 3:
            {
                size_t expected = assoc_model_count(&assoc_model, key);

                test_trace("assoc_remove", key, 0, 0);
                TEST_ASSERT_EQ(expected, desc->remove(self, key));
                TEST_ASSERT_EQ(expected, assoc_model_remove(&assoc_model, key));
                TEST_ASSERT_EQ(assoc_model.size, desc->size(self));
            }
            break;

        case 4:
            {
                int32_t expected_value = 0;
                int32_t actual = 0;
                bool found = assoc_model_find(&assoc_model, key,
                                              &expected_value);

                test_trace("assoc_find", key, 0, 0);
                TEST_ASSERT_EQ(found ? 1 : 0,
                               desc->find(self, key, &actual) ? 1 : 0);
                if (found)
                    TEST_ASSERT_TRUE(assoc_model_value_matches(&assoc_model,
                                                               key, actual));
                TEST_ASSERT_EQ(assoc_model_count(&assoc_model, key),
                               desc->count(self, key));
            }
            break;

        case 5:
            if ((desc->caps & TEST_ASSOC_BOUNDS) != 0)
                assoc_model_check_bounds(desc, self, &assoc_model);
            break;

        case 6:
            if (test_rng_percent(15) || assoc_model.size > ASSOC_MODEL_CAP - 16) {
                test_trace("assoc_clear", (long long)assoc_model.size, 0, 0);
                TEST_ASSERT_EQ(assoc_model.size, desc->clear(self));
                assoc_model.size = 0;
            }
            break;

        default:
            if (assoc_model.size > 0) {
                int32_t next_key = 0;
                bool next_found = false;
                size_t pick = test_rng_bounded(assoc_model.size);

                key = assoc_model.pairs[pick].key;
                test_trace("assoc_erase", key, 0, 1);
                TEST_ASSERT_TRUE(assoc_model_erase(&assoc_model, key,
                                                   &next_key, &next_found));
                TEST_ASSERT_TRUE(desc->erase_key(self, key, &next_key,
                                                 &next_found));
                TEST_ASSERT_EQ(assoc_model.size, desc->size(self));
            }
            break;
        }

        if ((step % 32) == 0)
            assoc_model_check(desc, self);
    }

    assoc_model_check(desc, self);
}

static void case_random(const void* ctx)
{
    const test_assoc_desc_t* desc = ctx;

    test_assoc_model_run(desc);
}

void test_assoc_model_register(const test_assoc_desc_t* desc)
{
    char name[96];

    snprintf(name, sizeof(name), "%s.random", desc->name);
    test_register_case("model", name, case_random, desc, __FILE__, __LINE__);
}
