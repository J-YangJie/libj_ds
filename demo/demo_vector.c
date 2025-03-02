/*
  Vector Demos
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

#include <vector/vector.h>
#include <_log.h>
#include <operations/ds_ops_string.h>

#define cds cvector
#define TAG "[demo_vector]"

#define _to(x)   ((vector_data_t)(x))
#define _from(x) ((x) ? (char*)(x) : "null string")

#define foreach()          { for (vector_iterator_t* it = cds->begin(&demo); cds->end(&demo) != it; it = cds->next(&demo, it)) pr_test("%zd", it->data); }
#define foreach_string()   { for (vector_iterator_t* it = cds->begin(&demo); cds->end(&demo) != it; it = cds->next(&demo, it)) pr_test("%s", _from(it->data)); }
#define foreach_r_string() { for (vector_r_iterator_t* it = cds->rbegin(&demo); cds->rend(&demo) != it; it = cds->rnext(&demo, it)) pr_test("%s", _from(it->data)); }

static void demo_base_and_iterator(void)
{
    vector_t demo = VECTOR_INIT(&demo);

    for (int i = 1; i <= 8; ++i)
        cds->push_back(&demo, i);
    // after for [ 1, 2, 3, 4, 5, 6, 7, 8 ]

    cds->push_back(&demo, 8); // [ 1, 2, 3, 4, 5, 6, 7, 8, 8 ], size = 9, capacity 8 -> 16
    cds->size(&demo);         // size = 9
    cds->capacity(&demo);     // capacity = 16
    cds->count(&demo, 8);     // count(8) = 2
    cds->first(&demo, -1);    // return 1
    cds->last(&demo, -1);     // return 8

    foreach(); // [ 1, 2, 3, 4, 5, 6, 7, 8, 8 ]
    pr_test("");

    {
        for (vector_iterator_t* it = cds->prev(&demo, cds->end(&demo)); cds->end(&demo) != it; it = cds->prev(&demo, it)) pr_test("%zd", it->data);       // [ 8, 8, 7, 6, 5, 4, 3, 2, 1 ]
        pr_test("");
        for (vector_r_iterator_t* it = cds->rbegin(&demo); cds->rend(&demo) != it; it = cds->rnext(&demo, it)) pr_test("%zd", it->data);                  // [ 8, 8, 7, 6, 5, 4, 3, 2, 1 ]
        pr_test("");
        for (vector_r_iterator_t* it = cds->rprev(&demo, cds->rend(&demo)); cds->rend(&demo) != it; it = cds->rprev(&demo, it)) pr_test("%zd", it->data); // [ 1, 2, 3, 4, 5, 6, 7, 8, 8 ]
        pr_test("");
    }

    VECTOR_DEINIT(&demo);
}

static void demo_about_insert(void)
{
    vector_t demo = VECTOR_INIT(&demo);

    cds->push_back(&demo, 5);  // [ 5 ]
    cds->push_back(&demo, 6);  // [ 5, 6 ]
    cds->push_back(&demo, 7);  // [ 5, 6, 7 ]

    cds->push_front(&demo, 0); // [ 0, 5, 6, 7 ]
    cds->push_front(&demo, 1); // [ 1, 0, 5, 6, 7 ]

    cds->insert(&demo, cds->begin(&demo), 66); // [ 66, 1, 0, 5, 6, 7 ]
    cds->insert(&demo, cds->end(&demo), 66);   // [ 66, 1, 0, 5, 6, 7, 66 ]

    VECTOR_DEINIT(&demo);
}

static bool demo_remove_if_condition(ds_data_t data)
{
    return data >= 7;
}

static void demo_about_erase(void)
{
    vector_t demo = VECTOR_INIT(&demo);
    vector_iterator_t* it = NULL;
    vector_size_t ret;

    (void)it;
    (void)ret;

    for (int i = 1; i <= 8; ++i)
        cds->push_back(&demo, i);
    // after for [ 1, 2, 3, 4, 5, 6, 7, 8 ]

    it = cds->erase_range(&demo, cds->at(&demo, 2), cds->at(&demo, 5)); // [ 1, 2, 6, 7, 8 ], it -> 6

    it = cds->erase(&demo, cds->begin(&demo)); // [ 2, 6, 7, 8 ], it -> 2

    cds->pop_front(&demo); // [ 6, 7, 8 ]
    cds->pop_back(&demo);  // [ 6, 7 ]

    ret = cds->remove(&demo, 66); // [ 6, 7 ], return 0(0 elements has been removed)

    ret = cds->remove_if(&demo, demo_remove_if_condition); // [ 6 ], return 1(1 elements has been removed)

    ret = cds->clear(&demo); // [ ], return 1(1 elements has been removed)

    VECTOR_DEINIT(&demo);
}

static void demo_about_find(void)
{
    vector_t demo = VECTOR_INIT(&demo);
    vector_iterator_t* it = NULL;

    (void)it;

    for (int i = 1; i <= 8; ++i)
        cds->push_back(&demo, i);
    // after for [ 1, 2, 3, 4, 5, 6, 7, 8 ]

    it = cds->find(&demo, 3);  // found, it -> 3
    it = cds->find(&demo, 66); // no found, it -> end()

    VECTOR_DEINIT(&demo);
}

static void demo_reserve_and_resize_and_sort(void)
{
    vector_t demo = VECTOR_INIT(&demo);
    vector_size_t ret;

    (void)ret;

    cds->push_back(&demo, 65);
    cds->push_back(&demo, 67);
    cds->push_back(&demo, 69); // [ 65, 67, 69 ], capacity = 4

    cds->reserve(&demo, 5);    // [ 65, 67, 69 ], capacity = 5

    cds->push_back(&demo, 1);  // [ 65, 67, 69, 1 ], capacity = 5
    cds->push_back(&demo, 3);  // [ 65, 67, 69, 1, 3 ], capacity = 5
    cds->push_back(&demo, 5);  // [ 65, 67, 69, 1, 3, 5 ], capacity 5 -> 10

    cds->resize(&demo, 8, 66); // [ 65, 67, 69, 1, 3, 5, 66, 66 ], size = 8, capacity = 10

    cds->sort(&demo, __ds_ops_gt_default); // [ 69, 67, 66, 66, 65, 5, 3, 1 ], size = 8, capacity = 10

    cds->resize(&demo, 3, 66); // [ 69, 67, 66 ], size = 3, capacity = 10

    VECTOR_DEINIT(&demo);
}

static void demo_string(void)
{
    vector_t demo = VECTOR_INIT_STRING(&demo);

    cds->push_back(&demo, _to("j"));
    cds->push_back(&demo, _to("and"));
    cds->push_back(&demo, _to("jerry")); // [ 'j', 'and', 'jerry' ]

    foreach_string();
    pr_test("");

    foreach_r_string();
    pr_test("");

    VECTOR_DEINIT(&demo);
}

int main(void)
{
    /* values */
    demo_base_and_iterator();
    demo_about_insert();
    demo_about_erase();
    demo_about_find();
    demo_reserve_and_resize_and_sort();

    /* strings */
    demo_string();
    return 0;
}
