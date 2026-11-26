/*
  libj_ds permanent tests map unit
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

#include <map/map.h>

#include <string.h>

#include "../../core/test.h"

static void map_free(void* self)
{
    map_t* map = self;

    MAP_DELETE(&map);
}

static bool map_at_end(const map_t* map, map_iterator_t it)
{
    return it_eq(it, cmap->end(map));
}

static void map_check_ascending(map_t* map)
{
    map_iterator_t it = cmap->begin(map);
    map_iterator_t end = cmap->end(map);
    int32_t expected_key = 1;
    int32_t expected_value = 10;

    while (it_ne(it, end)) {
        TEST_ASSERT_EQ(expected_key, it_s32key(it));
        TEST_ASSERT_EQ(expected_value, it_s32value(it));
        ++expected_key;
        expected_value += 10;
        it = cmap->next(it);
    }
    TEST_ASSERT_EQ(5, expected_key);
}

static void map_check_descending(map_t* map)
{
    map_r_iterator_t it = cmap->rbegin(map);
    map_r_iterator_t end = cmap->rend(map);
    int32_t expected_key = 4;
    int32_t expected_value = 40;

    while (it_ne(it, end)) {
        TEST_ASSERT_EQ(expected_key, it_s32key(it));
        TEST_ASSERT_EQ(expected_value, it_s32value(it));
        --expected_key;
        expected_value -= 10;
        it = cmap->rnext(it);
    }
    TEST_ASSERT_EQ(0, expected_key);
}

TEST_CASE(unit, map_ordered_basics)
{
    map_t* map = MAP_NEW_KV(int32_t, int32_t);
    map_iterator_t it;
    int32_t value = 0;

    TEST_ASSERT_NOT_NULL(map);
    test_cleanup_push(map_free, map);

    TEST_ASSERT_NOT_NULL(cmap->insert(map, __ds_s32key(3), __ds_s32value(30)).d);
    TEST_ASSERT_NOT_NULL(cmap->insert(map, __ds_s32key(1), __ds_s32value(10)).d);
    TEST_ASSERT_NOT_NULL(cmap->insert(map, __ds_s32key(2), __ds_s32value(20)).d);
    TEST_ASSERT_EQ(3, cmap->size(map));
    TEST_ASSERT_EQ(1, cmap->count(map, __ds_s32key(2)));

    TEST_ASSERT_NULL(cmap->insert(map, __ds_s32key(2), __ds_s32value(99)).d);
    TEST_ASSERT_EQ(3, cmap->size(map));
    it = cmap->find(map, __ds_s32key(2));
    TEST_ASSERT_FALSE(map_at_end(map, it));
    TEST_ASSERT_EQ(20, it_s32value(it));

    TEST_ASSERT_NOT_NULL(cmap->insert_replace(map, __ds_s32key(2),
                                              __ds_s32value(22)).d);
    it = cmap->find(map, __ds_s32key(2));
    TEST_ASSERT_FALSE(map_at_end(map, it));
    TEST_ASSERT_EQ(22, it_s32value(it));
    TEST_ASSERT_NOT_NULL(cmap->insert_replace(map, __ds_s32key(4),
                                              __ds_s32value(44)).d);
    TEST_ASSERT_EQ(4, cmap->size(map));

    value = it_s32value(cmap->lower_bound(map, __ds_s32key(0)));
    TEST_ASSERT_EQ(10, value);
    value = it_s32value(cmap->lower_bound(map, __ds_s32key(2)));
    TEST_ASSERT_EQ(22, value);
    value = it_s32value(cmap->upper_bound(map, __ds_s32key(2)));
    TEST_ASSERT_EQ(30, value);
    TEST_ASSERT_TRUE(map_at_end(map, cmap->lower_bound(map,
                                                       __ds_s32key(5))));
    TEST_ASSERT_TRUE(map_at_end(map, cmap->upper_bound(map,
                                                       __ds_s32key(4))));

    TEST_ASSERT_EQ(1, cmap->remove(map, __ds_s32key(2)));
    TEST_ASSERT_EQ(0, cmap->remove(map, __ds_s32key(2)));
    TEST_ASSERT_EQ(3, cmap->size(map));
    TEST_ASSERT_EQ(3, cmap->clear(map));
    TEST_ASSERT_EQ(0, cmap->size(map));

    TEST_ASSERT_NOT_NULL(cmap->insert(map, __ds_s32key(1), __ds_s32value(10)).d);
    TEST_ASSERT_NOT_NULL(cmap->insert(map, __ds_s32key(2), __ds_s32value(20)).d);
    TEST_ASSERT_NOT_NULL(cmap->insert(map, __ds_s32key(3), __ds_s32value(30)).d);
    TEST_ASSERT_NOT_NULL(cmap->insert(map, __ds_s32key(4), __ds_s32value(40)).d);
    map_check_ascending(map);
    map_check_descending(map);
}

TEST_CASE(unit, map_descending_order)
{
    map_t* map = MAP_NEW_KV_GT(int32_t, int32_t);
    map_iterator_t it;
    int32_t expected = 3;

    TEST_ASSERT_NOT_NULL(map);
    test_cleanup_push(map_free, map);

    TEST_ASSERT_NOT_NULL(cmap->insert(map, __ds_s32key(1), __ds_s32value(10)).d);
    TEST_ASSERT_NOT_NULL(cmap->insert(map, __ds_s32key(3), __ds_s32value(30)).d);
    TEST_ASSERT_NOT_NULL(cmap->insert(map, __ds_s32key(2), __ds_s32value(20)).d);

    for (it = cmap->begin(map); !map_at_end(map, it); it = cmap->next(it)) {
        TEST_ASSERT_EQ(expected, it_s32key(it));
        TEST_ASSERT_EQ(expected * 10, it_s32value(it));
        --expected;
    }
    TEST_ASSERT_EQ(0, expected);

    it = cmap->find(map, __ds_s32key(2));
    TEST_ASSERT_FALSE(map_at_end(map, it));
    TEST_ASSERT_EQ(20, it_s32value(it));
    TEST_ASSERT_EQ(1, cmap->remove(map, __ds_s32key(1)));
    TEST_ASSERT_EQ(2, cmap->clear(map));
}

TEST_CASE(unit, map_string_keys)
{
    map_t* map = MAP_NEW_KV(string, int32_t);
    map_iterator_t it;
    static const char* const keys[] = { "abc", "and", "j", "jerry" };
    size_t i = 0;

    TEST_ASSERT_NOT_NULL(map);
    test_cleanup_push(map_free, map);

    TEST_ASSERT_NOT_NULL(cmap->insert(map, ds_skey("j"), __ds_s32value(6)).d);
    TEST_ASSERT_NOT_NULL(cmap->insert(map, ds_skey("and"), __ds_s32value(6)).d);
    TEST_ASSERT_NOT_NULL(cmap->insert(map, ds_skey("abc"), __ds_s32value(6)).d);
    TEST_ASSERT_NOT_NULL(cmap->insert(map, ds_skey("jerry"),
                                      __ds_s32value(6)).d);
    TEST_ASSERT_NULL(cmap->insert(map, ds_skey("jerry"),
                                  __ds_s32value(7)).d);
    TEST_ASSERT_NOT_NULL(cmap->insert_replace(map, ds_skey("jerry"),
                                              __ds_s32value(99)).d);

    for (it = cmap->begin(map); !map_at_end(map, it); it = cmap->next(it)) {
        TEST_ASSERT_TRUE(i < sizeof(keys) / sizeof(keys[0]));
        TEST_ASSERT_STR_EQ(keys[i], it_skey(it));
        ++i;
    }
    TEST_ASSERT_EQ(sizeof(keys) / sizeof(keys[0]), i);

    it = cmap->find(map, ds_skey("jerry"));
    TEST_ASSERT_FALSE(map_at_end(map, it));
    TEST_ASSERT_EQ(99, it_s32value(it));
    TEST_ASSERT_TRUE(map_at_end(map, cmap->find(map, ds_skey("tom"))));

    TEST_ASSERT_EQ(1, cmap->remove(map, ds_skey("and")));
    TEST_ASSERT_EQ(3, cmap->clear(map));
}

TEST_CASE(unit, map_typed_values)
{
    map_t* wide = MAP_NEW_KV(int64_t, double);
    map_t* small = MAP_NEW_KV(uint8_t, float);
    map_iterator_t it;

    TEST_ASSERT_NOT_NULL(wide);
    TEST_ASSERT_NOT_NULL(small);
    test_cleanup_push(map_free, wide);
    test_cleanup_push(map_free, small);

    TEST_ASSERT_NOT_NULL(cmap->insert(wide, ds_s64key(INT64_C(0x1122334455667788)),
                                      ds_lfvalue(2.5)).d);
    TEST_ASSERT_NOT_NULL(cmap->insert(wide, ds_s64key(INT64_C(-7)),
                                      ds_lfvalue(-4.75)).d);
    it = cmap->find(wide, ds_s64key(INT64_C(0x1122334455667788)));
    TEST_ASSERT_FALSE(map_at_end(wide, it));
    TEST_ASSERT_EQ(INT64_C(0x1122334455667788), it_s64key(it));
    TEST_ASSERT_TRUE(it_lfvalue(it) == 2.5);

    TEST_ASSERT_NOT_NULL(cmap->insert(small, ds_u8key(250),
                                      ds_fvalue(1.25f)).d);
    TEST_ASSERT_NOT_NULL(cmap->insert(small, ds_u8key(3),
                                      ds_fvalue(-0.5f)).d);
    it = cmap->find(small, ds_u8key(250));
    TEST_ASSERT_FALSE(map_at_end(small, it));
    TEST_ASSERT_EQ(250, it_u8key(it));
    TEST_ASSERT_TRUE(it_fvalue(it) == 1.25f);

    TEST_ASSERT_EQ(2, cmap->clear(wide));
    TEST_ASSERT_EQ(2, cmap->clear(small));
}

static bool map_remove_even(ds_key_t key, ds_value_t value)
{
    int32_t typed_key = __ds_token_to_s32(key);

    return (typed_key % 2) == 0
           && __ds_token_to_s32(value) == typed_key * 10;
}

TEST_CASE(unit, map_remove_if)
{
    map_t* map = MAP_NEW_KV(int32_t, int32_t);
    int32_t key;

    TEST_ASSERT_NOT_NULL(map);
    test_cleanup_push(map_free, map);
    for (key = 1; key <= 6; ++key) {
        TEST_ASSERT_NOT_NULL(cmap->insert(map, __ds_s32key(key),
                                          __ds_s32value(key * 10)).d);
    }

    TEST_ASSERT_EQ(-1, cmap->remove_if(NULL, map_remove_even));
    TEST_ASSERT_EQ(-1, cmap->remove_if(map, NULL));
    TEST_ASSERT_EQ(3, cmap->remove_if(map, map_remove_even));
    TEST_ASSERT_EQ(3, cmap->size(map));

    for (key = 1; key <= 6; key += 2) {
        map_iterator_t it = cmap->find(map, __ds_s32key(key));

        TEST_ASSERT_FALSE(map_at_end(map, it));
        TEST_ASSERT_EQ(key * 10, it_s32value(it));
    }
    TEST_ASSERT_TRUE(map_at_end(map, cmap->find(map, __ds_s32key(2))));
    TEST_ASSERT_EQ(3, cmap->clear(map));
}

TEST_CASE(unit, map_null_guards)
{
    TEST_ASSERT_EQ(-1, cmap->size(NULL));
    TEST_ASSERT_EQ(-1, cmap->count(NULL, __ds_s32key(1)));
    TEST_ASSERT_EQ(-1, cmap->remove(NULL, __ds_s32key(1)));
    TEST_ASSERT_EQ(-1, cmap->clear(NULL));
    TEST_ASSERT_NULL(cmap->find(NULL, __ds_s32key(1)).d);
    TEST_ASSERT_NULL(cmap->lower_bound(NULL, __ds_s32key(1)).d);
    TEST_ASSERT_NULL(cmap->upper_bound(NULL, __ds_s32key(1)).d);
    TEST_ASSERT_NULL(cmap->insert(NULL, __ds_s32key(1),
                                  __ds_s32value(1)).d);
    TEST_ASSERT_NULL(cmap->insert_replace(NULL, __ds_s32key(1),
                                          __ds_s32value(1)).d);
    MAP_DELETE(NULL);
}
