/*
  Stack Demos
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

#include <stack/stack.h>
#include <_log.h>
#include <operations/ds_ops_string.h>
#include "demo_struct.h"

#define cds cstack
#define TAG "[demo_stack]"

#define _to(x)   ((stack_data_t)(x))

/*
    Numbers

    1. Signed and unsigned 1/2/4-byte integers are supported; see the
       `sequence_container_data_type_t` enumerators. 8-byte signed/unsigned
       integers are supported on 64-bit builds.
    2. Numbers are stored directly in the element.
    3. ds_data_t is always pointer-sized and integer-typed. For float,
       double, and int64_t/uint64_t on 32-bit builds, store the value
       inside a struct to avoid precision loss from casts (or use another
       representation that does not lose precision).
 */
static
void demo_values(void)
{
    stack_t* demo  = STACK_NEW_T(int32_t, VECTOR); /* STACK_NEW_T(int32_t, DEQUE/LIST) */
    stack_t* other = STACK_NEW_T(int32_t, VECTOR); /* STACK_NEW_T(int32_t, DEQUE/LIST) */
    stack_size_t ret; (void)ret;

    for (int i = -6; i <= 8; i += 2) {
        cds->push(demo, i);
        cds->push(other, i);
    }
    // after for [ -6, -4, -2, 0, 2, 4, 6, 8 ]

    for (int i = -5; i <= 8; i += 2) {
        cds->push(demo, i);
        cds->push(other, i);
    }
    // after for [ -6, -4, -2, 0, 2, 4, 6, 8, -5, -3, -1, 1, 3, 5, 7 ]

    ret = cds->size(demo);   // size = 15
    ret = cds->size(other);  // size = 15

    while (!cds->__empty(demo)) {
        pr_test("%d", *cds->top(demo).s32);
        cds->pop(demo);
    }
    pr_test("");

    pr_test("Size before swap, demo [ %zd ], other [ %zd ]", cds->size(demo), cds->size(other));  // demo size = 0, other size = 15
    cds->swap(demo, other);
    pr_test("Size after  swap, demo [ %zd ], other [ %zd ]", cds->size(demo), cds->size(other));  // demo size = 15, other size = 0

    ret = cds->clear(demo);  // [ ], return 15(15 elements has been removed)
    pr_test("Clear [ %zd ] elements from demo", ret);
    ret = cds->clear(other); // [ ], return 0(0 elements has been removed)
    pr_test("Clear [ %zd ] elements from other", ret);

    STACK_DELETE(&demo);
    STACK_DELETE(&other);
}

/*
    Strings

    1. STACK_NEW_CHAR_T wraps char* and does not store the string length.
    2. STACK_NEW_SSO_T is the optimized string form for sequence
       containers (vector/deque/list) and container adaptors
       (queue/stack/priority_queue); SSO stores the string length.
    3. STACK_NEW_CHAR_T and STACK_NEW_SSO_T are interchangeable without
       changing the code (as long as you do not use the SSO length field).
       SSO just gives O(1) access to the length instead of an O(n) strlen.
 */
static
void demo_string(void)
{
    stack_t* demo1 = STACK_NEW_SSO_T(VECTOR);    /* STACK_NEW_SSO_T(DEQUE/LIST) */
    stack_t* demo2 = STACK_NEW_CHAR_T(VECTOR);   /* STACK_NEW_CHAR_T(DEQUE/LIST) */
    stack_t* demo3 = STACK_NEW_STRING_T(VECTOR); /* STACK_NEW_STRING_T(DEQUE/LIST) */
    stack_size_t ret; (void)ret;

    char id[][20] = { "yj", "jy", "123", "?混搭33*&", "中文", "test" };

    for (int i = 0; i < sizeof(id) / sizeof(id[0]); ++i) {
        cds->push(demo1, _to(id[i]));
        cds->push(demo2, _to(id[i]));
        cds->push(demo3, _to(id[i]));
    }
    // after for [ 'yj', 'jy', '123', '?混搭33*&', '中文', 'test' ]

    ret = cds->size(demo1);  // size = 6

    while (!cds->__empty(demo1)) {
        pr_test("[ ( %s or %s ) | %s | %s ]", *cds->top(demo1).sd, cds->top(demo1).ssd->p, *cds->top(demo2).sd, *cds->top(demo3).sd);
        pr_test("sso len [ %zu ], char* len [ %zu ]", cds->top(demo1).ssd->len, strlen(*cds->top(demo2).sd)); // string_sso len by itself, string_char* len by strlen
        cds->pop(demo1);
        cds->pop(demo2);
        cds->pop(demo3);
    }

    ret = cds->clear(demo1); // [ ], return 0(0 elements has been removed)
    ret = cds->clear(demo2); // [ ], return 0(0 elements has been removed)
    ret = cds->clear(demo3); // [ ], return 0(0 elements has been removed)

    STACK_DELETE(&demo1);
    STACK_DELETE(&demo2);
    STACK_DELETE(&demo3);
}

/*
    Node mode

    1. The element stores a demo_struct_t object, like
       std::vector<demo_class> in STL.
    2. demo_struct_t lives inside the element; no extra allocation is
       needed for the object itself.
    3. The caller builds the object, and ops->copy_data shallow-copies it
       into the element.
 */
static class_stack_ops_t demo_struct_node_ops = {
    .valid_data = demo_struct_valid,
    .__eq       = __demo_struct_eq,
    .copy_data  = demo_struct_node_copy,
    .free_data  = demo_struct_node_free,
    .__move     = __demo_struct_node_move,
};

static
void demo_struct_node(void)
{
                 /* STACK_NEW_OPS_COMPLATE(&demo_struct_node_ops, SEQ_CNTR_DTYPE_STRUCT, demo_struct_t, DEQUE/LIST, 0) */
    stack_t* demo = STACK_NEW_OPS_COMPLATE(&demo_struct_node_ops, SEQ_CNTR_DTYPE_STRUCT, demo_struct_t, VECTOR, 0);
    stack_size_t ret; (void)ret;

    int ids[] = { 5, 1, 4, 2, 1 };
    char* names[] = { "e", "b", "d", "c", "a" };

    for (int i = 0; i < sizeof(ids) / sizeof(ids[0]); ++i) {
        demo_struct_t obj;

        if (__demo_struct_constructor(&obj, ids[i], names[i], names[i])) {
            if (!cds->push(demo, _to(&obj)))
                demo_struct_destructor(&obj);
        }
    }

    ret = cds->size(demo);  // size = 5

    while (!cds->__empty(demo)) {
        demo_struct_t* top = (demo_struct_t*)cds->top(demo).d;

        pr_test("ID [ %d ] name1 [ %s ] name2 [ %s ] sub_struct->a [ %d ] p ok [ %d ]",
                top->id, top->name1.p, top->name2, top->sub_struct->a, top->p == &top->c);
        cds->pop(demo);
    }

    ret = cds->clear(demo);

    STACK_DELETE(&demo);
}

/*
    Heap mode 1

    1. The element stores a pointer, like std::vector<demo_class*> in STL.
    2. The memory for demo_struct_t is allocated in ops->copy_data.
    3. The caller builds the object, and it is shallow-copied into the
       memory allocated by ops->copy_data.
 */
static class_stack_ops_t demo_struct_heap1_ops = {
    .valid_data = demo_struct_valid,
    .__eq       = __demo_struct_eq,
    .copy_data  = demo_struct_heap_shallow_copy,
    .free_data  = demo_struct_heap_free,
    .__move     = NULL,
};

static
void demo_struct_heap1(void)
{
                 /* STACK_NEW_OPS_COMPLATE(&demo_struct_heap1_ops, SEQ_CNTR_DTYPE_PTR, demo_struct_t*, DEQUE/LIST, 0) */
    stack_t* demo = STACK_NEW_OPS_COMPLATE(&demo_struct_heap1_ops, SEQ_CNTR_DTYPE_PTR, demo_struct_t*, VECTOR, 0);
    stack_size_t ret; (void)ret;

    int ids[] = { 5, 1, 4, 2, 1 };
    char* names[] = { "e", "b", "d", "c", "a" };

    for (int i = 0; i < sizeof(ids) / sizeof(ids[0]); ++i) {
        demo_struct_t obj;

        if (__demo_struct_constructor(&obj, ids[i], names[i], names[i])) {
            if (!cds->push(demo, _to(&obj)))
                demo_struct_destructor(&obj);
        }
    }

    ret = cds->size(demo);  // size = 5

    while (!cds->__empty(demo)) {
        demo_struct_t* top = (demo_struct_t*)(intptr_t)*cds->top(demo).d;

        pr_test("ID [ %d ] name1 [ %s ] name2 [ %s ] sub_struct->a [ %d ] p ok [ %d ]",
                top->id, top->name1.p, top->name2, top->sub_struct->a, top->p == &top->c);
        cds->pop(demo);
    }

    ret = cds->clear(demo);

    STACK_DELETE(&demo);
}

/*
    Heap mode 2

    1. The element stores a pointer, like std::vector<demo_class*> in STL.
    2. demo_struct_t is allocated outside the data structure, so
       ops->copy_data is NULL.
    3. The caller builds the object pointed to by p, and the address
       held by p is stored as a number in the element.
 */
static class_stack_ops_t demo_struct_heap2_ops = {
    .valid_data = demo_struct_valid,
    .__eq       = __demo_struct_eq,
    .copy_data  = NULL,
    .free_data  = demo_struct_heap_free,
    .__move     = NULL,
};

static void demo_struct_heap2(void)
{
                 /* STACK_NEW_OPS_COMPLATE(&demo_struct_heap2_ops, SEQ_CNTR_DTYPE_PTR, demo_struct_t*, DEQUE/LIST, 0) */
    stack_t* demo = STACK_NEW_OPS_COMPLATE(&demo_struct_heap2_ops, SEQ_CNTR_DTYPE_PTR, demo_struct_t*, VECTOR, 0);
    stack_size_t ret; (void)ret;

    int ids[] = { 5, 1, 4, 2, 1 };
    char* names[] = { "e", "b", "d", "c", "a" };

    for (int i = 0; i < sizeof(ids) / sizeof(ids[0]); ++i) {
        demo_struct_t* p = (demo_struct_t*)malloc(sizeof(demo_struct_t));

        if (!p)
            continue;

        if (__demo_struct_constructor(p, ids[i], names[i], names[i])) {
            if (!cds->push(demo, _to(p))) {
                demo_struct_destructor(p);
                free(p);
            }
        }
    }

    ret = cds->size(demo);  // size = 5

    while (!cds->__empty(demo)) {
        demo_struct_t* top = (demo_struct_t*)(intptr_t)*cds->top(demo).d;

        pr_test("ID [ %d ] name1 [ %s ] name2 [ %s ] sub_struct->a [ %d ] p ok [ %d ]",
                top->id, top->name1.p, top->name2, top->sub_struct->a, top->p == &top->c);
        cds->pop(demo);
    }

    ret = cds->clear(demo);

    STACK_DELETE(&demo);
}

#if JDSC_ALIGNMEM
/*
    Aligned node mode

    demo_align_struct_t is 512-byte aligned and stored directly in the
    node. Build with WITH_ALIGNMEM=y (JDSC_ALIGNMEM=1) if you want the
    container to allocate 512-byte-aligned nodes; JDSC_ALIGNMEM=0 only
    guarantees max_align_t.
 */
static class_stack_ops_t demo_align_node_ops = {
    .valid_data = NULL,
    .__eq       = __demo_align_struct_eq,
    .copy_data  = demo_align_struct_copy,
    .free_data  = demo_align_struct_free,
    .__move     = NULL,
};

static void demo_align_struct_node(void)
{
    const size_t size  = sizeof(demo_align_struct_t);
    const size_t align = (size_t)JDSC_ALIGNOF(demo_align_struct_t);

    pr_test("Align struct, sizeof [ %zu ] alignof [ %zu ]", size, align);

                 /* STACK_NEW_OPS_COMPLATE(&demo_align_node_ops, SEQ_CNTR_DTYPE_STRUCT, demo_align_struct_t, DEQUE/LIST, 0) */
    stack_t* demo = STACK_NEW_OPS_COMPLATE(&demo_align_node_ops, SEQ_CNTR_DTYPE_STRUCT, demo_align_struct_t, VECTOR, 0);
    stack_size_t ret; (void)ret;

    demo_align_struct_t objs[] = {
        { 'd', 4, 40, 400 },
        { 'b', 2, 20, 200 },
        { 'a', 1, 10, 100 },
        { 'c', 3, 30, 300 },
    };

    for (int i = 0; i < sizeof(objs) / sizeof(objs[0]); ++i)
        cds->push(demo, _to(&objs[i]));

    ret = cds->size(demo);  // size = 4

    while (!cds->__empty(demo)) {
        demo_align_struct_t* top = (demo_align_struct_t*)cds->top(demo).d;

        pr_test("Element: a [ %c ] b [ %d ] c [ %d ] d [ %ld ] addr [ %p ] %% 512 [ %zu ]",
                top->a, top->b, top->c, top->d, top,
                (size_t)((uintptr_t)top % 512));
        cds->pop(demo);
    }

    ret = cds->clear(demo);

    STACK_DELETE(&demo);
}
#endif /* JDSC_ALIGNMEM */

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
    demo_struct_heap1();
    pr_test("");
    demo_struct_heap2();
    pr_test("");

#if JDSC_ALIGNMEM
    /* aligned struct */
    demo_align_struct_node();
#endif /* JDSC_ALIGNMEM */
    return 0;
}
