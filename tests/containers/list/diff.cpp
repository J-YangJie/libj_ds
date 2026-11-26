/*
  libj_ds permanent tests list standard library differential
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
#include <functional>
#include <iterator>
#include <list>
#include <string>

#include "../../core/test.h"
#include "desc.h"

static void destroy_desc(void* self)
{
    list_desc_int32()->destroy(self);
}

static void destroy_str_desc(void* self)
{
    list_desc_string()->destroy(self);
}

struct int32_visit {
    const std::list<int32_t>* model;
    size_t                    at;
    bool                      reverse;
    bool                      ok;
};

static bool visit_int32(void* opaque, int32_t value)
{
    int32_visit* visit = static_cast<int32_visit*>(opaque);
    std::list<int32_t>::const_iterator it = visit->model->begin();
    size_t index;

    if (visit->at >= visit->model->size()) {
        visit->ok = false;
        return false;
    }
    index = visit->reverse ? visit->model->size() - 1 - visit->at : visit->at;
    std::advance(it, static_cast<std::ptrdiff_t>(index));
    if (*it != value) {
        visit->ok = false;
        return false;
    }
    ++visit->at;
    return true;
}

static int32_t int32_at(const std::list<int32_t>& model, size_t index)
{
    std::list<int32_t>::const_iterator it = model.begin();

    std::advance(it, static_cast<std::ptrdiff_t>(index));
    return *it;
}

static void check_int32(const test_seq_desc_t* desc, const void* self,
                        const std::list<int32_t>& model)
{
    int32_visit forward = { &model, 0, false, true };
    int32_visit reverse = { &model, 0, true, true };
    int32_t value = 0;
    size_t i;

    TEST_ASSERT_EQ(model.size(), desc->size(self));
    for (i = 0; i < model.size(); ++i) {
        TEST_ASSERT_TRUE(desc->get(self, i, &value));
        TEST_ASSERT_EQ(int32_at(model, i), value);
    }
    TEST_ASSERT_TRUE(desc->foreach(self, false, visit_int32, &forward));
    TEST_ASSERT_TRUE(forward.ok);
    TEST_ASSERT_EQ(model.size(), forward.at);
    TEST_ASSERT_TRUE(desc->foreach(self, true, visit_int32, &reverse));
    TEST_ASSERT_TRUE(reverse.ok);
    TEST_ASSERT_EQ(model.size(), reverse.at);
}

TEST_CASE(diff, list_int32)
{
    const test_seq_desc_t* desc = list_desc_int32();
    void* self = desc->create();
    void* other = desc->create();
    std::list<int32_t> model;
    std::list<int32_t> other_model;
    size_t step;

    TEST_ASSERT_NOT_NULL(self);
    TEST_ASSERT_NOT_NULL(other);
    test_cleanup_push(destroy_desc, self);
    test_cleanup_push(destroy_desc, other);

    for (step = 0; step < test_model_iters; ++step) {
        uint32_t op = static_cast<uint32_t>(test_rng_bounded(15));
        int32_t value = static_cast<int32_t>(test_rng_bounded(21)) - 10;
        size_t index;

        if (model.size() > 800)
            op = 12;

        switch (op) {
        case 0:
            TEST_ASSERT_TRUE(desc->push_back(self, value));
            model.push_back(value);
            break;

        case 1:
            TEST_ASSERT_TRUE(desc->push_front(self, value));
            model.push_front(value);
            break;

        case 2:
            if (!model.empty()) {
                desc->pop_back(self);
                model.pop_back();
            }
            break;

        case 3:
            if (!model.empty()) {
                desc->pop_front(self);
                model.pop_front();
            }
            break;

        case 4:
            index = test_rng_bounded(model.size() + 1);
            {
                size_t out = 0;

                TEST_ASSERT_TRUE(desc->insert(self, index, value, &out));
                TEST_ASSERT_EQ(index, out);
            }
            model.insert(std::next(model.begin(),
                                   static_cast<std::ptrdiff_t>(index)),
                         value);
            break;

        case 5:
            {
                size_t n = test_rng_bounded(5);
                size_t out = 0;

                index = test_rng_bounded(model.size() + 1);
                TEST_ASSERT_TRUE(desc->insert_n(self, index, n, value, &out));
                TEST_ASSERT_EQ(index, out);
                model.insert(std::next(model.begin(),
                                       static_cast<std::ptrdiff_t>(index)),
                             n, value);
            }
            break;

        case 6:
            {
                int32_t values[6];
                size_t n = test_rng_bounded(6);
                size_t out = 0;
                size_t i;

                for (i = 0; i < n; ++i)
                    values[i] = value + static_cast<int32_t>(i);
                index = test_rng_bounded(model.size() + 1);
                TEST_ASSERT_TRUE(desc->insert_range_values(self, index,
                                                           values, n, &out));
                TEST_ASSERT_EQ(index, out);
                model.insert(std::next(model.begin(),
                                       static_cast<std::ptrdiff_t>(index)),
                             values, values + n);
            }
            break;

        case 7:
            if (!model.empty()) {
                size_t out = 0;
                size_t old_size = model.size();

                index = test_rng_bounded(old_size);
                TEST_ASSERT_TRUE(desc->erase(self, index, &out));
                TEST_ASSERT_EQ(index < old_size - 1 ? index : old_size - 1,
                               out);
                model.erase(std::next(model.begin(),
                                      static_cast<std::ptrdiff_t>(index)));
            }
            break;

        case 8:
            {
                size_t out = 0;
                size_t first = test_rng_bounded(model.size() + 1);
                size_t last = first + test_rng_bounded(model.size() + 1 - first);

                TEST_ASSERT_TRUE(desc->erase_range(self, first, last, &out));
                TEST_ASSERT_EQ(first, out);
                model.erase(std::next(model.begin(),
                                      static_cast<std::ptrdiff_t>(first)),
                            std::next(model.begin(),
                                      static_cast<std::ptrdiff_t>(last)));
            }
            break;

        case 9:
            {
                size_t n = test_rng_bounded(40);

                TEST_ASSERT_TRUE(desc->assign(self, n, value));
                model.assign(n, value);
            }
            break;

        case 10:
            {
                int32_t values[5];
                size_t n = test_rng_bounded(5);
                size_t i;

                for (i = 0; i < n; ++i)
                    values[i] = value + static_cast<int32_t>(i * 2);
                TEST_ASSERT_TRUE(desc->assign_range_values(self, values, n));
                model.assign(values, values + n);
            }
            break;

        case 11:
            {
                size_t n = test_rng_bounded(60);

                TEST_ASSERT_TRUE(desc->resize(self, n, value));
                model.resize(n, value);
            }
            break;

        case 12:
            if (test_rng_percent(40)) {
                size_t expected = model.size();

                TEST_ASSERT_EQ(expected, desc->clear(self));
                model.clear();
            }
            break;

        case 13:
            {
                size_t expected =
                    static_cast<size_t>(std::count(model.begin(), model.end(),
                                                   value));
                size_t found = 0;

                if (!model.empty()) {
                    int32_t probe = int32_at(model, 0);
                    size_t probe_count = static_cast<size_t>(
                        std::count(model.begin(), model.end(), probe));

                    TEST_ASSERT_EQ(probe_count, desc->count(self, probe));
                    TEST_ASSERT_TRUE(desc->find(self, probe, &found));
                    TEST_ASSERT_EQ(0, found);
                }
                TEST_ASSERT_EQ(expected, desc->remove(self, value));
                model.remove(value);
                TEST_ASSERT_EQ(0, desc->count(self, value));
                TEST_ASSERT_FALSE(desc->find(self, value, &found));
            }
            break;

        case 14:
            if (test_rng_percent(20)) {
                desc->swap(self, other);
                model.swap(other_model);
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
    const std::list<std::string>* model;
    size_t                        at;
    bool                          reverse;
    bool                          ok;
};

static bool visit_str(void* opaque, const char* value)
{
    str_visit* visit = static_cast<str_visit*>(opaque);
    std::list<std::string>::const_iterator it = visit->model->begin();
    size_t index;

    if (visit->at >= visit->model->size()) {
        visit->ok = false;
        return false;
    }
    index = visit->reverse ? visit->model->size() - 1 - visit->at : visit->at;
    std::advance(it, static_cast<std::ptrdiff_t>(index));
    if (*it != value) {
        visit->ok = false;
        return false;
    }
    ++visit->at;
    return true;
}

static void check_str(const test_seq_str_desc_t* desc, const void* self,
                      const std::list<std::string>& model)
{
    str_visit forward = { &model, 0, false, true };
    str_visit reverse = { &model, 0, true, true };
    char buffer[128];
    size_t i;

    TEST_ASSERT_EQ(model.size(), desc->size(self));
    for (i = 0; i < model.size(); ++i) {
        TEST_ASSERT_TRUE(desc->get(self, i, buffer, sizeof(buffer)));
        TEST_ASSERT_STR_EQ(std::next(model.begin(),
                                     static_cast<std::ptrdiff_t>(i))->c_str(),
                           buffer);
    }
    TEST_ASSERT_TRUE(desc->foreach(self, false, visit_str, &forward));
    TEST_ASSERT_TRUE(forward.ok);
    TEST_ASSERT_EQ(model.size(), forward.at);
    TEST_ASSERT_TRUE(desc->foreach(self, true, visit_str, &reverse));
    TEST_ASSERT_TRUE(reverse.ok);
    TEST_ASSERT_EQ(model.size(), reverse.at);
}

TEST_CASE(diff, list_string)
{
    static const char* const pool[] = {
        "a", "short", "abcdefghijklmno", "abcdefghijklmnop",
        "this string is longer than the inline buffer", "tail"
    };
    const test_seq_str_desc_t* desc = list_desc_string();
    void* self = desc->create();
    void* other = desc->create();
    std::list<std::string> model;
    std::list<std::string> other_model;
    size_t step;

    TEST_ASSERT_NOT_NULL(self);
    TEST_ASSERT_NOT_NULL(other);
    test_cleanup_push(destroy_str_desc, self);
    test_cleanup_push(destroy_str_desc, other);

    for (step = 0; step < test_model_iters; ++step) {
        uint32_t op = static_cast<uint32_t>(test_rng_bounded(6));
        const char* value =
            pool[test_rng_bounded(sizeof(pool) / sizeof(pool[0]))];
        size_t index;

        if (model.size() > 300)
            op = 4;

        switch (op) {
        case 0:
            TEST_ASSERT_TRUE(desc->push_back(self, value));
            model.push_back(value);
            break;

        case 1:
            TEST_ASSERT_TRUE(desc->push_front(self, value));
            model.push_front(value);
            break;

        case 2:
            index = test_rng_bounded(model.size() + 1);
            {
                size_t out = 0;

                TEST_ASSERT_TRUE(desc->insert(self, index, value, &out));
                TEST_ASSERT_EQ(index, out);
            }
            model.insert(std::next(model.begin(),
                                   static_cast<std::ptrdiff_t>(index)),
                         value);
            break;

        case 3:
            if (!model.empty()) {
                size_t out = 0;
                size_t old_size = model.size();

                index = test_rng_bounded(old_size);
                TEST_ASSERT_TRUE(desc->erase(self, index, &out));
                TEST_ASSERT_EQ(index < old_size - 1 ? index : old_size - 1,
                               out);
                model.erase(std::next(model.begin(),
                                      static_cast<std::ptrdiff_t>(index)));
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

static void int32_ext_push_random(void* self, std::list<int32_t>* model,
                                  size_t count)
{
    const test_seq_desc_t* desc = list_desc_int32();
    size_t i;

    for (i = 0; i < count; ++i) {
        int32_t value = static_cast<int32_t>(test_rng_bounded(15));
        TEST_ASSERT_TRUE(desc->push_back(self, value));
        model->push_back(value);
    }
}

TEST_CASE(diff, list_int32_ext)
{
    const test_seq_desc_t* desc = list_desc_int32();
    const test_list_ext_desc_t* ext = list_desc_ext_int32();
    void* self = desc->create();
    void* source = desc->create();
    std::list<int32_t> model;
    std::list<int32_t> source_model;
    size_t step;

    TEST_ASSERT_NOT_NULL(self);
    TEST_ASSERT_NOT_NULL(source);
    test_cleanup_push(destroy_desc, self);
    test_cleanup_push(destroy_desc, source);

    for (step = 0; step < test_model_iters; ++step) {
        uint32_t op = static_cast<uint32_t>(test_rng_bounded(9));
        bool ascending = test_rng_percent(60);
        size_t index;

        if (model.size() > 600)
            op = 0;

        switch (op) {
        case 0:
            {
                size_t n = 1 + test_rng_bounded(8);

                int32_ext_push_random(self, &model, n);
            }
            break;

        case 1:
            {
                size_t n = 1 + test_rng_bounded(5);

                int32_ext_push_random(source, &source_model, n);
            }
            break;

        case 2:
            ext->sort(self, ascending);
            if (ascending)
                model.sort();
            else
                model.sort(std::greater<int32_t>());
            TEST_ASSERT_TRUE(ext->sorted(self, ascending));
            break;

        case 3:
            ext->reverse(self);
            model.reverse();
            break;

        case 4:
            {
                size_t before = model.size();
                size_t removed = ext->unique(self);

                model.unique();
                TEST_ASSERT_EQ(before - model.size(), removed);
            }
            break;

        case 5:
            if (!model.empty()) {
                int32_t value = static_cast<int32_t>(test_rng_bounded(15));
                std::list<int32_t>::iterator it = model.begin();

                index = test_rng_bounded(model.size());
                std::advance(it, static_cast<std::ptrdiff_t>(index));
                TEST_ASSERT_TRUE(ext->replace(self, index, value));
                *it = value;
            }
            break;

        case 6:
            {
                size_t splice_at = test_rng_bounded(model.size() + 1);
                std::list<int32_t>::iterator pos = model.begin();

                std::advance(pos, static_cast<std::ptrdiff_t>(splice_at));
                ext->splice(self, splice_at, source);
                model.splice(pos, source_model);
            }
            break;

        case 7:
            if (source_model.size() > 0) {
                size_t first = test_rng_bounded(source_model.size() + 1);
                size_t last = first
                              + test_rng_bounded(source_model.size() + 1
                                                 - first);
                std::list<int32_t>::iterator pos = model.begin();
                std::list<int32_t>::iterator range_begin = source_model.begin();
                std::list<int32_t>::iterator range_end = source_model.begin();

                index = test_rng_bounded(model.size() + 1);
                std::advance(pos, static_cast<std::ptrdiff_t>(index));
                std::advance(range_begin, static_cast<std::ptrdiff_t>(first));
                std::advance(range_end, static_cast<std::ptrdiff_t>(last));
                ext->splice_range(self, index, source, first, last);
                model.splice(pos, source_model, range_begin, range_end);
            }
            break;

        case 8:
            ext->sort(self, ascending);
            ext->sort(source, ascending);
            if (ascending) {
                model.sort();
                source_model.sort();
                ext->merge(self, source, true);
                model.merge(source_model);
            } else {
                model.sort(std::greater<int32_t>());
                source_model.sort(std::greater<int32_t>());
                ext->merge(self, source, false);
                model.merge(source_model, std::greater<int32_t>());
            }
            break;
        }

        if ((step % 16) == 0) {
            check_int32(desc, self, model);
            check_int32(desc, source, source_model);
        }
    }

    check_int32(desc, self, model);
    check_int32(desc, source, source_model);
}

static void str_ext_push_random(void* self, std::list<std::string>* model,
                                size_t count)
{
    static const char* const pool[] = {
        "a", "bb", "short", "abcdefghijklmno", "abcdefghijklmnop",
        "this string is longer than the inline buffer", "tail"
    };
    const test_seq_str_desc_t* desc = list_desc_string();
    size_t i;

    for (i = 0; i < count; ++i) {
        const char* value =
            pool[test_rng_bounded(sizeof(pool) / sizeof(pool[0]))];

        TEST_ASSERT_TRUE(desc->push_back(self, value));
        model->push_back(value);
    }
}

TEST_CASE(diff, list_string_ext)
{
    const test_seq_str_desc_t* desc = list_desc_string();
    const test_list_str_ext_desc_t* ext = list_desc_ext_string();
    void* self = desc->create();
    void* source = desc->create();
    std::list<std::string> model;
    std::list<std::string> source_model;
    size_t step;

    TEST_ASSERT_NOT_NULL(self);
    TEST_ASSERT_NOT_NULL(source);
    test_cleanup_push(destroy_str_desc, self);
    test_cleanup_push(destroy_str_desc, source);

    for (step = 0; step < test_model_iters; ++step) {
        uint32_t op = static_cast<uint32_t>(test_rng_bounded(9));
        bool ascending = test_rng_percent(60);

        if (model.size() > 300)
            op = 0;

        switch (op) {
        case 0:
            str_ext_push_random(self, &model, 1 + test_rng_bounded(5));
            break;

        case 1:
            str_ext_push_random(source, &source_model, 1 + test_rng_bounded(4));
            break;

        case 2:
            ext->sort(self, ascending);
            if (ascending)
                model.sort();
            else
                model.sort(std::greater<std::string>());
            TEST_ASSERT_TRUE(ext->sorted(self, ascending));
            break;

        case 3:
            ext->reverse(self);
            model.reverse();
            break;

        case 4:
            {
                size_t before = model.size();

                ext->unique(self);
                model.unique();
                TEST_ASSERT_EQ(before - model.size(),
                               before - desc->size(self));
            }
            break;

        case 5:
            if (!model.empty()) {
                static const char* const values[] = {
                    "a", "bb", "abcdefghijklmnop", "tail"
                };
                const char* value =
                    values[test_rng_bounded(sizeof(values) / sizeof(values[0]))];
                size_t index = test_rng_bounded(model.size());
                std::list<std::string>::iterator it = model.begin();

                std::advance(it, static_cast<std::ptrdiff_t>(index));
                TEST_ASSERT_TRUE(ext->replace(self, index, value));
                *it = value;
            }
            break;

        case 6:
            {
                size_t index = test_rng_bounded(model.size() + 1);
                std::list<std::string>::iterator pos = model.begin();

                ext->splice(self, index, source);
                std::advance(pos, static_cast<std::ptrdiff_t>(index));
                model.splice(pos, source_model);
            }
            break;

        case 7:
            if (source_model.size() > 0) {
                size_t first = test_rng_bounded(source_model.size() + 1);
                size_t last = first
                              + test_rng_bounded(source_model.size() + 1
                                                 - first);
                size_t index = test_rng_bounded(model.size() + 1);
                std::list<std::string>::iterator pos = model.begin();
                std::list<std::string>::iterator range_begin =
                    source_model.begin();
                std::list<std::string>::iterator range_end =
                    source_model.begin();

                std::advance(pos, static_cast<std::ptrdiff_t>(index));
                std::advance(range_begin, static_cast<std::ptrdiff_t>(first));
                std::advance(range_end, static_cast<std::ptrdiff_t>(last));
                ext->splice_range(self, index, source, first, last);
                model.splice(pos, source_model, range_begin, range_end);
            }
            break;

        case 8:
            ext->sort(self, ascending);
            ext->sort(source, ascending);
            if (ascending) {
                model.sort();
                source_model.sort();
                ext->merge(self, source, true);
                model.merge(source_model);
            } else {
                model.sort(std::greater<std::string>());
                source_model.sort(std::greater<std::string>());
                ext->merge(self, source, false);
                model.merge(source_model, std::greater<std::string>());
            }
            break;
        }

        if ((step % 16) == 0) {
            check_str(desc, self, model);
            check_str(desc, source, source_model);
        }
    }

    check_str(desc, self, model);
    check_str(desc, source, source_model);
}
