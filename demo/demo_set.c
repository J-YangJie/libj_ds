/*
  Set Demos
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

#include <set/set.h>
#include <string.h>
#include <_log.h>
#include <operations/ds_ops_string.h>

#define cds cset
#define TAG "[demo_set]"

#define _tov(x)  ((set_value_t)(x))
#define _from(x) ((x) ? (char*)(x) : "null string")

#define foreach()           { for (set_iterator_t* it = cds->begin(&demo); cds->end(&demo) != it; it = cds->next(&demo, it)) pr_test("%zd", it->value); }
#define foreach_vstring()   { for (set_iterator_t* it = cds->begin(&demo); cds->end(&demo) != it; it = cds->next(&demo, it)) pr_test("%s", _from(it->value)); }
#define foreach_r_vstring() { for (set_r_iterator_t* it = cds->rbegin(&demo); cds->rend(&demo) != it; it = cds->rnext(&demo, it)) pr_test("%s", _from(it->value)); }

static class_set_ops_t demo_ops = {
    .valid_value = ds_ops_valid_data_default_string,
    .__lt_value  = __ds_ops_lt_default_string,
    .copy_value  = ds_ops_copy_data_default_string,
    .free_value  = ds_ops_free_data_default_string,
};

static void demo_base_and_iterator(void)
{
    set_t demo = SET_INIT(&demo);
    set_size_t ret;
    set_count_t count;

    (void)ret;
    (void)count;

    for (int i = 1; i <= 8; ++i)
        cds->insert(&demo, i);
    // after for [ 1, 2, 3, 4, 5, 6, 7, 8 ]

    cds->insert(&demo, 0);        // [ 0, 1, 2, 3, 4, 5, 6, 7, 8 ]

    ret   = cds->size(&demo);     // size = 9
    count = cds->count(&demo, 8); // count = 1

    foreach();                    // [ 0, 1, 2, 3, 4, 5, 6, 7, 8 ]
    pr_test("");

    {
        for (set_iterator_t* it = cds->prev(&demo, cds->end(&demo)); cds->end(&demo) != it; it = cds->prev(&demo, it)) pr_test("%zd", it->value);
        pr_test("");             // [ 8, 7, 6, 5, 4, 3, 2, 1, 0 ]
        for (set_r_iterator_t* it = cds->rbegin(&demo); cds->rend(&demo) != it; it = cds->rnext(&demo, it)) pr_test("%zd", it->value);
        pr_test("");             // [ 8, 7, 6, 5, 4, 3, 2, 1, 0 ]
        for (set_r_iterator_t* it = cds->rprev(&demo, cds->rend(&demo)); cds->rend(&demo) != it; it = cds->rprev(&demo, it)) pr_test("%zd", it->value);
        pr_test("");             // [ 0, 1, 2, 3, 4, 5, 6, 7, 8 ]
    }

    SET_DEINIT(&demo);
}

static void demo_about_insert(void)
{
    set_t demo = SET_INIT_OPS(&demo, &demo_ops);

    cds->insert(&demo, _tov("j"));
    cds->insert(&demo, _tov("and"));
    cds->insert(&demo, _tov("abc"));
    cds->insert(&demo, _tov("jerry")); // [ 'abc', 'and', 'j', 'jerry' ]

    foreach_vstring();
    pr_test("");

    foreach_r_vstring();
    pr_test("");

    SET_DEINIT(&demo);
}

static bool demo_remove_if_condition_v(ds_value_t value)
{
    return NULL != strstr(_from(value), "3");
}

static void demo_about_erase(void)
{
    set_t demo = SET_INIT_OPS(&demo, &demo_ops);
    set_iterator_t* it = NULL;
    set_size_t ret;

    (void)it;
    (void)ret;

    char id[][20] = { "yj", "jy", "123", "?混搭33*&", "中文", "test" };
    for (int i = 0; i < sizeof(id) / sizeof(id[0]); ++i)
        cds->insert(&demo, _tov(id[i]));
    // after for [ '123', '?混搭33*&', 'jy', 'test', 'yj', '中文' ]

    for (it = cds->begin(&demo); cds->end(&demo) != it; ) {
        if (NULL != strstr(id[1], _from(it->value)))
            it = cds->erase(&demo, it);
        else
            it = cds->next(&demo, it);
    }
    // after for [ '123', '?混搭33*&', 'test', 'yj', '中文' ]

    it = cds->erase(&demo, cds->prev(&demo, cds->end(&demo))); // [ '123', '?混搭33*&', 'test', 'yj' ], it -> end()

    ret = cds->remove(&demo, _tov(id[0]));                     // [ '123', '?混搭33*&', 'test' ], return 1(1 element has been removed)

    ret = cds->remove_if(&demo, demo_remove_if_condition_v);   // [ 'test' ], return 2(2 element has been removed)

    ret = cds->clear(&demo);                                   // [ ], return 1(1 elements has been removed)

    SET_DEINIT(&demo);
}

static void demo_about_find(void)
{
    set_t demo = SET_INIT(&demo);
    set_iterator_t* it = NULL;

    (void)it;

    for (int i = 1; i <= 8; ++i)
        cds->insert(&demo, i);
    // after for [ 1, 2, 3, 4, 5, 6, 7, 8 ]

    it = cds->find(&demo, 3);  // found, it -> 3
    it = cds->find(&demo, 66); // no found, it -> end()

    it = cds->lower_bound(&demo, 0); pr_test("%zd", it && cds->end(&demo) != it ? it->value : -1); // it -> 1
    it = cds->lower_bound(&demo, 6); pr_test("%zd", it && cds->end(&demo) != it ? it->value : -1); // it -> 6
    it = cds->lower_bound(&demo, 8); pr_test("%zd", it && cds->end(&demo) != it ? it->value : -1); // it -> 8
    it = cds->lower_bound(&demo, 9); pr_test("%zd", it && cds->end(&demo) != it ? it->value : -1); // it -> end()

    it = cds->upper_bound(&demo, 0); pr_test("%zd", it && cds->end(&demo) != it ? it->value : -1); // it -> 1
    it = cds->upper_bound(&demo, 6); pr_test("%zd", it && cds->end(&demo) != it ? it->value : -1); // it -> 7
    it = cds->upper_bound(&demo, 8); pr_test("%zd", it && cds->end(&demo) != it ? it->value : -1); // it -> end()
    it = cds->upper_bound(&demo, 9); pr_test("%zd", it && cds->end(&demo) != it ? it->value : -1); // it -> end()
    pr_test("");

    SET_DEINIT(&demo);
}

int main(void)
{
    demo_base_and_iterator();
    demo_about_insert();
    demo_about_erase();
    demo_about_find();
    return 0;
}
