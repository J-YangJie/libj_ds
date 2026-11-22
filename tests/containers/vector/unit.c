/*
  libj_ds permanent tests vector unit semantics
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
#include "value.h"

vector_sort_define_sso_less(sort_sso_asc)

static void vector_free(void* self)
{
    vector_t* vector = self;

    VECTOR_DELETE(&vector);
}

static vector_t* open_int32(void)
{
    vector_t* vector = VECTOR_NEW_T(int32_t);

    TEST_ASSERT_NOT_NULL(vector);
    test_cleanup_push(vector_free, vector);
    TEST_ASSERT_NULL(vector_check_invariants(vector));
    return vector;
}

static vector_t* open_int32_capacity(vector_size_t capacity_init)
{
    vector_t* vector = VECTOR_NEW_COMPLATE(NULL, SEQ_CNTR_DTYPE_S32,
                                           int32_t, capacity_init);

    TEST_ASSERT_NOT_NULL(vector);
    test_cleanup_push(vector_free, vector);
    TEST_ASSERT_NULL(vector_check_invariants(vector));
    return vector;
}

static void array_insert(int32_t* values, size_t* size, size_t index,
                         size_t count, int32_t fill)
{
    size_t i;

    memmove(values + index + count, values + index,
            (*size - index) * sizeof(values[0]));
    for (i = 0; i < count; ++i)
        values[index + i] = fill;
    *size += count;
}

static void array_erase(int32_t* values, size_t* size, size_t index,
                        size_t count)
{
    memmove(values + index, values + index + count,
            (*size - index - count) * sizeof(values[0]));
    *size -= count;
}

static void push_all(vector_t* vector, int32_t first, size_t count)
{
    size_t i;

    for (i = 0; i < count; ++i)
        TEST_ASSERT_TRUE(cvector->push_back(vector,
                                            __ds_s32data(first + (int32_t)i)));
}

static void expect_values(const vector_t* vector, const int32_t* values,
                          size_t count)
{
    vector_iterator_t it;
    vector_iterator_t end;
    vector_r_iterator_t rit;
    vector_r_iterator_t rend;
    size_t i;

    TEST_ASSERT_NULL(vector_check_invariants(vector));
    TEST_ASSERT_EQ(count, (size_t)cvector->size(vector));

    end = cvector->end(vector);
    i = 0;
    for (it = cvector->begin(vector); it.cur != end.cur;
         it = cvector->next(it)) {
        TEST_ASSERT_TRUE(i < count);
        TEST_ASSERT_EQ(values[i], it_s32data(it));
        ++i;
    }
    TEST_ASSERT_EQ(count, i);

    rend = cvector->rend(vector);
    i = 0;
    for (rit = cvector->rbegin(vector); rit.cur != rend.cur;
         rit = cvector->rnext(rit)) {
        TEST_ASSERT_TRUE(i < count);
        TEST_ASSERT_EQ(values[count - 1 - i], it_s32data(rit));
        ++i;
    }
    TEST_ASSERT_EQ(count, i);
}

static vector_t* open_strings(const char* const* values, size_t count,
                              vector_size_t capacity)
{
    vector_t* vector = VECTOR_NEW_SSO();
    size_t i;

    TEST_ASSERT_NOT_NULL(vector);
    test_cleanup_push(vector_free, vector);
    if (capacity > 0)
        TEST_ASSERT_TRUE(cvector->reserve(vector, capacity));
    for (i = 0; i < count; ++i)
        TEST_ASSERT_TRUE(cvector->push_back(vector, ds_sdata(values[i])));
    TEST_ASSERT_NULL(vector_check_invariants(vector));
    return vector;
}

static void expect_strings(const vector_t* vector, const char* const* values,
                           size_t count)
{
    size_t i;

    TEST_ASSERT_NULL(vector_check_invariants(vector));
    TEST_ASSERT_EQ(count, (size_t)cvector->size(vector));
    for (i = 0; i < count; ++i)
        TEST_ASSERT_STR_EQ(values[i],
                           it_sdata(cvector->it(vector, i)));
}

static void push_tracked(vector_t* vector, int32_t payload)
{
    test_tracked_t value = test_tracked_make(payload);

    TEST_ASSERT_TRUE(test_tracked_valid_at(&value));
    TEST_ASSERT_TRUE(cvector->push_back(vector, ds_cstdata(&value)));
    test_tracked_dispose(&value);
}

static void expect_tracked(const vector_t* vector, const int32_t* values,
                           size_t count)
{
    size_t i;

    TEST_ASSERT_NULL(vector_check_invariants(vector));
    TEST_ASSERT_EQ(count, (size_t)cvector->size(vector));
    for (i = 0; i < count; ++i) {
        const test_tracked_t* stored =
            test_tracked_slot_at(cvector->at(vector, i).u8);

        TEST_ASSERT_TRUE(test_tracked_valid_at(stored));
        TEST_ASSERT_EQ(values[i], test_tracked_payload_at(stored));
    }
}

TEST_CASE(unit, vector_lifecycle)
{
    vector_t* vector = open_int32();
    const int32_t values[] = { 1, 2, 3, 4, 5 };
    uint8_t* begin;

    TEST_ASSERT_EQ(0, cvector->size(vector));
    TEST_ASSERT_EQ(0, cvector->capacity(vector));
    TEST_ASSERT_TRUE(__cvector_empty(vector));
    TEST_ASSERT_EQ(0, cvector->count(vector, __ds_s32data(3)));
    TEST_ASSERT_NULL(cvector->at(vector, 0).u8);
    TEST_ASSERT_TRUE(i_vector_is_null_iterator(cvector->it(vector, 0)));
    TEST_ASSERT_EQ(0, cvector->clear(vector));

    push_all(vector, 1, 5);
    TEST_ASSERT_FALSE(__cvector_empty(vector));
    TEST_ASSERT_EQ(5, cvector->size(vector));
    TEST_ASSERT_TRUE(cvector->capacity(vector) >= cvector->size(vector));
    TEST_ASSERT_EQ(1, *cvector->front(vector).s32);
    TEST_ASSERT_EQ(5, *cvector->back(vector).s32);
    expect_values(vector, values, 5);

    TEST_ASSERT_TRUE(cvector->push_back(vector, __ds_s32data(5)));
    TEST_ASSERT_EQ(2, cvector->count(vector, __ds_s32data(5)));
    TEST_ASSERT_EQ(6, cvector->size(vector));

    begin = cvector->begin(vector).cur;
    TEST_ASSERT_EQ(6, cvector->clear(vector));
    TEST_ASSERT_EQ(0, cvector->size(vector));
    TEST_ASSERT_TRUE(__cvector_empty(vector));
    TEST_ASSERT_TRUE(cvector->begin(vector).cur == begin);
    TEST_ASSERT_TRUE(cvector->push_back(vector, __ds_s32data(9)));
    TEST_ASSERT_EQ(9, *cvector->at(vector, 0).s32);
}

TEST_CASE(unit, vector_capacity_init)
{
    vector_t* zero = open_int32();
    vector_t* one = open_int32_capacity(1);
    vector_t* four = open_int32_capacity(4);
    vector_t* eight = open_int32_capacity(8);
    uint8_t* begin;

    TEST_ASSERT_EQ(0, cvector->capacity(zero));
    TEST_ASSERT_EQ(4, cvector->capacity(one));
    TEST_ASSERT_EQ(4, cvector->capacity(four));
    TEST_ASSERT_EQ(8, cvector->capacity(eight));

    TEST_ASSERT_TRUE(cvector->push_back(zero, __ds_s32data(1)));
    TEST_ASSERT_EQ(1, cvector->capacity(zero));
    TEST_ASSERT_TRUE(cvector->push_back(zero, __ds_s32data(2)));
    TEST_ASSERT_EQ(2, cvector->capacity(zero));

    begin = cvector->begin(four).cur;
    push_all(four, 1, 4);
    TEST_ASSERT_TRUE(cvector->begin(four).cur == begin);
    TEST_ASSERT_EQ(4, cvector->capacity(four));
    TEST_ASSERT_TRUE(cvector->push_back(four, __ds_s32data(5)));
    TEST_ASSERT_EQ(5, cvector->size(four));
    TEST_ASSERT_EQ(8, cvector->capacity(four));

    TEST_ASSERT_TRUE(cvector->push_back(eight, __ds_s32data(1)));
    TEST_ASSERT_NULL(vector_check_invariants(zero));
    TEST_ASSERT_NULL(vector_check_invariants(one));
    TEST_ASSERT_NULL(vector_check_invariants(four));
    TEST_ASSERT_NULL(vector_check_invariants(eight));
}

TEST_CASE(unit, vector_reserve_and_shrink)
{
    vector_t* vector = open_int32();
    int32_t expected[64];
    size_t size = 0;
    size_t i;

    for (i = 0; i < 10; ++i) {
        TEST_ASSERT_TRUE(cvector->push_back(vector,
                                            __ds_s32data((int32_t)i)));
        expected[size++] = (int32_t)i;
    }

    TEST_ASSERT_TRUE(cvector->reserve(vector, 100));
    TEST_ASSERT_EQ(100, cvector->capacity(vector));
    expect_values(vector, expected, size);

    TEST_ASSERT_TRUE(cvector->reserve(vector, 20));
    TEST_ASSERT_EQ(100, cvector->capacity(vector));
    TEST_ASSERT_TRUE(cvector->reserve(vector, 10));
    TEST_ASSERT_EQ(100, cvector->capacity(vector));
    TEST_ASSERT_FALSE(cvector->reserve(vector, -1));
    TEST_ASSERT_EQ(100, cvector->capacity(vector));

    cvector->shrink_to_fit(vector);
    TEST_ASSERT_EQ(10, cvector->capacity(vector));
    expect_values(vector, expected, size);

    for (i = 10; i < 40; ++i) {
        TEST_ASSERT_TRUE(cvector->push_back(vector,
                                            __ds_s32data((int32_t)i)));
        expected[size++] = (int32_t)i;
    }
    TEST_ASSERT_TRUE(cvector->capacity(vector) >= size);
    expect_values(vector, expected, size);

    TEST_ASSERT_TRUE(cvector->erase_range(vector, cvector->it(vector, 5),
                                          cvector->it(vector, 30)).cur
                     != NULL);
    array_erase(expected, &size, 5, 25);
    cvector->shrink_to_fit(vector);
    TEST_ASSERT_EQ((vector_size_t)size, cvector->capacity(vector));
    expect_values(vector, expected, size);

    TEST_ASSERT_EQ((vector_size_t)size, cvector->clear(vector));
    cvector->shrink_to_fit(vector);
    TEST_ASSERT_EQ(0, cvector->capacity(vector));
    TEST_ASSERT_NULL(vector_check_invariants(vector));
}

TEST_CASE(unit, vector_growth_preserves_values)
{
    vector_t* vector = open_int32();
    int32_t expected[3000];
    size_t size = 0;
    size_t i;

    for (i = 0; i < 2000; ++i) {
        TEST_ASSERT_TRUE(cvector->push_back(vector,
                                            __ds_s32data((int32_t)i)));
        expected[size++] = (int32_t)i;
        TEST_ASSERT_TRUE(cvector->capacity(vector) >= cvector->size(vector));
    }
    expect_values(vector, expected, size);

    for (i = 0; i < 500; ++i) {
        TEST_ASSERT_EQ((int32_t)(1999 - i), *cvector->back(vector).s32);
        cvector->pop_back(vector);
        array_erase(expected, &size, size - 1, 1);
    }
    expect_values(vector, expected, size);

    for (i = 0; i < 100; ++i) {
        TEST_ASSERT_TRUE(cvector->erase(vector, cvector->begin(vector)).cur
                         != NULL);
        array_erase(expected, &size, 0, 1);
    }
    expect_values(vector, expected, size);
    TEST_ASSERT_NULL(vector_check_invariants(vector));
}

TEST_CASE(unit, vector_insert_returns)
{
    vector_t* vector = open_int32();
    vector_t* source = open_int32();
    vector_iterator_t it;
    int32_t expected[64];
    size_t size = 0;
    size_t i;

    for (i = 1; i <= 5; ++i) {
        TEST_ASSERT_TRUE(cvector->push_back(vector, __ds_s32data((int32_t)i)));
        expected[size++] = (int32_t)i;
    }

    it = cvector->insert(vector, cvector->it(vector, 2), __ds_s32data(9));
    TEST_ASSERT_FALSE(i_vector_is_null_iterator(it));
    TEST_ASSERT_EQ(9, it_s32data(it));
    array_insert(expected, &size, 2, 1, 9);
    expect_values(vector, expected, size);

    it = cvector->insert(vector, cvector->begin(vector), __ds_s32data(0));
    TEST_ASSERT_EQ(0, it_s32data(it));
    array_insert(expected, &size, 0, 1, 0);

    it = cvector->insert(vector, cvector->end(vector), __ds_s32data(6));
    TEST_ASSERT_EQ(6, it_s32data(it));
    array_insert(expected, &size, size, 1, 6);

    it = cvector->insert_n(vector, cvector->it(vector, 3), 2,
                           __ds_s32data(8));
    TEST_ASSERT_EQ(8, it_s32data(it));
    array_insert(expected, &size, 3, 2, 8);

    it = cvector->insert_n(vector, cvector->end(vector), 0,
                           __ds_s32data(7));
    TEST_ASSERT_TRUE(it.cur == cvector->end(vector).cur);

    it = cvector->insert_n(vector, cvector->end(vector), 2,
                           __ds_s32data(7));
    TEST_ASSERT_EQ(7, it_s32data(it));
    array_insert(expected, &size, size, 2, 7);

    it = cvector->insert_n(vector, cvector->begin(vector), 1,
                           __ds_s32data(4));
    TEST_ASSERT_EQ(4, it_s32data(it));
    array_insert(expected, &size, 0, 1, 4);
    expect_values(vector, expected, size);

    TEST_ASSERT_TRUE(cvector->push_back(source, __ds_s32data(10)));
    TEST_ASSERT_TRUE(cvector->push_back(source, __ds_s32data(20)));
    it = cvector->insert_range(vector, cvector->it(vector, 2),
                               cvector->begin(source), cvector->end(source));
    TEST_ASSERT_EQ(10, it_s32data(it));
    array_insert(expected, &size, 2, 1, 10);
    array_insert(expected, &size, 3, 1, 20);
    expect_values(vector, expected, size);

    TEST_ASSERT_TRUE(cvector->assign_range(vector, cvector->begin(source),
                                           cvector->end(source)));
    expected[0] = 10;
    expected[1] = 20;
    size = 2;
    expect_values(vector, expected, size);

    TEST_ASSERT_TRUE(cvector->assign_range(vector, cvector->begin(source),
                                           cvector->begin(source)));
    TEST_ASSERT_EQ(0, cvector->size(vector));
    TEST_ASSERT_NULL(vector_check_invariants(vector));
}

TEST_CASE(unit, vector_erase_returns)
{
    vector_t* vector = open_int32();
    vector_iterator_t it;
    const int32_t after_begin[] = { 2, 3, 4, 5 };
    const int32_t after_middle[] = { 2, 3, 5 };
    const int32_t after_last[] = { 2, 3 };
    const int32_t after_one[] = { 2 };
    const int32_t final_one[] = { 42 };

    push_all(vector, 1, 5);

    it = cvector->erase(vector, cvector->begin(vector));
    TEST_ASSERT_EQ(2, it_s32data(it));
    expect_values(vector, after_begin, 4);

    it = cvector->erase(vector, cvector->it(vector, 2));
    TEST_ASSERT_EQ(5, it_s32data(it));
    expect_values(vector, after_middle, 3);

    it = cvector->erase(vector, cvector->it(vector, 2));
    TEST_ASSERT_TRUE(it.cur == cvector->end(vector).cur);
    expect_values(vector, after_last, 2);

    it = cvector->erase_range(vector, cvector->begin(vector),
                              cvector->begin(vector));
    TEST_ASSERT_TRUE(it.cur == cvector->begin(vector).cur);
    expect_values(vector, after_last, 2);

    it = cvector->erase_range(vector, cvector->it(vector, 1),
                              cvector->end(vector));
    TEST_ASSERT_TRUE(it.cur == cvector->end(vector).cur);
    expect_values(vector, after_one, 1);

    it = cvector->erase_range(vector, cvector->begin(vector),
                              cvector->end(vector));
    TEST_ASSERT_TRUE(it.cur == cvector->end(vector).cur);
    TEST_ASSERT_EQ(0, cvector->size(vector));

    TEST_ASSERT_TRUE(cvector->push_back(vector, __ds_s32data(42)));
    TEST_ASSERT_EQ(1, cvector->size(vector));
    TEST_ASSERT_EQ(42, *cvector->front(vector).s32);
    TEST_ASSERT_NULL(vector_check_invariants(vector));

    expect_values(vector, final_one, 1);
}

TEST_CASE(unit, vector_resize_and_assign)
{
    vector_t* vector = open_int32();
    const int32_t resized[] = { 1, 2, 3, 7, 7, 7 };
    const int32_t assigned[] = { 9, 9 };
    const int32_t shortened[] = { 1, 2, 3 };

    TEST_ASSERT_TRUE(cvector->resize(vector, 3, __ds_s32data(0)));
    TEST_ASSERT_EQ(3, cvector->size(vector));
    TEST_ASSERT_EQ(0, *cvector->at(vector, 0).s32);
    TEST_ASSERT_EQ(0, *cvector->at(vector, 2).s32);

    TEST_ASSERT_TRUE(cvector->assign(vector, 3, __ds_s32data(0)));
    TEST_ASSERT_TRUE(cvector->replace_index(vector, 0, __ds_s32data(1)));
    TEST_ASSERT_TRUE(cvector->replace_index(vector, 1, __ds_s32data(2)));
    TEST_ASSERT_TRUE(cvector->replace_index(vector, 2, __ds_s32data(3)));

    TEST_ASSERT_TRUE(cvector->resize(vector, 6, __ds_s32data(7)));
    expect_values(vector, resized, 6);

    TEST_ASSERT_TRUE(cvector->resize(vector, 3, __ds_s32data(0)));
    expect_values(vector, shortened, 3);

    TEST_ASSERT_TRUE(cvector->resize(vector, 3, __ds_s32data(0)));
    TEST_ASSERT_TRUE(cvector->assign(vector, 2, __ds_s32data(9)));
    expect_values(vector, assigned, 2);

    TEST_ASSERT_TRUE(cvector->resize(vector, 0, __ds_s32data(0)));
    TEST_ASSERT_EQ(0, cvector->size(vector));
    TEST_ASSERT_FALSE(cvector->resize(vector, -1, __ds_s32data(0)));
    TEST_ASSERT_NULL(vector_check_invariants(vector));
}

TEST_CASE(unit, vector_replace_and_find)
{
    vector_t* vector = open_int32();
    vector_iterator_t it;

    push_all(vector, 1, 5);

    TEST_ASSERT_TRUE(cvector->replace_index(vector, 0, __ds_s32data(10)));
    TEST_ASSERT_EQ(10, *cvector->at(vector, 0).s32);
    TEST_ASSERT_FALSE(cvector->replace_index(vector, 5, __ds_s32data(11)));
    TEST_ASSERT_FALSE(cvector->replace_index(vector, -1, __ds_s32data(11)));
    TEST_ASSERT_EQ(5, cvector->size(vector));

    it = cvector->it(vector, 2);
    TEST_ASSERT_TRUE(cvector->replace_it(vector, it, __ds_s32data(30)));
    TEST_ASSERT_EQ(30, *cvector->at(vector, 2).s32);
    TEST_ASSERT_FALSE(cvector->replace_it(vector, cvector->end(vector),
                                          __ds_s32data(31)));

    it = cvector->find(vector, __ds_s32data(30));
    TEST_ASSERT_FALSE(i_vector_is_null_iterator(it));
    TEST_ASSERT_TRUE(it.cur != cvector->end(vector).cur);
    TEST_ASSERT_EQ(30, it_s32data(it));

    it = cvector->find(vector, __ds_s32data(99));
    TEST_ASSERT_TRUE(it.cur == cvector->end(vector).cur);
    TEST_ASSERT_EQ(0, cvector->count(vector, __ds_s32data(99)));
    TEST_ASSERT_NULL(vector_check_invariants(vector));
}

TEST_CASE(unit, vector_swap_plain)
{
    vector_t* first = open_int32();
    vector_t* second = open_int32();
    const int32_t first_values[] = { 1, 2 };
    const int32_t second_values[] = { 8, 9, 10 };

    push_all(first, 1, 2);
    push_all(second, 8, 3);
    cvector->swap(first, second);
    expect_values(first, second_values, 3);
    expect_values(second, first_values, 2);

    cvector->swap(first, first);
    expect_values(first, second_values, 3);
    TEST_ASSERT_NULL(vector_check_invariants(first));
    TEST_ASSERT_NULL(vector_check_invariants(second));
}

TEST_CASE(unit, vector_dtype_matrix)
{
    {
        vector_t* vector = VECTOR_NEW_T(uint8_t);

        TEST_ASSERT_NOT_NULL(vector);
        test_cleanup_push(vector_free, vector);
        TEST_ASSERT_TRUE(cvector->push_back(vector, __ds_u8data(250)));
        TEST_ASSERT_EQ(250, *cvector->at(vector, 0).u8);
        TEST_ASSERT_EQ(1, cvector->count(vector, __ds_u8data(250)));
    }
    {
        vector_t* vector = VECTOR_NEW_T(uint16_t);

        TEST_ASSERT_NOT_NULL(vector);
        test_cleanup_push(vector_free, vector);
        TEST_ASSERT_TRUE(cvector->push_back(vector, __ds_u16data(60000)));
        TEST_ASSERT_EQ(60000, *cvector->at(vector, 0).u16);
        TEST_ASSERT_EQ(1, cvector->count(vector, __ds_u16data(60000)));
    }
    {
        vector_t* vector = VECTOR_NEW_T(uint32_t);

        TEST_ASSERT_NOT_NULL(vector);
        test_cleanup_push(vector_free, vector);
        TEST_ASSERT_TRUE(cvector->push_back(vector, __ds_u32data(4000000000u)));
        TEST_ASSERT_EQ(4000000000u, *cvector->at(vector, 0).u32);
        TEST_ASSERT_EQ(1, cvector->count(vector, __ds_u32data(4000000000u)));
    }
    {
        vector_t* vector = VECTOR_NEW_T(uint64_t);

        TEST_ASSERT_NOT_NULL(vector);
        test_cleanup_push(vector_free, vector);
        TEST_ASSERT_TRUE(cvector->push_back(vector,
                                            ds_u64data(1234567890123ULL)));
        TEST_ASSERT_EQ(1234567890123LL, *cvector->at(vector, 0).u64);
        TEST_ASSERT_EQ(1, cvector->count(vector,
                                         ds_u64data(1234567890123ULL)));
    }
    {
        vector_t* vector = VECTOR_NEW_T(int8_t);

        TEST_ASSERT_NOT_NULL(vector);
        test_cleanup_push(vector_free, vector);
        TEST_ASSERT_TRUE(cvector->push_back(vector, __ds_s8data(-100)));
        TEST_ASSERT_EQ(-100, *cvector->at(vector, 0).s8);
        TEST_ASSERT_EQ(1, cvector->count(vector, __ds_s8data(-100)));
    }
    {
        vector_t* vector = VECTOR_NEW_T(int16_t);

        TEST_ASSERT_NOT_NULL(vector);
        test_cleanup_push(vector_free, vector);
        TEST_ASSERT_TRUE(cvector->push_back(vector, __ds_s16data(-30000)));
        TEST_ASSERT_EQ(-30000, *cvector->at(vector, 0).s16);
        TEST_ASSERT_EQ(1, cvector->count(vector, __ds_s16data(-30000)));
    }
    {
        vector_t* vector = VECTOR_NEW_T(int32_t);

        TEST_ASSERT_NOT_NULL(vector);
        test_cleanup_push(vector_free, vector);
        TEST_ASSERT_TRUE(cvector->push_back(vector,
                                            __ds_s32data(-2000000000)));
        TEST_ASSERT_EQ(-2000000000, *cvector->at(vector, 0).s32);
    }
    {
        vector_t* vector = VECTOR_NEW_T(int64_t);

        TEST_ASSERT_NOT_NULL(vector);
        test_cleanup_push(vector_free, vector);
        TEST_ASSERT_TRUE(cvector->push_back(vector,
                                            ds_s64data(-1234567890123LL)));
        TEST_ASSERT_EQ(-1234567890123LL, *cvector->at(vector, 0).s64);
        TEST_ASSERT_EQ(1, cvector->count(vector,
                                         ds_s64data(-1234567890123LL)));
    }
    {
        vector_t* vector = VECTOR_NEW_T(uintptr_t);

        TEST_ASSERT_NOT_NULL(vector);
        test_cleanup_push(vector_free, vector);
        TEST_ASSERT_TRUE(cvector->push_back(vector, __ds_uptrdata(123456u)));
        TEST_ASSERT_EQ(123456, *cvector->at(vector, 0).d);
    }
    {
        vector_t* vector = VECTOR_NEW_T(intptr_t);

        TEST_ASSERT_NOT_NULL(vector);
        test_cleanup_push(vector_free, vector);
        TEST_ASSERT_TRUE(cvector->push_back(vector, __ds_iptrdata(-12345)));
        TEST_ASSERT_EQ(-12345, *cvector->at(vector, 0).d);
    }
    {
        vector_t* vector = VECTOR_NEW_T(size_t);

        TEST_ASSERT_NOT_NULL(vector);
        test_cleanup_push(vector_free, vector);
        TEST_ASSERT_TRUE(cvector->push_back(vector, __ds_sizedata(123456)));
        TEST_ASSERT_EQ(123456, *cvector->at(vector, 0).d);
    }
    {
        vector_t* vector = VECTOR_NEW_T(ssize_t);

        TEST_ASSERT_NOT_NULL(vector);
        test_cleanup_push(vector_free, vector);
        TEST_ASSERT_TRUE(cvector->push_back(vector, __ds_ssizedata(-123456)));
        TEST_ASSERT_EQ(-123456, *cvector->at(vector, 0).d);
    }
    {
        vector_t* vector = VECTOR_NEW_T(float);

        TEST_ASSERT_NOT_NULL(vector);
        test_cleanup_push(vector_free, vector);
        TEST_ASSERT_TRUE(cvector->push_back(vector, ds_fdata(1.5f)));
        TEST_ASSERT_TRUE(cvector->push_back(vector, ds_fdata(-0.0f)));
        TEST_ASSERT_EQ(1.5f, *cvector->at(vector, 0).f);
        TEST_ASSERT_EQ(1, cvector->count(vector, ds_fdata(0.0f)));
    }
    {
        vector_t* vector = VECTOR_NEW_T(double);

        TEST_ASSERT_NOT_NULL(vector);
        test_cleanup_push(vector_free, vector);
        TEST_ASSERT_TRUE(cvector->push_back(vector, ds_lfdata(-2.25)));
        TEST_ASSERT_TRUE(cvector->push_back(vector, ds_lfdata(-0.0)));
        TEST_ASSERT_EQ(-2.25, *cvector->at(vector, 0).lf);
        TEST_ASSERT_EQ(1, cvector->count(vector, ds_lfdata(0.0)));
    }
}

TEST_CASE(unit, vector_inline_strings)
{
    static const char* const short15 = "abcdefghijklmno";
    static const char* const long16 = "abcdefghijklmnop";
    static const char* const long45 = "this string is longer than the inline buffer";
    vector_t* sso = VECTOR_NEW_SSO();
    vector_t* chars = VECTOR_NEW_CHAR();
    vector_iterator_t it;

    TEST_ASSERT_NOT_NULL(sso);
    TEST_ASSERT_NOT_NULL(chars);
    test_cleanup_push(vector_free, sso);
    test_cleanup_push(vector_free, chars);

    TEST_ASSERT_TRUE(cvector->push_back(sso, ds_sdata(short15)));
    TEST_ASSERT_TRUE(cvector->push_back(sso, ds_sdata(long16)));
    TEST_ASSERT_TRUE(cvector->push_back(sso, ds_sdata(long45)));
    TEST_ASSERT_TRUE(cvector->push_back(chars, ds_sdata(short15)));
    TEST_ASSERT_TRUE(cvector->push_back(chars, ds_sdata(long16)));
    TEST_ASSERT_TRUE(cvector->push_back(chars, ds_sdata(long45)));

    it = cvector->begin(sso);
    TEST_ASSERT_EQ(15, it.ssd->len);
    TEST_ASSERT_TRUE(it.ssd->p == it.ssd->buf);
    TEST_ASSERT_STR_EQ(short15, it_sdata(it));
    it = cvector->next(it);
    TEST_ASSERT_EQ(16, it.ssd->len);
    TEST_ASSERT_TRUE(it.ssd->p != it.ssd->buf);
    TEST_ASSERT_STR_EQ(long16, it_sdata(it));
    it = cvector->next(it);
    TEST_ASSERT_EQ(44, it.ssd->len);
    TEST_ASSERT_STR_EQ(long45, it_sdata(it));

    it = cvector->begin(chars);
    TEST_ASSERT_STR_EQ(short15, it_sdata(it));
    it = cvector->next(it);
    TEST_ASSERT_STR_EQ(long16, it_sdata(it));
    it = cvector->next(it);
    TEST_ASSERT_STR_EQ(long45, it_sdata(it));

    it = cvector->find(sso, ds_sdata(long16));
    TEST_ASSERT_TRUE(it.cur != cvector->end(sso).cur);
    TEST_ASSERT_STR_EQ(long16, it_sdata(it));
    TEST_ASSERT_EQ(1, cvector->count(sso, ds_sdata(long45)));
    TEST_ASSERT_EQ(0, cvector->count(chars, ds_sdata("missing")));

    TEST_ASSERT_EQ(1, cvector->remove(sso, ds_sdata(long16)));
    TEST_ASSERT_EQ(2, cvector->size(sso));
    TEST_ASSERT_EQ(1, cvector->remove(chars, ds_sdata(long16)));
    TEST_ASSERT_EQ(2, cvector->size(chars));

    TEST_ASSERT_TRUE(cvector->replace_index(sso, 0, ds_sdata(long45)));
    TEST_ASSERT_EQ(44, cvector->at(sso, 0).ssd->len);
    TEST_ASSERT_STR_EQ(long45, cvector->at(sso, 0).ssd->p);
    cvector->shrink_to_fit(sso);
    TEST_ASSERT_TRUE(cvector->insert(sso, cvector->begin(sso),
                                     ds_sdata(short15)).cur != NULL);
    TEST_ASSERT_EQ(3, cvector->size(sso));
    TEST_ASSERT_NULL(vector_check_invariants(sso));

    TEST_ASSERT_EQ(3, cvector->clear(sso));
    TEST_ASSERT_EQ(2, cvector->clear(chars));
}

TEST_CASE(unit, vector_sso_sort)
{
    static const char* const inputs[] = {
        "dddd", "bbbbbbbbbbbbbbbb", "aaaa", "cccccccccccccccccccccccc"
    };
    static const char* const ordered[] = {
        "aaaa", "bbbbbbbbbbbbbbbb", "cccccccccccccccccccccccc", "dddd"
    };
    vector_t* vector = VECTOR_NEW_SSO();
    size_t i;

    TEST_ASSERT_NOT_NULL(vector);
    test_cleanup_push(vector_free, vector);

    for (i = 0; i < sizeof(inputs) / sizeof(inputs[0]); ++i)
        TEST_ASSERT_TRUE(cvector->push_back(vector, ds_sdata(inputs[i])));

    sort_sso_asc(cvector->begin(vector), cvector->end(vector));
    TEST_ASSERT_TRUE(sort_sso_asc_sorted(cvector->begin(vector),
                                         cvector->end(vector)));
    TEST_ASSERT_NULL(vector_check_invariants(vector));

    for (i = 0; i < sizeof(ordered) / sizeof(ordered[0]); ++i) {
        vector_iterator_t it = cvector->it(vector, i);
        const ds_sso_t* value = it.ssd;

        TEST_ASSERT_STR_EQ(ordered[i], it_sdata(it));
        TEST_ASSERT_EQ(strlen(ordered[i]), value->len);
        TEST_ASSERT_TRUE((value->len <= __DS_SSO_LOCAL_CAP)
                         == (value->p == value->buf));
    }

    cvector->shrink_to_fit(vector);
    TEST_ASSERT_NULL(vector_check_invariants(vector));
    TEST_ASSERT_TRUE(cvector->insert(vector, cvector->begin(vector),
                                     ds_sdata("abcdefghijklmnop")).cur
                     != NULL);
    TEST_ASSERT_EQ(5, cvector->size(vector));
    TEST_ASSERT_STR_EQ("abcdefghijklmnop",
                       it_sdata(cvector->at(vector, 0)));
    TEST_ASSERT_STR_EQ(ordered[3], it_sdata(cvector->at(vector, 4)));
    TEST_ASSERT_NULL(vector_check_invariants(vector));
}

TEST_CASE(unit, vector_invariant_checker_negative)
{
    vector_t* vector = open_int32();
    vector_t snapshot;
    size_t i;

    for (i = 0; i < 300; ++i)
        TEST_ASSERT_TRUE(cvector->push_back(vector, __ds_s32data((int32_t)i)));
    TEST_ASSERT_NULL(vector_check_invariants(vector));

    snapshot = *vector;
    snapshot.rend = NULL;
    TEST_ASSERT_NOT_NULL(vector_check_invariants(&snapshot));

    snapshot = *vector;
    snapshot.begin = snapshot.rend;
    TEST_ASSERT_NOT_NULL(vector_check_invariants(&snapshot));

    snapshot = *vector;
    snapshot.end = snapshot.end_of_storage + snapshot.step;
    TEST_ASSERT_NOT_NULL(vector_check_invariants(&snapshot));

    snapshot = *vector;
    snapshot.end_of_storage = __i_vector_ptr_add(snapshot.begin, 10,
                                                 snapshot.step);
    TEST_ASSERT_NOT_NULL(vector_check_invariants(&snapshot));

    snapshot = *vector;
    snapshot.step = 0;
    TEST_ASSERT_NOT_NULL(vector_check_invariants(&snapshot));

    TEST_ASSERT_NULL(vector_check_invariants(vector));
}

TEST_CASE(unit, vector_tracked_ops)
{
    vector_t* vector = VECTOR_NEW_COMPLATE(test_ops_tracked(),
                                           SEQ_CNTR_DTYPE_STRUCT,
                                           test_tracked_t, 0);
    test_tracked_t first = test_tracked_make(11);
    test_tracked_t second = test_tracked_make(22);
    test_tracked_t probe = test_tracked_make(22);
    test_tracked_t replacement = test_tracked_make(44);
    const test_tracked_t* stored;
    size_t copies;
    size_t frees;

    TEST_ASSERT_NOT_NULL(vector);
    test_cleanup_push(vector_free, vector);
    TEST_ASSERT_TRUE(test_tracked_valid_at(&first));
    TEST_ASSERT_TRUE(test_tracked_valid_at(&second));
    TEST_ASSERT_TRUE(test_tracked_valid_at(&probe));
    TEST_ASSERT_TRUE(test_tracked_valid_at(&replacement));

    TEST_ASSERT_TRUE(cvector->push_back(vector, ds_cstdata(&first)));
    test_tracked_dispose(&first);
    TEST_ASSERT_TRUE(cvector->push_back(vector, ds_cstdata(&second)));
    test_tracked_dispose(&second);

    stored = test_tracked_slot_at(cvector->at(vector, 0).u8);
    TEST_ASSERT_TRUE(test_tracked_valid_at(stored));
    TEST_ASSERT_EQ(11, test_tracked_payload_at(stored));
    stored = test_tracked_slot_at(cvector->at(vector, 1).u8);
    TEST_ASSERT_EQ(22, test_tracked_payload_at(stored));

    TEST_ASSERT_EQ(1, cvector->count(vector, ds_cstdata(&probe)));
    TEST_ASSERT_TRUE(cvector->find(vector, ds_cstdata(&probe)).cur
                     != cvector->end(vector).cur);
    test_tracked_dispose(&probe);

    copies = test_tracked_copies();
    frees = test_tracked_frees();
    TEST_ASSERT_TRUE(cvector->replace_index(vector, 0,
                                            ds_cstdata(&replacement)));
    test_tracked_dispose(&replacement);
    TEST_ASSERT_EQ(copies + 1, test_tracked_copies());
    TEST_ASSERT_EQ(frees + 1, test_tracked_frees());
    stored = test_tracked_slot_at(cvector->at(vector, 0).u8);
    TEST_ASSERT_EQ(44, test_tracked_payload_at(stored));
    TEST_ASSERT_NULL(vector_check_invariants(vector));

    TEST_ASSERT_EQ(2, cvector->clear(vector));
    TEST_ASSERT_EQ(0, test_tracked_live());
}

TEST_CASE(unit, vector_self_reference_strings)
{
    static const char* const initial[6] = {
        "aaaa", "bbbb", "cccc", "dddd", "eeee", "ffff"
    };
    static const char* const middle_n[8] = {
        "aaaa", "bbbb", "dddd", "dddd", "cccc", "dddd", "eeee", "ffff"
    };
    static const char* const prefix[7] = {
        "aaaa", "bbbb", "cccc", "dddd", "bbbb", "eeee", "ffff"
    };
    static const char* const front_n[9] = {
        "eeee", "eeee", "eeee", "aaaa", "bbbb", "cccc", "dddd", "eeee",
        "ffff"
    };
    static const char* const realloc_insert[5] = {
        "aaaa", "bbbb", "dddd", "cccc", "dddd"
    };
    static const char* const realloc_insert_n[6] = {
        "aaaa", "dddd", "dddd", "bbbb", "cccc", "dddd"
    };
    static const char* const push_realloc[5] = {
        "aaaa", "bbbb", "cccc", "dddd", "bbbb"
    };
    static const char* const resize_realloc[6] = {
        "aaaa", "bbbb", "cccc", "dddd", "bbbb", "bbbb"
    };
    static const char* const own_slot[5] = {
        "aaaa", "bbbb", "cccc", "cccc", "dddd"
    };
    static const char* const heap_items[4] = {
        "aaaa", "bbbb", "abcdefghijklmnop", "dddd"
    };
    static const char* const heap_inserted[5] = {
        "abcdefghijklmnop", "aaaa", "bbbb", "abcdefghijklmnop", "dddd"
    };
    vector_t* vector;
    const char* alias;

    /* The source tail element moves when the middle is opened. */
    vector = open_strings(initial, 6, 16);
    alias = cvector->at(vector, 3).ssd->p;
    TEST_ASSERT_TRUE(cvector->insert_n(vector, cvector->it(vector, 2), 2,
                                       ds_sdata(alias)).cur != NULL);
    expect_strings(vector, middle_n, 8);

    /* A prefix source does not move; it is still read from the same slot. */
    vector = open_strings(initial, 6, 16);
    alias = cvector->at(vector, 1).ssd->p;
    TEST_ASSERT_TRUE(cvector->insert(vector, cvector->it(vector, 4),
                                     ds_sdata(alias)).cur != NULL);
    expect_strings(vector, prefix, 7);

    /* A later source shifts by `n` when the insert is at the front. */
    vector = open_strings(initial, 6, 16);
    alias = cvector->at(vector, 4).ssd->p;
    TEST_ASSERT_TRUE(cvector->insert_n(vector, cvector->begin(vector), 3,
                                       ds_sdata(alias)).cur != NULL);
    expect_strings(vector, front_n, 9);

    /* The growing insert reallocates the buffer before the tail moves. */
    vector = open_strings(initial, 4, 4);
    alias = cvector->at(vector, 3).ssd->p;
    TEST_ASSERT_TRUE(cvector->insert(vector, cvector->it(vector, 2),
                                     ds_sdata(alias)).cur != NULL);
    expect_strings(vector, realloc_insert, 5);

    vector = open_strings(initial, 4, 4);
    alias = cvector->at(vector, 3).ssd->p;
    TEST_ASSERT_TRUE(cvector->insert_n(vector, cvector->it(vector, 1), 2,
                                       ds_sdata(alias)).cur != NULL);
    expect_strings(vector, realloc_insert_n, 6);

    /* push_back and growing resize append through the same growing path. */
    vector = open_strings(initial, 4, 4);
    alias = cvector->at(vector, 1).ssd->p;
    TEST_ASSERT_TRUE(cvector->push_back(vector, ds_sdata(alias)));
    expect_strings(vector, push_realloc, 5);

    vector = open_strings(initial, 4, 4);
    alias = cvector->at(vector, 1).ssd->p;
    TEST_ASSERT_TRUE(cvector->resize(vector, 6, ds_sdata(alias)));
    expect_strings(vector, resize_realloc, 6);

    /* Inserting at the source's own position moves it after the new run. */
    vector = open_strings(initial, 4, 16);
    alias = cvector->at(vector, 2).ssd->p;
    TEST_ASSERT_TRUE(cvector->insert(vector, cvector->it(vector, 2),
                                     ds_sdata(alias)).cur != NULL);
    expect_strings(vector, own_slot, 5);

    /* A heap-backed token is outside the live range and stays valid. */
    vector = open_strings(heap_items, 4, 16);
    alias = cvector->at(vector, 2).ssd->p;
    TEST_ASSERT_TRUE(alias != cvector->at(vector, 2).ssd->buf);
    TEST_ASSERT_TRUE(cvector->insert(vector, cvector->begin(vector),
                                     ds_sdata(alias)).cur != NULL);
    expect_strings(vector, heap_inserted, 5);
}

TEST_CASE(unit, vector_self_reference_tracked)
{
    static const int32_t insert_expected[5] = { 11, 22, 33, 22, 44 };
    static const int32_t insert_n_expected[6] = { 11, 44, 44, 22, 33, 44 };
    vector_t* vector;
    vector_t* other;
    const test_tracked_t* alias;
    size_t copies;

    vector = VECTOR_NEW_COMPLATE(test_ops_tracked(), SEQ_CNTR_DTYPE_STRUCT,
                                 test_tracked_t, 4);
    TEST_ASSERT_NOT_NULL(vector);
    test_cleanup_push(vector_free, vector);
    push_tracked(vector, 11);
    push_tracked(vector, 22);
    push_tracked(vector, 33);
    push_tracked(vector, 44);

    /* Size equals capacity, so the insert reallocates and moves all slots. */
    alias = test_tracked_slot_at(cvector->at(vector, 1).u8);
    copies = test_tracked_copies();
    TEST_ASSERT_TRUE(cvector->insert(vector, cvector->it(vector, 3),
                                     ds_cstdata(alias)).cur != NULL);
    TEST_ASSERT_EQ(copies + 1, test_tracked_copies());
    expect_tracked(vector, insert_expected, 5);

    other = VECTOR_NEW_COMPLATE(test_ops_tracked(), SEQ_CNTR_DTYPE_STRUCT,
                                test_tracked_t, 4);
    TEST_ASSERT_NOT_NULL(other);
    test_cleanup_push(vector_free, other);
    push_tracked(other, 11);
    push_tracked(other, 22);
    push_tracked(other, 33);
    push_tracked(other, 44);

    /* A source after the insertion point shifts by `n` as well. */
    alias = test_tracked_slot_at(cvector->at(other, 3).u8);
    copies = test_tracked_copies();
    TEST_ASSERT_TRUE(cvector->insert_n(other, cvector->it(other, 1), 2,
                                       ds_cstdata(alias)).cur != NULL);
    TEST_ASSERT_EQ(copies + 2, test_tracked_copies());
    expect_tracked(other, insert_n_expected, 6);

    TEST_ASSERT_EQ(11, test_tracked_live());
    TEST_ASSERT_EQ(6, cvector->clear(other));
    TEST_ASSERT_EQ(5, cvector->clear(vector));
    TEST_ASSERT_EQ(0, test_tracked_live());
}

#ifdef JDSC_32BIT
TEST_CASE(unit, vector_self_reference_w8_32bit)
{
    static const int64_t insert_expected[4] = { 20, 10, 20, 30 };
    static const int64_t resize_expected[5] = { 10, 20, 30, 20, 20 };
    vector_t* vector;
    int64_t* alias;
    size_t i;

    /* On 32-bit, an 8-byte element token is a pointer to the boxed slot. */
    vector = VECTOR_NEW_COMPLATE(NULL, SEQ_CNTR_DTYPE_S64, int64_t, 3);
    TEST_ASSERT_NOT_NULL(vector);
    test_cleanup_push(vector_free, vector);
    TEST_ASSERT_TRUE(cvector->push_back(vector, ds_s64data(10)));
    TEST_ASSERT_TRUE(cvector->push_back(vector, ds_s64data(20)));
    TEST_ASSERT_TRUE(cvector->push_back(vector, ds_s64data(30)));
    alias = cvector->at(vector, 1).s64;
    TEST_ASSERT_TRUE(cvector->insert(vector, cvector->begin(vector),
                                     ds_cstdata(alias)).cur != NULL);
    TEST_ASSERT_NULL(vector_check_invariants(vector));
    TEST_ASSERT_EQ(4, cvector->size(vector));
    for (i = 0; i < 4; ++i)
        TEST_ASSERT_EQ(insert_expected[i],
                       *cvector->at(vector, i).s64);

    vector = VECTOR_NEW_COMPLATE(NULL, SEQ_CNTR_DTYPE_S64, int64_t, 3);
    TEST_ASSERT_NOT_NULL(vector);
    test_cleanup_push(vector_free, vector);
    TEST_ASSERT_TRUE(cvector->push_back(vector, ds_s64data(10)));
    TEST_ASSERT_TRUE(cvector->push_back(vector, ds_s64data(20)));
    TEST_ASSERT_TRUE(cvector->push_back(vector, ds_s64data(30)));
    alias = cvector->at(vector, 1).s64;
    TEST_ASSERT_TRUE(cvector->resize(vector, 5, ds_cstdata(alias)));
    TEST_ASSERT_NULL(vector_check_invariants(vector));
    TEST_ASSERT_EQ(5, cvector->size(vector));
    for (i = 0; i < 5; ++i)
        TEST_ASSERT_EQ(resize_expected[i],
                       *cvector->at(vector, i).s64);
}
#endif /* JDSC_32BIT */

#ifndef JDSC_DEBUG
TEST_CASE(unit, vector_over_limit)
{
    vector_t* vector = open_int32();
    vector_size_t max_size = cvector->max_size(vector);

    TEST_ASSERT_TRUE(max_size > 0);
    TEST_ASSERT_TRUE(cvector->push_back(vector, __ds_s32data(1)));
    TEST_ASSERT_FALSE(cvector->reserve(vector, max_size + 1));
    TEST_ASSERT_FALSE(cvector->resize(vector, max_size + 1,
                                      __ds_s32data(2)));
    TEST_ASSERT_TRUE(i_vector_is_null_iterator(
        cvector->insert_n(vector, cvector->end(vector), max_size,
                          __ds_s32data(3))));
    TEST_ASSERT_EQ(1, cvector->size(vector));
    TEST_ASSERT_EQ(1, *cvector->at(vector, 0).s32);
    TEST_ASSERT_NULL(vector_check_invariants(vector));
}

TEST_CASE(unit, vector_constructor_rejects_invalid)
{
    TEST_ASSERT_NULL(__vector_new(NULL, SEQ_CNTR_DTYPE_S32, 0, 0, 0));
    TEST_ASSERT_NULL(__vector_new(NULL, SEQ_CNTR_DTYPE_S32,
                                  sizeof(int32_t), 3, 0));
    TEST_ASSERT_NULL(__vector_new(NULL, SEQ_CNTR_DTYPE_S32,
                                  sizeof(int32_t), sizeof(int32_t) * 2, 0));
    TEST_ASSERT_NULL(__vector_new(NULL, SEQ_CNTR_DTYPE_S32,
                                  sizeof(int32_t), 0, -1));
    TEST_ASSERT_NULL(__vector_new(NULL, (vector_dtype_t)0,
                                  sizeof(int32_t), 0, 0));
    TEST_ASSERT_NULL(__vector_new(g_class_vector_ops_sso(),
                                  SEQ_CNTR_DTYPE_S32,
                                  sizeof(int32_t), 0, 0));
}
#endif /* !JDSC_DEBUG */

#if JDSC_ALIGNMEM
typedef struct wide_element {
    unsigned char bytes[64];
} __attribute__((aligned(64))) wide_element_t;

static bool wide_valid(ds_data_t data)
{
    return data != 0;
}

static bool wide_copy(ds_data_t in, ds_data_t* out)
{
    const wide_element_t* src = (const wide_element_t*)(uintptr_t)in;
    wide_element_t* dst = (wide_element_t*)out;

    memcpy(dst, src, sizeof(*dst));
    return true;
}

static void wide_free(ds_data_t* data)
{
    (void)data;
}

static const class_vector_ops_t wide_ops = {
    .valid_data = wide_valid,
    .copy_data = wide_copy,
    .free_data = wide_free,
};
#endif /* JDSC_ALIGNMEM */

TEST_CASE(unit, vector_align_config)
{
#if JDSC_ALIGNMEM
    vector_t* vector = VECTOR_NEW_COMPLATE(&wide_ops, SEQ_CNTR_DTYPE_STRUCT,
                                           wide_element_t, 0);
    wide_element_t element;
    vector_iterator_t it;
    vector_iterator_t end;
    size_t i;

    TEST_ASSERT_NOT_NULL(vector);
    test_cleanup_push(vector_free, vector);
    memset(&element, 0x5a, sizeof(element));
    for (i = 0; i < 100; ++i) {
        element.bytes[0] = (unsigned char)i;
        TEST_ASSERT_TRUE(cvector->push_back(vector, ds_cstdata(&element)));
    }

    end = cvector->end(vector);
    for (it = cvector->begin(vector); it.cur != end.cur;
         it = cvector->next(it))
        TEST_ASSERT_EQ(0, (uintptr_t)it.cur % 64);
    TEST_ASSERT_NULL(vector_check_invariants(vector));
#else
    TEST_SKIP("Aligned allocation is disabled in this configuration.");
#endif
}
