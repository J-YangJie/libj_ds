/*
  libj_ds permanent tests vector standard library differential
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

/*
  This file includes only test-owned headers. The production C headers are
  reached through the descriptor tables, so no libj_ds header needs C++
  linkage guards here.
*/

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <string>
#include <vector>

#include "../../core/test.h"
#include "desc.h"

static void destroy_desc(void* self)
{
    vector_desc_int32()->destroy(self);
}

static void destroy_str_desc(void* self)
{
    vector_desc_string()->destroy(self);
}

struct int32_visit {
    const std::vector<int32_t>* model;
    size_t                      at;
    bool                        reverse;
    bool                        ok;
};

static bool visit_int32(void* opaque, int32_t value)
{
    int32_visit* visit = static_cast<int32_visit*>(opaque);
    size_t index;

    if (visit->at >= visit->model->size()) {
        visit->ok = false;
        return false;
    }
    index = visit->reverse ? visit->model->size() - 1 - visit->at : visit->at;
    if ((*visit->model)[index] != value) {
        visit->ok = false;
        return false;
    }
    ++visit->at;
    return true;
}

static void check_int32(const test_seq_desc_t* desc, const void* self,
                        const std::vector<int32_t>& model)
{
    int32_visit forward = { &model, 0, false, true };
    int32_visit reverse = { &model, 0, true, true };
    int32_t value = 0;
    size_t i;

    TEST_ASSERT_EQ(model.size(), desc->size(self));
    for (i = 0; i < model.size(); ++i) {
        TEST_ASSERT_TRUE(desc->get(self, i, &value));
        TEST_ASSERT_EQ(model[i], value);
    }
    TEST_ASSERT_TRUE(desc->foreach(self, false, visit_int32, &forward));
    TEST_ASSERT_TRUE(forward.ok);
    TEST_ASSERT_EQ(model.size(), forward.at);
    TEST_ASSERT_TRUE(desc->foreach(self, true, visit_int32, &reverse));
    TEST_ASSERT_TRUE(reverse.ok);
    TEST_ASSERT_EQ(model.size(), reverse.at);
}

TEST_CASE(diff, vector_int32)
{
    const test_seq_desc_t* desc = vector_desc_int32();
    void* self = desc->create();
    void* other = desc->create();
    std::vector<int32_t> model;
    std::vector<int32_t> other_model;
    size_t step;

    TEST_ASSERT_NOT_NULL(self);
    TEST_ASSERT_NOT_NULL(other);
    test_cleanup_push(destroy_desc, self);
    test_cleanup_push(destroy_desc, other);

    for (step = 0; step < test_model_iters; ++step) {
        uint32_t op = static_cast<uint32_t>(test_rng_bounded(12));
        int32_t value = static_cast<int32_t>(test_rng_bounded(41)) - 20;
        size_t index;

        if (model.size() > 3000)
            op = 8;

        switch (op) {
        case 0:
            TEST_ASSERT_TRUE(desc->push_back(self, value));
            model.push_back(value);
            break;

        case 1:
            if (!model.empty()) {
                desc->pop_back(self);
                model.pop_back();
            }
            break;

        case 2:
            index = test_rng_bounded(model.size() + 1);
            {
                size_t out = 0;

                TEST_ASSERT_TRUE(desc->insert(self, index, value, &out));
                TEST_ASSERT_EQ(index, out);
            }
            model.insert(model.begin() + index, value);
            break;

        case 3:
            if (!model.empty()) {
                size_t out = 0;

                index = test_rng_bounded(model.size());
                TEST_ASSERT_TRUE(desc->erase(self, index, &out));
                TEST_ASSERT_EQ(index < model.size() - 1 ? index
                                                        : model.size() - 1,
                               out);
                model.erase(model.begin() + index);
            }
            break;

        case 4:
            {
                size_t n = test_rng_bounded(33);

                TEST_ASSERT_TRUE(desc->assign(self, n, value));
                model.assign(n, value);
            }
            break;

        case 5:
            {
                size_t n = test_rng_bounded(65);

                TEST_ASSERT_TRUE(desc->resize(self, n, value));
                model.resize(n, value);
            }
            break;

        case 6:
            {
                size_t expected = static_cast<size_t>(
                    std::count(model.begin(), model.end(), value));

                TEST_ASSERT_EQ(expected, desc->remove(self, value));
                model.erase(std::remove(model.begin(), model.end(), value),
                            model.end());
            }
            break;

        case 7:
            if (test_rng_percent(20)) {
                desc->swap(self, other);
                model.swap(other_model);
            }
            break;

        case 8:
            if (test_rng_percent(30)) {
                size_t expected = model.size();

                TEST_ASSERT_EQ(expected, desc->clear(self));
                model.clear();
            }
            break;

        case 9:
            TEST_ASSERT_TRUE(desc->reserve(self,
                                           test_rng_bounded(model.size() + 80)));
            break;

        case 10:
            if (test_rng_percent(20)) {
                desc->shrink_to_fit(self);
                model.shrink_to_fit();
            }
            break;

        case 11:
            {
                size_t n = test_rng_bounded(9);
                size_t out = 0;

                index = test_rng_bounded(model.size() + 1);
                TEST_ASSERT_TRUE(desc->insert_n(self, index, n, value, &out));
                TEST_ASSERT_EQ(index, out);
                model.insert(model.begin() + index, n, value);
            }
            break;
        }

        if ((step % 16) == 0) {
            check_int32(desc, self, model);
            check_int32(desc, other, other_model);
        }
    }

    check_int32(desc, self, model);
    check_int32(desc, other, other_model);
}

struct str_visit {
    const std::vector<std::string>* model;
    size_t                          at;
    bool                            reverse;
    bool                            ok;
};

static bool visit_str(void* opaque, const char* value)
{
    str_visit* visit = static_cast<str_visit*>(opaque);
    size_t index;

    if (visit->at >= visit->model->size()) {
        visit->ok = false;
        return false;
    }
    index = visit->reverse ? visit->model->size() - 1 - visit->at : visit->at;
    if ((*visit->model)[index] != value) {
        visit->ok = false;
        return false;
    }
    ++visit->at;
    return true;
}

static void check_str(const test_seq_str_desc_t* desc, const void* self,
                      const std::vector<std::string>& model)
{
    str_visit forward = { &model, 0, false, true };
    str_visit reverse = { &model, 0, true, true };
    char buffer[128];
    size_t i;

    TEST_ASSERT_EQ(model.size(), desc->size(self));
    for (i = 0; i < model.size(); ++i) {
        TEST_ASSERT_TRUE(desc->get(self, i, buffer, sizeof(buffer)));
        TEST_ASSERT_STR_EQ(model[i].c_str(), buffer);
    }
    TEST_ASSERT_TRUE(desc->foreach(self, false, visit_str, &forward));
    TEST_ASSERT_TRUE(forward.ok);
    TEST_ASSERT_EQ(model.size(), forward.at);
    TEST_ASSERT_TRUE(desc->foreach(self, true, visit_str, &reverse));
    TEST_ASSERT_TRUE(reverse.ok);
    TEST_ASSERT_EQ(model.size(), reverse.at);
}

TEST_CASE(diff, vector_string)
{
    static const char* const pool[] = {
        "a", "short", "abcdefghijklmno", "abcdefghijklmnop",
        "this string is longer than the inline buffer", "tail"
    };
    const test_seq_str_desc_t* desc = vector_desc_string();
    void* self = desc->create();
    void* other = desc->create();
    std::vector<std::string> model;
    std::vector<std::string> other_model;
    size_t step;

    TEST_ASSERT_NOT_NULL(self);
    TEST_ASSERT_NOT_NULL(other);
    test_cleanup_push(destroy_str_desc, self);
    test_cleanup_push(destroy_str_desc, other);

    for (step = 0; step < test_model_iters; ++step) {
        uint32_t op = static_cast<uint32_t>(test_rng_bounded(6));
        const char* value = pool[test_rng_bounded(sizeof(pool) / sizeof(pool[0]))];
        size_t index;

        if (model.size() > 200)
            op = 4;

        switch (op) {
        case 0:
            TEST_ASSERT_TRUE(desc->push_back(self, value));
            model.push_back(value);
            break;

        case 1:
            TEST_ASSERT_TRUE(desc->push_front(self, value));
            model.insert(model.begin(), value);
            break;

        case 2:
            index = test_rng_bounded(model.size() + 1);
            {
                size_t out = 0;

                TEST_ASSERT_TRUE(desc->insert(self, index, value, &out));
                TEST_ASSERT_EQ(index, out);
            }
            model.insert(model.begin() + index, value);
            break;

        case 3:
            if (!model.empty()) {
                size_t out = 0;

                index = test_rng_bounded(model.size());
                TEST_ASSERT_TRUE(desc->erase(self, index, &out));
                TEST_ASSERT_EQ(index < model.size() - 1 ? index
                                                        : model.size() - 1,
                               out);
                model.erase(model.begin() + index);
            }
            break;

        case 4:
            if (test_rng_percent(50)) {
                size_t expected = model.size();

                TEST_ASSERT_EQ(expected, desc->clear(self));
                model.clear();
            }
            break;

        case 5:
            if (test_rng_percent(20)) {
                desc->swap(self, other);
                model.swap(other_model);
            }
            break;
        }

        if ((step % 16) == 0) {
            check_str(desc, self, model);
            check_str(desc, other, other_model);
        }
    }

    check_str(desc, self, model);
    check_str(desc, other, other_model);
}
