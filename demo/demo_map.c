/*
  Map Demos
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

#include <map/map.h>
#include <string.h>
#include <_log.h>
#include <operations/ds_ops_string.h>

#define cds cmap
#define TAG "[demo_map]"

#define _tok(x)  ((map_key_t)(x))
#define _from(x) ((x) ? (char*)(x) : "null string")

#define foreach()           { for (map_iterator_t* it = cds->begin(&demo); cds->end(&demo) != it; it = cds->next(&demo, it)) pr_test("(%zd, %zd)", it->key, it->value); }
#define foreach_kstring()   { for (map_iterator_t* it = cds->begin(&demo); cds->end(&demo) != it; it = cds->next(&demo, it)) pr_test("(%s, %zd)", _from(it->key), it->value); }
#define foreach_r_kstring() { for (map_r_iterator_t* it = cds->rbegin(&demo); cds->rend(&demo) != it; it = cds->rnext(&demo, it)) pr_test("(%s, %zd)", _from(it->key), it->value); }

static class_map_ops_t demo_ops = {
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
    map_t demo = MAP_INIT(&demo);
    map_size_t ret;
    map_count_t count;

    (void)ret;
    (void)count;

    for (int i = 1; i <= 8; ++i)
        cds->insert(&demo, i, i);
    // after for [ (1, 1), (2, 2), (3, 3), (4, 4), (5, 5), (6, 6), (7, 7), (8, 8) ]

    cds->insert(&demo, 0, 0);     // [ (0, 0), (1, 1), (2, 2), (3, 3), (4, 4), (5, 5), (6, 6), (7, 7), (8, 8) ]

    ret   = cds->size(&demo);     // size = 9
    count = cds->count(&demo, 8); // count = 1

    foreach();                    // [ (0, 0), (1, 1), (2, 2), (3, 3), (4, 4), (5, 5), (6, 6), (7, 7), (8, 8) ]
    pr_test("");

    {
        for (map_iterator_t* it = cds->prev(&demo, cds->end(&demo)); cds->end(&demo) != it; it = cds->prev(&demo, it)) pr_test("(%zd, %zd)", it->key, it->value);
        pr_test("");             // [ (8, 8), (7, 7), (6, 6), (5, 5), (4, 4), (3, 3), (2, 2), (1, 1), (0, 0) ]
        for (map_r_iterator_t* it = cds->rbegin(&demo); cds->rend(&demo) != it; it = cds->rnext(&demo, it)) pr_test("(%zd, %zd)", it->key, it->value);
        pr_test("");             // [ (8, 8), (7, 7), (6, 6), (5, 5), (4, 4), (3, 3), (2, 2), (1, 1), (0, 0) ]
        for (map_r_iterator_t* it = cds->rprev(&demo, cds->rend(&demo)); cds->rend(&demo) != it; it = cds->rprev(&demo, it)) pr_test("(%zd, %zd)", it->key, it->value);
        pr_test("");             // [ (0, 0), (1, 1), (2, 2), (3, 3), (4, 4), (5, 5), (6, 6), (7, 7), (8, 8) ]
    }

    MAP_DEINIT(&demo);
}

static void demo_about_insert(void)
{
    map_t demo = MAP_INIT_OPS(&demo, &demo_ops);

    cds->insert(&demo, _tok("j"),     6);
    cds->insert(&demo, _tok("and"),   6);
    cds->insert(&demo, _tok("abc"),   6);
    cds->insert(&demo, _tok("jerry"), 6);          // [ ('abc', 6), ('and', 6), ('j', 6), ('jerry', 6) ]
    cds->insert_replace(&demo, _tok("jerry"), 99); // [ ('abc', 6), ('and', 6), ('j', 6), ('jerry', 99) ]

    foreach_kstring();
    pr_test("");

    foreach_r_kstring();
    pr_test("");

    MAP_DEINIT(&demo);
}

static bool demo_remove_if_condition_kv(ds_key_t key, ds_value_t value)
{
    return NULL != strstr(_from(key), "3");
}

static void demo_about_erase(void)
{
    map_t demo = MAP_INIT_OPS(&demo, &demo_ops);
    map_iterator_t* it = NULL;
    map_size_t ret;

    (void)it;
    (void)ret;

    char id[][20] = { "yj", "jy", "123", "?混搭33*&", "中文", "test" };
    for (int i = 0; i < sizeof(id) / sizeof(id[0]); ++i)
        cds->insert(&demo, _tok(id[i]), i);
    // after for [ ('123', 2), ('?混搭33*&', 3), ('jy', 1), ('test', 5), ('yj', 0), ('中文', 4) ]

    for (it = cds->begin(&demo); cds->end(&demo) != it; ) {
        if (NULL != strstr(id[1], _from(it->key)))
            it = cds->erase(&demo, it);
        else
            it = cds->next(&demo, it);
    }
    // after for [ ('123', 2), ('?混搭33*&', 3), ('test', 5), ('yj', 0), ('中文', 4) ]

    it = cds->erase(&demo, cds->prev(&demo, cds->end(&demo))); // [ ('123', 2), ('?混搭33*&', 3), ('test', 5), ('yj', 0) ], it -> end()

    ret = cds->remove(&demo, _tok(id[0]));                     // [ ('123', 2), ('?混搭33*&', 3), ('test', 5) ], return 1(1 element has been removed)

    ret = cds->remove_if(&demo, demo_remove_if_condition_kv);  // [ ('test', 5) ], return 2(2 element has been removed)

    ret = cds->clear(&demo);                                   // [ ], return 1(1 elements has been removed)

    MAP_DEINIT(&demo);
}

static void demo_about_find(void)
{
    map_t demo = MAP_INIT(&demo);
    map_iterator_t* it = NULL;

    (void)it;

    for (int i = 1; i <= 8; ++i)
        cds->insert(&demo, i, i);
    // after for [ (1, 1), (2, 2), (3, 3), (4, 4), (5, 5), (6, 6), (7, 7), (8, 8) ]

    it = cds->find(&demo, 3);  // found, it -> (3, 3)
    it = cds->find(&demo, 66); // no found, it -> end()

    it = cds->lower_bound(&demo, 0); pr_test("%zd", it && cds->end(&demo) != it ? it->key : -1); // it -> (1, 1)
    it = cds->lower_bound(&demo, 6); pr_test("%zd", it && cds->end(&demo) != it ? it->key : -1); // it -> (6, 6)
    it = cds->lower_bound(&demo, 8); pr_test("%zd", it && cds->end(&demo) != it ? it->key : -1); // it -> (8, 8)
    it = cds->lower_bound(&demo, 9); pr_test("%zd", it && cds->end(&demo) != it ? it->key : -1); // it -> end()

    it = cds->upper_bound(&demo, 0); pr_test("%zd", it && cds->end(&demo) != it ? it->key : -1); // it -> (1, 1)
    it = cds->upper_bound(&demo, 6); pr_test("%zd", it && cds->end(&demo) != it ? it->key : -1); // it -> (7, 7)
    it = cds->upper_bound(&demo, 8); pr_test("%zd", it && cds->end(&demo) != it ? it->key : -1); // it -> end()
    it = cds->upper_bound(&demo, 9); pr_test("%zd", it && cds->end(&demo) != it ? it->key : -1); // it -> end()
    pr_test("");

    MAP_DEINIT(&demo);
}

int main(void)
{
    demo_base_and_iterator();
    demo_about_insert();
    demo_about_erase();
    demo_about_find();
    return 0;
}
