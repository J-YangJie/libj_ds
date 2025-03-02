/*
  Hashmap Demos
  Copyright (C) 2021  YangJie <yangjie98765@yeah.net>

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
#include <string.h>
#include <_log.h>
#include <operations/ds_ops_string.h>

#define cds chashmap
#define TAG "[demo_hashmap]"

#define _tok(x)  ((hashmap_key_t)(x))
#define _from(x) ((x) ? (char*)(x) : "null string")

#define foreach()           { for (hashmap_iterator_t* it = cds->begin(&demo); cds->end(&demo) != it; it = cds->next(&demo, it)) pr_test("(%zd, %zd)", it->key, it->value); }
#define foreach_kstring()   { for (hashmap_iterator_t* it = cds->begin(&demo); cds->end(&demo) != it; it = cds->next(&demo, it)) pr_test("(%s(0x%zx), %zd)", _from(it->key), it->hash, it->value); }
#define foreach_r_kstring() { for (hashmap_r_iterator_t* it = cds->rbegin(&demo); cds->rend(&demo) != it; it = cds->rnext(&demo, it)) pr_test("(%s(0x%zx), %zd)", _from(it->key), it->hash, it->value); }

static class_hashmap_ops_t demo_ops = {
    .__hash      = __ds_ops_hash_default_string,
    .valid_key   = ds_ops_valid_key_default_string_max_128,
    .__lt        = __ds_ops_lt_default_string,
    .copy_key    = ds_ops_copy_data_default_string,
    .free_key    = ds_ops_free_data_default_string,
    .valid_value = NULL,
    .copy_value  = NULL,
    .free_value  = NULL,
};

static void demo_base_and_iterator(void)
{
    hashmap_t demo = HASHMAP_INIT(&demo);
    hashmap_size_t ret;
    hashmap_bcount_t bcount;
    hashmap_count_t count;

    (void)ret;
    (void)bcount;
    (void)count;

    for (int i = 1; i <= 8; ++i)
        cds->insert(&demo, i, i);
    // after for [ (1, 1), (2, 2), (3, 3), (4, 4), (5, 5), (6, 6), (7, 7), (8, 8) ]

    cds->insert(&demo, 35, 35);        // [ (1, 1), (2, 2), (35, 35), (3, 3), (4, 4), (5, 5), (6, 6), (7, 7), (8, 8) ]
    cds->insert(&demo, 19, 19);        // [ (1, 1), (2, 2), (19, 19), (35, 35), (3, 3), (4, 4), (5, 5), (6, 6), (7, 7), (8, 8) ]

    ret    = cds->size(&demo);         // size = 10
    ret    = cds->capacity(&demo);     // capacity = 16
    bcount = cds->bucket_count(&demo); // bucket count = 8
    count  = cds->count(&demo, 8);     // count = 1

    foreach();                         // [ (1, 1), (2, 2), (19, 19), (35, 35), (3, 3), (4, 4), (5, 5), (6, 6), (7, 7), (8, 8) ]
    pr_test("");

    {
        for (hashmap_iterator_t* it = cds->prev(&demo, cds->end(&demo)); cds->end(&demo) != it; it = cds->prev(&demo, it)) pr_test("(%zd, %zd)", it->key, it->value);
        pr_test("");                  // [ (8, 8), (7, 7), (6, 6), (5, 5), (4, 4), (3, 3), (35, 35), (19, 19), (2, 2), (1, 1) ]
        for (hashmap_r_iterator_t* it = cds->rbegin(&demo); cds->rend(&demo) != it; it = cds->rnext(&demo, it)) pr_test("(%zd, %zd)", it->key, it->value);
        pr_test("");                  // [ (8, 8), (7, 7), (6, 6), (5, 5), (4, 4), (3, 3), (35, 35), (19, 19), (2, 2), (1, 1) ]
        for (hashmap_r_iterator_t* it = cds->rprev(&demo, cds->rend(&demo)); cds->rend(&demo) != it; it = cds->rprev(&demo, it)) pr_test("(%zd, %zd)", it->key, it->value);
        pr_test("");                  // [ (1, 1), (2, 2), (19, 19), (35, 35), (3, 3), (4, 4), (5, 5), (6, 6), (7, 7), (8, 8) ]
    }

    HASHMAP_DEINIT(&demo);
}

static void demo_about_insert(void)
{
    hashmap_t demo = HASHMAP_INIT_OPS(&demo, &demo_ops);

    cds->insert(&demo, _tok("j"),     6);
    cds->insert(&demo, _tok("and"),   6);
    cds->insert(&demo, _tok("abc"),   6);
    cds->insert(&demo, _tok("jerry"), 6);          // [ ('jerry', 6), ('abc', 6), ('and', 6), ('j', 6) ]
    cds->insert_replace(&demo, _tok("jerry"), 99); // [ ('jerry', 99), ('abc', 6), ('and', 6), ('j', 6) ]

    foreach_kstring();
    pr_test("");

    foreach_r_kstring();
    pr_test("");

    HASHMAP_DEINIT(&demo);
}

static void demo_about_erase(void)
{
    hashmap_t demo = HASHMAP_INIT_OPS(&demo, &demo_ops);
    hashmap_iterator_t* it = NULL;
    hashmap_size_t ret;

    (void)it;
    (void)ret;

    char id[][20] = { "yj", "jy", "123", "?混搭33*&", "中文", "test" };
    for (int i = 0; i < sizeof(id) / sizeof(id[0]); ++i)
        cds->insert(&demo, _tok(id[i]), i);
    // after for [ ('?混搭33*&', 3), ('中文', 4), ('123', 2), ('yj', 0), ('test', 5), ('jy', 1) ]

    for (it = cds->begin(&demo); cds->end(&demo) != it; ) {
        if (NULL != strstr(id[1], _from(it->key)))
            it = cds->erase(&demo, it);
        else
            it = cds->next(&demo, it);
    }
    // after for [ ('?混搭33*&', 3), ('中文', 4), ('123', 2), ('yj', 0), ('test', 5) ]

    it = cds->erase(&demo, cds->prev(&demo, cds->end(&demo))); // [ ('?混搭33*&', 3), ('中文', 4), ('123', 2), ('yj', 0) ], it -> end()

    ret = cds->remove(&demo, _tok(id[0]));                     // [ ('?混搭33*&', 3), ('中文', 4), ('123', 2) ], return 1(1 element has been removed)

    ret = cds->clear(&demo);                                   // [ ], return 3(3 elements has been removed)

    HASHMAP_DEINIT(&demo);
}

static void demo_about_find(void)
{
    hashmap_t demo = HASHMAP_INIT_OPS(&demo, &demo_ops);
    hashmap_iterator_t* it = NULL;

    (void)it;

    char id[][20] = { "yj", "jy", "123", "?混搭33*&", "中文", "test" };
    for (int i = 0; i < sizeof(id) / sizeof(id[0]); ++i)
        cds->insert(&demo, _tok(id[i]), i);
    // after for [ ('?混搭33*&', 3), ('中文', 4), ('123', 2), ('yj', 0), ('test', 5), ('jy', 1) ]

    it = cds->find(&demo, _tok(id[0]));   // found, it -> ('yj', 0)
    it = cds->find(&demo, _tok("jerry")); // no found, it -> end()

    HASHMAP_DEINIT(&demo);
}

int main(void)
{
    demo_base_and_iterator();
    demo_about_insert();
    demo_about_erase();
    demo_about_find();
    return 0;
}
