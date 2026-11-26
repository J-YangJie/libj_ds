/*
  libj_ds permanent tests set unit
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

#include <set/set.h>

#include "../../core/test.h"

static void set_free(void* self)
{
    set_t* set = self;

    SET_DELETE(&set);
}

static bool set_at_end(const set_t* set, set_iterator_t it)
{
    return it_eq(it, cset->end(set));
}

TEST_CASE(unit, set_ordered_basics)
{
    set_t* set = SET_NEW_T(int32_t);
    set_iterator_t it;
    int32_t expected = 1;

    TEST_ASSERT_NOT_NULL(set);
    test_cleanup_push(set_free, set);

    TEST_ASSERT_NOT_NULL(cset->insert(set, __ds_s32key(3)).d);
    TEST_ASSERT_NOT_NULL(cset->insert(set, __ds_s32key(1)).d);
    TEST_ASSERT_NOT_NULL(cset->insert(set, __ds_s32key(2)).d);
    TEST_ASSERT_NULL(cset->insert(set, __ds_s32key(2)).d);
    TEST_ASSERT_EQ(3, cset->size(set));
    TEST_ASSERT_EQ(1, cset->count(set, __ds_s32key(2)));

    for (it = cset->begin(set); !set_at_end(set, it); it = cset->next(it)) {
        TEST_ASSERT_EQ(expected, it_s32data(it));
        ++expected;
    }
    TEST_ASSERT_EQ(4, expected);

    it = cset->find(set, __ds_s32key(2));
    TEST_ASSERT_FALSE(set_at_end(set, it));
    TEST_ASSERT_EQ(2, it_s32data(it));
    TEST_ASSERT_TRUE(set_at_end(set, cset->find(set, __ds_s32key(9))));

    TEST_ASSERT_EQ(2, it_s32data(cset->lower_bound(set, __ds_s32key(2))));
    TEST_ASSERT_EQ(3, it_s32data(cset->upper_bound(set, __ds_s32key(2))));
    TEST_ASSERT_TRUE(set_at_end(set, cset->lower_bound(set,
                                                       __ds_s32key(4))));

    it = cset->find(set, __ds_s32key(1));
    it = cset->erase(set, it);
    TEST_ASSERT_FALSE(set_at_end(set, it));
    TEST_ASSERT_EQ(2, it_s32data(it));
    TEST_ASSERT_EQ(2, cset->size(set));
    TEST_ASSERT_EQ(1, cset->remove(set, __ds_s32key(3)));
    TEST_ASSERT_EQ(0, cset->remove(set, __ds_s32key(3)));
    TEST_ASSERT_EQ(1, cset->clear(set));
    TEST_ASSERT_EQ(0, cset->size(set));
}

TEST_CASE(unit, set_descending_order)
{
    set_t* set = SET_NEW_T_GT(int32_t);
    set_iterator_t it;
    int32_t expected = 3;

    TEST_ASSERT_NOT_NULL(set);
    test_cleanup_push(set_free, set);

    TEST_ASSERT_NOT_NULL(cset->insert(set, __ds_s32key(1)).d);
    TEST_ASSERT_NOT_NULL(cset->insert(set, __ds_s32key(3)).d);
    TEST_ASSERT_NOT_NULL(cset->insert(set, __ds_s32key(2)).d);

    for (it = cset->begin(set); !set_at_end(set, it); it = cset->next(it)) {
        TEST_ASSERT_EQ(expected, it_s32data(it));
        --expected;
    }
    TEST_ASSERT_EQ(0, expected);
    TEST_ASSERT_EQ(1, cset->count(set, __ds_s32key(2)));
    TEST_ASSERT_EQ(3, cset->clear(set));
}

TEST_CASE(unit, set_string_keys)
{
    set_t* set = SET_NEW_T(string);
    set_iterator_t it;
    static const char* const keys[] = { "abc", "and", "j", "jerry" };
    size_t i = 0;

    TEST_ASSERT_NOT_NULL(set);
    test_cleanup_push(set_free, set);

    TEST_ASSERT_NOT_NULL(cset->insert(set, ds_skey("j")).d);
    TEST_ASSERT_NOT_NULL(cset->insert(set, ds_skey("and")).d);
    TEST_ASSERT_NOT_NULL(cset->insert(set, ds_skey("abc")).d);
    TEST_ASSERT_NOT_NULL(cset->insert(set, ds_skey("jerry")).d);
    TEST_ASSERT_NULL(cset->insert(set, ds_skey("jerry")).d);
    TEST_ASSERT_EQ(4, cset->size(set));

    for (it = cset->begin(set); !set_at_end(set, it); it = cset->next(it)) {
        TEST_ASSERT_TRUE(i < sizeof(keys) / sizeof(keys[0]));
        TEST_ASSERT_STR_EQ(keys[i], it_sdata(it));
        ++i;
    }
    TEST_ASSERT_EQ(sizeof(keys) / sizeof(keys[0]), i);

    it = cset->find(set, ds_skey("and"));
    TEST_ASSERT_FALSE(set_at_end(set, it));
    TEST_ASSERT_STR_EQ("and", it_sdata(it));
    TEST_ASSERT_EQ(1, cset->remove(set, ds_skey("jerry")));
    TEST_ASSERT_EQ(3, cset->clear(set));
}

static bool set_remove_even(ds_key_t key)
{
    return (__ds_token_to_s32(key) % 2) == 0;
}

TEST_CASE(unit, set_remove_if)
{
    set_t* set = SET_NEW_T(int32_t);
    int32_t key;

    TEST_ASSERT_NOT_NULL(set);
    test_cleanup_push(set_free, set);
    for (key = 1; key <= 6; ++key)
        TEST_ASSERT_NOT_NULL(cset->insert(set, __ds_s32key(key)).d);

    TEST_ASSERT_EQ(-1, cset->remove_if(NULL, set_remove_even));
    TEST_ASSERT_EQ(-1, cset->remove_if(set, NULL));
    TEST_ASSERT_EQ(3, cset->remove_if(set, set_remove_even));
    TEST_ASSERT_EQ(3, cset->size(set));

    for (key = 1; key <= 6; key += 2) {
        set_iterator_t it = cset->find(set, __ds_s32key(key));

        TEST_ASSERT_FALSE(set_at_end(set, it));
        TEST_ASSERT_EQ(key, it_s32data(it));
    }
    TEST_ASSERT_TRUE(set_at_end(set, cset->find(set, __ds_s32key(2))));
    TEST_ASSERT_EQ(3, cset->clear(set));
}

TEST_CASE(unit, set_null_guards)
{
    TEST_ASSERT_EQ(-1, cset->size(NULL));
    TEST_ASSERT_EQ(-1, cset->count(NULL, __ds_s32key(1)));
    TEST_ASSERT_EQ(-1, cset->remove(NULL, __ds_s32key(1)));
    TEST_ASSERT_EQ(-1, cset->clear(NULL));
    TEST_ASSERT_NULL(cset->find(NULL, __ds_s32key(1)).d);
    TEST_ASSERT_NULL(cset->lower_bound(NULL, __ds_s32key(1)).d);
    TEST_ASSERT_NULL(cset->upper_bound(NULL, __ds_s32key(1)).d);
    TEST_ASSERT_NULL(cset->insert(NULL, __ds_s32key(1)).d);
    SET_DELETE(NULL);
}
