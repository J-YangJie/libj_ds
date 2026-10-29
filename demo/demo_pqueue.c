/*
  Priority Queue Demos
  Copyright (C) 2021  YangJie <yangjie98765@yeah.net>
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

#include <string.h>

#include <priority_queue/priority_queue.h>
#include <_log.h>
#include <operations/ds_ops_string.h>

#include "demo_struct.h"

#define cds cpriority_queue
#define TAG "[demo_priority_queue]"

#define _to(x)   ((priority_queue_data_t)(x))

static void demo_values(void)
{
    priority_queue_t* demo  = PRIORITY_QUEUE_NEW_T(int32_t, VECTOR); /* PRIORITY_QUEUE_NEW_T(int_32_t, DEQUE) */
    priority_queue_t* other = PRIORITY_QUEUE_NEW_T(int32_t, VECTOR); /* PRIORITY_QUEUE_NEW_T(int_32_t, DEQUE) */
    priority_queue_size_t ret; (void)ret;

    for (int i = -6; i <= 8; i += 2) {
        cds->push(demo, i);
        cds->push(other, i);
    }
    // after for [ 8, 6, 4, 2, 0, -2, -4, -6 ]

    for (int i = -5; i <= 8; i += 2) {
        cds->push(demo, i);
        cds->push(other, i);
    }
    // after for [ 8, 7, 6, 5, 4, 3, 2, 1, 0, -1, -2, -3, -4, -5, -6, -7, -8 ]

    ret = cds->size(demo);   // size = 8
    ret = cds->size(other);  // size = 8

    while (!cds->__empty(demo)) {
        pr_test("%d", *cds->top(demo).s32);
        cds->pop(demo);
    }
    pr_test("");

    pr_test("Size before swap, demo [ %zd ], other [ %zd ]", cds->size(demo), cds->size(other));  // demo size = 0, other size = 8
    cds->swap(demo, other);
    pr_test("Size after  swap, demo [ %zd ], other [ %zd ]", cds->size(demo), cds->size(other));  // demo size = 8, other size = 0

    ret = cds->clear(demo);  // [ ], return 8(8 elements has been removed)
    pr_test("Clear [ %zd ] elements from demo", ret);
    ret = cds->clear(other); // [ ], return 0(0 elements has been removed)
    pr_test("Clear [ %zd ] elements from other", ret);

    PRIORITY_QUEUE_DELETE(&demo);
    PRIORITY_QUEUE_DELETE(&other);
}

static void demo_string(void)
{
    priority_queue_t* demo1 = PRIORITY_QUEUE_NEW_SSO_T(VECTOR);    /* PRIORITY_QUEUE_NEW_SSO_T(DEQUE) */
    priority_queue_t* demo2 = PRIORITY_QUEUE_NEW_CHAR_T(VECTOR);   /* PRIORITY_QUEUE_NEW_CHAR_T(DEQUE) */
    priority_queue_t* demo3 = PRIORITY_QUEUE_NEW_STRING_T(VECTOR); /* PRIORITY_QUEUE_NEW_STRING_T(DEQUE) */
    priority_queue_size_t ret; (void)ret;

    char id[][20] = { "yj", "jy", "123", "?混搭33*&", "中文", "test" };

    for (int i = 0; i < sizeof(id) / sizeof(id[0]); ++i) {
        cds->push(demo1, _to(id[i]));
        cds->push(demo2, _to(id[i]));
        cds->push(demo3, _to(id[i]));
    }
    // after for [ '中文', 'yj', 'test', 'jy', '?混搭33*&', '123' ]

    ret = cds->size(demo1);  // size = 6

    while (!cds->__empty(demo1)) {
        pr_test("[ %s | %s | %s ]", *cds->top(demo1).sd, *cds->top(demo2).sd, *cds->top(demo3).sd);
        pr_test("sso len [ %zu ], char* len [ %zu ]", cds->top(demo1).ssd->len, strlen(*cds->top(demo2).sd)); // string_sso len by itself, string_char* len by strlen
        cds->pop(demo1);
        cds->pop(demo2);
        cds->pop(demo3);
    }

    ret = cds->clear(demo1); // [ ], return 0(0 elements has been removed)
    ret = cds->clear(demo2); // [ ], return 0(0 elements has been removed)
    ret = cds->clear(demo3); // [ ], return 0(0 elements has been removed)

    PRIORITY_QUEUE_DELETE(&demo1);
    PRIORITY_QUEUE_DELETE(&demo2);
    PRIORITY_QUEUE_DELETE(&demo3);
}

static class_priority_queue_ops_t demo_struct_node_ops = {
    .valid_data = demo_struct_valid,
    .__lt       = __demo_struct_gt,
    .copy_data  = demo_struct_node_copy,
    .free_data  = demo_struct_node_free,
    .__move     = __demo_struct_node_move,
};

static void demo_struct_node(void)
{
                          /* PRIORITY_QUEUE_NEW_OPS_COMPLATE(&demo_struct_node_ops, SEQ_CNTR_DTYPE_STRUCT, demo_struct_t, DEQUE, 0) */
    priority_queue_t* demo = PRIORITY_QUEUE_NEW_OPS_COMPLATE(&demo_struct_node_ops, SEQ_CNTR_DTYPE_STRUCT, demo_struct_t, VECTOR, 0);
    priority_queue_size_t ret; (void)ret;

    int ids[] = { 5, 1, 4, 2, 1 };
    char* names[] = { "e", "b", "d", "c", "a" };

    for (int i = 0; i < sizeof(ids) / sizeof(ids[0]); ++i) {
        demo_struct_t item;

        if (__demo_struct_init(&item, ids[i], names[i], names[i])) {
            if (!cds->push(demo, _to(&item)))
                demo_struct_deinit(&item);
        }
    }

    ret = cds->size(demo);  // size = 5

    while (!cds->__empty(demo)) {
        demo_struct_t* top = (demo_struct_t*)cds->top(demo).d;

        pr_test("id [ %d ] name1 [ %s ] name2 [ %s ] sub_struct->a [ %d ] p ok [ %d ]",
                top->id, top->name1.p, top->name2, top->sub_struct->a, top->p == &top->c);
        cds->pop(demo);
    }

    cds->clear(demo);

    PRIORITY_QUEUE_DELETE(&demo);
}

static class_priority_queue_ops_t demo_struct_heap_ops = {
    .valid_data = demo_struct_valid,
    .__lt       = __demo_struct_gt,
    .copy_data  = demo_struct_heap_copy,
    .free_data  = demo_struct_heap_free,
    .__move     = NULL,
};

static void demo_struct_heap(void)
{
                          /* PRIORITY_QUEUE_NEW_OPS_COMPLATE(&demo_struct_heap_ops, SEQ_CNTR_DTYPE_PTR, demo_struct_t*, DEQUE, 0) */
    priority_queue_t* demo = PRIORITY_QUEUE_NEW_OPS_COMPLATE(&demo_struct_heap_ops, SEQ_CNTR_DTYPE_PTR, demo_struct_t*, VECTOR, 0);
    priority_queue_size_t ret; (void)ret;

    int ids[] = { 5, 1, 4, 2, 1 };
    char* names[] = { "e", "b", "d", "c", "a" };

    for (int i = 0; i < sizeof(ids) / sizeof(ids[0]); ++i) {
        demo_struct_t item;

        if (__demo_struct_init(&item, ids[i], names[i], names[i])) {
            if (!cds->push(demo, _to(&item)))
                demo_struct_deinit(&item);
        }
    }

    ret = cds->size(demo);  // size = 5

    while (!cds->__empty(demo)) {
        demo_struct_t* top = (demo_struct_t*)(intptr_t)*cds->top(demo).d;

        pr_test("id [ %d ] name1 [ %s ] name2 [ %s ] sub_struct->a [ %d ] p ok [ %d ]",
                top->id, top->name1.p, top->name2, top->sub_struct->a, top->p == &top->c);
        cds->pop(demo);
    }

    cds->clear(demo);

    PRIORITY_QUEUE_DELETE(&demo);
}

int main(void)
{
    /* values */
    demo_values();
    pr_test("");

    /* strings */
    demo_string();
    pr_test("");

    /* struct */
    demo_struct_node();
    pr_test("");
    demo_struct_heap();
    return 0;
}
