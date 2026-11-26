/*
  libj_ds permanent tests list stress
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

#include <list/list.h>

#include <string.h>

#include "../../core/test.h"
#include "desc.h"
#include "value.h"

#define STRESS_CAP 2048
#define STRESS_OTHER_CAP 512

static void list_free(void* self)
{
    list_t* list = self;

    LIST_DELETE(&list);
}

static void stress_model_insert(int32_t* values, size_t* size, size_t index,
                                int32_t value)
{
    memmove(values + index + 1, values + index,
            (*size - index) * sizeof(values[0]));
    values[index] = value;
    ++*size;
}

static void stress_model_erase_range(int32_t* values, size_t* size,
                                     size_t first, size_t last)
{
    memmove(values + first, values + last,
            (*size - last) * sizeof(values[0]));
    *size -= last - first;
}

static void stress_model_sort(int32_t* values, size_t size, bool ascending)
{
    size_t i;

    for (i = 1; i < size; ++i) {
        int32_t key = values[i];
        size_t j = i;

        while (j > 0 && (ascending ? values[j - 1] > key
                                   : values[j - 1] < key)) {
            values[j] = values[j - 1];
            --j;
        }
        values[j] = key;
    }
}

static size_t stress_model_unique(int32_t* values, size_t size)
{
    size_t write = 0;
    size_t read;

    if (size == 0)
        return 0;
    for (read = 1; read < size; ++read) {
        if (values[read] != values[write])
            values[++write] = values[read];
    }
    return size - (write + 1);
}

static void stress_model_splice(int32_t* target, size_t* target_size,
                                size_t index, int32_t* source,
                                size_t* source_size, size_t first,
                                size_t last)
{
    size_t n = last - first;

    memmove(target + index + n, target + index,
            (*target_size - index) * sizeof(target[0]));
    memcpy(target + index, source + first, n * sizeof(target[0]));
    *target_size += n;
    memmove(source + first, source + last,
            (*source_size - last) * sizeof(source[0]));
    *source_size -= n;
}

static void stress_model_merge(int32_t* target, size_t* target_size,
                               const int32_t* source, size_t source_size)
{
    int32_t merged[STRESS_CAP + STRESS_OTHER_CAP];
    size_t left = 0;
    size_t right = 0;
    size_t out = 0;

    while (left < *target_size && right < source_size) {
        if (target[left] <= source[right])
            merged[out++] = target[left++];
        else
            merged[out++] = source[right++];
    }
    while (left < *target_size)
        merged[out++] = target[left++];
    while (right < source_size)
        merged[out++] = source[right++];
    memcpy(target, merged, out * sizeof(target[0]));
    *target_size = out;
}

static void stress_check(const list_t* list, const int32_t* values,
                         size_t size)
{
    list_iterator_t it;
    list_iterator_t end;
    list_r_iterator_t rit;
    list_r_iterator_t rend;
    size_t i;

    TEST_ASSERT_NULL(list_check_invariants(list));
    TEST_ASSERT_EQ(size, (size_t)clist->size(list));

    end = clist->end(list);
    i = 0;
    for (it = clist->begin(list); it.d != end.d; it = clist->next(it)) {
        TEST_ASSERT_TRUE(i < size);
        TEST_ASSERT_EQ(values[i], it_s32data(it));
        ++i;
    }
    TEST_ASSERT_EQ(size, i);

    rend = clist->rend(list);
    i = 0;
    for (rit = clist->rbegin(list); rit.d != rend.d; rit = clist->rnext(rit)) {
        TEST_ASSERT_TRUE(i < size);
        TEST_ASSERT_EQ(values[size - 1 - i], it_s32data(rit));
        ++i;
    }
    TEST_ASSERT_EQ(size, i);
}

TEST_CASE(stress, list_int32_model)
{
    test_sequence_model_run(list_desc_int32(), test_model_iters, false);
}

TEST_CASE(stress, list_int32_invariants)
{
    list_t* list = LIST_NEW_T(int32_t);
    list_t* other = LIST_NEW_T(int32_t);
    int32_t values[STRESS_CAP];
    int32_t other_values[STRESS_OTHER_CAP];
    size_t size = 0;
    size_t other_size = 0;
    size_t step;

    TEST_ASSERT_NOT_NULL(list);
    TEST_ASSERT_NOT_NULL(other);
    test_cleanup_push(list_free, list);
    test_cleanup_push(list_free, other);

    for (step = 0; step < test_model_iters; ++step) {
        uint32_t op = (uint32_t)test_rng_bounded(20);
        int32_t value = (int32_t)test_rng_bounded(21) - 10;
        size_t index;

        if (size > STRESS_CAP - 64) {
            test_trace("cap_clear", (long long)size, 0, 0);
            TEST_ASSERT_EQ(size, (size_t)clist->clear(list));
            size = 0;
        }
        if (other_size > STRESS_OTHER_CAP - 32) {
            test_trace("other_clear", (long long)other_size, 0, 0);
            TEST_ASSERT_EQ(other_size, (size_t)clist->clear(other));
            other_size = 0;
        }

        switch (op) {
        case 0:
            test_trace("push_back", value, 0, 0);
            TEST_ASSERT_TRUE(clist->push_back(list, __ds_s32data(value)));
            values[size++] = value;
            break;

        case 1:
            test_trace("push_front", value, 0, 0);
            TEST_ASSERT_TRUE(clist->push_front(list, __ds_s32data(value)));
            stress_model_insert(values, &size, 0, value);
            break;

        case 2:
            if (size > 0) {
                test_trace("pop_back", 0, 0, 0);
                clist->pop_back(list);
                --size;
            }
            break;

        case 3:
            if (size > 0) {
                test_trace("pop_front", 0, 0, 0);
                clist->pop_front(list);
                stress_model_erase_range(values, &size, 0, 1);
            }
            break;

        case 4:
            index = test_rng_bounded(size + 1);
            test_trace("insert", (long long)index, value, 0);
            TEST_ASSERT_TRUE(clist->insert(list,
                                           index == size
                                               ? clist->end(list)
                                               : clist->it(list, index),
                                           __ds_s32data(value)).d != NULL);
            stress_model_insert(values, &size, index, value);
            break;

        case 5:
            {
                size_t n = test_rng_bounded(6);

                index = test_rng_bounded(size + 1);
                test_trace("insert_n", (long long)index, (long long)n, 0);
                TEST_ASSERT_TRUE(clist->insert_n(list,
                                                 index == size
                                                     ? clist->end(list)
                                                     : clist->it(list, index),
                                                 (list_size_t)n,
                                                 __ds_s32data(value)).d
                                 != NULL);
                while (n-- > 0)
                    stress_model_insert(values, &size, index++, value);
            }
            break;

        case 6:
            if (size > 0) {
                index = test_rng_bounded(size);
                test_trace("erase", (long long)index, 0, 0);
                TEST_ASSERT_TRUE(clist->erase(list, clist->it(list, index)).d
                                 != NULL);
                stress_model_erase_range(values, &size, index, index + 1);
            }
            break;

        case 7:
            {
                size_t first = test_rng_bounded(size + 1);
                size_t last = first + test_rng_bounded(size + 1 - first);

                test_trace("erase_range", (long long)first,
                           (long long)last, 0);
                TEST_ASSERT_TRUE(clist->erase_range(
                                     list,
                                     first == size ? clist->end(list)
                                                   : clist->it(list, first),
                                     last == size ? clist->end(list)
                                                  : clist->it(list, last)).d
                                 != NULL);
                stress_model_erase_range(values, &size, first, last);
            }
            break;

        case 8:
            {
                size_t n = test_rng_bounded(65);

                test_trace("assign", (long long)n, value, 0);
                TEST_ASSERT_TRUE(clist->assign(list, (list_size_t)n,
                                               __ds_s32data(value)));
                for (index = 0; index < n; ++index)
                    values[index] = value;
                size = n;
            }
            break;

        case 9:
            {
                size_t n = test_rng_bounded(size + 33);

                test_trace("resize", (long long)n, value, 0);
                TEST_ASSERT_TRUE(clist->resize(list, (list_size_t)n,
                                               __ds_s32data(value)));
                if (n < size) {
                    size = n;
                } else {
                    while (size < n)
                        values[size++] = value;
                }
            }
            break;

        case 10:
            if (test_rng_percent(15)) {
                test_trace("clear", (long long)size, 0, 0);
                TEST_ASSERT_EQ(size, (size_t)clist->clear(list));
                size = 0;
            }
            break;

        case 11:
            test_trace("sort_asc", (long long)size, 0, 0);
            clist->sort(list, __list_lt_default);
            stress_model_sort(values, size, true);
            TEST_ASSERT_TRUE(clist_sorted_default(list, true));
            break;

        case 12:
            test_trace("sort_desc", (long long)size, 0, 0);
            clist->sort(list, __list_gt_default);
            stress_model_sort(values, size, false);
            TEST_ASSERT_TRUE(clist_sorted_default(list, false));
            break;

        case 13:
            test_trace("reverse", (long long)size, 0, 0);
            clist->reverse(list);
            for (index = 0; index < size / 2; ++index) {
                int32_t temporary = values[index];

                values[index] = values[size - 1 - index];
                values[size - 1 - index] = temporary;
            }
            break;

        case 14:
            {
                size_t removed = (size_t)clist->unique(list);

                test_trace("unique", (long long)size, 0, 0);
                TEST_ASSERT_EQ(stress_model_unique(values, size), removed);
                size -= removed;
            }
            break;

        case 15:
            if (other_size > 0 && size + other_size <= STRESS_CAP) {
                index = test_rng_bounded(size + 1);
                test_trace("splice", (long long)index,
                           (long long)other_size, 0);
                clist->splice(list,
                              index == size ? clist->end(list)
                                            : clist->it(list, index),
                              other);
                stress_model_splice(values, &size, index, other_values,
                                    &other_size, 0, other_size);
                TEST_ASSERT_EQ(0, clist->size(other));
            }
            break;

        case 16:
            if (other_size > 0 && size + other_size <= STRESS_CAP) {
                size_t first = test_rng_bounded(other_size + 1);
                size_t last = first + test_rng_bounded(other_size + 1 - first);

                index = test_rng_bounded(size + 1);
                test_trace("splice_range", (long long)index,
                           (long long)first, (long long)last);
                clist->splice_range(list,
                                    index == size ? clist->end(list)
                                                  : clist->it(list, index),
                                    other,
                                    first == other_size
                                        ? clist->end(other)
                                        : clist->it(other, first),
                                    last == other_size
                                        ? clist->end(other)
                                        : clist->it(other, last));
                stress_model_splice(values, &size, index, other_values,
                                    &other_size, first, last);
            }
            break;

        case 17:
            test_trace("other_push", value, 0, 0);
            TEST_ASSERT_TRUE(clist->push_back(other, __ds_s32data(value)));
            other_values[other_size++] = value;
            break;

        case 18:
            if (other_size == 0) {
                size_t n = 1 + test_rng_bounded(8);

                test_trace("other_fill", (long long)n, 0, 0);
                while (n-- > 0) {
                    TEST_ASSERT_TRUE(clist->push_back(other,
                                                      __ds_s32data(value)));
                    other_values[other_size++] = value;
                }
            }
            if (size + other_size <= STRESS_CAP) {
                test_trace("merge", (long long)size,
                           (long long)other_size, 0);
                clist->sort(list, __list_lt_default);
                clist->sort(other, __list_lt_default);
                stress_model_sort(values, size, true);
                stress_model_sort(other_values, other_size, true);
                clist->merge(list, other, __list_lt_default);
                stress_model_merge(values, &size, other_values, other_size);
                other_size = 0;
                TEST_ASSERT_TRUE(clist_sorted_default(list, true));
            }
            break;

        case 19:
            if (size > 0) {
                index = test_rng_bounded(size);
                test_trace("replace", (long long)index, value, 0);
                TEST_ASSERT_TRUE(clist->replace_index(list, (list_index_t)index,
                                                      __ds_s32data(value)));
                values[index] = value;
            }
            break;
        }

        if ((step % 256) == 0) {
            stress_check(list, values, size);
            stress_check(other, other_values, other_size);
        }
    }

    stress_check(list, values, size);
    stress_check(other, other_values, other_size);
}
