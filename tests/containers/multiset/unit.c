/*
  libj_ds permanent tests multiset unit
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

#include <multiset/multiset.h>

#include "../../core/test.h"

static void multiset_free(void* self)
{
    multiset_t* multiset = self;

    MULTISET_DELETE(&multiset);
}

static bool multiset_at_end(const multiset_t* multiset, multiset_iterator_t it)
{
    return it_eq(it, cmultiset->end(multiset));
}

TEST_CASE(unit, multiset_duplicates)
{
    multiset_t* multiset = MULTISET_NEW_T(int32_t);
    multiset_iterator_t it;
    int32_t expected_keys[] = { 1, 5, 5, 9 };
    size_t i = 0;

    TEST_ASSERT_NOT_NULL(multiset);
    test_cleanup_push(multiset_free, multiset);

    TEST_ASSERT_NOT_NULL(cmultiset->insert(multiset, __ds_s32key(5)).d);
    TEST_ASSERT_NOT_NULL(cmultiset->insert(multiset, __ds_s32key(5)).d);
    TEST_ASSERT_NOT_NULL(cmultiset->insert(multiset, __ds_s32key(1)).d);
    TEST_ASSERT_NOT_NULL(cmultiset->insert(multiset, __ds_s32key(9)).d);
    TEST_ASSERT_EQ(4, cmultiset->size(multiset));
    TEST_ASSERT_EQ(2, cmultiset->count(multiset, __ds_s32key(5)));
    TEST_ASSERT_EQ(0, cmultiset->count(multiset, __ds_s32key(6)));

    for (it = cmultiset->begin(multiset);
         !multiset_at_end(multiset, it); it = cmultiset->next(it)) {
        TEST_ASSERT_TRUE(i < sizeof(expected_keys) / sizeof(expected_keys[0]));
        TEST_ASSERT_EQ(expected_keys[i], it_s32data(it));
        ++i;
    }
    TEST_ASSERT_EQ(sizeof(expected_keys) / sizeof(expected_keys[0]), i);

    TEST_ASSERT_EQ(5, it_s32data(cmultiset->lower_bound(multiset,
                                                        __ds_s32key(5))));
    TEST_ASSERT_EQ(9, it_s32data(cmultiset->upper_bound(multiset,
                                                        __ds_s32key(5))));
    TEST_ASSERT_EQ(9, it_s32data(cmultiset->lower_bound(multiset,
                                                        __ds_s32key(6))));
    TEST_ASSERT_TRUE(multiset_at_end(multiset,
                                     cmultiset->upper_bound(multiset,
                                                            __ds_s32key(9))));

    it = cmultiset->find(multiset, __ds_s32key(5));
    TEST_ASSERT_FALSE(multiset_at_end(multiset, it));
    it = cmultiset->erase(multiset, it);
    TEST_ASSERT_FALSE(multiset_at_end(multiset, it));
    TEST_ASSERT_EQ(5, it_s32data(it));
    TEST_ASSERT_EQ(3, cmultiset->size(multiset));
    TEST_ASSERT_EQ(1, cmultiset->count(multiset, __ds_s32key(5)));
    TEST_ASSERT_EQ(1, cmultiset->remove(multiset, __ds_s32key(5)));
    TEST_ASSERT_EQ(2, cmultiset->clear(multiset));
}

TEST_CASE(unit, multiset_descending_order)
{
    multiset_t* multiset = MULTISET_NEW_T_GT(int32_t);
    multiset_iterator_t it;
    int32_t expected_keys[] = { 3, 2, 2, 1 };
    size_t i = 0;

    TEST_ASSERT_NOT_NULL(multiset);
    test_cleanup_push(multiset_free, multiset);

    TEST_ASSERT_NOT_NULL(cmultiset->insert(multiset, __ds_s32key(1)).d);
    TEST_ASSERT_NOT_NULL(cmultiset->insert(multiset, __ds_s32key(3)).d);
    TEST_ASSERT_NOT_NULL(cmultiset->insert(multiset, __ds_s32key(2)).d);
    TEST_ASSERT_NOT_NULL(cmultiset->insert(multiset, __ds_s32key(2)).d);

    for (it = cmultiset->begin(multiset);
         !multiset_at_end(multiset, it); it = cmultiset->next(it)) {
        TEST_ASSERT_TRUE(i < sizeof(expected_keys) / sizeof(expected_keys[0]));
        TEST_ASSERT_EQ(expected_keys[i], it_s32data(it));
        ++i;
    }
    TEST_ASSERT_EQ(sizeof(expected_keys) / sizeof(expected_keys[0]), i);
    TEST_ASSERT_EQ(2, cmultiset->count(multiset, __ds_s32key(2)));
    TEST_ASSERT_EQ(4, cmultiset->clear(multiset));
}

TEST_CASE(unit, multiset_string_keys)
{
    multiset_t* multiset = MULTISET_NEW_T(string);
    multiset_iterator_t it;
    static const char* const keys[] = { "abc", "and", "j", "j" };
    size_t i = 0;

    TEST_ASSERT_NOT_NULL(multiset);
    test_cleanup_push(multiset_free, multiset);

    TEST_ASSERT_NOT_NULL(cmultiset->insert(multiset, ds_skey("j")).d);
    TEST_ASSERT_NOT_NULL(cmultiset->insert(multiset, ds_skey("j")).d);
    TEST_ASSERT_NOT_NULL(cmultiset->insert(multiset, ds_skey("and")).d);
    TEST_ASSERT_NOT_NULL(cmultiset->insert(multiset, ds_skey("abc")).d);
    TEST_ASSERT_EQ(2, cmultiset->count(multiset, ds_skey("j")));

    for (it = cmultiset->begin(multiset);
         !multiset_at_end(multiset, it); it = cmultiset->next(it)) {
        TEST_ASSERT_TRUE(i < sizeof(keys) / sizeof(keys[0]));
        TEST_ASSERT_STR_EQ(keys[i], it_sdata(it));
        ++i;
    }
    TEST_ASSERT_EQ(sizeof(keys) / sizeof(keys[0]), i);

    TEST_ASSERT_EQ(2, cmultiset->remove(multiset, ds_skey("j")));
    TEST_ASSERT_EQ(2, cmultiset->clear(multiset));
}

static bool multiset_remove_even(ds_key_t key)
{
    return (__ds_token_to_s32(key) % 2) == 0;
}

TEST_CASE(unit, multiset_remove_if)
{
    multiset_t* multiset = MULTISET_NEW_T(int32_t);
    int32_t key;

    TEST_ASSERT_NOT_NULL(multiset);
    test_cleanup_push(multiset_free, multiset);
    for (key = 1; key <= 6; ++key)
        TEST_ASSERT_NOT_NULL(cmultiset->insert(multiset,
                                               __ds_s32key(key)).d);
    TEST_ASSERT_NOT_NULL(cmultiset->insert(multiset, __ds_s32key(2)).d);
    TEST_ASSERT_EQ(2, cmultiset->count(multiset, __ds_s32key(2)));

    TEST_ASSERT_EQ(-1, cmultiset->remove_if(NULL, multiset_remove_even));
    TEST_ASSERT_EQ(-1, cmultiset->remove_if(multiset, NULL));
    TEST_ASSERT_EQ(4, cmultiset->remove_if(multiset, multiset_remove_even));
    TEST_ASSERT_EQ(3, cmultiset->size(multiset));
    TEST_ASSERT_EQ(0, cmultiset->count(multiset, __ds_s32key(2)));

    for (key = 1; key <= 5; key += 2)
        TEST_ASSERT_FALSE(multiset_at_end(multiset,
                                          cmultiset->find(multiset,
                                                          __ds_s32key(key))));
    TEST_ASSERT_EQ(3, cmultiset->clear(multiset));
}

TEST_CASE(unit, multiset_null_guards)
{
    TEST_ASSERT_EQ(-1, cmultiset->size(NULL));
    TEST_ASSERT_EQ(-1, cmultiset->count(NULL, __ds_s32key(1)));
    TEST_ASSERT_EQ(-1, cmultiset->remove(NULL, __ds_s32key(1)));
    TEST_ASSERT_EQ(-1, cmultiset->clear(NULL));
    TEST_ASSERT_NULL(cmultiset->find(NULL, __ds_s32key(1)).d);
    TEST_ASSERT_NULL(cmultiset->lower_bound(NULL, __ds_s32key(1)).d);
    TEST_ASSERT_NULL(cmultiset->upper_bound(NULL, __ds_s32key(1)).d);
    TEST_ASSERT_NULL(cmultiset->insert(NULL, __ds_s32key(1)).d);
    MULTISET_DELETE(NULL);
}
