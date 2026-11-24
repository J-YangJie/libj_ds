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

#include <priority_queue/priority_queue.h>
#include <string.h>
#include <_log.h>
#include <operations/ds_ops_string.h>
#include "demo_struct.h"

#define cds cpriority_queue
#define TAG "[demo_priority_queue]"

/*
 *  Numbers
 *
 *  1. PRIORITY_QUEUE_NEW_T(_type, _BASE_NAME) selects the element width and
 *     the matching preset ops. Use the matching `ds_*data` constructor with
 *     `push`, and read the value back through a field such as
 *     `top(...).s32`.
 *  2. Use `ds_u64data`/`ds_s64data` for `uint64_t`/`int64_t`,
 *     `ds_fdata`/`ds_lfdata` for floating-point values, and `ds_sdata`
 *     for strings.
 *  3. Other integer types may be passed directly; their `ds_*data`
 *     constructors are optional.
 *  4. Without explicit ops, the default comparison is (left < right), which
 *     builds a max-heap. For a min-heap, use
 *     PRIORITY_QUEUE_NEW_T_GT(int32_t, VECTOR); it is equivalent to
 *     std::priority_queue<int32_t, std::vector<int32_t>,
 *     std::greater<int32_t>> in STL.
 */
#if 0
static
bool __num_gt(priority_queue_data_t left, priority_queue_data_t right)
{
    /* return (uint8_t)left > (uint8_t)right; */
    return (int32_t)left > (int32_t)right;
}

static class_priority_queue_ops_t demo_struct_num_greater_ops = {
    .valid_data = NULL,
    .__lt       = __num_gt,
    .copy_data  = NULL,
    .free_data  = NULL,
    .__move     = NULL,
};
#endif

static
void demo_values(void)
{
    priority_queue_t* demo  = PRIORITY_QUEUE_NEW_T(int32_t, VECTOR); /* PRIORITY_QUEUE_NEW_T(int32_t, DEQUE) */
    priority_queue_t* other = PRIORITY_QUEUE_NEW_T(int32_t, VECTOR); /* PRIORITY_QUEUE_NEW_T(int32_t, DEQUE) */
    priority_queue_size_t ret; (void)ret;

    for (int32_t i = -6; i <= 8; i += 2) {
        cds->push(demo, i); /* cds->push(demo, ds_s32data(i)); */
        cds->push(other, i);
    }
    // pop order after for [ 8, 6, 4, 2, 0, -2, -4, -6 ]

    for (int i = -5; i <= 8; i += 2) {
        cds->push(demo, i);
        cds->push(other, i);
    }
    // pop order after for [ 8, 7, 6, 5, 4, 3, 2, 1, 0, -1, -2, -3, -4, -5, -6 ]

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

    PRIORITY_QUEUE_DELETE(&demo);
    PRIORITY_QUEUE_DELETE(&other);
}

/*
 *  Low-level interfaces
 *
 *  `__`-prefixed methods skip parameter checks. Use them only when the
 *  priority queue is already known to be non-empty.
 */
static void demo_low_level_api(void)
{
    priority_queue_t* demo = PRIORITY_QUEUE_NEW_T(int32_t, VECTOR); /* PRIORITY_QUEUE_NEW_T(int32_t, DEQUE) */

    cds->push(demo, 1);
    cds->push(demo, 3);
    cds->push(demo, 2);
    // [ 1, 3, 2 ] pushed, top = 3 in the default max-heap

    pr_test("__empty [ %d ], __top [ %d ]", cds->__empty(demo), *cds->__top(demo).s32);

    PRIORITY_QUEUE_DELETE(&demo);
}

/*
 *  Strings
 *
 *  1. PRIORITY_QUEUE_NEW_CHAR_T wraps char* and does not store the string length.
 *  2. PRIORITY_QUEUE_NEW_SSO_T is the optimized string form for sequence
 *     containers (vector/deque/list) and container adaptors
 *     (queue/stack/priority_queue); SSO stores the string length.
 *  3. PRIORITY_QUEUE_NEW_CHAR_T and PRIORITY_QUEUE_NEW_SSO_T are interchangeable without
 *     changing the code (as long as you do not use the SSO length field).
 *     SSO just gives O(1) access to the length instead of an O(n) strlen.
 */
static
void demo_string(void)
{
    priority_queue_t* demo1 = PRIORITY_QUEUE_NEW_SSO_T(VECTOR);    /* PRIORITY_QUEUE_NEW_SSO_T(DEQUE) */
    priority_queue_t* demo2 = PRIORITY_QUEUE_NEW_CHAR_T(VECTOR);   /* PRIORITY_QUEUE_NEW_CHAR_T(DEQUE) */
    priority_queue_t* demo3 = PRIORITY_QUEUE_NEW_STRING_T(VECTOR); /* PRIORITY_QUEUE_NEW_STRING_T(DEQUE) */
    priority_queue_size_t ret; (void)ret;

    char id[][20] = { "yj", "jy", "123", "?混搭33*&", "中文", "test" };

    for (int i = 0; i < sizeof(id) / sizeof(id[0]); ++i) {
        cds->push(demo1, ds_sdata(id[i]));
        cds->push(demo2, ds_sdata(id[i]));
        cds->push(demo3, ds_sdata(id[i]));
    }
    // pop order after for [ '中文', 'yj', 'test', 'jy', '?混搭33*&', '123' ]

    ret = cds->size(demo1);  // size = 6

    while (!cds->__empty(demo1)) {
        pr_test("[ ( \"%s\" or \"%s\" ) | \"%s\" | \"%s\" ]", *cds->top(demo1).sd, cds->top(demo1).ssd->p, *cds->top(demo2).sd, *cds->top(demo3).sd);
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

/*
 *  Node mode
 *
 *  1. The element stores a demo_struct_t object, like
 *     std::priority_queue<demo_class> in STL.
 *  2. demo_struct_t lives inside the element; no extra allocation is
 *     needed for the object itself.
 *  3. The caller builds the object, and ops->copy_data deep-copies it
 *     into the element.
 *  4. `.__lt` is set to a greater callback, so `top()` returns the
 *     smallest id, like a min-heap.
 */
static class_priority_queue_ops_t demo_struct_node_ops = {
    .valid_data = demo_struct_node_valid,
    .__lt       = __demo_struct_node_gt,
    .copy_data  = demo_struct_node_deep_copy,
    .free_data  = demo_struct_node_free,
    .__move     = __demo_struct_node_move,
};

static
void demo_struct_node(void)
{
                          /* PRIORITY_QUEUE_NEW_OPS_COMPLATE(&demo_struct_node_ops, SEQ_CNTR_DTYPE_STRUCT, demo_struct_t, DEQUE, 0) */
    priority_queue_t* demo = PRIORITY_QUEUE_NEW_OPS_COMPLATE(&demo_struct_node_ops, SEQ_CNTR_DTYPE_STRUCT, demo_struct_t, VECTOR, 0);
    priority_queue_size_t ret; (void)ret;

    int ids[] = { 5, 1, 4, 2, 1 };
    char* names[] = { "e", "b", "d", "c", "a" };

    for (int i = 0; i < sizeof(ids) / sizeof(ids[0]); ++i) {
        demo_struct_t obj;

        if (__demo_struct_constructor(&obj, ids[i], names[i], names[i])) {
            cds->push(demo, ds_cstdata(&obj));
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

    PRIORITY_QUEUE_DELETE(&demo);
}

/*
 *  Heap mode
 *
 *  1. The element stores a pointer, like std::priority_queue<demo_class*>
 *     in STL.
 *  2. The memory for demo_struct_t is allocated in ops->copy_data.
 *  3. The caller builds the object, and it is deep-copied into the
 *     memory allocated by ops->copy_data.
 *  4. The same greater callback as node mode is used, so `top()` returns
 *     the smallest id.
 */
static class_priority_queue_ops_t demo_struct_heap_ops = {
    .valid_data = demo_struct_heap_valid,
    .__lt       = __demo_struct_heap_gt,
    .copy_data  = demo_struct_heap_deep_copy,
    .free_data  = demo_struct_heap_free,
    .__move     = NULL,
};

static
void demo_struct_heap(void)
{
                          /* PRIORITY_QUEUE_NEW_OPS_COMPLATE(&demo_struct_heap_ops, SEQ_CNTR_DTYPE_PTR, demo_struct_t*, DEQUE, 0) */
    priority_queue_t* demo = PRIORITY_QUEUE_NEW_OPS_COMPLATE(&demo_struct_heap_ops, SEQ_CNTR_DTYPE_PTR, demo_struct_t*, VECTOR, 0);
    priority_queue_size_t ret; (void)ret;

    int ids[] = { 5, 1, 4, 2, 1 };
    char* names[] = { "e", "b", "d", "c", "a" };

    for (int i = 0; i < sizeof(ids) / sizeof(ids[0]); ++i) {
        demo_struct_t obj;

        if (__demo_struct_constructor(&obj, ids[i], names[i], names[i])) {
            cds->push(demo, ds_cstdata(&obj));
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

    PRIORITY_QUEUE_DELETE(&demo);
}

#if JDSC_ALIGNMEM
/*
 *  Aligned node mode
 *
 *  demo_align_struct_t is 512-byte aligned and stored directly in the
 *  node. Build with WITH_ALIGNMEM=y (JDSC_ALIGNMEM=1) if you want the
 *  container to allocate 512-byte-aligned nodes; JDSC_ALIGNMEM=0 only
 *  guarantees max_align_t.
 */
static class_priority_queue_ops_t demo_align_node_ops = {
    .valid_data = NULL,
    .__lt       = __demo_align_struct_node_gt,
    .copy_data  = demo_align_struct_node_copy,
    .free_data  = demo_align_struct_node_free,
    .__move     = NULL,
};

static void demo_align_struct_node(void)
{
    const size_t size  = sizeof(demo_align_struct_t);
    const size_t align = (size_t)JDSC_ALIGNOF(demo_align_struct_t);

    pr_test("Align struct, sizeof [ %zu ] alignof [ %zu ]", size, align);

                          /* PRIORITY_QUEUE_NEW_OPS_COMPLATE(&demo_align_node_ops, SEQ_CNTR_DTYPE_STRUCT, demo_align_struct_t, DEQUE, 0) */
    priority_queue_t* demo = PRIORITY_QUEUE_NEW_OPS_COMPLATE(&demo_align_node_ops, SEQ_CNTR_DTYPE_STRUCT, demo_align_struct_t, VECTOR, 0);
    priority_queue_size_t ret; (void)ret;

    demo_align_struct_t objs[] = {
        { 'd', 4, 40, 400 },
        { 'b', 2, 20, 200 },
        { 'a', 1, 10, 100 },
        { 'c', 3, 30, 300 },
    };

    for (int i = 0; i < sizeof(objs) / sizeof(objs[0]); ++i)
        cds->push(demo, ds_cstdata(&objs[i]));

    ret = cds->size(demo);  // size = 4

    while (!cds->__empty(demo)) {
        demo_align_struct_t* top = (demo_align_struct_t*)cds->top(demo).d;

        pr_test("Element: a [ %c ] b [ %d ] c [ %d ] d [ %ld ] addr [ %p ] %% 512 [ %zu ]",
                top->a, top->b, top->c, top->d, top,
                (size_t)((uintptr_t)top % 512));
        cds->pop(demo);
    }

    ret = cds->clear(demo);

    PRIORITY_QUEUE_DELETE(&demo);
}

/*
 *  Aligned heap mode
 *
 *  demo_align_struct_t is 512-byte aligned, and ops->copy_data
 *  allocates it on the heap; the slot stores the pointer.
 *  Build with WITH_ALIGNMEM=y (JDSC_ALIGNMEM=1) if you want the
 *  element allocated with 512-byte alignment; JDSC_ALIGNMEM=0 only
 *  guarantees max_align_t.
 */
static class_priority_queue_ops_t demo_align_heap_ops = {
    .valid_data = NULL,
    .__lt       = __demo_align_struct_heap_gt,
    .copy_data  = demo_align_struct_heap_copy,
    .free_data  = demo_align_struct_heap_free,
    .__move     = NULL,
};

static void demo_align_struct_heap(void)
{
    const size_t size  = sizeof(demo_align_struct_t);
    const size_t align = (size_t)JDSC_ALIGNOF(demo_align_struct_t);

    pr_test("Align struct, sizeof [ %zu ] alignof [ %zu ]", size, align);

                          /* PRIORITY_QUEUE_NEW_OPS_COMPLATE(&demo_align_heap_ops, SEQ_CNTR_DTYPE_PTR, demo_align_struct_t*, DEQUE, 0) */
    priority_queue_t* demo = PRIORITY_QUEUE_NEW_OPS_COMPLATE(&demo_align_heap_ops, SEQ_CNTR_DTYPE_PTR, demo_align_struct_t*, VECTOR, 0);
    priority_queue_size_t ret; (void)ret;

    demo_align_struct_t objs[] = {
        { 'd', 4, 40, 400 },
        { 'b', 2, 20, 200 },
        { 'a', 1, 10, 100 },
        { 'c', 3, 30, 300 },
    };

    for (int i = 0; i < sizeof(objs) / sizeof(objs[0]); ++i)
        cds->push(demo, ds_cstdata(&objs[i]));

    ret = cds->size(demo);  // size = 4

    while (!cds->__empty(demo)) {
        demo_align_struct_t* top = (demo_align_struct_t*)(intptr_t)*cds->top(demo).d;

        pr_test("Element: a [ %c ] b [ %d ] c [ %d ] d [ %ld ] addr [ %p ] %% 512 [ %zu ]",
                top->a, top->b, top->c, top->d, top,
                (size_t)((uintptr_t)top % 512));
        cds->pop(demo);
    }

    ret = cds->clear(demo);

    PRIORITY_QUEUE_DELETE(&demo);
}
#endif /* JDSC_ALIGNMEM */

int main(void)
{
    /* values */
    demo_values();
    pr_test("");

    /* low-level interfaces */
    demo_low_level_api();
    pr_test("");

    /* strings */
    demo_string();
    pr_test("");

    /* struct */
    demo_struct_node();
    pr_test("");
    demo_struct_heap();
    pr_test("");

#if JDSC_ALIGNMEM
    /* aligned struct */
    demo_align_struct_node();
    pr_test("");
    demo_align_struct_heap();
#endif /* JDSC_ALIGNMEM */
    return 0;
}
