/*
  List Demos
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

#include <list/list.h>
#include <_log.h>
#include <operations/ds_ops_string.h>

#define cds clist
#define TAG "[demo_list]"

#define _to(x)   ((list_data_t)(x))
#define _from(x) ((x) ? (char*)(x) : "null string")

#define foreach()          { for (list_iterator_t* it = cds->begin(&demo); cds->end(&demo) != it; it = cds->next(&demo, it)) pr_test("%zd", it->data); }
#define foreach_string()   { for (list_iterator_t* it = cds->begin(&demo); cds->end(&demo) != it; it = cds->next(&demo, it)) pr_test("%s", _from(it->data)); }
#define foreach_r_string() { for (list_r_iterator_t* it = cds->rbegin(&demo); cds->rend(&demo) != it; it = cds->rnext(&demo, it)) pr_test("%s", _from(it->data)); }

static void demo_base_and_iterator(void)
{
    list_t demo = LIST_INIT(&demo);

    for (int i = 1; i <= 8; ++i)
        cds->push_back(&demo, i);
    // after for [ 1, 2, 3, 4, 5, 6, 7, 8 ]

    cds->push_back(&demo, 8); // [ 1, 2, 3, 4, 5, 6, 7, 8, 8 ], size = 9
    cds->size(&demo);         // size = 9
    cds->count(&demo, 8);     // count(8) = 2
    cds->first(&demo, -1);    // return 1
    cds->last(&demo, -1);     // return 8

    foreach(); // [ 1, 2, 3, 4, 5, 6, 7, 8, 8 ]
    pr_test("");

    {
        for (list_iterator_t* it = cds->prev(&demo, cds->end(&demo)); cds->end(&demo) != it; it = cds->prev(&demo, it)) pr_test("%zd", it->data);       // [ 8, 8, 7, 6, 5, 4, 3, 2, 1 ]
        pr_test("");
        for (list_r_iterator_t* it = cds->rbegin(&demo); cds->rend(&demo) != it; it = cds->rnext(&demo, it)) pr_test("%zd", it->data);                  // [ 8, 8, 7, 6, 5, 4, 3, 2, 1 ]
        pr_test("");
        for (list_r_iterator_t* it = cds->rprev(&demo, cds->rend(&demo)); cds->rend(&demo) != it; it = cds->rprev(&demo, it)) pr_test("%zd", it->data); // [ 1, 2, 3, 4, 5, 6, 7, 8, 8 ]
        pr_test("");
    }

    LIST_DEINIT(&demo);
}

static void demo_about_insert(void)
{
    list_t demo = LIST_INIT(&demo);

    cds->push_back(&demo, 5);  // [ 5 ]
    cds->push_back(&demo, 6);  // [ 5, 6 ]
    cds->push_back(&demo, 7);  // [ 5, 6, 7 ]

    cds->push_front(&demo, 0); // [ 0, 5, 6, 7 ]
    cds->push_front(&demo, 1); // [ 1, 0, 5, 6, 7 ]

    cds->insert(&demo, cds->begin(&demo), 66); // [ 66, 1, 0, 5, 6, 7 ]
    cds->insert(&demo, cds->end(&demo), 66);   // [ 66, 1, 0, 5, 6, 7, 66 ]

    LIST_DEINIT(&demo);
}

static bool demo_remove_if_condition(ds_data_t data)
{
    return data >= 5;
}

static void demo_about_erase(void)
{
    list_t demo = LIST_INIT(&demo);
    list_iterator_t* it = NULL;
    list_size_t ret;

    (void)it;
    (void)ret;

    for (int i = 1; i <= 8; ++i)
        cds->push_back(&demo, i);
    // after for [ 1, 2, 3, 4, 5, 6, 7, 8 ]

    it = cds->erase_range(&demo, cds->prev(&demo, cds->end(&demo)), cds->end(&demo)); // [ 1, 2, 3, 4, 5, 6, 7 ], it -> end()

    it = cds->erase(&demo, cds->begin(&demo)); // [ 2, 3, 4, 5, 6, 7 ], it -> 2

    cds->pop_front(&demo); // [ 3, 4, 5, 6, 7 ]
    cds->pop_back(&demo);  // [ 3, 4, 5, 6 ]

    ret = cds->remove(&demo, 66); // [ 3, 4, 5, 6 ], return 0(0 elements has been removed)

    ret = cds->remove_if(&demo, demo_remove_if_condition); // [ 3, 4 ], return 2(2 elements has been removed)

    ret = cds->clear(&demo); // [ ], return 2(2 elements has been removed)

    LIST_DEINIT(&demo);
}

static void demo_about_find(void)
{
    list_t demo = LIST_INIT(&demo);
    list_iterator_t* it = NULL;

    (void)it;

    for (int i = 1; i <= 8; ++i)
        cds->push_back(&demo, i);
    // after for [ 1, 2, 3, 4, 5, 6, 7, 8 ]

    it = cds->find(&demo, 3);  // found, it -> 3
    it = cds->find(&demo, 66); // no found, it -> end()

    LIST_DEINIT(&demo);
}

static void demo_string(void)
{
    list_t demo = LIST_INIT_STRING(&demo);

    cds->push_back(&demo, _to("j"));
    cds->push_back(&demo, _to("and"));
    cds->push_back(&demo, _to("jerry")); // [ 'j', 'and', 'jerry' ]

    foreach_string();
    pr_test("");

    foreach_r_string();
    pr_test("");

    LIST_DEINIT(&demo);
}

int main(void)
{
    /* values */
    demo_base_and_iterator();
    demo_about_insert();
    demo_about_erase();
    demo_about_find();

    /* strings */
    demo_string();
    return 0;
}
