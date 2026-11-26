/*
  libj_ds permanent tests priority queue unit
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

#include "../../core/test.h"
#include "desc.h"

static void pq_free(void* self)
{
    priority_queue_t* pq = self;

    PRIORITY_QUEUE_DELETE(&pq);
}

static void pq_expect_string(const priority_queue_t* pq, const char* expected)
{
    priority_queue_value_t value = cpriority_queue->top(pq);

    TEST_ASSERT_NOT_NULL(value.ssd);
    TEST_ASSERT_NOT_NULL(value.ssd->p);
    TEST_ASSERT_STR_EQ(expected, value.ssd->p);
}

static bool pq_tracked_valid(priority_queue_data_t data)
{
    return data != 0
        && test_tracked_valid_at((const test_tracked_t*)(uintptr_t)data);
}

static bool pq_tracked_lt(priority_queue_data_t left, priority_queue_data_t right)
{
    const test_tracked_t* a = (const test_tracked_t*)(uintptr_t)left;
    const test_tracked_t* b = (const test_tracked_t*)(uintptr_t)right;

    return a->payload < b->payload;
}

static bool pq_tracked_copy(priority_queue_data_t in, priority_queue_data_t* out)
{
    const test_tracked_t* src = (const test_tracked_t*)(uintptr_t)in;
    test_tracked_t* dst = (test_tracked_t*)(void*)out;
    test_tracked_t made;

    if (!test_tracked_valid_at(src))
        return false;
    made = test_tracked_make(src->payload);
    if (!test_tracked_valid_at(&made))
        return false;
    *dst = made;
    return true;
}

static void pq_tracked_free(priority_queue_data_t* data)
{
    test_tracked_dispose((test_tracked_t*)(void*)data);
}

static void pq_tracked_move(priority_queue_data_t dest, priority_queue_data_t src)
{
    test_tracked_t* to = (test_tracked_t*)(uintptr_t)dest;
    test_tracked_t* from = (test_tracked_t*)(uintptr_t)src;

    if (to == from)
        return;
    *to = *from;
    from->magic = 0;
    from->owned = NULL;
}

static const class_priority_queue_ops_t* pq_tracked_ops(void)
{
    static const class_priority_queue_ops_t ops = {
        .valid_data = pq_tracked_valid,
        .__lt       = pq_tracked_lt,
        .copy_data  = pq_tracked_copy,
        .free_data  = pq_tracked_free,
        .__move     = pq_tracked_move,
    };

    return &ops;
}

TEST_CASE(unit, pq_numeric_orders)
{
    priority_queue_t* i8_max = PRIORITY_QUEUE_NEW_T(int8_t, VECTOR);
    priority_queue_t* i8_min = PRIORITY_QUEUE_NEW_T_GT(int8_t, VECTOR);
    priority_queue_t* i16_max = PRIORITY_QUEUE_NEW_T(int16_t, DEQUE);
    priority_queue_t* i16_min = PRIORITY_QUEUE_NEW_T_GT(int16_t, DEQUE);
    priority_queue_t* u32_max = PRIORITY_QUEUE_NEW_T(uint32_t, VECTOR);
    priority_queue_t* u32_min = PRIORITY_QUEUE_NEW_T_GT(uint32_t, VECTOR);
    priority_queue_t* u64_max = PRIORITY_QUEUE_NEW_T(uint64_t, DEQUE);
    priority_queue_t* u64_min = PRIORITY_QUEUE_NEW_T_GT(uint64_t, DEQUE);
    priority_queue_t* i64_max = PRIORITY_QUEUE_NEW_T(int64_t, VECTOR);
    priority_queue_t* i64_min = PRIORITY_QUEUE_NEW_T_GT(int64_t, VECTOR);
    priority_queue_t* f_max = PRIORITY_QUEUE_NEW_T(float, VECTOR);
    priority_queue_t* f_min = PRIORITY_QUEUE_NEW_T_GT(float, VECTOR);
    priority_queue_t* d_max = PRIORITY_QUEUE_NEW_T(double, DEQUE);
    priority_queue_t* d_min = PRIORITY_QUEUE_NEW_T_GT(double, DEQUE);
    priority_queue_value_t value;

    TEST_ASSERT_NOT_NULL(i8_max);
    TEST_ASSERT_NOT_NULL(i8_min);
    TEST_ASSERT_NOT_NULL(i16_max);
    TEST_ASSERT_NOT_NULL(i16_min);
    TEST_ASSERT_NOT_NULL(u32_max);
    TEST_ASSERT_NOT_NULL(u32_min);
    TEST_ASSERT_NOT_NULL(u64_max);
    TEST_ASSERT_NOT_NULL(u64_min);
    TEST_ASSERT_NOT_NULL(i64_max);
    TEST_ASSERT_NOT_NULL(i64_min);
    TEST_ASSERT_NOT_NULL(f_max);
    TEST_ASSERT_NOT_NULL(f_min);
    TEST_ASSERT_NOT_NULL(d_max);
    TEST_ASSERT_NOT_NULL(d_min);
    test_cleanup_push(pq_free, i8_max);
    test_cleanup_push(pq_free, i8_min);
    test_cleanup_push(pq_free, i16_max);
    test_cleanup_push(pq_free, i16_min);
    test_cleanup_push(pq_free, u32_max);
    test_cleanup_push(pq_free, u32_min);
    test_cleanup_push(pq_free, u64_max);
    test_cleanup_push(pq_free, u64_min);
    test_cleanup_push(pq_free, i64_max);
    test_cleanup_push(pq_free, i64_min);
    test_cleanup_push(pq_free, f_max);
    test_cleanup_push(pq_free, f_min);
    test_cleanup_push(pq_free, d_max);
    test_cleanup_push(pq_free, d_min);

    TEST_ASSERT_TRUE(cpriority_queue->push(i8_max, ds_s8data(-5)));
    TEST_ASSERT_TRUE(cpriority_queue->push(i8_max, ds_s8data(7)));
    TEST_ASSERT_TRUE(cpriority_queue->push(i8_max, ds_s8data(0)));
    TEST_ASSERT_TRUE(cpriority_queue->push(i8_max, ds_s8data(7)));
    value = cpriority_queue->top(i8_max);
    TEST_ASSERT_NOT_NULL(value.s8);
    TEST_ASSERT_EQ(7, *value.s8);
    cpriority_queue->pop(i8_max);
    TEST_ASSERT_EQ(3, cpriority_queue->size(i8_max));
    TEST_ASSERT_EQ(3, cpriority_queue->clear(i8_max));

    TEST_ASSERT_TRUE(cpriority_queue->push(i8_min, ds_s8data(-5)));
    TEST_ASSERT_TRUE(cpriority_queue->push(i8_min, ds_s8data(7)));
    TEST_ASSERT_TRUE(cpriority_queue->push(i8_min, ds_s8data(0)));
    value = cpriority_queue->top(i8_min);
    TEST_ASSERT_NOT_NULL(value.s8);
    TEST_ASSERT_EQ(-5, *value.s8);
    TEST_ASSERT_EQ(3, cpriority_queue->clear(i8_min));

    TEST_ASSERT_TRUE(cpriority_queue->push(i16_max, ds_s16data(-300)));
    TEST_ASSERT_TRUE(cpriority_queue->push(i16_max, ds_s16data(200)));
    TEST_ASSERT_TRUE(cpriority_queue->push(i16_max, ds_s16data(0)));
    value = cpriority_queue->top(i16_max);
    TEST_ASSERT_NOT_NULL(value.s16);
    TEST_ASSERT_EQ(200, *value.s16);
    TEST_ASSERT_EQ(3, cpriority_queue->clear(i16_max));

    TEST_ASSERT_TRUE(cpriority_queue->push(i16_min, ds_s16data(-300)));
    TEST_ASSERT_TRUE(cpriority_queue->push(i16_min, ds_s16data(200)));
    TEST_ASSERT_TRUE(cpriority_queue->push(i16_min, ds_s16data(0)));
    value = cpriority_queue->top(i16_min);
    TEST_ASSERT_NOT_NULL(value.s16);
    TEST_ASSERT_EQ(-300, *value.s16);
    TEST_ASSERT_EQ(3, cpriority_queue->clear(i16_min));

    TEST_ASSERT_TRUE(cpriority_queue->push(u32_max, ds_u32data(1u)));
    TEST_ASSERT_TRUE(cpriority_queue->push(u32_max, ds_u32data(0x80000000u)));
    TEST_ASSERT_TRUE(cpriority_queue->push(u32_max, ds_u32data(7u)));
    value = cpriority_queue->top(u32_max);
    TEST_ASSERT_NOT_NULL(value.u32);
    TEST_ASSERT_TRUE(*value.u32 == 0x80000000u);
    TEST_ASSERT_EQ(3, cpriority_queue->clear(u32_max));

    TEST_ASSERT_TRUE(cpriority_queue->push(u32_min, ds_u32data(1u)));
    TEST_ASSERT_TRUE(cpriority_queue->push(u32_min, ds_u32data(0x80000000u)));
    TEST_ASSERT_TRUE(cpriority_queue->push(u32_min, ds_u32data(7u)));
    value = cpriority_queue->top(u32_min);
    TEST_ASSERT_NOT_NULL(value.u32);
    TEST_ASSERT_TRUE(*value.u32 == 1u);
    TEST_ASSERT_EQ(3, cpriority_queue->clear(u32_min));

    TEST_ASSERT_TRUE(cpriority_queue->push(u64_max, ds_u64data(1u)));
    TEST_ASSERT_TRUE(cpriority_queue->push(u64_max,
                                           ds_u64data(UINT64_C(0x8000000000000000))));
    TEST_ASSERT_TRUE(cpriority_queue->push(u64_max, ds_u64data(3u)));
    value = cpriority_queue->top(u64_max);
    TEST_ASSERT_NOT_NULL(value.u64);
    TEST_ASSERT_TRUE(*value.u64 == UINT64_C(0x8000000000000000));
    TEST_ASSERT_EQ(3, cpriority_queue->clear(u64_max));

    TEST_ASSERT_TRUE(cpriority_queue->push(u64_min, ds_u64data(1u)));
    TEST_ASSERT_TRUE(cpriority_queue->push(u64_min,
                                           ds_u64data(UINT64_C(0x8000000000000000))));
    TEST_ASSERT_TRUE(cpriority_queue->push(u64_min, ds_u64data(3u)));
    value = cpriority_queue->top(u64_min);
    TEST_ASSERT_NOT_NULL(value.u64);
    TEST_ASSERT_TRUE(*value.u64 == 1u);
    TEST_ASSERT_EQ(3, cpriority_queue->clear(u64_min));

    TEST_ASSERT_TRUE(cpriority_queue->push(i64_max, ds_s64data(INT64_C(-9000000000))));
    TEST_ASSERT_TRUE(cpriority_queue->push(i64_max, ds_s64data(INT64_C(42))));
    TEST_ASSERT_TRUE(cpriority_queue->push(i64_max, ds_s64data(INT64_C(9000000000))));
    value = cpriority_queue->top(i64_max);
    TEST_ASSERT_NOT_NULL(value.s64);
    TEST_ASSERT_TRUE(*value.s64 == INT64_C(9000000000));
    TEST_ASSERT_EQ(3, cpriority_queue->clear(i64_max));

    TEST_ASSERT_TRUE(cpriority_queue->push(i64_min, ds_s64data(INT64_C(-9000000000))));
    TEST_ASSERT_TRUE(cpriority_queue->push(i64_min, ds_s64data(INT64_C(42))));
    TEST_ASSERT_TRUE(cpriority_queue->push(i64_min, ds_s64data(INT64_C(9000000000))));
    value = cpriority_queue->top(i64_min);
    TEST_ASSERT_NOT_NULL(value.s64);
    TEST_ASSERT_TRUE(*value.s64 == INT64_C(-9000000000));
    TEST_ASSERT_EQ(3, cpriority_queue->clear(i64_min));

    TEST_ASSERT_TRUE(cpriority_queue->push(f_max, ds_fdata(1.5f)));
    TEST_ASSERT_TRUE(cpriority_queue->push(f_max, ds_fdata(-2.5f)));
    TEST_ASSERT_TRUE(cpriority_queue->push(f_max, ds_fdata(3.25f)));
    value = cpriority_queue->top(f_max);
    TEST_ASSERT_NOT_NULL(value.f);
    TEST_ASSERT_TRUE(*value.f == 3.25f);
    cpriority_queue->pop(f_max);
    value = cpriority_queue->top(f_max);
    TEST_ASSERT_NOT_NULL(value.f);
    TEST_ASSERT_TRUE(*value.f == 1.5f);
    TEST_ASSERT_EQ(2, cpriority_queue->clear(f_max));

    TEST_ASSERT_TRUE(cpriority_queue->push(f_min, ds_fdata(1.5f)));
    TEST_ASSERT_TRUE(cpriority_queue->push(f_min, ds_fdata(-2.5f)));
    TEST_ASSERT_TRUE(cpriority_queue->push(f_min, ds_fdata(3.25f)));
    value = cpriority_queue->top(f_min);
    TEST_ASSERT_NOT_NULL(value.f);
    TEST_ASSERT_TRUE(*value.f == -2.5f);
    TEST_ASSERT_EQ(3, cpriority_queue->clear(f_min));

    TEST_ASSERT_TRUE(cpriority_queue->push(d_max, ds_lfdata(2.5)));
    TEST_ASSERT_TRUE(cpriority_queue->push(d_max, ds_lfdata(-4.75)));
    TEST_ASSERT_TRUE(cpriority_queue->push(d_max, ds_lfdata(0.0)));
    value = cpriority_queue->top(d_max);
    TEST_ASSERT_NOT_NULL(value.lf);
    TEST_ASSERT_TRUE(*value.lf == 2.5);
    TEST_ASSERT_EQ(3, cpriority_queue->clear(d_max));

    TEST_ASSERT_TRUE(cpriority_queue->push(d_min, ds_lfdata(2.5)));
    TEST_ASSERT_TRUE(cpriority_queue->push(d_min, ds_lfdata(-4.75)));
    TEST_ASSERT_TRUE(cpriority_queue->push(d_min, ds_lfdata(0.0)));
    value = cpriority_queue->top(d_min);
    TEST_ASSERT_NOT_NULL(value.lf);
    TEST_ASSERT_TRUE(*value.lf == -4.75);
    TEST_ASSERT_EQ(3, cpriority_queue->clear(d_min));
}

TEST_CASE(unit, pq_inline_strings)
{
    priority_queue_t* vmax = PRIORITY_QUEUE_NEW_SSO_T(VECTOR);
    priority_queue_t* vmin = PRIORITY_QUEUE_NEW_SSO_T_GT(VECTOR);
    priority_queue_t* dmax = PRIORITY_QUEUE_NEW_SSO_T(DEQUE);
    priority_queue_t* dmin = PRIORITY_QUEUE_NEW_SSO_T_GT(DEQUE);
    priority_queue_t* cmax = PRIORITY_QUEUE_NEW_CHAR_T(VECTOR);
    priority_queue_t* cmin = PRIORITY_QUEUE_NEW_CHAR_T_GT(VECTOR);

    TEST_ASSERT_NOT_NULL(vmax);
    TEST_ASSERT_NOT_NULL(vmin);
    TEST_ASSERT_NOT_NULL(dmax);
    TEST_ASSERT_NOT_NULL(dmin);
    TEST_ASSERT_NOT_NULL(cmax);
    TEST_ASSERT_NOT_NULL(cmin);
    test_cleanup_push(pq_free, vmax);
    test_cleanup_push(pq_free, vmin);
    test_cleanup_push(pq_free, dmax);
    test_cleanup_push(pq_free, dmin);
    test_cleanup_push(pq_free, cmax);
    test_cleanup_push(pq_free, cmin);

    TEST_ASSERT_TRUE(cpriority_queue->push(vmax, ds_sdata("b")));
    TEST_ASSERT_TRUE(cpriority_queue->push(vmax, ds_sdata("a")));
    TEST_ASSERT_TRUE(cpriority_queue->push(vmax, ds_sdata("ccc")));
    TEST_ASSERT_TRUE(cpriority_queue->push(vmax, ds_sdata("aa")));
    pq_expect_string(vmax, "ccc");
    cpriority_queue->pop(vmax);
    pq_expect_string(vmax, "b");
    cpriority_queue->pop(vmax);
    pq_expect_string(vmax, "aa");
    cpriority_queue->pop(vmax);
    pq_expect_string(vmax, "a");
    cpriority_queue->pop(vmax);
    TEST_ASSERT_NULL(cpriority_queue->top(vmax).u8);
    TEST_ASSERT_EQ(0, cpriority_queue->size(vmax));

    TEST_ASSERT_TRUE(cpriority_queue->push(vmin, ds_sdata("b")));
    TEST_ASSERT_TRUE(cpriority_queue->push(vmin, ds_sdata("a")));
    TEST_ASSERT_TRUE(cpriority_queue->push(vmin, ds_sdata("ccc")));
    TEST_ASSERT_TRUE(cpriority_queue->push(vmin, ds_sdata("aa")));
    pq_expect_string(vmin, "a");
    cpriority_queue->pop(vmin);
    pq_expect_string(vmin, "aa");
    cpriority_queue->pop(vmin);
    pq_expect_string(vmin, "b");
    cpriority_queue->pop(vmin);
    pq_expect_string(vmin, "ccc");
    TEST_ASSERT_EQ(1, cpriority_queue->clear(vmin));

    TEST_ASSERT_TRUE(cpriority_queue->push(dmax, ds_sdata("b")));
    TEST_ASSERT_TRUE(cpriority_queue->push(dmax, ds_sdata("a")));
    TEST_ASSERT_TRUE(cpriority_queue->push(dmax, ds_sdata("ccc")));
    TEST_ASSERT_TRUE(cpriority_queue->push(dmax, ds_sdata("aa")));
    pq_expect_string(dmax, "ccc");
    TEST_ASSERT_EQ(4, cpriority_queue->clear(dmax));

    TEST_ASSERT_TRUE(cpriority_queue->push(dmin, ds_sdata("b")));
    TEST_ASSERT_TRUE(cpriority_queue->push(dmin, ds_sdata("a")));
    TEST_ASSERT_TRUE(cpriority_queue->push(dmin, ds_sdata("ccc")));
    TEST_ASSERT_TRUE(cpriority_queue->push(dmin, ds_sdata("aa")));
    pq_expect_string(dmin, "a");
    TEST_ASSERT_EQ(4, cpriority_queue->clear(dmin));

    TEST_ASSERT_TRUE(cpriority_queue->push(cmax, ds_sdata("b")));
    TEST_ASSERT_TRUE(cpriority_queue->push(cmax, ds_sdata("a")));
    TEST_ASSERT_TRUE(cpriority_queue->push(cmax, ds_sdata("ccc")));
    TEST_ASSERT_TRUE(cpriority_queue->push(cmax, ds_sdata("aa")));
    pq_expect_string(cmax, "ccc");
    TEST_ASSERT_EQ(4, cpriority_queue->clear(cmax));

    TEST_ASSERT_TRUE(cpriority_queue->push(cmin, ds_sdata("b")));
    TEST_ASSERT_TRUE(cpriority_queue->push(cmin, ds_sdata("a")));
    TEST_ASSERT_TRUE(cpriority_queue->push(cmin, ds_sdata("ccc")));
    TEST_ASSERT_TRUE(cpriority_queue->push(cmin, ds_sdata("aa")));
    pq_expect_string(cmin, "a");
    TEST_ASSERT_EQ(4, cpriority_queue->clear(cmin));
}

TEST_CASE(unit, pq_tracked_ops)
{
    priority_queue_t* vector = PRIORITY_QUEUE_NEW_OPS_COMPLATE(pq_tracked_ops(),
                                                               SEQ_CNTR_DTYPE_STRUCT,
                                                               test_tracked_t, VECTOR, 0);
    priority_queue_t* deque = PRIORITY_QUEUE_NEW_OPS_COMPLATE(pq_tracked_ops(),
                                                              SEQ_CNTR_DTYPE_STRUCT,
                                                              test_tracked_t, DEQUE, 0);
    test_tracked_t value;
    priority_queue_value_t top;

    TEST_ASSERT_NOT_NULL(vector);
    TEST_ASSERT_NOT_NULL(deque);
    test_cleanup_push(pq_free, vector);
    test_cleanup_push(pq_free, deque);

    value = test_tracked_make(30);
    TEST_ASSERT_TRUE(cpriority_queue->push(vector, ds_cstdata(&value)));
    test_tracked_dispose(&value);
    value = test_tracked_make(10);
    TEST_ASSERT_TRUE(cpriority_queue->push(vector, ds_cstdata(&value)));
    test_tracked_dispose(&value);
    value = test_tracked_make(20);
    TEST_ASSERT_TRUE(cpriority_queue->push(vector, ds_cstdata(&value)));
    test_tracked_dispose(&value);
    value = test_tracked_make(40);
    TEST_ASSERT_TRUE(cpriority_queue->push(vector, ds_cstdata(&value)));
    test_tracked_dispose(&value);

    top = cpriority_queue->top(vector);
    TEST_ASSERT_NOT_NULL(top.u8);
    TEST_ASSERT_EQ(40, test_tracked_payload_at(test_tracked_slot_at(top.u8)));
    cpriority_queue->pop(vector);
    top = cpriority_queue->top(vector);
    TEST_ASSERT_EQ(30, test_tracked_payload_at(test_tracked_slot_at(top.u8)));
    cpriority_queue->pop(vector);
    top = cpriority_queue->top(vector);
    TEST_ASSERT_EQ(20, test_tracked_payload_at(test_tracked_slot_at(top.u8)));
    cpriority_queue->pop(vector);
    top = cpriority_queue->top(vector);
    TEST_ASSERT_EQ(10, test_tracked_payload_at(test_tracked_slot_at(top.u8)));
    TEST_ASSERT_EQ(1, cpriority_queue->clear(vector));
    TEST_ASSERT_EQ(0, test_tracked_live());

    value = test_tracked_make(1);
    TEST_ASSERT_TRUE(cpriority_queue->push(deque, ds_cstdata(&value)));
    test_tracked_dispose(&value);
    value = test_tracked_make(3);
    TEST_ASSERT_TRUE(cpriority_queue->push(deque, ds_cstdata(&value)));
    test_tracked_dispose(&value);
    value = test_tracked_make(2);
    TEST_ASSERT_TRUE(cpriority_queue->push(deque, ds_cstdata(&value)));
    test_tracked_dispose(&value);
    top = cpriority_queue->top(deque);
    TEST_ASSERT_NOT_NULL(top.u8);
    TEST_ASSERT_EQ(3, test_tracked_payload_at(test_tracked_slot_at(top.u8)));
    TEST_ASSERT_EQ(3, cpriority_queue->clear(deque));
    TEST_ASSERT_EQ(0, test_tracked_live());
}

static void pq_check_swap(const test_adaptor_desc_t* desc, int32_t expected_a,
                          int32_t expected_b)
{
    void* a = desc->create();
    void* b = desc->create();
    int32_t seen;

    TEST_ASSERT_NOT_NULL(a);
    TEST_ASSERT_NOT_NULL(b);
    TEST_ASSERT_TRUE(desc->push(a, 1));
    TEST_ASSERT_TRUE(desc->push(a, 2));
    TEST_ASSERT_TRUE(desc->push(b, 3));
    desc->swap(a, b);
    TEST_ASSERT_EQ(1, desc->size(a));
    TEST_ASSERT_TRUE(desc->peek(a, &seen));
    TEST_ASSERT_EQ(expected_a, seen);
    TEST_ASSERT_EQ(2, desc->size(b));
    TEST_ASSERT_TRUE(desc->peek(b, &seen));
    TEST_ASSERT_EQ(expected_b, seen);
    desc->destroy(a);
    desc->destroy(b);
}

TEST_CASE(unit, pq_swap_and_null_guards)
{
    priority_queue_t* pq = PRIORITY_QUEUE_NEW_T(int32_t, VECTOR);
    priority_queue_value_t value;

    TEST_ASSERT_NOT_NULL(pq);
    test_cleanup_push(pq_free, pq);

    TEST_ASSERT_EQ(-1, cpriority_queue->size(NULL));
    TEST_ASSERT_FALSE(cpriority_queue->push(NULL, ds_s32data(1)));
    TEST_ASSERT_NULL(cpriority_queue->top(NULL).u8);
    cpriority_queue->pop(NULL);
    cpriority_queue->swap(NULL, NULL);
    TEST_ASSERT_EQ(-1, cpriority_queue->clear(NULL));

    TEST_ASSERT_NULL(cpriority_queue->top(pq).u8);
    TEST_ASSERT_EQ(0, cpriority_queue->size(pq));
    TEST_ASSERT_TRUE(cpriority_queue->push(pq, ds_s32data(5)));
    TEST_ASSERT_EQ(1, cpriority_queue->size(pq));
    cpriority_queue->swap(pq, pq);
    TEST_ASSERT_EQ(1, cpriority_queue->size(pq));
    value = cpriority_queue->top(pq);
    TEST_ASSERT_NOT_NULL(value.s32);
    TEST_ASSERT_EQ(5, *value.s32);
    cpriority_queue->pop(pq);
    TEST_ASSERT_NULL(cpriority_queue->top(pq).u8);
    TEST_ASSERT_EQ(0, cpriority_queue->size(pq));

    pq_check_swap(priority_queue_desc_vector_max(), 3, 2);
    pq_check_swap(priority_queue_desc_vector_min(), 3, 1);
    pq_check_swap(priority_queue_desc_deque_max(), 3, 2);
    pq_check_swap(priority_queue_desc_deque_min(), 3, 1);
}
