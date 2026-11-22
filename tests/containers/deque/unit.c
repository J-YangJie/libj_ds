/*
  libj_ds permanent tests deque unit semantics
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

#include <stdio.h>
#include <string.h>

#include "../../core/test.h"
#include "value.h"

deque_sort_define_sso_less(sort_sso_asc)

static void deque_free(void* self)
{
    deque_t* deque = self;

    DEQUE_DELETE(&deque);
}

static deque_t* open_int32(void)
{
    deque_t* deque = DEQUE_NEW_T(int32_t);

    TEST_ASSERT_NOT_NULL(deque);
    test_cleanup_push(deque_free, deque);
    TEST_ASSERT_NULL(deque_check_invariants(deque));
    return deque;
}

static deque_t* open_sso(void)
{
    deque_t* deque = DEQUE_NEW_SSO();

    TEST_ASSERT_NOT_NULL(deque);
    test_cleanup_push(deque_free, deque);
    TEST_ASSERT_NULL(deque_check_invariants(deque));
    return deque;
}

static void push_strings(deque_t* deque, const char* const* values,
                         size_t count)
{
    size_t i;

    for (i = 0; i < count; ++i)
        TEST_ASSERT_TRUE(cdeque->push_back(deque, ds_sdata(values[i])));
}

static void string_insert(const char** values, size_t* size, size_t index,
                          size_t count, const char* fill)
{
    size_t i;

    memmove(values + index + count, values + index,
            (*size - index) * sizeof(values[0]));
    for (i = 0; i < count; ++i)
        values[index + i] = fill;
    *size += count;
}

static deque_t* open_strings(const char* const* values, size_t count)
{
    deque_t* deque = open_sso();

    push_strings(deque, values, count);
    return deque;
}

static void expect_strings(const deque_t* deque, const char* const* values,
                           size_t count)
{
    size_t i;

    TEST_ASSERT_NULL(deque_check_invariants(deque));
    TEST_ASSERT_EQ(count, (size_t)cdeque->size(deque));
    for (i = 0; i < count; ++i) {
        deque_iterator_t it = cdeque->it(deque, i);

        TEST_ASSERT_STR_EQ(values[i], it_sdata(it));
        TEST_ASSERT_EQ(strlen(values[i]), it.ssd->len);
        TEST_ASSERT_TRUE((it.ssd->len <= __DS_SSO_LOCAL_CAP)
                         == (it.ssd->p == it.ssd->buf));
    }
}

static deque_t* open_tracked(void)
{
    deque_t* deque = DEQUE_NEW_COMPLATE(test_ops_tracked(),
                                        SEQ_CNTR_DTYPE_STRUCT, test_tracked_t);

    TEST_ASSERT_NOT_NULL(deque);
    test_cleanup_push(deque_free, deque);
    TEST_ASSERT_NULL(deque_check_invariants(deque));
    return deque;
}

static void push_tracked(deque_t* deque, int32_t payload)
{
    test_tracked_t value = test_tracked_make(payload);

    TEST_ASSERT_TRUE(test_tracked_valid_at(&value));
    TEST_ASSERT_TRUE(cdeque->push_back(deque, ds_cstdata(&value)));
    test_tracked_dispose(&value);
}

static void expect_tracked(const deque_t* deque, const int32_t* values,
                           size_t count)
{
    size_t i;

    TEST_ASSERT_NULL(deque_check_invariants(deque));
    TEST_ASSERT_EQ(count, (size_t)cdeque->size(deque));
    for (i = 0; i < count; ++i) {
        const test_tracked_t* stored =
            test_tracked_slot_at(cdeque->at(deque, i).u8);

        TEST_ASSERT_TRUE(test_tracked_valid_at(stored));
        TEST_ASSERT_EQ(values[i], test_tracked_payload_at(stored));
    }
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

static void push_all(deque_t* deque, int32_t first, size_t count)
{
    size_t i;

    for (i = 0; i < count; ++i)
        TEST_ASSERT_TRUE(cdeque->push_back(deque,
                                           __ds_s32data(first + (int32_t)i)));
}

static void expect_values(const deque_t* deque, const int32_t* values,
                          size_t count)
{
    deque_iterator_t it;
    deque_iterator_t end;
    deque_r_iterator_t rit;
    deque_r_iterator_t rend;
    size_t i;

    TEST_ASSERT_NULL(deque_check_invariants(deque));
    TEST_ASSERT_EQ(count, (size_t)cdeque->size(deque));

    end = cdeque->end(deque);
    i = 0;
    for (it = cdeque->begin(deque); it.cur != end.cur; it = cdeque->next(it)) {
        TEST_ASSERT_TRUE(i < count);
        TEST_ASSERT_EQ(values[i], it_s32data(it));
        ++i;
    }
    TEST_ASSERT_EQ(count, i);

    rend = cdeque->rend(deque);
    i = 0;
    for (rit = cdeque->rbegin(deque); rit.cur != rend.cur;
         rit = cdeque->rnext(rit)) {
        TEST_ASSERT_TRUE(i < count);
        TEST_ASSERT_EQ(values[count - 1 - i], it_s32data(rit));
        ++i;
    }
    TEST_ASSERT_EQ(count, i);
}

TEST_CASE(unit, deque_lifecycle)
{
    deque_t* deque = open_int32();
    const int32_t values[] = { 1, 2, 3, 4, 5 };

    TEST_ASSERT_EQ(0, cdeque->size(deque));
    TEST_ASSERT_TRUE(__cdeque_empty(deque));
    TEST_ASSERT_EQ(0, cdeque->count(deque, __ds_s32data(3)));
    TEST_ASSERT_NULL(cdeque->at(deque, 0).u8);
    TEST_ASSERT_TRUE(i_deque_is_null_iterator(cdeque->it(deque, 0)));
    TEST_ASSERT_EQ(0, cdeque->clear(deque));

    push_all(deque, 1, 5);
    TEST_ASSERT_FALSE(__cdeque_empty(deque));
    TEST_ASSERT_EQ(5, cdeque->size(deque));
    TEST_ASSERT_EQ(1, *cdeque->front(deque).s32);
    TEST_ASSERT_EQ(5, *cdeque->back(deque).s32);
    expect_values(deque, values, 5);

    TEST_ASSERT_TRUE(cdeque->push_back(deque, __ds_s32data(5)));
    TEST_ASSERT_EQ(2, cdeque->count(deque, __ds_s32data(5)));
    TEST_ASSERT_EQ(6, cdeque->size(deque));

    TEST_ASSERT_EQ(6, cdeque->clear(deque));
    TEST_ASSERT_EQ(0, cdeque->size(deque));
    TEST_ASSERT_TRUE(__cdeque_empty(deque));
    TEST_ASSERT_TRUE(cdeque->push_back(deque, __ds_s32data(9)));
    TEST_ASSERT_EQ(9, *cdeque->at(deque, 0).s32);
}

TEST_CASE(unit, deque_bucket_growth)
{
    deque_t* deque = open_int32();
    int32_t expected[3000];
    size_t size = 0;
    size_t i;

    for (i = 0; i < 2000; ++i) {
        TEST_ASSERT_TRUE(cdeque->push_back(deque, __ds_s32data((int32_t)i)));
        expected[size++] = (int32_t)i;
    }
    for (i = 2000; i < 3000; ++i) {
        TEST_ASSERT_TRUE(cdeque->push_front(deque, __ds_s32data((int32_t)i)));
        array_insert(expected, &size, 0, 1, (int32_t)i);
    }
    expect_values(deque, expected, size);

    for (i = 0; i < 1000; ++i) {
        cdeque->pop_front(deque);
        array_erase(expected, &size, 0, 1);
    }
    for (i = 0; i < 1000; ++i) {
        cdeque->pop_back(deque);
        array_erase(expected, &size, size - 1, 1);
    }
    expect_values(deque, expected, size);

    TEST_ASSERT_EQ(1000, cdeque->size(deque));
    TEST_ASSERT_EQ(0, *cdeque->front(deque).s32);
    TEST_ASSERT_EQ(999, *cdeque->back(deque).s32);

    for (i = 0; i < 500; ++i)
        TEST_ASSERT_TRUE(cdeque->push_back(deque, __ds_s32data(-1)));
    TEST_ASSERT_EQ(1500, cdeque->size(deque));
    TEST_ASSERT_EQ(1500, cdeque->clear(deque));
    TEST_ASSERT_NULL(deque_check_invariants(deque));
}

TEST_CASE(unit, deque_insert_returns)
{
    deque_t* deque = open_int32();
    deque_t* source = open_int32();
    deque_iterator_t it;
    int32_t expected[64];
    size_t size = 0;
    size_t i;

    for (i = 1; i <= 5; ++i) {
        TEST_ASSERT_TRUE(cdeque->push_back(deque, __ds_s32data((int32_t)i)));
        expected[size++] = (int32_t)i;
    }

    it = cdeque->insert(deque, cdeque->it(deque, 2), __ds_s32data(9));
    TEST_ASSERT_FALSE(i_deque_is_null_iterator(it));
    TEST_ASSERT_EQ(9, it_s32data(it));
    array_insert(expected, &size, 2, 1, 9);
    expect_values(deque, expected, size);

    it = cdeque->insert(deque, cdeque->begin(deque), __ds_s32data(0));
    TEST_ASSERT_EQ(0, it_s32data(it));
    array_insert(expected, &size, 0, 1, 0);

    it = cdeque->insert(deque, cdeque->end(deque), __ds_s32data(6));
    TEST_ASSERT_EQ(6, it_s32data(it));
    array_insert(expected, &size, size, 1, 6);

    it = cdeque->insert_n(deque, cdeque->it(deque, 3), 2, __ds_s32data(8));
    TEST_ASSERT_EQ(8, it_s32data(it));
    array_insert(expected, &size, 3, 2, 8);

    it = cdeque->insert_n(deque, cdeque->end(deque), 0, __ds_s32data(7));
    TEST_ASSERT_TRUE(it.cur == cdeque->end(deque).cur);

    it = cdeque->insert_n(deque, cdeque->end(deque), 2, __ds_s32data(7));
    TEST_ASSERT_EQ(7, it_s32data(it));
    array_insert(expected, &size, size, 2, 7);

    it = cdeque->insert_n(deque, cdeque->begin(deque), 1, __ds_s32data(4));
    TEST_ASSERT_EQ(4, it_s32data(it));
    array_insert(expected, &size, 0, 1, 4);
    expect_values(deque, expected, size);

    TEST_ASSERT_TRUE(cdeque->push_back(source, __ds_s32data(10)));
    TEST_ASSERT_TRUE(cdeque->push_back(source, __ds_s32data(20)));
    it = cdeque->insert_range(deque, cdeque->it(deque, 2),
                              cdeque->begin(source), cdeque->end(source));
    TEST_ASSERT_EQ(10, it_s32data(it));
    array_insert(expected, &size, 2, 1, 10);
    array_insert(expected, &size, 3, 1, 20);
    expect_values(deque, expected, size);

    TEST_ASSERT_TRUE(cdeque->assign_range(deque, cdeque->begin(source),
                                          cdeque->end(source)));
    expected[0] = 10;
    expected[1] = 20;
    size = 2;
    expect_values(deque, expected, size);

    TEST_ASSERT_TRUE(cdeque->assign_range(deque, cdeque->begin(source),
                                          cdeque->begin(source)));
    TEST_ASSERT_EQ(0, cdeque->size(deque));
    TEST_ASSERT_NULL(deque_check_invariants(deque));
}

TEST_CASE(unit, deque_erase_returns)
{
    deque_t* deque = open_int32();
    deque_iterator_t it;
    const int32_t after_begin[] = { 2, 3, 4, 5 };
    const int32_t after_middle[] = { 2, 3, 5 };
    const int32_t after_last[] = { 2, 3 };
    const int32_t after_one[] = { 2 };
    const int32_t final_one[] = { 42 };

    push_all(deque, 1, 5);

    it = cdeque->erase(deque, cdeque->begin(deque));
    TEST_ASSERT_EQ(2, it_s32data(it));
    expect_values(deque, after_begin, 4);

    it = cdeque->erase(deque, cdeque->it(deque, 2));
    TEST_ASSERT_EQ(5, it_s32data(it));
    expect_values(deque, after_middle, 3);

    it = cdeque->erase(deque, cdeque->it(deque, 2));
    TEST_ASSERT_TRUE(it.cur == cdeque->end(deque).cur);
    expect_values(deque, after_last, 2);

    it = cdeque->erase_range(deque, cdeque->begin(deque),
                             cdeque->begin(deque));
    TEST_ASSERT_TRUE(it.cur == cdeque->begin(deque).cur);
    expect_values(deque, after_last, 2);

    it = cdeque->erase_range(deque, cdeque->it(deque, 1),
                             cdeque->end(deque));
    TEST_ASSERT_TRUE(it.cur == cdeque->end(deque).cur);
    expect_values(deque, after_one, 1);

    it = cdeque->erase_range(deque, cdeque->begin(deque),
                             cdeque->end(deque));
    TEST_ASSERT_TRUE(it.cur == cdeque->end(deque).cur);
    TEST_ASSERT_EQ(0, cdeque->size(deque));

    TEST_ASSERT_TRUE(cdeque->push_back(deque, __ds_s32data(42)));
    TEST_ASSERT_EQ(1, cdeque->size(deque));
    TEST_ASSERT_EQ(42, *cdeque->front(deque).s32);
    TEST_ASSERT_NULL(deque_check_invariants(deque));

    expect_values(deque, final_one, 1);
}

TEST_CASE(unit, deque_shrink_regression)
{
    deque_t* deque = open_int32();
    int32_t expected[2048];
    size_t size = 0;
    size_t i;

    for (i = 0; i < 300; ++i) {
        TEST_ASSERT_TRUE(cdeque->push_back(deque, __ds_s32data((int32_t)i)));
        expected[size++] = (int32_t)i;
    }
    TEST_ASSERT_TRUE(cdeque->erase_range(deque, cdeque->it(deque, 50),
                                         cdeque->it(deque, 250)).cur != NULL);
    array_erase(expected, &size, 50, 200);
    cdeque->shrink_to_fit(deque);
    expect_values(deque, expected, size);

    TEST_ASSERT_TRUE(cdeque->push_front(deque, __ds_s32data(-1)));
    array_insert(expected, &size, 0, 1, -1);
    TEST_ASSERT_TRUE(cdeque->push_front(deque, __ds_s32data(-2)));
    array_insert(expected, &size, 0, 1, -2);
    expect_values(deque, expected, size);

    TEST_ASSERT_TRUE(cdeque->insert_n(deque, cdeque->it(deque, 10), 20,
                                      __ds_s32data(7)).cur != NULL);
    array_insert(expected, &size, 10, 20, 7);
    TEST_ASSERT_TRUE(cdeque->erase_range(deque, cdeque->it(deque, 25),
                                         cdeque->it(deque, 45)).cur != NULL);
    array_erase(expected, &size, 25, 20);
    cdeque->shrink_to_fit(deque);
    expect_values(deque, expected, size);

    for (i = 0; i < 50; ++i) {
        TEST_ASSERT_TRUE(cdeque->push_back(deque,
                                           __ds_s32data(1000 + (int32_t)i)));
        expected[size++] = 1000 + (int32_t)i;
    }
    TEST_ASSERT_TRUE(cdeque->erase_range(deque, cdeque->it(deque, 20),
                                         cdeque->it(deque, 50)).cur != NULL);
    array_erase(expected, &size, 20, 30);
    cdeque->shrink_to_fit(deque);
    expect_values(deque, expected, size);

    TEST_ASSERT_TRUE(cdeque->push_front(deque, __ds_s32data(77)));
    array_insert(expected, &size, 0, 1, 77);
    TEST_ASSERT_TRUE(cdeque->push_back(deque, __ds_s32data(88)));
    array_insert(expected, &size, size, 1, 88);
    expect_values(deque, expected, size);
}

TEST_CASE(unit, deque_shrink_edges)
{
    deque_t* deque = open_int32();
    const int32_t one[] = { 5 };
    int32_t expected[128];
    size_t size = 0;
    size_t i;

    cdeque->shrink_to_fit(deque);
    TEST_ASSERT_EQ(0, cdeque->size(deque));
    TEST_ASSERT_NULL(deque_check_invariants(deque));

    TEST_ASSERT_TRUE(cdeque->push_back(deque, __ds_s32data(5)));
    array_insert(expected, &size, 0, 1, 5);
    cdeque->shrink_to_fit(deque);
    expect_values(deque, one, 1);

    TEST_ASSERT_TRUE(cdeque->push_back(deque, __ds_s32data(6)));
    array_insert(expected, &size, size, 1, 6);
    cdeque->shrink_to_fit(deque);
    for (i = 0; i < 40; ++i) {
        int32_t value = (int32_t)(i % 2 == 0 ? i : -i);

        TEST_ASSERT_TRUE(cdeque->push_front(deque, __ds_s32data(value)));
        array_insert(expected, &size, 0, 1, value);
    }
    for (i = 0; i < 40; ++i) {
        TEST_ASSERT_TRUE(cdeque->push_back(deque, __ds_s32data((int32_t)i)));
        expected[size++] = (int32_t)i;
    }
    expect_values(deque, expected, size);
    TEST_ASSERT_EQ(82, cdeque->size(deque));
    TEST_ASSERT_NULL(deque_check_invariants(deque));
    TEST_ASSERT_EQ(82, cdeque->clear(deque));
}

TEST_CASE(unit, deque_replace_and_find)
{
    deque_t* deque = open_int32();
    deque_iterator_t it;

    push_all(deque, 1, 5);

    TEST_ASSERT_TRUE(cdeque->replace_index(deque, 0, __ds_s32data(10)));
    TEST_ASSERT_EQ(10, *cdeque->at(deque, 0).s32);
    TEST_ASSERT_FALSE(cdeque->replace_index(deque, 5, __ds_s32data(11)));
    TEST_ASSERT_EQ(5, cdeque->size(deque));

    it = cdeque->it(deque, 2);
    TEST_ASSERT_TRUE(cdeque->replace_it(deque, it, __ds_s32data(30)));
    TEST_ASSERT_EQ(30, *cdeque->at(deque, 2).s32);
    TEST_ASSERT_FALSE(cdeque->replace_it(deque, cdeque->end(deque),
                                         __ds_s32data(31)));

    it = cdeque->find(deque, __ds_s32data(30));
    TEST_ASSERT_FALSE(i_deque_is_null_iterator(it));
    TEST_ASSERT_TRUE(it.cur != cdeque->end(deque).cur);
    TEST_ASSERT_EQ(30, it_s32data(it));

    it = cdeque->find(deque, __ds_s32data(99));
    TEST_ASSERT_TRUE(it.cur == cdeque->end(deque).cur);
    TEST_ASSERT_EQ(0, cdeque->count(deque, __ds_s32data(99)));
    TEST_ASSERT_NULL(deque_check_invariants(deque));
}

TEST_CASE(unit, deque_element_address_stability)
{
    deque_t* deque = open_int32();
    deque_iterator_t it;
    uint8_t* address;
    int32_t value;
    size_t i;

    push_all(deque, 0, 200);
    it = cdeque->it(deque, 100);
    address = it.cur;
    value = it_s32data(it);

    for (i = 0; i < 200; ++i)
        TEST_ASSERT_TRUE(cdeque->push_front(deque, __ds_s32data(-1)));
    TEST_ASSERT_TRUE(it.cur == address);
    TEST_ASSERT_EQ(value, *cdeque->at(deque, 300).s32);
    TEST_ASSERT_TRUE(cdeque->it(deque, 300).cur == address);

    for (i = 0; i < 100; ++i)
        cdeque->pop_front(deque);
    TEST_ASSERT_TRUE(cdeque->it(deque, 200).cur == address);
    TEST_ASSERT_EQ(100, it_s32data(cdeque->it(deque, 200)));

    for (i = 0; i < 50; ++i)
        cdeque->pop_back(deque);
    TEST_ASSERT_TRUE(cdeque->it(deque, 200).cur == address);
    TEST_ASSERT_EQ(100, it_s32data(cdeque->it(deque, 200)));
    TEST_ASSERT_NULL(deque_check_invariants(deque));
}

TEST_CASE(unit, deque_dtype_matrix)
{
    {
        deque_t* deque = DEQUE_NEW_T(uint8_t);

        TEST_ASSERT_NOT_NULL(deque);
        test_cleanup_push(deque_free, deque);
        TEST_ASSERT_TRUE(cdeque->push_back(deque, __ds_u8data(250)));
        TEST_ASSERT_EQ(250, *cdeque->at(deque, 0).u8);
        TEST_ASSERT_EQ(1, cdeque->count(deque, __ds_u8data(250)));
    }
    {
        deque_t* deque = DEQUE_NEW_T(uint16_t);

        TEST_ASSERT_NOT_NULL(deque);
        test_cleanup_push(deque_free, deque);
        TEST_ASSERT_TRUE(cdeque->push_back(deque, __ds_u16data(60000)));
        TEST_ASSERT_EQ(60000, *cdeque->at(deque, 0).u16);
        TEST_ASSERT_EQ(1, cdeque->count(deque, __ds_u16data(60000)));
    }
    {
        deque_t* deque = DEQUE_NEW_T(uint32_t);

        TEST_ASSERT_NOT_NULL(deque);
        test_cleanup_push(deque_free, deque);
        TEST_ASSERT_TRUE(cdeque->push_back(deque, __ds_u32data(4000000000u)));
        TEST_ASSERT_EQ(4000000000u, *cdeque->at(deque, 0).u32);
        TEST_ASSERT_EQ(1, cdeque->count(deque, __ds_u32data(4000000000u)));
    }
    {
        deque_t* deque = DEQUE_NEW_T(uint64_t);

        TEST_ASSERT_NOT_NULL(deque);
        test_cleanup_push(deque_free, deque);
        TEST_ASSERT_TRUE(cdeque->push_back(deque,
                                           ds_u64data(1234567890123ULL)));
        TEST_ASSERT_EQ(1234567890123LL, *cdeque->at(deque, 0).u64);
        TEST_ASSERT_EQ(1, cdeque->count(deque,
                                        ds_u64data(1234567890123ULL)));
    }
    {
        deque_t* deque = DEQUE_NEW_T(int8_t);

        TEST_ASSERT_NOT_NULL(deque);
        test_cleanup_push(deque_free, deque);
        TEST_ASSERT_TRUE(cdeque->push_back(deque, __ds_s8data(-100)));
        TEST_ASSERT_EQ(-100, *cdeque->at(deque, 0).s8);
        TEST_ASSERT_EQ(1, cdeque->count(deque, __ds_s8data(-100)));
    }
    {
        deque_t* deque = DEQUE_NEW_T(int16_t);

        TEST_ASSERT_NOT_NULL(deque);
        test_cleanup_push(deque_free, deque);
        TEST_ASSERT_TRUE(cdeque->push_back(deque, __ds_s16data(-30000)));
        TEST_ASSERT_EQ(-30000, *cdeque->at(deque, 0).s16);
        TEST_ASSERT_EQ(1, cdeque->count(deque, __ds_s16data(-30000)));
    }
    {
        deque_t* deque = DEQUE_NEW_T(int32_t);

        TEST_ASSERT_NOT_NULL(deque);
        test_cleanup_push(deque_free, deque);
        TEST_ASSERT_TRUE(cdeque->push_back(deque,
                                           __ds_s32data(-2000000000)));
        TEST_ASSERT_EQ(-2000000000, *cdeque->at(deque, 0).s32);
    }
    {
        deque_t* deque = DEQUE_NEW_T(int64_t);

        TEST_ASSERT_NOT_NULL(deque);
        test_cleanup_push(deque_free, deque);
        TEST_ASSERT_TRUE(cdeque->push_back(deque,
                                           ds_s64data(-1234567890123LL)));
        TEST_ASSERT_EQ(-1234567890123LL, *cdeque->at(deque, 0).s64);
        TEST_ASSERT_EQ(1, cdeque->count(deque,
                                        ds_s64data(-1234567890123LL)));
    }
    {
        deque_t* deque = DEQUE_NEW_T(uintptr_t);

        TEST_ASSERT_NOT_NULL(deque);
        test_cleanup_push(deque_free, deque);
        TEST_ASSERT_TRUE(cdeque->push_back(deque, __ds_uptrdata(123456u)));
        TEST_ASSERT_EQ(123456, *cdeque->at(deque, 0).d);
    }
    {
        deque_t* deque = DEQUE_NEW_T(intptr_t);

        TEST_ASSERT_NOT_NULL(deque);
        test_cleanup_push(deque_free, deque);
        TEST_ASSERT_TRUE(cdeque->push_back(deque, __ds_iptrdata(-12345)));
        TEST_ASSERT_EQ(-12345, *cdeque->at(deque, 0).d);
    }
    {
        deque_t* deque = DEQUE_NEW_T(size_t);

        TEST_ASSERT_NOT_NULL(deque);
        test_cleanup_push(deque_free, deque);
        TEST_ASSERT_TRUE(cdeque->push_back(deque, __ds_sizedata(123456)));
        TEST_ASSERT_EQ(123456, *cdeque->at(deque, 0).d);
    }
    {
        deque_t* deque = DEQUE_NEW_T(ssize_t);

        TEST_ASSERT_NOT_NULL(deque);
        test_cleanup_push(deque_free, deque);
        TEST_ASSERT_TRUE(cdeque->push_back(deque, __ds_ssizedata(-123456)));
        TEST_ASSERT_EQ(-123456, *cdeque->at(deque, 0).d);
    }
    {
        deque_t* deque = DEQUE_NEW_T(float);

        TEST_ASSERT_NOT_NULL(deque);
        test_cleanup_push(deque_free, deque);
        TEST_ASSERT_TRUE(cdeque->push_back(deque, ds_fdata(1.5f)));
        TEST_ASSERT_TRUE(cdeque->push_back(deque, ds_fdata(-0.0f)));
        TEST_ASSERT_EQ(1.5f, *cdeque->at(deque, 0).f);
        TEST_ASSERT_EQ(1, cdeque->count(deque, ds_fdata(0.0f)));
    }
    {
        deque_t* deque = DEQUE_NEW_T(double);

        TEST_ASSERT_NOT_NULL(deque);
        test_cleanup_push(deque_free, deque);
        TEST_ASSERT_TRUE(cdeque->push_back(deque, ds_lfdata(-2.25)));
        TEST_ASSERT_TRUE(cdeque->push_back(deque, ds_lfdata(-0.0)));
        TEST_ASSERT_EQ(-2.25, *cdeque->at(deque, 0).lf);
        TEST_ASSERT_EQ(1, cdeque->count(deque, ds_lfdata(0.0)));
    }
}

TEST_CASE(unit, deque_inline_strings)
{
    static const char* const short15 = "abcdefghijklmno";
    static const char* const long16 = "abcdefghijklmnop";
    static const char* const long45 = "this string is longer than the inline buffer";
    deque_t* sso = DEQUE_NEW_SSO();
    deque_t* chars = DEQUE_NEW_CHAR();
    deque_iterator_t it;

    TEST_ASSERT_NOT_NULL(sso);
    TEST_ASSERT_NOT_NULL(chars);
    test_cleanup_push(deque_free, sso);
    test_cleanup_push(deque_free, chars);

    TEST_ASSERT_TRUE(cdeque->push_back(sso, ds_sdata(short15)));
    TEST_ASSERT_TRUE(cdeque->push_back(sso, ds_sdata(long16)));
    TEST_ASSERT_TRUE(cdeque->push_back(sso, ds_sdata(long45)));
    TEST_ASSERT_TRUE(cdeque->push_back(chars, ds_sdata(short15)));
    TEST_ASSERT_TRUE(cdeque->push_back(chars, ds_sdata(long16)));
    TEST_ASSERT_TRUE(cdeque->push_back(chars, ds_sdata(long45)));

    it = cdeque->begin(sso);
    TEST_ASSERT_EQ(15, it.ssd->len);
    TEST_ASSERT_TRUE(it.ssd->p == it.ssd->buf);
    TEST_ASSERT_STR_EQ(short15, it_sdata(it));
    it = cdeque->next(it);
    TEST_ASSERT_EQ(16, it.ssd->len);
    TEST_ASSERT_TRUE(it.ssd->p != it.ssd->buf);
    TEST_ASSERT_STR_EQ(long16, it_sdata(it));
    it = cdeque->next(it);
    TEST_ASSERT_EQ(44, it.ssd->len);
    TEST_ASSERT_STR_EQ(long45, it_sdata(it));

    it = cdeque->begin(chars);
    TEST_ASSERT_STR_EQ(short15, it_sdata(it));
    it = cdeque->next(it);
    TEST_ASSERT_STR_EQ(long16, it_sdata(it));
    it = cdeque->next(it);
    TEST_ASSERT_STR_EQ(long45, it_sdata(it));

    it = cdeque->find(sso, ds_sdata(long16));
    TEST_ASSERT_TRUE(it.cur != cdeque->end(sso).cur);
    TEST_ASSERT_STR_EQ(long16, it_sdata(it));
    TEST_ASSERT_EQ(1, cdeque->count(sso, ds_sdata(long45)));
    TEST_ASSERT_EQ(0, cdeque->count(chars, ds_sdata("missing")));

    TEST_ASSERT_EQ(1, cdeque->remove(sso, ds_sdata(long16)));
    TEST_ASSERT_EQ(2, cdeque->size(sso));
    TEST_ASSERT_EQ(1, cdeque->remove(chars, ds_sdata(long16)));
    TEST_ASSERT_EQ(2, cdeque->size(chars));

    TEST_ASSERT_TRUE(cdeque->replace_index(sso, 0, ds_sdata(long45)));
    TEST_ASSERT_EQ(44, cdeque->at(sso, 0).ssd->len);
    TEST_ASSERT_STR_EQ(long45, cdeque->at(sso, 0).ssd->p);
    cdeque->shrink_to_fit(sso);
    TEST_ASSERT_TRUE(cdeque->push_front(sso, ds_sdata(short15)));
    TEST_ASSERT_EQ(3, cdeque->size(sso));
    TEST_ASSERT_NULL(deque_check_invariants(sso));

    TEST_ASSERT_EQ(3, cdeque->clear(sso));
    TEST_ASSERT_EQ(2, cdeque->clear(chars));
}

TEST_CASE(unit, deque_sso_sort)
{
    static const char* const inputs[] = {
        "dddd", "bbbbbbbbbbbbbbbb", "aaaa", "cccccccccccccccccccccccc"
    };
    static const char* const ordered[] = {
        "aaaa", "bbbbbbbbbbbbbbbb", "cccccccccccccccccccccccc", "dddd"
    };
    deque_t* deque = DEQUE_NEW_SSO();
    size_t i;

    TEST_ASSERT_NOT_NULL(deque);
    test_cleanup_push(deque_free, deque);

    for (i = 0; i < sizeof(inputs) / sizeof(inputs[0]); ++i)
        TEST_ASSERT_TRUE(cdeque->push_back(deque, ds_sdata(inputs[i])));

    sort_sso_asc(cdeque->begin(deque), cdeque->end(deque));
    TEST_ASSERT_TRUE(sort_sso_asc_sorted(cdeque->begin(deque),
                                         cdeque->end(deque)));
    TEST_ASSERT_NULL(deque_check_invariants(deque));

    for (i = 0; i < sizeof(ordered) / sizeof(ordered[0]); ++i) {
        deque_iterator_t it = cdeque->it(deque, i);
        const ds_sso_t* value = it.ssd;

        TEST_ASSERT_STR_EQ(ordered[i], it_sdata(it));
        TEST_ASSERT_EQ(strlen(ordered[i]), value->len);
        TEST_ASSERT_TRUE((value->len <= __DS_SSO_LOCAL_CAP)
                         == (value->p == value->buf));
    }

    cdeque->shrink_to_fit(deque);
    TEST_ASSERT_NULL(deque_check_invariants(deque));
    TEST_ASSERT_TRUE(cdeque->push_front(deque,
                                        ds_sdata("abcdefghijklmnop")));
    TEST_ASSERT_EQ(5, cdeque->size(deque));
    TEST_ASSERT_STR_EQ("abcdefghijklmnop",
                       it_sdata(cdeque->at(deque, 0)));
    TEST_ASSERT_STR_EQ(ordered[3], it_sdata(cdeque->at(deque, 4)));
    TEST_ASSERT_NULL(deque_check_invariants(deque));
}

TEST_CASE(unit, deque_self_reference_strings)
{
    static const char* const initial[6] = {
        "aaaa", "bbbb", "cccc", "dddd", "eeee", "ffff"
    };
    static const char* const front_one[7] = {
        "aaaa", "bbbb", "aaaa", "cccc", "dddd", "eeee", "ffff"
    };
    static const char* const front_many[8] = {
        "aaaa", "bbbb", "aaaa", "aaaa", "cccc", "dddd", "eeee", "ffff"
    };
    static const char* const back_one[7] = {
        "aaaa", "bbbb", "cccc", "eeee", "dddd", "eeee", "ffff"
    };
    static const char* const back_many[8] = {
        "aaaa", "bbbb", "cccc", "ffff", "ffff", "dddd", "eeee", "ffff"
    };
    static const char* const own_back[7] = {
        "aaaa", "bbbb", "cccc", "dddd", "dddd", "eeee", "ffff"
    };
    static const char* const own_front[7] = {
        "aaaa", "bbbb", "cccc", "cccc", "dddd", "eeee", "ffff"
    };
    static const char* const front_unmoved[8] = {
        "aaaa", "eeee", "eeee", "bbbb", "cccc", "dddd", "eeee", "ffff"
    };
    static const char* const back_unmoved[8] = {
        "aaaa", "bbbb", "cccc", "dddd", "bbbb", "bbbb", "eeee", "ffff"
    };
    static const char* const initial_long[6] = {
        "aaaa", "bbbb", "cccc", "dddd", "eeee", "abcdefghijklmnop"
    };
    static const char* const long_moved[7] = {
        "aaaa", "bbbb", "cccc", "abcdefghijklmnop", "dddd", "eeee",
        "abcdefghijklmnop"
    };
    static const char* const begin_expected[8] = {
        "aaaa", "aaaa", "aaaa", "bbbb", "cccc", "dddd", "eeee", "ffff"
    };
    static const char* const end_expected[8] = {
        "aaaa", "bbbb", "cccc", "dddd", "eeee", "ffff", "ffff", "ffff"
    };
    static const char* const push_front_expected[7] = {
        "aaaa", "aaaa", "bbbb", "cccc", "dddd", "eeee", "ffff"
    };
    static const char* const push_back_expected[7] = {
        "aaaa", "bbbb", "cccc", "dddd", "eeee", "ffff", "ffff"
    };
    static const char* const resize_expected[8] = {
        "aaaa", "bbbb", "cccc", "dddd", "eeee", "ffff", "aaaa", "aaaa"
    };
    static const char* const assign_expected[8] = {
        "aaaa", "aaaa", "aaaa", "aaaa", "aaaa", "aaaa", "aaaa", "aaaa"
    };
    deque_t* deque;
    const char* alias;

    /* A front shift overwrites a prefix source slot before it is read. */
    deque = open_strings(initial, 6);
    alias = cdeque->at(deque, 0).ssd->p;
    TEST_ASSERT_TRUE(cdeque->insert(deque, cdeque->it(deque, 2),
                                    ds_sdata(alias)).cur != NULL);
    expect_strings(deque, front_one, 7);

    deque = open_strings(initial, 6);
    alias = cdeque->at(deque, 0).ssd->p;
    TEST_ASSERT_TRUE(cdeque->insert_n(deque, cdeque->it(deque, 2), 2,
                                      ds_sdata(alias)).cur != NULL);
    expect_strings(deque, front_many, 8);

    /* A back shift overwrites a suffix source slot before it is read. */
    deque = open_strings(initial, 6);
    alias = cdeque->at(deque, 4).ssd->p;
    TEST_ASSERT_TRUE(cdeque->insert(deque, cdeque->it(deque, 3),
                                    ds_sdata(alias)).cur != NULL);
    expect_strings(deque, back_one, 7);

    deque = open_strings(initial, 6);
    alias = cdeque->at(deque, 5).ssd->p;
    TEST_ASSERT_TRUE(cdeque->insert_n(deque, cdeque->it(deque, 3), 2,
                                      ds_sdata(alias)).cur != NULL);
    expect_strings(deque, back_many, 8);

    /* The source may also be exactly the insertion position. */
    deque = open_strings(initial, 6);
    alias = cdeque->at(deque, 3).ssd->p;
    TEST_ASSERT_TRUE(cdeque->insert(deque, cdeque->it(deque, 3),
                                    ds_sdata(alias)).cur != NULL);
    expect_strings(deque, own_back, 7);

    deque = open_strings(initial, 6);
    alias = cdeque->at(deque, 2).ssd->p;
    TEST_ASSERT_TRUE(cdeque->insert(deque, cdeque->it(deque, 2),
                                    ds_sdata(alias)).cur != NULL);
    expect_strings(deque, own_front, 7);

    /* Sources on the side that does not move keep their value. */
    deque = open_strings(initial, 6);
    alias = cdeque->at(deque, 4).ssd->p;
    TEST_ASSERT_TRUE(cdeque->insert_n(deque, cdeque->it(deque, 1), 2,
                                      ds_sdata(alias)).cur != NULL);
    expect_strings(deque, front_unmoved, 8);

    deque = open_strings(initial, 6);
    alias = cdeque->at(deque, 1).ssd->p;
    TEST_ASSERT_TRUE(cdeque->insert_n(deque, cdeque->it(deque, 4), 2,
                                      ds_sdata(alias)).cur != NULL);
    expect_strings(deque, back_unmoved, 8);

    /* A heap-backed source token stays valid across the shift. */
    deque = open_strings(initial_long, 6);
    alias = cdeque->at(deque, 5).ssd->p;
    TEST_ASSERT_TRUE(alias != cdeque->at(deque, 5).ssd->buf);
    TEST_ASSERT_TRUE(cdeque->insert(deque, cdeque->it(deque, 3),
                                    ds_sdata(alias)).cur != NULL);
    expect_strings(deque, long_moved, 7);

    /* The source and the insertion point are in different buckets. */
    {
        enum { COUNT = 40 };
        char labels[COUNT][8];
        const char* expected[COUNT + 3];
        size_t size = 0;
        size_t i;

        deque = open_sso();
        for (i = 0; i < COUNT; ++i) {
            snprintf(labels[i], sizeof(labels[i]), "e%02u", (unsigned)i);
            TEST_ASSERT_TRUE(cdeque->push_back(deque, ds_sdata(labels[i])));
            expected[size++] = labels[i];
        }
        alias = cdeque->at(deque, 0).ssd->p;
        TEST_ASSERT_TRUE(cdeque->insert_n(deque, cdeque->it(deque, 17), 2,
                                          ds_sdata(alias)).cur != NULL);
        string_insert(expected, &size, 17, 2, labels[0]);
        expect_strings(deque, expected, size);

        deque = open_sso();
        for (i = 0; i < COUNT; ++i)
            TEST_ASSERT_TRUE(cdeque->push_back(deque, ds_sdata(labels[i])));
        size = 0;
        for (i = 0; i < COUNT; ++i)
            expected[size++] = labels[i];
        alias = cdeque->at(deque, 35).ssd->p;
        TEST_ASSERT_TRUE(cdeque->insert_n(deque, cdeque->it(deque, 20), 3,
                                          ds_sdata(alias)).cur != NULL);
        string_insert(expected, &size, 20, 3, labels[35]);
        expect_strings(deque, expected, size);
    }

    /* Opening a new front bucket reallocates the map during the insert. */
    {
        enum { BACK = 81, PAD = 32, TOTAL = BACK + PAD };
        char labels[TOTAL][8];
        const char* expected[TOTAL + 1];
        size_t size = 0;
        size_t i;

        deque = open_sso();
        for (i = 0; i < TOTAL; ++i)
            snprintf(labels[i], sizeof(labels[i]), "e%03u", (unsigned)i);
        for (i = 0; i < BACK; ++i) {
            TEST_ASSERT_TRUE(cdeque->push_back(deque, ds_sdata(labels[i])));
            expected[size++] = labels[i];
        }
        cdeque->shrink_to_fit(deque);
        i = 0;
        while (!(deque->begin.bkt == deque->bkt
                 && deque->begin.cur == deque->begin.begin)) {
            TEST_ASSERT_TRUE(cdeque->push_front(deque,
                                                ds_sdata(labels[BACK + i])));
            string_insert(expected, &size, 0, 1, labels[BACK + i]);
            TEST_ASSERT_TRUE(++i < PAD);
        }
        alias = cdeque->at(deque, 0).ssd->p;
        TEST_ASSERT_TRUE(cdeque->insert_n(deque, cdeque->it(deque, 40), 1,
                                          ds_sdata(alias)).cur != NULL);
        string_insert(expected, &size, 40, 1, labels[BACK + i - 1]);
        expect_strings(deque, expected, size);
    }

    /* Begin/end inserts and the push paths never shift existing slots. */
    deque = open_strings(initial, 6);
    alias = cdeque->at(deque, 0).ssd->p;
    TEST_ASSERT_TRUE(cdeque->insert_n(deque, cdeque->begin(deque), 2,
                                      ds_sdata(alias)).cur != NULL);
    expect_strings(deque, begin_expected, 8);

    deque = open_strings(initial, 6);
    alias = cdeque->at(deque, 5).ssd->p;
    TEST_ASSERT_TRUE(cdeque->insert_n(deque, cdeque->end(deque), 2,
                                      ds_sdata(alias)).cur != NULL);
    expect_strings(deque, end_expected, 8);

    deque = open_strings(initial, 6);
    alias = cdeque->at(deque, 0).ssd->p;
    TEST_ASSERT_TRUE(cdeque->push_front(deque, ds_sdata(alias)));
    expect_strings(deque, push_front_expected, 7);

    deque = open_strings(initial, 6);
    alias = cdeque->at(deque, 5).ssd->p;
    TEST_ASSERT_TRUE(cdeque->push_back(deque, ds_sdata(alias)));
    expect_strings(deque, push_back_expected, 7);

    deque = open_strings(initial, 6);
    alias = cdeque->at(deque, 0).ssd->p;
    TEST_ASSERT_TRUE(cdeque->resize(deque, 8, ds_sdata(alias)));
    expect_strings(deque, resize_expected, 8);

    deque = open_strings(initial, 6);
    alias = cdeque->at(deque, 0).ssd->p;
    TEST_ASSERT_TRUE(cdeque->assign(deque, 8, ds_sdata(alias)));
    expect_strings(deque, assign_expected, 8);
}

TEST_CASE(unit, deque_self_reference_tracked)
{
    static const int32_t insert_expected[5] = { 11, 11, 22, 33, 44 };
    static const int32_t insert_n_expected[6] = { 11, 22, 44, 44, 33, 44 };
    int32_t cross_expected[42];
    static const size_t CROSS = 40;
    deque_t* deque;
    const test_tracked_t* alias;
    size_t baseline = test_tracked_live();
    size_t copies;
    size_t moves;
    size_t live;
    size_t i;

    deque = open_tracked();
    push_tracked(deque, 11);
    push_tracked(deque, 22);
    push_tracked(deque, 33);
    push_tracked(deque, 44);
    alias = test_tracked_slot_at(cdeque->at(deque, 0).u8);
    copies = test_tracked_copies();
    moves = test_tracked_moves();
    live = test_tracked_live();
    TEST_ASSERT_TRUE(cdeque->insert(deque, cdeque->it(deque, 1),
                                    ds_cstdata(alias)).cur != NULL);
    TEST_ASSERT_EQ(copies + 1, test_tracked_copies());
    TEST_ASSERT_EQ(moves + 2, test_tracked_moves());
    TEST_ASSERT_EQ(live + 1, test_tracked_live());
    expect_tracked(deque, insert_expected, 5);
    TEST_ASSERT_EQ(5, cdeque->clear(deque));
    TEST_ASSERT_EQ(baseline, test_tracked_live());

    deque = open_tracked();
    push_tracked(deque, 11);
    push_tracked(deque, 22);
    push_tracked(deque, 33);
    push_tracked(deque, 44);
    alias = test_tracked_slot_at(cdeque->at(deque, 3).u8);
    copies = test_tracked_copies();
    moves = test_tracked_moves();
    live = test_tracked_live();
    TEST_ASSERT_TRUE(cdeque->insert_n(deque, cdeque->it(deque, 2), 2,
                                      ds_cstdata(alias)).cur != NULL);
    TEST_ASSERT_EQ(copies + 2, test_tracked_copies());
    TEST_ASSERT_EQ(moves + 3, test_tracked_moves());
    TEST_ASSERT_EQ(live + 2, test_tracked_live());
    expect_tracked(deque, insert_n_expected, 6);
    TEST_ASSERT_EQ(6, cdeque->clear(deque));
    TEST_ASSERT_EQ(baseline, test_tracked_live());

    deque = open_tracked();
    for (i = 0; i < CROSS; ++i) {
        push_tracked(deque, 100 + (int32_t)i);
        cross_expected[i] = 100 + (int32_t)i;
    }
    alias = test_tracked_slot_at(cdeque->at(deque, 35).u8);
    copies = test_tracked_copies();
    moves = test_tracked_moves();
    live = test_tracked_live();
    TEST_ASSERT_TRUE(cdeque->insert_n(deque, cdeque->it(deque, 20), 2,
                                      ds_cstdata(alias)).cur != NULL);
    TEST_ASSERT_EQ(copies + 2, test_tracked_copies());
    TEST_ASSERT_EQ(moves + 21, test_tracked_moves());
    TEST_ASSERT_EQ(live + 2, test_tracked_live());
    memmove(cross_expected + 22, cross_expected + 20,
            (CROSS - 20) * sizeof(cross_expected[0]));
    cross_expected[20] = 135;
    cross_expected[21] = 135;
    expect_tracked(deque, cross_expected, CROSS + 2);
    TEST_ASSERT_EQ(CROSS + 2, cdeque->clear(deque));
    TEST_ASSERT_EQ(baseline, test_tracked_live());
}

#ifdef JDSC_32BIT
TEST_CASE(unit, deque_self_reference_w8_32bit)
{
    static const int64_t begin_expected[4] = { 20, 10, 20, 30 };
    static const int64_t middle_expected[7] = { 10, 20, 30, 50, 50, 40, 50 };
    deque_t* deque;
    int64_t* alias;
    size_t i;

    /* On 32-bit, an 8-byte element token points to the boxed slot. */
    deque = DEQUE_NEW_COMPLATE(NULL, SEQ_CNTR_DTYPE_S64, int64_t);
    TEST_ASSERT_NOT_NULL(deque);
    test_cleanup_push(deque_free, deque);
    TEST_ASSERT_TRUE(cdeque->push_back(deque, ds_s64data(10)));
    TEST_ASSERT_TRUE(cdeque->push_back(deque, ds_s64data(20)));
    TEST_ASSERT_TRUE(cdeque->push_back(deque, ds_s64data(30)));
    alias = cdeque->at(deque, 1).s64;
    TEST_ASSERT_TRUE(cdeque->insert(deque, cdeque->begin(deque),
                                    ds_cstdata(alias)).cur != NULL);
    TEST_ASSERT_NULL(deque_check_invariants(deque));
    TEST_ASSERT_EQ(4, cdeque->size(deque));
    for (i = 0; i < 4; ++i)
        TEST_ASSERT_EQ(begin_expected[i], *cdeque->at(deque, i).s64);

    deque = DEQUE_NEW_COMPLATE(NULL, SEQ_CNTR_DTYPE_S64, int64_t);
    TEST_ASSERT_NOT_NULL(deque);
    test_cleanup_push(deque_free, deque);
    TEST_ASSERT_TRUE(cdeque->push_back(deque, ds_s64data(10)));
    TEST_ASSERT_TRUE(cdeque->push_back(deque, ds_s64data(20)));
    TEST_ASSERT_TRUE(cdeque->push_back(deque, ds_s64data(30)));
    TEST_ASSERT_TRUE(cdeque->push_back(deque, ds_s64data(40)));
    TEST_ASSERT_TRUE(cdeque->push_back(deque, ds_s64data(50)));
    alias = cdeque->at(deque, 4).s64;
    TEST_ASSERT_TRUE(cdeque->insert_n(deque, cdeque->it(deque, 3), 2,
                                      ds_cstdata(alias)).cur != NULL);
    TEST_ASSERT_NULL(deque_check_invariants(deque));
    TEST_ASSERT_EQ(7, cdeque->size(deque));
    for (i = 0; i < 7; ++i)
        TEST_ASSERT_EQ(middle_expected[i], *cdeque->at(deque, i).s64);
}
#endif /* JDSC_32BIT */

TEST_CASE(unit, deque_invariant_checker_negative)
{
    deque_t* deque = open_int32();
    deque_t snapshot;
    size_t i;

    for (i = 0; i < 300; ++i)
        TEST_ASSERT_TRUE(cdeque->push_back(deque, __ds_s32data((int32_t)i)));
    TEST_ASSERT_NULL(deque_check_invariants(deque));

    snapshot = *deque;
    snapshot.bkt_size = 7;
    TEST_ASSERT_NOT_NULL(deque_check_invariants(&snapshot));

    snapshot = *deque;
    snapshot.begin.cur = snapshot.begin.end;
    TEST_ASSERT_NOT_NULL(deque_check_invariants(&snapshot));

    snapshot = *deque;
    snapshot.step = 0;
    TEST_ASSERT_NOT_NULL(deque_check_invariants(&snapshot));

    TEST_ASSERT_NULL(deque_check_invariants(deque));
}

TEST_CASE(unit, deque_tracked_ops)
{
    deque_t* deque = DEQUE_NEW_COMPLATE(test_ops_tracked(),
                                       SEQ_CNTR_DTYPE_STRUCT, test_tracked_t);
    test_tracked_t first = test_tracked_make(11);
    test_tracked_t second = test_tracked_make(22);
    test_tracked_t probe = test_tracked_make(22);
    test_tracked_t replacement = test_tracked_make(44);
    const test_tracked_t* stored;
    size_t copies;
    size_t frees;

    TEST_ASSERT_NOT_NULL(deque);
    test_cleanup_push(deque_free, deque);
    TEST_ASSERT_TRUE(test_tracked_valid_at(&first));
    TEST_ASSERT_TRUE(test_tracked_valid_at(&second));
    TEST_ASSERT_TRUE(test_tracked_valid_at(&probe));
    TEST_ASSERT_TRUE(test_tracked_valid_at(&replacement));

    TEST_ASSERT_TRUE(cdeque->push_back(deque, ds_cstdata(&first)));
    test_tracked_dispose(&first);
    TEST_ASSERT_TRUE(cdeque->push_back(deque, ds_cstdata(&second)));
    test_tracked_dispose(&second);

    stored = test_tracked_slot_at(cdeque->at(deque, 0).u8);
    TEST_ASSERT_TRUE(test_tracked_valid_at(stored));
    TEST_ASSERT_EQ(11, test_tracked_payload_at(stored));
    stored = test_tracked_slot_at(cdeque->at(deque, 1).u8);
    TEST_ASSERT_EQ(22, test_tracked_payload_at(stored));

    TEST_ASSERT_EQ(1, cdeque->count(deque, ds_cstdata(&probe)));
    TEST_ASSERT_TRUE(cdeque->find(deque, ds_cstdata(&probe)).cur
                     != cdeque->end(deque).cur);
    test_tracked_dispose(&probe);

    copies = test_tracked_copies();
    frees = test_tracked_frees();
    TEST_ASSERT_TRUE(cdeque->replace_index(deque, 0,
                                           ds_cstdata(&replacement)));
    test_tracked_dispose(&replacement);
    TEST_ASSERT_EQ(copies + 1, test_tracked_copies());
    TEST_ASSERT_EQ(frees + 1, test_tracked_frees());
    stored = test_tracked_slot_at(cdeque->at(deque, 0).u8);
    TEST_ASSERT_EQ(44, test_tracked_payload_at(stored));
    TEST_ASSERT_NULL(deque_check_invariants(deque));

    TEST_ASSERT_EQ(2, cdeque->clear(deque));
    TEST_ASSERT_EQ(0, test_tracked_live());
}

#ifndef JDSC_DEBUG
TEST_CASE(unit, deque_constructor_rejects_invalid)
{
    TEST_ASSERT_NULL(__deque_new(NULL, SEQ_CNTR_DTYPE_S32, 0, 1));
    TEST_ASSERT_NULL(__deque_new(NULL, SEQ_CNTR_DTYPE_S32,
                                 sizeof(int32_t), 3));
    TEST_ASSERT_NULL(__deque_new(NULL, SEQ_CNTR_DTYPE_S32,
                                 sizeof(int32_t), sizeof(int32_t) * 2 + 1));
    TEST_ASSERT_NULL(__deque_new(NULL, (deque_dtype_t)0,
                                 sizeof(int32_t), sizeof(int32_t)));
    TEST_ASSERT_NULL(__deque_new(NULL, SEQ_CNTR_DTYPE_S32, SIZE_MAX, 1));
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

static const class_deque_ops_t wide_ops = {
    .valid_data = wide_valid,
    .copy_data = wide_copy,
    .free_data = wide_free,
};
#endif /* JDSC_ALIGNMEM */

TEST_CASE(unit, deque_align_config)
{
#if JDSC_ALIGNMEM
    deque_t* deque = DEQUE_NEW_COMPLATE(&wide_ops, SEQ_CNTR_DTYPE_STRUCT,
                                        wide_element_t);
    wide_element_t element;
    deque_iterator_t it;
    deque_iterator_t end;
    size_t i;

    TEST_ASSERT_NOT_NULL(deque);
    test_cleanup_push(deque_free, deque);
    memset(&element, 0x5a, sizeof(element));
    for (i = 0; i < 100; ++i) {
        element.bytes[0] = (unsigned char)i;
        TEST_ASSERT_TRUE(cdeque->push_back(deque, ds_cstdata(&element)));
    }

    end = cdeque->end(deque);
    for (it = cdeque->begin(deque); it.cur != end.cur;
         it = cdeque->next(it))
        TEST_ASSERT_EQ(0, (uintptr_t)it.cur % 64);
    TEST_ASSERT_NULL(deque_check_invariants(deque));
#else
    TEST_SKIP("Aligned allocation is disabled in this configuration.");
#endif
}
