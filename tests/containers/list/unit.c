/*
  libj_ds permanent tests list unit semantics
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

#include <stdint.h>
#include <string.h>

#include "../../core/test.h"
#include "value.h"

static void list_free(void* self)
{
    list_t* list = self;

    LIST_DELETE(&list);
}

static list_t* open_int32(void)
{
    list_t* list = LIST_NEW_T(int32_t);

    TEST_ASSERT_NOT_NULL(list);
    test_cleanup_push(list_free, list);
    TEST_ASSERT_NULL(list_check_invariants(list));
    return list;
}

static void push_all(list_t* list, int32_t first, size_t count)
{
    size_t i;

    for (i = 0; i < count; ++i)
        TEST_ASSERT_TRUE(clist->push_back(list,
                                          __ds_s32data(first + (int32_t)i)));
}

static void expect_values(const list_t* list, const int32_t* values,
                          size_t count)
{
    list_iterator_t it;
    list_iterator_t end;
    list_r_iterator_t rit;
    list_r_iterator_t rend;
    size_t i;

    TEST_ASSERT_NULL(list_check_invariants(list));
    TEST_ASSERT_EQ(count, (size_t)clist->size(list));

    end = clist->end(list);
    i = 0;
    for (it = clist->begin(list); it.d != end.d; it = clist->next(it)) {
        TEST_ASSERT_TRUE(i < count);
        TEST_ASSERT_EQ(values[i], it_s32data(it));
        ++i;
    }
    TEST_ASSERT_EQ(count, i);

    rend = clist->rend(list);
    i = 0;
    for (rit = clist->rbegin(list); rit.d != rend.d;
         rit = clist->rnext(rit)) {
        TEST_ASSERT_TRUE(i < count);
        TEST_ASSERT_EQ(values[count - 1 - i], it_s32data(rit));
        ++i;
    }
    TEST_ASSERT_EQ(count, i);
}

static uint8_t* address_at(const list_t* list, size_t index)
{
    return (uint8_t*)clist->it(list, index).d;
}

static size_t address_index(const list_t* list, const void* address)
{
    list_iterator_t it;
    list_iterator_t end = clist->end(list);
    size_t index = 0;

    for (it = clist->begin(list); it.d != end.d; it = clist->next(it)) {
        if ((const void*)it.d == address)
            return index;
        ++index;
    }
    return index;
}

static bool less_by_key(ds_data_t left, ds_data_t right)
{
    return __ds_token_to_s32(left) / 1000 < __ds_token_to_s32(right) / 1000;
}

TEST_CASE(unit, list_lifecycle)
{
    list_t* list = open_int32();
    list_iterator_t end;
    const int32_t values[] = { 1, 2, 3, 4, 5 };

    TEST_ASSERT_EQ(0, clist->size(list));
    TEST_ASSERT_TRUE(__clist_empty(list));
    TEST_ASSERT_EQ(0, clist->count(list, __ds_s32data(3)));
    TEST_ASSERT_NULL(clist->at(list, 0).d);
    TEST_ASSERT_NULL(clist->it(list, 0).d);
    TEST_ASSERT_EQ(0, clist->clear(list));

    end = clist->end(list);
    TEST_ASSERT_PTR_EQ(end.d, clist->begin(list).d);
    TEST_ASSERT_PTR_EQ(end.d, clist->rend(list).d);
    TEST_ASSERT_PTR_EQ(end.d, clist->rbegin(list).d);
    TEST_ASSERT_PTR_EQ(end.d, clist->next(end).d);
    TEST_ASSERT_PTR_EQ(end.d, clist->prev(end).d);

    push_all(list, 1, 5);
    TEST_ASSERT_FALSE(__clist_empty(list));
    TEST_ASSERT_EQ(5, clist->size(list));
    TEST_ASSERT_EQ(1, *clist->front(list).s32);
    TEST_ASSERT_EQ(5, *clist->back(list).s32);
    TEST_ASSERT_PTR_EQ(clist->it(list, 0).d, clist->begin(list).d);
    TEST_ASSERT_PTR_EQ(clist->it(list, 4).d, clist->rbegin(list).d);
    expect_values(list, values, 5);

    TEST_ASSERT_TRUE(clist->push_front(list, __ds_s32data(0)));
    TEST_ASSERT_EQ(0, *clist->front(list).s32);
    TEST_ASSERT_EQ(6, clist->size(list));
    TEST_ASSERT_EQ(1, clist->count(list, __ds_s32data(2)));
    TEST_ASSERT_TRUE(clist->push_front(list, __ds_s32data(2)));
    TEST_ASSERT_EQ(2, clist->count(list, __ds_s32data(2)));

    TEST_ASSERT_EQ(7, clist->clear(list));
    TEST_ASSERT_EQ(0, clist->size(list));
    TEST_ASSERT_TRUE(__clist_empty(list));
    TEST_ASSERT_TRUE(clist->push_back(list, __ds_s32data(9)));
    TEST_ASSERT_EQ(9, *clist->front(list).s32);
    TEST_ASSERT_EQ(9, *clist->at(list, 0).s32);
    TEST_ASSERT_NULL(list_check_invariants(list));
}

TEST_CASE(unit, list_iterator_boundaries)
{
    list_t* list = open_int32();
    list_iterator_t begin;
    list_iterator_t end;
    list_r_iterator_t rbegin;
    list_r_iterator_t rend;
    const int32_t values[] = { 1, 2, 3 };

    push_all(list, 1, 3);
    begin = clist->begin(list);
    end = clist->end(list);
    rbegin = clist->rbegin(list);
    rend = clist->rend(list);

    TEST_ASSERT_PTR_EQ(end.d, clist->prev(begin).d);
    TEST_ASSERT_PTR_EQ(begin.d, clist->next(clist->prev(begin)).d);
    TEST_ASSERT_PTR_EQ(clist->it(list, 2).d, clist->prev(end).d);
    TEST_ASSERT_PTR_EQ(end.d, clist->next(clist->it(list, 2)).d);
    TEST_ASSERT_PTR_EQ(rend.d, clist->rprev(rbegin).d);
    TEST_ASSERT_PTR_EQ(clist->it(list, 2).d, clist->rbegin(list).d);
    TEST_ASSERT_PTR_EQ(clist->it(list, 0).d, clist->rprev(rend).d);

    TEST_ASSERT_TRUE(clist->next(i_dlist_null_iterator()).d == NULL);
    TEST_ASSERT_TRUE(clist->prev(i_dlist_null_iterator()).d == NULL);
    TEST_ASSERT_TRUE(clist->rnext(i_dlist_null_r_iterator()).d == NULL);
    TEST_ASSERT_TRUE(clist->rprev(i_dlist_null_r_iterator()).d == NULL);
    expect_values(list, values, 3);
}

TEST_CASE(unit, list_push_pop_both_ends)
{
    list_t* list = open_int32();
    int32_t values[8];
    size_t size = 0;
    size_t i;

    for (i = 0; i < 4; ++i) {
        TEST_ASSERT_TRUE(clist->push_back(list, __ds_s32data((int32_t)i)));
        values[size++] = (int32_t)i;
    }
    for (i = 0; i < 4; ++i) {
        TEST_ASSERT_TRUE(clist->push_front(list, __ds_s32data((int32_t)(i + 10))));
        memmove(values + 1, values, size * sizeof(values[0]));
        values[0] = (int32_t)(i + 10);
        ++size;
    }
    expect_values(list, values, size);

    clist->pop_front(list);
    clist->pop_front(list);
    clist->pop_back(list);
    memmove(values, values + 2, (size - 2) * sizeof(values[0]));
    size -= 3;
    expect_values(list, values, size);
    TEST_ASSERT_EQ(values[0], *clist->front(list).s32);
    TEST_ASSERT_EQ(values[size - 1], *clist->back(list).s32);

    while (size > 0) {
        clist->pop_front(list);
        --size;
    }
    TEST_ASSERT_EQ(0, clist->size(list));
    TEST_ASSERT_TRUE(__clist_empty(list));
    TEST_ASSERT_TRUE(clist->push_back(list, __ds_s32data(42)));
    TEST_ASSERT_TRUE(clist->push_front(list, __ds_s32data(43)));
    TEST_ASSERT_EQ(2, clist->size(list));
    TEST_ASSERT_NULL(list_check_invariants(list));
}

TEST_CASE(unit, list_insert_returns)
{
    list_t* list = open_int32();
    list_t* source = open_int32();
    list_iterator_t it;
    int32_t expected[64];
    size_t size = 0;
    size_t i;

    push_all(list, 1, 5);
    for (i = 0; i < 5; ++i)
        expected[size++] = (int32_t)(i + 1);

    it = clist->insert(list, clist->it(list, 2), __ds_s32data(9));
    TEST_ASSERT_EQ(9, it_s32data(it));
    memmove(expected + 3, expected + 2, 3 * sizeof(expected[0]));
    expected[2] = 9;
    ++size;
    expect_values(list, expected, size);

    it = clist->insert(list, clist->begin(list), __ds_s32data(0));
    TEST_ASSERT_EQ(0, it_s32data(it));
    memmove(expected + 1, expected, size * sizeof(expected[0]));
    expected[0] = 0;
    ++size;

    it = clist->insert(list, clist->end(list), __ds_s32data(6));
    TEST_ASSERT_EQ(6, it_s32data(it));
    expected[size++] = 6;

    it = clist->insert_n(list, clist->it(list, 3), 2, __ds_s32data(8));
    TEST_ASSERT_EQ(8, it_s32data(it));
    memmove(expected + 5, expected + 3, (size - 3) * sizeof(expected[0]));
    expected[3] = expected[4] = 8;
    size += 2;

    it = clist->insert_n(list, clist->end(list), 0, __ds_s32data(7));
    TEST_ASSERT_PTR_EQ(clist->end(list).d, it.d);

    it = clist->insert_n(list, clist->end(list), 2, __ds_s32data(7));
    TEST_ASSERT_EQ(7, it_s32data(it));
    expected[size++] = 7;
    expected[size++] = 7;
    expect_values(list, expected, size);

    TEST_ASSERT_TRUE(clist->push_back(source, __ds_s32data(10)));
    TEST_ASSERT_TRUE(clist->push_back(source, __ds_s32data(20)));
    it = clist->insert_range(list, clist->it(list, 2), clist->begin(source),
                             clist->end(source));
    TEST_ASSERT_EQ(10, it_s32data(it));
    memmove(expected + 4, expected + 2, (size - 2) * sizeof(expected[0]));
    expected[2] = 10;
    expected[3] = 20;
    size += 2;
    expect_values(list, expected, size);

    it = clist->insert_range(list, clist->it(list, 4), clist->begin(source),
                             clist->begin(source));
    TEST_ASSERT_PTR_EQ(clist->it(list, 4).d, it.d);
    expect_values(list, expected, size);

    TEST_ASSERT_TRUE(clist->assign_range(list, clist->begin(source),
                                         clist->end(source)));
    expected[0] = 10;
    expected[1] = 20;
    size = 2;
    expect_values(list, expected, size);

    TEST_ASSERT_TRUE(clist->assign_range(list, clist->begin(source),
                                         clist->begin(source)));
    TEST_ASSERT_EQ(0, clist->size(list));
    TEST_ASSERT_NULL(list_check_invariants(list));
}

TEST_CASE(unit, list_erase_returns)
{
    list_t* list = open_int32();
    list_iterator_t it;
    uint8_t* kept;
    const int32_t after_begin[] = { 2, 3, 4, 5 };
    const int32_t after_middle[] = { 2, 3, 5 };
    const int32_t after_last[] = { 2, 3 };
    const int32_t after_one[] = { 2 };

    push_all(list, 1, 5);
    kept = address_at(list, 4);

    it = clist->erase(list, clist->begin(list));
    TEST_ASSERT_EQ(2, it_s32data(it));
    expect_values(list, after_begin, 4);
    TEST_ASSERT_PTR_EQ(kept, address_at(list, 3));

    it = clist->erase(list, clist->it(list, 2));
    TEST_ASSERT_EQ(5, it_s32data(it));
    expect_values(list, after_middle, 3);

    it = clist->erase(list, clist->it(list, 2));
    TEST_ASSERT_PTR_EQ(clist->end(list).d, it.d);
    expect_values(list, after_last, 2);

    it = clist->erase_range(list, clist->begin(list), clist->begin(list));
    TEST_ASSERT_PTR_EQ(clist->begin(list).d, it.d);
    expect_values(list, after_last, 2);

    it = clist->erase_range(list, clist->it(list, 1), clist->end(list));
    TEST_ASSERT_PTR_EQ(clist->end(list).d, it.d);
    expect_values(list, after_one, 1);

    it = clist->erase_range(list, clist->begin(list), clist->end(list));
    TEST_ASSERT_PTR_EQ(clist->end(list).d, it.d);
    TEST_ASSERT_EQ(0, clist->size(list));

    TEST_ASSERT_TRUE(clist->push_back(list, __ds_s32data(42)));
    TEST_ASSERT_EQ(1, clist->size(list));
    TEST_ASSERT_NULL(list_check_invariants(list));
}

TEST_CASE(unit, list_resize_and_assign)
{
    list_t* list = open_int32();
    list_t* source = open_int32();
    const int32_t resized[] = { 1, 2, 3, 7, 7, 7 };
    const int32_t assigned[] = { 9, 9 };
    const int32_t shortened[] = { 1, 2, 3 };
    const int32_t range[] = { 4, 5, 6 };

    TEST_ASSERT_TRUE(clist->resize(list, 3, __ds_s32data(0)));
    TEST_ASSERT_EQ(3, clist->size(list));
    TEST_ASSERT_EQ(0, *clist->at(list, 0).s32);
    TEST_ASSERT_EQ(0, *clist->at(list, 2).s32);

    TEST_ASSERT_TRUE(clist->assign(list, 3, __ds_s32data(0)));
    TEST_ASSERT_TRUE(clist->replace_index(list, 0, __ds_s32data(1)));
    TEST_ASSERT_TRUE(clist->replace_index(list, 1, __ds_s32data(2)));
    TEST_ASSERT_TRUE(clist->replace_index(list, 2, __ds_s32data(3)));

    TEST_ASSERT_TRUE(clist->resize(list, 6, __ds_s32data(7)));
    expect_values(list, resized, 6);

    TEST_ASSERT_TRUE(clist->resize(list, 3, __ds_s32data(0)));
    expect_values(list, shortened, 3);
    TEST_ASSERT_TRUE(clist->resize(list, 3, __ds_s32data(0)));
    expect_values(list, shortened, 3);

    TEST_ASSERT_TRUE(clist->assign(list, 2, __ds_s32data(9)));
    expect_values(list, assigned, 2);
    TEST_ASSERT_FALSE(clist->assign(list, -1, __ds_s32data(9)));
    TEST_ASSERT_FALSE(clist->resize(list, -1, __ds_s32data(9)));

    TEST_ASSERT_TRUE(clist->resize(list, 0, __ds_s32data(0)));
    TEST_ASSERT_EQ(0, clist->size(list));
    TEST_ASSERT_TRUE(clist->push_back(list, __ds_s32data(9)));

    TEST_ASSERT_TRUE(clist->push_back(source, __ds_s32data(range[0])));
    TEST_ASSERT_TRUE(clist->push_back(source, __ds_s32data(range[1])));
    TEST_ASSERT_TRUE(clist->push_back(source, __ds_s32data(range[2])));
    TEST_ASSERT_TRUE(clist->assign_range(list, clist->begin(source),
                                         clist->end(source)));
    expect_values(list, range, 3);
    TEST_ASSERT_TRUE(clist->assign_range(list, clist->begin(source),
                                         clist->begin(source)));
    TEST_ASSERT_EQ(0, clist->size(list));
    TEST_ASSERT_NULL(list_check_invariants(list));
}

TEST_CASE(unit, list_find_count_remove)
{
    list_t* list = open_int32();
    const int32_t values[] = { 1, 2, 2, 3, 2, 4 };
    const int32_t after[] = { 1, 3, 4 };
    size_t i;

    for (i = 0; i < sizeof(values) / sizeof(values[0]); ++i)
        TEST_ASSERT_TRUE(clist->push_back(list, __ds_s32data(values[i])));

    TEST_ASSERT_PTR_EQ(clist->it(list, 1).d,
                       clist->find(list, __ds_s32data(2)).d);
    TEST_ASSERT_PTR_EQ(clist->end(list).d,
                       clist->find(list, __ds_s32data(9)).d);
    TEST_ASSERT_PTR_EQ(clist->it(list, 5).d,
                       clist->find(list, __ds_s32data(4)).d);
    TEST_ASSERT_EQ(3, clist->count(list, __ds_s32data(2)));
    TEST_ASSERT_EQ(0, clist->count(list, __ds_s32data(9)));
    TEST_ASSERT_EQ(3, clist->remove(list, __ds_s32data(2)));
    expect_values(list, after, 3);
    TEST_ASSERT_EQ(0, clist->remove(list, __ds_s32data(2)));
    TEST_ASSERT_NULL(list_check_invariants(list));
}

static bool remove_even(list_data_t data)
{
    return (__ds_token_to_s32(data) & 1) == 0;
}

static bool remove_same_parity(list_data_t left, list_data_t right)
{
    return (__ds_token_to_s32(left) & 1) == (__ds_token_to_s32(right) & 1);
}

TEST_CASE(unit, list_remove_if)
{
    list_t* list = open_int32();
    const int32_t values[] = { 2, 4, 5, 6, 7 };
    const int32_t left[] = { 5, 7 };
    size_t i;

    for (i = 0; i < sizeof(values) / sizeof(values[0]); ++i)
        TEST_ASSERT_TRUE(clist->push_back(list, __ds_s32data(values[i])));

    TEST_ASSERT_EQ(3, clist->remove_if(list, remove_even));
    expect_values(list, left, 2);
    TEST_ASSERT_EQ(0, clist->remove_if(list, remove_even));
    TEST_ASSERT_NULL(list_check_invariants(list));
}

TEST_CASE(unit, list_replace_keeps_iterator)
{
    list_t* list = open_int32();
    list_iterator_t it;
    uint8_t* address;

    push_all(list, 1, 5);

    TEST_ASSERT_TRUE(clist->replace_index(list, 0, __ds_s32data(10)));
    TEST_ASSERT_EQ(10, *clist->at(list, 0).s32);
    TEST_ASSERT_FALSE(clist->replace_index(list, 5, __ds_s32data(11)));
    TEST_ASSERT_FALSE(clist->replace_index(list, (list_index_t)-1,
                                           __ds_s32data(11)));
    TEST_ASSERT_EQ(5, clist->size(list));

    address = address_at(list, 2);
    it = clist->it(list, 2);
    TEST_ASSERT_TRUE(clist->replace_it(list, it, __ds_s32data(30)));
    TEST_ASSERT_PTR_EQ(address, it.d);
    TEST_ASSERT_PTR_EQ(address, address_at(list, 2));
    TEST_ASSERT_EQ(30, it_s32data(it));
    TEST_ASSERT_EQ(30, it_s32data(clist->next(clist->prev(it))));
    TEST_ASSERT_FALSE(clist->replace_it(list, clist->end(list),
                                        __ds_s32data(31)));
    TEST_ASSERT_EQ(5, clist->size(list));

    it = clist->find(list, __ds_s32data(30));
    TEST_ASSERT_PTR_EQ(address, it.d);
    it = clist->find(list, __ds_s32data(99));
    TEST_ASSERT_PTR_EQ(clist->end(list).d, it.d);
    TEST_ASSERT_EQ(0, clist->count(list, __ds_s32data(99)));
    TEST_ASSERT_NULL(list_check_invariants(list));
}

TEST_CASE(unit, list_swap_plain)
{
    list_t* first = open_int32();
    list_t* second = open_int32();
    list_iterator_t it;
    uint8_t* address;
    const int32_t first_values[] = { 1, 2 };
    const int32_t second_values[] = { 8, 9, 10 };

    push_all(first, 1, 2);
    push_all(second, 8, 3);
    address = address_at(first, 1);
    it = clist->it(first, 1);

    clist->swap(first, second);
    expect_values(first, second_values, 3);
    expect_values(second, first_values, 2);
    TEST_ASSERT_PTR_EQ(address, it.d);
    TEST_ASSERT_EQ(2, it_s32data(it));
    TEST_ASSERT_PTR_EQ(address, address_at(second, 1));

    clist->swap(first, first);
    expect_values(first, second_values, 3);
    TEST_ASSERT_NULL(list_check_invariants(first));
    TEST_ASSERT_NULL(list_check_invariants(second));
}

TEST_CASE(unit, list_node_address_stability)
{
    list_t* list = open_int32();
    uint8_t* addresses[100];
    uint8_t* current[100];
    size_t i;

    push_all(list, 0, 100);
    for (i = 0; i < 100; ++i)
        addresses[i] = address_at(list, i);

    TEST_ASSERT_TRUE(clist->insert(list, clist->it(list, 50),
                                   __ds_s32data(-1)).d != NULL);
    for (i = 0; i < 50; ++i)
        TEST_ASSERT_PTR_EQ(addresses[i], address_at(list, i));
    for (i = 50; i < 100; ++i)
        TEST_ASSERT_PTR_EQ(addresses[i], address_at(list, i + 1));

    TEST_ASSERT_TRUE(clist->erase(list, clist->it(list, 10)).d != NULL);
    for (i = 0; i < 10; ++i)
        TEST_ASSERT_PTR_EQ(addresses[i], address_at(list, i));
    for (i = 11; i < 50; ++i)
        TEST_ASSERT_PTR_EQ(addresses[i], address_at(list, i - 1));
    for (i = 50; i < 100; ++i)
        TEST_ASSERT_PTR_EQ(addresses[i], address_at(list, i));
    for (i = 0; i < 100; ++i)
        current[i] = address_at(list, i);

    clist->reverse(list);
    for (i = 0; i < 100; ++i)
        TEST_ASSERT_PTR_EQ(current[99 - i], address_at(list, i));

    clist->sort(list, __list_lt_default);
    for (i = 0; i < 100; ++i) {
        uint8_t* sorted = address_at(list, i);
        size_t j;
        bool found = false;

        for (j = 0; j < 100; ++j) {
            if (sorted == current[j]) {
                found = true;
                break;
            }
        }
        TEST_ASSERT_TRUE(found);
    }
    TEST_ASSERT_NULL(list_check_invariants(list));
}

TEST_CASE(unit, list_splice)
{
    list_t* target = open_int32();
    list_t* other = open_int32();
    list_t* empty = open_int32();
    const int32_t merged[] = { 1, 10, 11, 12, 2, 3 };
    const int32_t appended[] = { 1, 10, 11, 12, 2, 3, 20 };
    uint8_t* moved;

    push_all(target, 1, 3);
    push_all(other, 10, 3);
    moved = address_at(other, 0);

    clist->splice(target, clist->it(target, 1), other);
    expect_values(target, merged, 6);
    TEST_ASSERT_EQ(0, clist->size(other));
    TEST_ASSERT_TRUE(__clist_empty(other));
    TEST_ASSERT_EQ(1, address_index(target, moved));

    TEST_ASSERT_TRUE(clist->push_back(other, __ds_s32data(20)));
    clist->splice(target, clist->end(target), other);
    expect_values(target, appended, 7);
    TEST_ASSERT_EQ(0, clist->size(other));

    clist->splice(target, clist->begin(target), empty);
    expect_values(target, appended, 7);
    clist->splice(empty, clist->end(empty), target);
    TEST_ASSERT_EQ(0, clist->size(target));
    expect_values(empty, appended, 7);

    clist->splice(empty, clist->begin(empty), empty);
    expect_values(empty, appended, 7);
    TEST_ASSERT_NULL(list_check_invariants(target));
    TEST_ASSERT_NULL(list_check_invariants(empty));
}

TEST_CASE(unit, list_splice_range)
{
    list_t* target = open_int32();
    list_t* other = open_int32();
    list_t* self = open_int32();
    const int32_t spliced[] = { 1, 11, 12, 2 };
    const int32_t other_left[] = { 10, 13 };
    const int32_t self_middle[] = { 0, 3, 4, 1, 2, 5 };
    const int32_t self_tail[] = { 0, 1, 2, 3, 4, 5 };
    const size_t order[] = { 0, 3, 4, 1, 2, 5 };
    uint8_t* addresses[6];
    size_t i;

    push_all(target, 1, 2);
    push_all(other, 10, 4);

    clist->splice_range(target, clist->it(target, 1), other,
                        clist->it(other, 1), clist->it(other, 3));
    expect_values(target, spliced, 4);
    expect_values(other, other_left, 2);

    clist->splice_range(target, clist->end(target), other, clist->begin(other),
                        clist->begin(other));
    expect_values(target, spliced, 4);
    expect_values(other, other_left, 2);

    push_all(self, 0, 6);
    for (i = 0; i < 6; ++i)
        addresses[i] = address_at(self, i);

    clist->splice_range(self, clist->it(self, 5), self, clist->it(self, 1),
                        clist->it(self, 3));
    expect_values(self, self_middle, 6);
    for (i = 0; i < 6; ++i)
        TEST_ASSERT_PTR_EQ(address_at(self, i), addresses[order[i]]);

    clist->splice_range(self, clist->it(self, 1), self, clist->it(self, 3),
                        clist->it(self, 5));
    expect_values(self, self_tail, 6);
    for (i = 0; i < 6; ++i)
        TEST_ASSERT_PTR_EQ(address_at(self, i), addresses[i]);
    TEST_ASSERT_NULL(list_check_invariants(target));
    TEST_ASSERT_NULL(list_check_invariants(other));
    TEST_ASSERT_NULL(list_check_invariants(self));
}

TEST_CASE(unit, list_merge)
{
    list_t* left = open_int32();
    list_t* right = open_int32();
    list_t* appended = open_int32();
    const int32_t merged[] = { 1, 2, 3, 4, 5, 6 };
    const int32_t appended_values[] = { 1, 2, 3, 4, 5, 6, 7, 8 };
    uint8_t* moved;

    push_all(left, 1, 1);
    TEST_ASSERT_TRUE(clist->push_back(left, __ds_s32data(3)));
    TEST_ASSERT_TRUE(clist->push_back(left, __ds_s32data(5)));
    push_all(right, 2, 1);
    TEST_ASSERT_TRUE(clist->push_back(right, __ds_s32data(4)));
    TEST_ASSERT_TRUE(clist->push_back(right, __ds_s32data(6)));
    moved = address_at(right, 0);

    clist->merge(left, right, __list_lt_default);
    expect_values(left, merged, 6);
    TEST_ASSERT_EQ(0, clist->size(right));
    TEST_ASSERT_EQ(1, address_index(left, moved));

    push_all(appended, 7, 2);
    clist->merge(left, appended, NULL);
    expect_values(left, appended_values, 8);
    TEST_ASSERT_EQ(0, clist->size(appended));

    clist->merge(left, appended, __list_lt_default);
    expect_values(left, appended_values, 8);
    TEST_ASSERT_NULL(list_check_invariants(left));
    TEST_ASSERT_NULL(list_check_invariants(right));
    TEST_ASSERT_NULL(list_check_invariants(appended));
}

TEST_CASE(unit, list_sort_reverse)
{
    list_t* list = open_int32();
    list_t* stable = open_int32();
    const size_t count = 200;
    const size_t stable_count = 300;
    int32_t previous;
    int32_t previous_key;
    int32_t previous_seq;
    size_t i;

    clist->sort(list, __list_lt_default);
    clist->reverse(list);
    TEST_ASSERT_EQ(0, clist->size(list));
    TEST_ASSERT_TRUE(clist->push_back(list, __ds_s32data(5)));
    clist->sort(list, __list_lt_default);
    clist->reverse(list);
    TEST_ASSERT_EQ(5, *clist->front(list).s32);

    for (i = 0; i < count; ++i)
        TEST_ASSERT_TRUE(clist->push_back(list,
                                          __ds_s32data((int32_t)((i * 37) % 101) - 50)));
    clist->sort(list, __list_lt_default);
    TEST_ASSERT_TRUE(clist_sorted_default(list, true));
    previous = *clist->front(list).s32;
    for (i = 1; i < count; ++i) {
        int32_t value = *clist->at(list, i).s32;

        TEST_ASSERT_TRUE(value >= previous);
        previous = value;
    }

    clist->sort(list, __list_gt_default);
    TEST_ASSERT_TRUE(clist_sorted_default(list, false));
    clist->reverse(list);
    TEST_ASSERT_TRUE(clist_sorted_default(list, true));
    clist->reverse(list);
    clist->reverse(list);
    TEST_ASSERT_TRUE(clist_sorted_default(list, true));

    for (i = 0; i < stable_count; ++i) {
        int32_t key = (int32_t)test_rng_bounded(5);
        int32_t value = key * 1000 + (int32_t)i;

        TEST_ASSERT_TRUE(clist->push_back(stable, __ds_s32data(value)));
    }
    clist->sort(stable, less_by_key);
    previous_key = -1;
    previous_seq = -1;
    for (i = 0; i < stable_count; ++i) {
        int32_t value = it_s32data(clist->it(stable, i));
        int32_t key = value / 1000;
        int32_t seq = value % 1000;

        TEST_ASSERT_TRUE(key >= previous_key);
        if (key == previous_key)
            TEST_ASSERT_TRUE(seq > previous_seq);
        previous_key = key;
        previous_seq = seq;
    }
    TEST_ASSERT_NULL(list_check_invariants(list));
    TEST_ASSERT_NULL(list_check_invariants(stable));
}

TEST_CASE(unit, list_merge_stability)
{
    list_t* left = open_int32();
    list_t* right = open_int32();
    list_iterator_t it;
    list_iterator_t end;
    int32_t previous_key = -1;
    int32_t previous_seq = -1;
    int32_t key;
    int32_t seq;

    TEST_ASSERT_TRUE(clist->push_back(left, __ds_s32data(0 * 1000 + 0)));
    TEST_ASSERT_TRUE(clist->push_back(left, __ds_s32data(1 * 1000 + 1)));
    TEST_ASSERT_TRUE(clist->push_back(left, __ds_s32data(1 * 1000 + 2)));
    TEST_ASSERT_TRUE(clist->push_back(left, __ds_s32data(2 * 1000 + 3)));
    TEST_ASSERT_TRUE(clist->push_back(right, __ds_s32data(1 * 1000 + 100)));
    TEST_ASSERT_TRUE(clist->push_back(right, __ds_s32data(1 * 1000 + 101)));
    TEST_ASSERT_TRUE(clist->push_back(right, __ds_s32data(3 * 1000 + 102)));

    clist->merge(left, right, less_by_key);
    end = clist->end(left);
    for (it = clist->begin(left); it.d != end.d; it = clist->next(it)) {
        int32_t value = it_s32data(it);

        key = value / 1000;
        seq = value % 1000;
        TEST_ASSERT_TRUE(key >= previous_key);
        if (key == previous_key)
            TEST_ASSERT_TRUE(seq > previous_seq);
        previous_key = key;
        previous_seq = seq;
    }
    TEST_ASSERT_EQ(7, clist->size(left));
    TEST_ASSERT_EQ(0, clist->size(right));
    TEST_ASSERT_NULL(list_check_invariants(left));
}

TEST_CASE(unit, list_unique)
{
    list_t* list = open_int32();
    const int32_t values[] = { 1, 1, 2, 2, 2, 3, 1 };
    const int32_t unique[] = { 1, 2, 3, 1 };
    const int32_t adjacent[] = { 2, 4, 5, 7, 8 };
    const int32_t after_if[] = { 2, 5, 8 };
    size_t i;

    for (i = 0; i < sizeof(values) / sizeof(values[0]); ++i)
        TEST_ASSERT_TRUE(clist->push_back(list, __ds_s32data(values[i])));
    TEST_ASSERT_EQ(3, clist->unique(list));
    expect_values(list, unique, 4);
    TEST_ASSERT_EQ(0, clist->unique(list));

    TEST_ASSERT_EQ(4, clist->clear(list));
    for (i = 0; i < sizeof(adjacent) / sizeof(adjacent[0]); ++i)
        TEST_ASSERT_TRUE(clist->push_back(list, __ds_s32data(adjacent[i])));
    TEST_ASSERT_EQ(2, clist->unique_if(list, remove_same_parity));
    expect_values(list, after_if, 3);
    TEST_ASSERT_NULL(list_check_invariants(list));
}

TEST_CASE(unit, list_dtype_matrix)
{
    {
        list_t* list = LIST_NEW_T(uint8_t);

        TEST_ASSERT_NOT_NULL(list);
        test_cleanup_push(list_free, list);
        TEST_ASSERT_TRUE(clist->push_back(list, __ds_u8data(250)));
        TEST_ASSERT_EQ(250, *clist->at(list, 0).u8);
        TEST_ASSERT_EQ(1, clist->count(list, __ds_u8data(250)));
    }
    {
        list_t* list = LIST_NEW_T(uint16_t);

        TEST_ASSERT_NOT_NULL(list);
        test_cleanup_push(list_free, list);
        TEST_ASSERT_TRUE(clist->push_back(list, __ds_u16data(60000)));
        TEST_ASSERT_EQ(60000, *clist->at(list, 0).u16);
        TEST_ASSERT_EQ(1, clist->count(list, __ds_u16data(60000)));
    }
    {
        list_t* list = LIST_NEW_T(uint32_t);

        TEST_ASSERT_NOT_NULL(list);
        test_cleanup_push(list_free, list);
        TEST_ASSERT_TRUE(clist->push_back(list, __ds_u32data(4000000000u)));
        TEST_ASSERT_EQ(4000000000u, *clist->at(list, 0).u32);
        TEST_ASSERT_EQ(1, clist->count(list, __ds_u32data(4000000000u)));
    }
    {
        list_t* list = LIST_NEW_T(uint64_t);

        TEST_ASSERT_NOT_NULL(list);
        test_cleanup_push(list_free, list);
        TEST_ASSERT_TRUE(clist->push_back(list,
                                          ds_u64data(1234567890123ULL)));
        TEST_ASSERT_EQ(1234567890123LL, *clist->at(list, 0).u64);
        TEST_ASSERT_EQ(1, clist->count(list,
                                       ds_u64data(1234567890123ULL)));
    }
    {
        list_t* list = LIST_NEW_T(int8_t);

        TEST_ASSERT_NOT_NULL(list);
        test_cleanup_push(list_free, list);
        TEST_ASSERT_TRUE(clist->push_back(list, __ds_s8data(-100)));
        TEST_ASSERT_EQ(-100, *clist->at(list, 0).s8);
        TEST_ASSERT_EQ(1, clist->count(list, __ds_s8data(-100)));
    }
    {
        list_t* list = LIST_NEW_T(int16_t);

        TEST_ASSERT_NOT_NULL(list);
        test_cleanup_push(list_free, list);
        TEST_ASSERT_TRUE(clist->push_back(list, __ds_s16data(-30000)));
        TEST_ASSERT_EQ(-30000, *clist->at(list, 0).s16);
        TEST_ASSERT_EQ(1, clist->count(list, __ds_s16data(-30000)));
    }
    {
        list_t* list = LIST_NEW_T(int32_t);

        TEST_ASSERT_NOT_NULL(list);
        test_cleanup_push(list_free, list);
        TEST_ASSERT_TRUE(clist->push_back(list,
                                          __ds_s32data(-2000000000)));
        TEST_ASSERT_EQ(-2000000000, *clist->at(list, 0).s32);
    }
    {
        list_t* list = LIST_NEW_T(int64_t);

        TEST_ASSERT_NOT_NULL(list);
        test_cleanup_push(list_free, list);
        TEST_ASSERT_TRUE(clist->push_back(list,
                                          ds_s64data(-1234567890123LL)));
        TEST_ASSERT_EQ(-1234567890123LL, *clist->at(list, 0).s64);
        TEST_ASSERT_EQ(1, clist->count(list,
                                       ds_s64data(-1234567890123LL)));
    }
    {
        list_t* list = LIST_NEW_T(uintptr_t);

        TEST_ASSERT_NOT_NULL(list);
        test_cleanup_push(list_free, list);
        TEST_ASSERT_TRUE(clist->push_back(list, __ds_uptrdata(123456u)));
        TEST_ASSERT_EQ(123456, *clist->at(list, 0).d);
    }
    {
        list_t* list = LIST_NEW_T(intptr_t);

        TEST_ASSERT_NOT_NULL(list);
        test_cleanup_push(list_free, list);
        TEST_ASSERT_TRUE(clist->push_back(list, __ds_iptrdata(-12345)));
        TEST_ASSERT_EQ(-12345, *clist->at(list, 0).d);
    }
    {
        list_t* list = LIST_NEW_T(size_t);

        TEST_ASSERT_NOT_NULL(list);
        test_cleanup_push(list_free, list);
        TEST_ASSERT_TRUE(clist->push_back(list, __ds_sizedata(123456)));
        TEST_ASSERT_EQ(123456, *clist->at(list, 0).d);
    }
    {
        list_t* list = LIST_NEW_T(ssize_t);

        TEST_ASSERT_NOT_NULL(list);
        test_cleanup_push(list_free, list);
        TEST_ASSERT_TRUE(clist->push_back(list, __ds_ssizedata(-123456)));
        TEST_ASSERT_EQ(-123456, *clist->at(list, 0).d);
    }
    {
        list_t* list = LIST_NEW_T(float);

        TEST_ASSERT_NOT_NULL(list);
        test_cleanup_push(list_free, list);
        TEST_ASSERT_TRUE(clist->push_back(list, ds_fdata(1.5f)));
        TEST_ASSERT_TRUE(clist->push_back(list, ds_fdata(-0.0f)));
        TEST_ASSERT_EQ(1.5f, *clist->at(list, 0).f);
        TEST_ASSERT_EQ(1, clist->count(list, ds_fdata(0.0f)));
    }
    {
        list_t* list = LIST_NEW_T(double);

        TEST_ASSERT_NOT_NULL(list);
        test_cleanup_push(list_free, list);
        TEST_ASSERT_TRUE(clist->push_back(list, ds_lfdata(-2.25)));
        TEST_ASSERT_TRUE(clist->push_back(list, ds_lfdata(-0.0)));
        TEST_ASSERT_EQ(-2.25, *clist->at(list, 0).lf);
        TEST_ASSERT_EQ(1, clist->count(list, ds_lfdata(0.0)));
    }
}

TEST_CASE(unit, list_inline_strings)
{
    static const char* const short15 = "abcdefghijklmno";
    static const char* const long16 = "abcdefghijklmnop";
    static const char* const long45 = "this string is longer than the inline buffer";
    list_t* sso = LIST_NEW_SSO();
    list_t* chars = LIST_NEW_CHAR();
    list_t* dup = LIST_NEW_SSO();
    list_t* target = LIST_NEW_SSO();
    list_t* source = LIST_NEW_SSO();
    list_iterator_t it;

    TEST_ASSERT_NOT_NULL(sso);
    TEST_ASSERT_NOT_NULL(chars);
    TEST_ASSERT_NOT_NULL(dup);
    TEST_ASSERT_NOT_NULL(target);
    TEST_ASSERT_NOT_NULL(source);
    test_cleanup_push(list_free, sso);
    test_cleanup_push(list_free, chars);
    test_cleanup_push(list_free, dup);
    test_cleanup_push(list_free, target);
    test_cleanup_push(list_free, source);

    TEST_ASSERT_TRUE(clist->push_back(sso, ds_sdata(short15)));
    TEST_ASSERT_TRUE(clist->push_back(sso, ds_sdata(long16)));
    TEST_ASSERT_TRUE(clist->push_back(sso, ds_sdata(long45)));
    TEST_ASSERT_TRUE(clist->push_back(chars, ds_sdata(short15)));
    TEST_ASSERT_TRUE(clist->push_back(chars, ds_sdata(long16)));
    TEST_ASSERT_TRUE(clist->push_back(chars, ds_sdata(long45)));

    it = clist->begin(sso);
    TEST_ASSERT_EQ(15, it.ssd->len);
    TEST_ASSERT_TRUE(it.ssd->p == it.ssd->buf);
    TEST_ASSERT_STR_EQ(short15, it_sdata(it));
    it = clist->next(it);
    TEST_ASSERT_EQ(16, it.ssd->len);
    TEST_ASSERT_TRUE(it.ssd->p != it.ssd->buf);
    TEST_ASSERT_STR_EQ(long16, it_sdata(it));
    it = clist->next(it);
    TEST_ASSERT_EQ(44, it.ssd->len);
    TEST_ASSERT_STR_EQ(long45, it_sdata(it));

    it = clist->begin(chars);
    TEST_ASSERT_STR_EQ(short15, it_sdata(it));
    it = clist->next(it);
    TEST_ASSERT_STR_EQ(long16, it_sdata(it));
    it = clist->next(it);
    TEST_ASSERT_STR_EQ(long45, it_sdata(it));

    it = clist->find(sso, ds_sdata(long16));
    TEST_ASSERT_TRUE(it.d != clist->end(sso).d);
    TEST_ASSERT_STR_EQ(long16, it_sdata(it));
    TEST_ASSERT_EQ(1, clist->count(sso, ds_sdata(long45)));
    TEST_ASSERT_EQ(0, clist->count(chars, ds_sdata("missing")));

    TEST_ASSERT_EQ(1, clist->remove(sso, ds_sdata(long16)));
    TEST_ASSERT_EQ(2, clist->size(sso));
    TEST_ASSERT_EQ(1, clist->remove(chars, ds_sdata(long16)));
    TEST_ASSERT_EQ(2, clist->size(chars));

    TEST_ASSERT_TRUE(clist->replace_index(sso, 0, ds_sdata(long45)));
    TEST_ASSERT_EQ(44, clist->at(sso, 0).ssd->len);
    TEST_ASSERT_STR_EQ(long45, clist->at(sso, 0).ssd->p);
    it = clist->begin(sso);
    TEST_ASSERT_TRUE(clist->replace_it(sso, it, ds_sdata(it.ssd->p)));
    TEST_ASSERT_STR_EQ(long45, it_sdata(it));
    TEST_ASSERT_TRUE(clist->insert(sso, clist->begin(sso),
                                   ds_sdata(short15)).d != NULL);
    TEST_ASSERT_EQ(3, clist->size(sso));
    TEST_ASSERT_NULL(list_check_invariants(sso));

    TEST_ASSERT_TRUE(clist->push_back(dup, ds_sdata("aaaa")));
    TEST_ASSERT_TRUE(clist->push_back(dup, ds_sdata("aaaa")));
    TEST_ASSERT_TRUE(clist->push_back(dup, ds_sdata("bbbb")));
    TEST_ASSERT_EQ(1, clist->unique(dup));
    TEST_ASSERT_EQ(2, clist->size(dup));
    TEST_ASSERT_STR_EQ("aaaa", it_sdata(clist->begin(dup)));
    TEST_ASSERT_STR_EQ("bbbb", it_sdata(clist->next(clist->begin(dup))));
    TEST_ASSERT_NULL(list_check_invariants(dup));

    TEST_ASSERT_TRUE(clist->push_back(target, ds_sdata("tail")));
    TEST_ASSERT_TRUE(clist->push_back(source, ds_sdata(short15)));
    TEST_ASSERT_TRUE(clist->push_back(source, ds_sdata(long16)));
    clist->splice(target, clist->begin(target), source);
    TEST_ASSERT_EQ(3, clist->size(target));
    TEST_ASSERT_EQ(0, clist->size(source));
    TEST_ASSERT_STR_EQ(short15, it_sdata(clist->begin(target)));
    TEST_ASSERT_STR_EQ(long16,
                       it_sdata(clist->next(clist->begin(target))));
    TEST_ASSERT_NULL(list_check_invariants(target));

    TEST_ASSERT_EQ(2, clist->clear(chars));
    TEST_ASSERT_EQ(3, clist->clear(sso));
}

TEST_CASE(unit, list_sso_sort)
{
    static const char* const inputs[] = {
        "dddd", "bbbbbbbbbbbbbbbb", "aaaa", "cccccccccccccccccccccccc"
    };
    static const char* const ordered[] = {
        "aaaa", "bbbbbbbbbbbbbbbb", "cccccccccccccccccccccccc", "dddd"
    };
    list_t* list = LIST_NEW_SSO();
    size_t i;

    TEST_ASSERT_NOT_NULL(list);
    test_cleanup_push(list_free, list);

    for (i = 0; i < sizeof(inputs) / sizeof(inputs[0]); ++i)
        TEST_ASSERT_TRUE(clist->push_back(list, ds_sdata(inputs[i])));

    clist->sort(list, __list_lt_sso);
    TEST_ASSERT_TRUE(clist_sorted_sso(list, true));
    TEST_ASSERT_NULL(list_check_invariants(list));

    for (i = 0; i < sizeof(ordered) / sizeof(ordered[0]); ++i) {
        list_iterator_t it = clist->it(list, i);
        const ds_sso_t* value = it.ssd;

        TEST_ASSERT_STR_EQ(ordered[i], it_sdata(it));
        TEST_ASSERT_EQ(strlen(ordered[i]), value->len);
        TEST_ASSERT_TRUE((value->len <= __DS_SSO_LOCAL_CAP)
                         == (value->p == value->buf));
    }

    clist->sort(list, __list_gt_sso);
    TEST_ASSERT_TRUE(clist_sorted_sso(list, false));
    TEST_ASSERT_STR_EQ(ordered[3], it_sdata(clist->begin(list)));
    TEST_ASSERT_TRUE(clist->insert(list, clist->begin(list),
                                   ds_sdata("abcdefghijklmnop")).d != NULL);
    TEST_ASSERT_EQ(5, clist->size(list));
    TEST_ASSERT_STR_EQ("abcdefghijklmnop", it_sdata(clist->begin(list)));
    TEST_ASSERT_NULL(list_check_invariants(list));
}

TEST_CASE(unit, list_invariant_checker_negative)
{
    list_t* list = open_int32();
    list_t snapshot;

    push_all(list, 0, 5);
    TEST_ASSERT_NULL(list_check_invariants(list));

    snapshot = *list;
    snapshot.step = 0;
    TEST_ASSERT_NOT_NULL(list_check_invariants(&snapshot));

    snapshot = *list;
    snapshot.swap = NULL;
    TEST_ASSERT_NOT_NULL(list_check_invariants(&snapshot));

    snapshot = *list;
    snapshot.size = -1;
    TEST_ASSERT_NOT_NULL(list_check_invariants(&snapshot));

    snapshot = *list;
    snapshot.head.next = NULL;
    TEST_ASSERT_NOT_NULL(list_check_invariants(&snapshot));

    snapshot = *list;
    snapshot.size = 1;
    TEST_ASSERT_NOT_NULL(list_check_invariants(&snapshot));

    snapshot = *list;
    snapshot.sso = true;
    TEST_ASSERT_NOT_NULL(list_check_invariants(&snapshot));

    TEST_ASSERT_NULL(list_check_invariants(list));
}

TEST_CASE(unit, list_tracked_ops)
{
    list_t* list = LIST_NEW_COMPLATE(test_ops_tracked(), SEQ_CNTR_DTYPE_STRUCT,
                                     test_tracked_t);
    test_tracked_t first = test_tracked_make(11);
    test_tracked_t second = test_tracked_make(22);
    test_tracked_t probe = test_tracked_make(22);
    test_tracked_t replacement = test_tracked_make(44);
    const test_tracked_t* stored;
    size_t copies;
    size_t frees;

    TEST_ASSERT_NOT_NULL(list);
    test_cleanup_push(list_free, list);
    TEST_ASSERT_TRUE(test_tracked_valid_at(&first));
    TEST_ASSERT_TRUE(test_tracked_valid_at(&second));
    TEST_ASSERT_TRUE(test_tracked_valid_at(&probe));
    TEST_ASSERT_TRUE(test_tracked_valid_at(&replacement));

    TEST_ASSERT_TRUE(clist->push_back(list, ds_cstdata(&first)));
    test_tracked_dispose(&first);
    TEST_ASSERT_TRUE(clist->push_front(list, ds_cstdata(&second)));
    test_tracked_dispose(&second);

    stored = test_tracked_slot_at(clist->at(list, 0).u8);
    TEST_ASSERT_TRUE(test_tracked_valid_at(stored));
    TEST_ASSERT_EQ(22, test_tracked_payload_at(stored));
    stored = test_tracked_slot_at(clist->at(list, 1).u8);
    TEST_ASSERT_EQ(11, test_tracked_payload_at(stored));

    TEST_ASSERT_EQ(1, clist->count(list, ds_cstdata(&probe)));
    TEST_ASSERT_TRUE(clist->find(list, ds_cstdata(&probe)).d
                     != clist->end(list).d);
    test_tracked_dispose(&probe);

    copies = test_tracked_copies();
    frees = test_tracked_frees();
    TEST_ASSERT_TRUE(clist->replace_index(list, 0, ds_cstdata(&replacement)));
    test_tracked_dispose(&replacement);
    TEST_ASSERT_EQ(copies + 1, test_tracked_copies());
    TEST_ASSERT_EQ(frees + 1, test_tracked_frees());
    stored = test_tracked_slot_at(clist->at(list, 0).u8);
    TEST_ASSERT_EQ(44, test_tracked_payload_at(stored));
    TEST_ASSERT_NULL(list_check_invariants(list));

    TEST_ASSERT_EQ(2, clist->clear(list));
    TEST_ASSERT_EQ(0, test_tracked_live());
}

TEST_CASE(unit, list_stranger_iterator_guards)
{
    list_t* list = open_int32();
    list_iterator_t end;

    push_all(list, 1, 3);
    end = clist->end(list);

    TEST_ASSERT_TRUE(clist->erase(list, end).d == NULL);
    TEST_ASSERT_TRUE(clist->erase(list, i_dlist_null_iterator()).d == NULL);
    TEST_ASSERT_TRUE(clist->erase_range(list, end, end).d == end.d);
    TEST_ASSERT_FALSE(clist->replace_it(list, i_dlist_null_iterator(),
                                        __ds_s32data(7)));
    TEST_ASSERT_EQ(3, clist->size(list));
    TEST_ASSERT_NULL(list_check_invariants(list));
}

#ifndef JDSC_DEBUG
TEST_CASE(unit, list_constructor_rejects_invalid)
{
    TEST_ASSERT_NULL(__list_new(NULL, SEQ_CNTR_DTYPE_S32, 0, 0));
    TEST_ASSERT_NULL(__list_new(NULL, SEQ_CNTR_DTYPE_S32,
                                sizeof(int32_t), 3));
    TEST_ASSERT_NULL(__list_new(NULL, SEQ_CNTR_DTYPE_S32,
                                sizeof(int32_t), sizeof(int32_t) * 2));
    TEST_ASSERT_NULL(__list_new(NULL, SEQ_CNTR_DTYPE_S32,
                                sizeof(int32_t), (list_align_t)-1));
    TEST_ASSERT_NULL(__list_new(NULL, (list_dtype_t)0, sizeof(int32_t), 0));
    TEST_ASSERT_NULL(__list_new(g_class_list_ops_sso(), SEQ_CNTR_DTYPE_S32,
                                sizeof(int32_t), 0));
    TEST_ASSERT_NULL(__list_new(NULL, SEQ_CNTR_DTYPE_STRUCT,
                                sizeof(int32_t), 0));
}

TEST_CASE(unit, list_over_limit)
{
    list_t* list = open_int32();
    list_size_t max_size = clist->max_size(list);

    TEST_ASSERT_TRUE(max_size > 0);
    TEST_ASSERT_TRUE(clist->push_back(list, __ds_s32data(1)));
    TEST_ASSERT_TRUE(clist->insert_n(list, clist->end(list), -1,
                                     __ds_s32data(3)).d == NULL);
    TEST_ASSERT_FALSE(clist->assign(list, -1, __ds_s32data(2)));
    TEST_ASSERT_FALSE(clist->resize(list, -1, __ds_s32data(2)));
    TEST_ASSERT_EQ(1, clist->size(list));
    TEST_ASSERT_EQ(1, *clist->at(list, 0).s32);
    TEST_ASSERT_NULL(list_check_invariants(list));
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

static const class_list_ops_t wide_ops = {
    .valid_data = wide_valid,
    .copy_data = wide_copy,
    .free_data = wide_free,
};
#endif /* JDSC_ALIGNMEM */

TEST_CASE(unit, list_align_config)
{
#if JDSC_ALIGNMEM
    list_t* list = LIST_NEW_COMPLATE(&wide_ops, SEQ_CNTR_DTYPE_STRUCT,
                                     wide_element_t);
    wide_element_t element;
    list_iterator_t it;
    list_iterator_t end;
    size_t i;

    TEST_ASSERT_NOT_NULL(list);
    test_cleanup_push(list_free, list);
    memset(&element, 0x5a, sizeof(element));
    for (i = 0; i < 100; ++i) {
        element.bytes[0] = (unsigned char)i;
        TEST_ASSERT_TRUE(clist->push_back(list, ds_cstdata(&element)));
    }

    end = clist->end(list);
    for (it = clist->begin(list); it.d != end.d; it = clist->next(it))
        TEST_ASSERT_EQ(0, (uintptr_t)it.d % 64);
    TEST_ASSERT_NULL(list_check_invariants(list));
#else
    TEST_SKIP("Aligned allocation is disabled in this configuration.");
#endif
}
