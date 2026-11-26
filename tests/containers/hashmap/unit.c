/*
  libj_ds permanent tests hashmap unit
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

#include <hashmap/hashmap.h>

#include "../../core/test.h"

static void hashmap_free(void* self)
{
    hashmap_t* hashmap = self;

    HASHMAP_DELETE(&hashmap);
}

static bool hashmap_at_end(const hashmap_t* hashmap, hashmap_iterator_t it)
{
    return it_eq(it, chashmap->end(hashmap));
}

TEST_CASE(unit, hashmap_basics)
{
    hashmap_t* hashmap = HASHMAP_NEW_KV(int32_t, int32_t);
    hashmap_iterator_t it;
    hashmap_r_iterator_t rit;
    int32_t key_sum = 0;
    int32_t value_sum = 0;
    int32_t count = 0;

    TEST_ASSERT_NOT_NULL(hashmap);
    test_cleanup_push(hashmap_free, hashmap);
    TEST_ASSERT_EQ(0, chashmap->bucket_count(hashmap));

    TEST_ASSERT_NOT_NULL(chashmap->insert(hashmap, __ds_s32key(3),
                                          __ds_s32value(30)).d);
    TEST_ASSERT_TRUE(chashmap->bucket_count(hashmap) > 0);
    TEST_ASSERT_TRUE(chashmap->bucket_valid_count(hashmap)
                     <= chashmap->bucket_count(hashmap));
    TEST_ASSERT_NOT_NULL(chashmap->insert(hashmap, __ds_s32key(1),
                                          __ds_s32value(10)).d);
    TEST_ASSERT_NOT_NULL(chashmap->insert(hashmap, __ds_s32key(2),
                                          __ds_s32value(20)).d);
    TEST_ASSERT_NULL(chashmap->insert(hashmap, __ds_s32key(2),
                                      __ds_s32value(99)).d);
    TEST_ASSERT_EQ(3, chashmap->size(hashmap));
    TEST_ASSERT_EQ(1, chashmap->count(hashmap, __ds_s32key(2)));
    TEST_ASSERT_EQ(20, it_s32value(chashmap->find(hashmap,
                                                  __ds_s32key(2))));

    TEST_ASSERT_NOT_NULL(chashmap->insert_replace(hashmap, __ds_s32key(2),
                                                  __ds_s32value(22)).d);
    TEST_ASSERT_NOT_NULL(chashmap->insert_replace(hashmap, __ds_s32key(4),
                                                  __ds_s32value(44)).d);
    TEST_ASSERT_EQ(4, chashmap->size(hashmap));

    for (it = chashmap->begin(hashmap); !hashmap_at_end(hashmap, it);
         it = chashmap->next(it)) {
        key_sum += it_s32key(it);
        value_sum += it_s32value(it);
        ++count;
    }
    TEST_ASSERT_EQ(4, count);
    TEST_ASSERT_EQ(10, key_sum);
    TEST_ASSERT_EQ(106, value_sum);

    key_sum = 0;
    value_sum = 0;
    count = 0;
    for (rit = chashmap->rbegin(hashmap);
         it_ne(rit, chashmap->rend(hashmap)); rit = chashmap->rnext(rit)) {
        key_sum += it_s32key(rit);
        value_sum += it_s32value(rit);
        ++count;
    }
    TEST_ASSERT_EQ(4, count);
    TEST_ASSERT_EQ(10, key_sum);
    TEST_ASSERT_EQ(106, value_sum);

    TEST_ASSERT_EQ(1, chashmap->remove(hashmap, __ds_s32key(2)));
    TEST_ASSERT_EQ(0, chashmap->remove(hashmap, __ds_s32key(2)));
    TEST_ASSERT_EQ(3, chashmap->clear(hashmap));
    TEST_ASSERT_EQ(0, chashmap->size(hashmap));
    TEST_ASSERT_EQ(0, chashmap->bucket_valid_count(hashmap));
    TEST_ASSERT_EQ(0, chashmap->clear(hashmap));
}

TEST_CASE(unit, hashmap_bucket_growth)
{
    hashmap_t* hashmap = HASHMAP_NEW_KV_1(int32_t, int32_t, 4);
    hashmap_bcount_t initial;
    size_t i;
    int32_t index;

    TEST_ASSERT_NOT_NULL(hashmap);
    test_cleanup_push(hashmap_free, hashmap);
    TEST_ASSERT_EQ(0, chashmap->bucket_count(hashmap));

    TEST_ASSERT_NOT_NULL(chashmap->insert(hashmap, __ds_s32key(0),
                                          __ds_s32value(0)).d);
    initial = chashmap->bucket_count(hashmap);
    TEST_ASSERT_TRUE(initial > 0);

    for (i = 1; i < 200; ++i) {
        TEST_ASSERT_NOT_NULL(chashmap->insert(hashmap, __ds_s32key((int32_t)i),
                                              __ds_s32value((int32_t)i * 10)).d);
    }
    TEST_ASSERT_EQ(200, chashmap->size(hashmap));
    TEST_ASSERT_TRUE(chashmap->bucket_count(hashmap) >= initial);
    TEST_ASSERT_TRUE(chashmap->bucket_valid_count(hashmap)
                     <= chashmap->bucket_count(hashmap));

    for (i = 0; i < 200; ++i) {
        hashmap_iterator_t it = chashmap->find(hashmap, __ds_s32key((int32_t)i));

        TEST_ASSERT_FALSE(hashmap_at_end(hashmap, it));
        TEST_ASSERT_EQ((int32_t)i, it_s32key(it));
        TEST_ASSERT_EQ((int32_t)i * 10, it_s32value(it));
    }
    TEST_ASSERT_EQ(0, chashmap->count(hashmap, __ds_s32key(1000)));

    for (i = 0; i < 200; i += 2)
        TEST_ASSERT_EQ(1, chashmap->remove(hashmap, __ds_s32key((int32_t)i)));
    TEST_ASSERT_EQ(100, chashmap->size(hashmap));
    TEST_ASSERT_TRUE(chashmap->bucket_valid_count(hashmap)
                     <= chashmap->bucket_count(hashmap));

    for (index = 0; index < 200; ++index) {
        hashmap_iterator_t it = chashmap->find(hashmap, __ds_s32key(index));

        if ((index % 2) == 0)
            TEST_ASSERT_TRUE(hashmap_at_end(hashmap, it));
        else
            TEST_ASSERT_FALSE(hashmap_at_end(hashmap, it));
    }

    TEST_ASSERT_EQ(100, chashmap->clear(hashmap));
    TEST_ASSERT_EQ(0, chashmap->size(hashmap));
    TEST_ASSERT_EQ(0, chashmap->bucket_valid_count(hashmap));
}

TEST_CASE(unit, hashmap_string_keys)
{
    hashmap_t* hashmap = HASHMAP_NEW_KV(string, int32_t);
    hashmap_iterator_t it;
    int32_t count = 0;
    int32_t value_sum = 0;

    TEST_ASSERT_NOT_NULL(hashmap);
    test_cleanup_push(hashmap_free, hashmap);

    TEST_ASSERT_NOT_NULL(chashmap->insert(hashmap, ds_skey("j"),
                                          __ds_s32value(6)).d);
    TEST_ASSERT_NOT_NULL(chashmap->insert(hashmap, ds_skey("and"),
                                          __ds_s32value(6)).d);
    TEST_ASSERT_NOT_NULL(chashmap->insert(hashmap, ds_skey("abc"),
                                          __ds_s32value(6)).d);
    TEST_ASSERT_NULL(chashmap->insert(hashmap, ds_skey("j"),
                                      __ds_s32value(7)).d);
    TEST_ASSERT_NOT_NULL(chashmap->insert_replace(hashmap, ds_skey("j"),
                                                  __ds_s32value(99)).d);
    TEST_ASSERT_EQ(3, chashmap->size(hashmap));

    it = chashmap->find(hashmap, ds_skey("j"));
    TEST_ASSERT_FALSE(hashmap_at_end(hashmap, it));
    TEST_ASSERT_STR_EQ("j", it_skey(it));
    TEST_ASSERT_EQ(99, it_s32value(it));
    TEST_ASSERT_TRUE(hashmap_at_end(hashmap, chashmap->find(hashmap,
                                                           ds_skey("tom"))));

    for (it = chashmap->begin(hashmap); !hashmap_at_end(hashmap, it);
         it = chashmap->next(it)) {
        ++count;
        value_sum += it_s32value(it);
    }
    TEST_ASSERT_EQ(3, count);
    TEST_ASSERT_EQ(111, value_sum);

    TEST_ASSERT_EQ(1, chashmap->remove(hashmap, ds_skey("and")));
    TEST_ASSERT_EQ(2, chashmap->clear(hashmap));
}

TEST_CASE(unit, hashmap_typed_values)
{
    hashmap_t* hashmap = HASHMAP_NEW_KV(int64_t, double);
    hashmap_iterator_t it;

    TEST_ASSERT_NOT_NULL(hashmap);
    test_cleanup_push(hashmap_free, hashmap);

    TEST_ASSERT_NOT_NULL(chashmap->insert(hashmap,
                                          ds_s64key(INT64_C(0x1122334455667788)),
                                          ds_lfvalue(2.5)).d);
    TEST_ASSERT_NOT_NULL(chashmap->insert(hashmap, ds_s64key(INT64_C(-7)),
                                          ds_lfvalue(-4.75)).d);
    it = chashmap->find(hashmap, ds_s64key(INT64_C(0x1122334455667788)));
    TEST_ASSERT_FALSE(hashmap_at_end(hashmap, it));
    TEST_ASSERT_EQ(INT64_C(0x1122334455667788), it_s64key(it));
    TEST_ASSERT_TRUE(it_lfvalue(it) == 2.5);

    TEST_ASSERT_EQ(1, chashmap->remove(hashmap, ds_s64key(INT64_C(-7))));
    TEST_ASSERT_EQ(1, chashmap->clear(hashmap));
}

TEST_CASE(unit, hashmap_null_guards)
{
    TEST_ASSERT_EQ(-1, chashmap->size(NULL));
    TEST_ASSERT_EQ(-1, chashmap->bucket_count(NULL));
    TEST_ASSERT_EQ(-1, chashmap->bucket_valid_count(NULL));
    TEST_ASSERT_EQ(-1, chashmap->count(NULL, __ds_s32key(1)));
    TEST_ASSERT_EQ(-1, chashmap->remove(NULL, __ds_s32key(1)));
    TEST_ASSERT_EQ(-1, chashmap->clear(NULL));
    TEST_ASSERT_NULL(chashmap->find(NULL, __ds_s32key(1)).d);
    TEST_ASSERT_NULL(chashmap->insert(NULL, __ds_s32key(1),
                                      __ds_s32value(1)).d);
    TEST_ASSERT_NULL(chashmap->insert_replace(NULL, __ds_s32key(1),
                                              __ds_s32value(1)).d);
    HASHMAP_DELETE(NULL);
}
