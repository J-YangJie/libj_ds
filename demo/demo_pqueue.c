/*
  Priority Queue Demos
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

#include <priority_queue/priority_queue.h>
#include <_log.h>
#include <operations/ds_ops_string.h>

#define cds cpriority_queue
#define TAG "[demo_priority_queue]"

#define _to(x)   ((priority_queue_data_t)(x))
#define _from(x) ((x) ? (char*)(x) : "null string")

#define foreach()        { for (priority_iterator_t* it = cds->begin(&demo); cds->end(&demo) != it; it = cds->next(&demo, it)) pr_test("%zd", it->data); }
#define foreach_string() { for (priority_iterator_t* it = cds->begin(&demo); cds->end(&demo) != it; it = cds->next(&demo, it)) pr_test("%s", _from(it->data)); }

static void demo_values(void)
{
    priority_queue_t demo = PRIORITY_QUEUE_INIT(&demo);
    priority_queue_size_t ret;

    (void)ret;

    for (int i = 1; i <= 8; ++i)
        cds->push(&demo, i);
    // after for [ 8, 7, 6, 5, 4, 3, 2, 1 ]

    ret = cds->size(&demo);  // size = 8

    while (cds->size(&demo) > 0) {
        pr_test("%zd", cds->top(&demo, -1));
        cds->pop(&demo, -1);
    }
    pr_test("");

    ret = cds->clear(&demo); // [ ], return 0(0 elements has been removed)

    PRIORITY_QUEUE_DEINIT(&demo);
}

static void demo_string(void)
{
    priority_queue_t demo = PRIORITY_QUEUE_INIT_STRING(&demo);
    priority_queue_size_t ret;

    (void)ret;

    char id[][20] = { "yj", "jy", "123", "?混搭33*&", "中文", "test" };

    for (int i = 0; i < sizeof(id) / sizeof(id[0]); ++i)
        cds->push(&demo, _to(id[i]));
    // after for [ '中文', 'yj', 'test', 'jy', '?混搭33*&', '123' ]

    ret = cds->size(&demo);  // size = 6

    while (cds->size(&demo) > 0) {
        pr_test("%s", _from(cds->top(&demo, 0)));
        cds->pop(&demo, 0);
    }
    pr_test("");

    ret = cds->clear(&demo); // [ ], return 0(0 elements has been removed)

    PRIORITY_QUEUE_DEINIT(&demo);
}

int main(void)
{
    /* values */
    demo_values();

    /* strings */
    demo_string();
    return 0;
}
