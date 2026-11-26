/*
  libj_ds permanent tests multimap unit
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

#include <multimap/multimap.h>

#include "../../core/test.h"

static void multimap_free(void* self)
{
    multimap_t* multimap = self;

    MULTIMAP_DELETE(&multimap);
}

static bool multimap_at_end(const multimap_t* multimap, multimap_iterator_t it)
{
    return it_eq(it, cmultimap->end(multimap));
}

TEST_CASE(unit, multimap_duplicates)
{
    multimap_t* multimap = MULTIMAP_NEW_KV(int32_t, int32_t);
    multimap_iterator_t it;
    int32_t expected_keys[] = { 1, 5, 5, 9 };
    int32_t value_sum = 0;
    size_t i = 0;

    TEST_ASSERT_NOT_NULL(multimap);
    test_cleanup_push(multimap_free, multimap);

    TEST_ASSERT_NOT_NULL(cmultimap->insert(multimap, __ds_s32key(5),
                                           __ds_s32value(51)).d);
    TEST_ASSERT_NOT_NULL(cmultimap->insert(multimap, __ds_s32key(5),
                                           __ds_s32value(52)).d);
    TEST_ASSERT_NOT_NULL(cmultimap->insert(multimap, __ds_s32key(1),
                                           __ds_s32value(10)).d);
    TEST_ASSERT_NOT_NULL(cmultimap->insert(multimap, __ds_s32key(9),
                                           __ds_s32value(90)).d);
    TEST_ASSERT_EQ(4, cmultimap->size(multimap));
    TEST_ASSERT_EQ(2, cmultimap->count(multimap, __ds_s32key(5)));
    TEST_ASSERT_EQ(0, cmultimap->count(multimap, __ds_s32key(6)));

    for (it = cmultimap->begin(multimap);
         !multimap_at_end(multimap, it); it = cmultimap->next(it)) {
        TEST_ASSERT_TRUE(i < sizeof(expected_keys) / sizeof(expected_keys[0]));
        TEST_ASSERT_EQ(expected_keys[i], it_s32key(it));
        value_sum += it_s32value(it);
        ++i;
    }
    TEST_ASSERT_EQ(sizeof(expected_keys) / sizeof(expected_keys[0]), i);
    TEST_ASSERT_EQ(203, value_sum);

    TEST_ASSERT_EQ(5, it_s32key(cmultimap->lower_bound(multimap,
                                                       __ds_s32key(5))));
    TEST_ASSERT_EQ(9, it_s32key(cmultimap->upper_bound(multimap,
                                                       __ds_s32key(5))));
    TEST_ASSERT_EQ(9, it_s32key(cmultimap->lower_bound(multimap,
                                                       __ds_s32key(6))));
    TEST_ASSERT_TRUE(multimap_at_end(multimap,
                                     cmultimap->upper_bound(multimap,
                                                            __ds_s32key(9))));

    it = cmultimap->find(multimap, __ds_s32key(5));
    TEST_ASSERT_FALSE(multimap_at_end(multimap, it));
    TEST_ASSERT_EQ(5, it_s32key(it));
    it = cmultimap->erase(multimap, it);
    TEST_ASSERT_FALSE(multimap_at_end(multimap, it));
    TEST_ASSERT_EQ(5, it_s32key(it));
    TEST_ASSERT_EQ(3, cmultimap->size(multimap));
    TEST_ASSERT_EQ(1, cmultimap->count(multimap, __ds_s32key(5)));

    TEST_ASSERT_EQ(1, cmultimap->remove(multimap, __ds_s32key(5)));
    TEST_ASSERT_EQ(0, cmultimap->remove(multimap, __ds_s32key(5)));
    TEST_ASSERT_EQ(2, cmultimap->clear(multimap));
    TEST_ASSERT_EQ(0, cmultimap->size(multimap));
}

TEST_CASE(unit, multimap_descending_order)
{
    multimap_t* multimap = MULTIMAP_NEW_KV_GT(int32_t, int32_t);
    multimap_iterator_t it;
    int32_t expected_keys[] = { 3, 2, 2, 1 };
    size_t i = 0;

    TEST_ASSERT_NOT_NULL(multimap);
    test_cleanup_push(multimap_free, multimap);

    TEST_ASSERT_NOT_NULL(cmultimap->insert(multimap, __ds_s32key(1),
                                           __ds_s32value(10)).d);
    TEST_ASSERT_NOT_NULL(cmultimap->insert(multimap, __ds_s32key(3),
                                           __ds_s32value(30)).d);
    TEST_ASSERT_NOT_NULL(cmultimap->insert(multimap, __ds_s32key(2),
                                           __ds_s32value(20)).d);
    TEST_ASSERT_NOT_NULL(cmultimap->insert(multimap, __ds_s32key(2),
                                           __ds_s32value(21)).d);

    for (it = cmultimap->begin(multimap);
         !multimap_at_end(multimap, it); it = cmultimap->next(it)) {
        TEST_ASSERT_TRUE(i < sizeof(expected_keys) / sizeof(expected_keys[0]));
        TEST_ASSERT_EQ(expected_keys[i], it_s32key(it));
        ++i;
    }
    TEST_ASSERT_EQ(sizeof(expected_keys) / sizeof(expected_keys[0]), i);
    TEST_ASSERT_EQ(2, cmultimap->count(multimap, __ds_s32key(2)));
    TEST_ASSERT_EQ(4, cmultimap->clear(multimap));
}

TEST_CASE(unit, multimap_string_keys)
{
    multimap_t* multimap = MULTIMAP_NEW_KV(string, int32_t);
    multimap_iterator_t it;
    static const char* const keys[] = { "abc", "and", "j", "j" };
    size_t i = 0;

    TEST_ASSERT_NOT_NULL(multimap);
    test_cleanup_push(multimap_free, multimap);

    TEST_ASSERT_NOT_NULL(cmultimap->insert(multimap, ds_skey("j"),
                                           __ds_s32value(1)).d);
    TEST_ASSERT_NOT_NULL(cmultimap->insert(multimap, ds_skey("j"),
                                           __ds_s32value(2)).d);
    TEST_ASSERT_NOT_NULL(cmultimap->insert(multimap, ds_skey("and"),
                                           __ds_s32value(3)).d);
    TEST_ASSERT_NOT_NULL(cmultimap->insert(multimap, ds_skey("abc"),
                                           __ds_s32value(4)).d);
    TEST_ASSERT_EQ(2, cmultimap->count(multimap, ds_skey("j")));

    for (it = cmultimap->begin(multimap);
         !multimap_at_end(multimap, it); it = cmultimap->next(it)) {
        TEST_ASSERT_TRUE(i < sizeof(keys) / sizeof(keys[0]));
        TEST_ASSERT_STR_EQ(keys[i], it_skey(it));
        ++i;
    }
    TEST_ASSERT_EQ(sizeof(keys) / sizeof(keys[0]), i);

    TEST_ASSERT_EQ(2, cmultimap->remove(multimap, ds_skey("j")));
    TEST_ASSERT_EQ(2, cmultimap->clear(multimap));
}

static bool multimap_remove_even(ds_key_t key, ds_value_t value)
{
    (void)value;
    return (__ds_token_to_s32(key) % 2) == 0;
}

TEST_CASE(unit, multimap_remove_if)
{
    multimap_t* multimap = MULTIMAP_NEW_KV(int32_t, int32_t);
    int32_t key;

    TEST_ASSERT_NOT_NULL(multimap);
    test_cleanup_push(multimap_free, multimap);
    for (key = 1; key <= 6; ++key) {
        TEST_ASSERT_NOT_NULL(cmultimap->insert(multimap, __ds_s32key(key),
                                               __ds_s32value(key * 10)).d);
    }
    TEST_ASSERT_NOT_NULL(cmultimap->insert(multimap, __ds_s32key(2),
                                           __ds_s32value(21)).d);
    TEST_ASSERT_EQ(2, cmultimap->count(multimap, __ds_s32key(2)));

    TEST_ASSERT_EQ(-1, cmultimap->remove_if(NULL, multimap_remove_even));
    TEST_ASSERT_EQ(-1, cmultimap->remove_if(multimap, NULL));
    TEST_ASSERT_EQ(4, cmultimap->remove_if(multimap, multimap_remove_even));
    TEST_ASSERT_EQ(3, cmultimap->size(multimap));
    TEST_ASSERT_EQ(0, cmultimap->count(multimap, __ds_s32key(2)));

    for (key = 1; key <= 5; key += 2)
        TEST_ASSERT_FALSE(multimap_at_end(multimap,
                                          cmultimap->find(multimap,
                                                          __ds_s32key(key))));
    TEST_ASSERT_EQ(3, cmultimap->clear(multimap));
}

TEST_CASE(unit, multimap_null_guards)
{
    TEST_ASSERT_EQ(-1, cmultimap->size(NULL));
    TEST_ASSERT_EQ(-1, cmultimap->count(NULL, __ds_s32key(1)));
    TEST_ASSERT_EQ(-1, cmultimap->remove(NULL, __ds_s32key(1)));
    TEST_ASSERT_EQ(-1, cmultimap->clear(NULL));
    TEST_ASSERT_NULL(cmultimap->find(NULL, __ds_s32key(1)).d);
    TEST_ASSERT_NULL(cmultimap->lower_bound(NULL, __ds_s32key(1)).d);
    TEST_ASSERT_NULL(cmultimap->upper_bound(NULL, __ds_s32key(1)).d);
    TEST_ASSERT_NULL(cmultimap->insert(NULL, __ds_s32key(1),
                                       __ds_s32value(1)).d);
    MULTIMAP_DELETE(NULL);
}
