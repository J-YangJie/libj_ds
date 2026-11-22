/*
  libj_ds permanent tests queue unit
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

#include <queue/queue.h>

#include <string.h>

#include "../../core/test.h"
#include "desc.h"

static void queue_free(void* self)
{
    queue_t* queue = self;

    QUEUE_DELETE(&queue);
}

static void queue_check_float(queue_t* queue)
{
    queue_value_t value;

    TEST_ASSERT_TRUE(cqueue->push(queue, ds_fdata(1.5f)));
    TEST_ASSERT_TRUE(cqueue->push(queue, ds_fdata(-2.5f)));
    TEST_ASSERT_TRUE(cqueue->push(queue, ds_fdata(3.25f)));
    TEST_ASSERT_EQ(3, cqueue->size(queue));

    value = cqueue->front(queue);
    TEST_ASSERT_NOT_NULL(value.f);
    TEST_ASSERT_TRUE(*value.f == 1.5f);
    value = cqueue->back(queue);
    TEST_ASSERT_NOT_NULL(value.f);
    TEST_ASSERT_TRUE(*value.f == 3.25f);
    cqueue->pop(queue);
    value = cqueue->front(queue);
    TEST_ASSERT_NOT_NULL(value.f);
    TEST_ASSERT_TRUE(*value.f == -2.5f);
    TEST_ASSERT_EQ(2, cqueue->clear(queue));
}

static void queue_check_double(queue_t* queue)
{
    queue_value_t value;

    TEST_ASSERT_TRUE(cqueue->push(queue, ds_lfdata(2.5)));
    TEST_ASSERT_TRUE(cqueue->push(queue, ds_lfdata(-4.75)));
    value = cqueue->front(queue);
    TEST_ASSERT_NOT_NULL(value.lf);
    TEST_ASSERT_TRUE(*value.lf == 2.5);
    value = cqueue->back(queue);
    TEST_ASSERT_NOT_NULL(value.lf);
    TEST_ASSERT_TRUE(*value.lf == -4.75);
    TEST_ASSERT_EQ(2, cqueue->clear(queue));
}

static void queue_check_int64(queue_t* queue)
{
    queue_value_t value;

    TEST_ASSERT_TRUE(cqueue->push(queue,
                                  ds_s64data(INT64_C(0x1122334455667788))));
    TEST_ASSERT_TRUE(cqueue->push(queue, ds_s64data(INT64_C(-7))));
    value = cqueue->front(queue);
    TEST_ASSERT_NOT_NULL(value.s64);
    TEST_ASSERT_EQ(INT64_C(0x1122334455667788), *value.s64);
    value = cqueue->back(queue);
    TEST_ASSERT_NOT_NULL(value.s64);
    TEST_ASSERT_EQ(INT64_C(-7), *value.s64);
    TEST_ASSERT_EQ(2, cqueue->clear(queue));
}

static void queue_check_strings(queue_t* sso, queue_t* chars)
{
    static const char* const short15 = "abcdefghijklmno";
    static const char* const long16 = "abcdefghijklmnop";
    static const char* const long45 = "this string is longer than the inline buffer";
    queue_value_t value;

    TEST_ASSERT_TRUE(cqueue->push(sso, ds_sdata(short15)));
    TEST_ASSERT_TRUE(cqueue->push(sso, ds_sdata(long16)));
    TEST_ASSERT_TRUE(cqueue->push(sso, ds_sdata(long45)));

    value = cqueue->front(sso);
    TEST_ASSERT_NOT_NULL(value.ssd);
    TEST_ASSERT_EQ(15, value.ssd->len);
    TEST_ASSERT_TRUE(value.ssd->p == value.ssd->buf);
    TEST_ASSERT_STR_EQ(short15, value.ssd->p);
    value = cqueue->back(sso);
    TEST_ASSERT_NOT_NULL(value.ssd);
    TEST_ASSERT_EQ(44, value.ssd->len);
    TEST_ASSERT_STR_EQ(long45, value.ssd->p);
    cqueue->pop(sso);
    value = cqueue->front(sso);
    TEST_ASSERT_NOT_NULL(value.ssd);
    TEST_ASSERT_EQ(16, value.ssd->len);
    TEST_ASSERT_TRUE(value.ssd->p != value.ssd->buf);
    TEST_ASSERT_STR_EQ(long16, value.ssd->p);
    TEST_ASSERT_EQ(2, cqueue->clear(sso));

    TEST_ASSERT_TRUE(cqueue->push(chars, ds_sdata(short15)));
    TEST_ASSERT_TRUE(cqueue->push(chars, ds_sdata(long45)));
    value = cqueue->front(chars);
    TEST_ASSERT_NOT_NULL(value.sd);
    TEST_ASSERT_STR_EQ(short15, *value.sd);
    value = cqueue->back(chars);
    TEST_ASSERT_NOT_NULL(value.sd);
    TEST_ASSERT_STR_EQ(long45, *value.sd);
    cqueue->pop(chars);
    value = cqueue->front(chars);
    TEST_ASSERT_NOT_NULL(value.sd);
    TEST_ASSERT_STR_EQ(long45, *value.sd);
    TEST_ASSERT_EQ(1, cqueue->clear(chars));
}

static void queue_check_tracked(queue_t* queue)
{
    test_tracked_t first = test_tracked_make(11);
    test_tracked_t second = test_tracked_make(22);
    queue_value_t value;

    TEST_ASSERT_TRUE(test_tracked_valid_at(&first));
    TEST_ASSERT_TRUE(test_tracked_valid_at(&second));

    TEST_ASSERT_TRUE(cqueue->push(queue, ds_cstdata(&first)));
    test_tracked_dispose(&first);
    TEST_ASSERT_TRUE(cqueue->push(queue, ds_cstdata(&second)));
    test_tracked_dispose(&second);
    TEST_ASSERT_EQ(2, cqueue->size(queue));

    value = cqueue->front(queue);
    TEST_ASSERT_NOT_NULL(value.u8);
    TEST_ASSERT_TRUE(test_tracked_valid_at(test_tracked_slot_at(value.u8)));
    TEST_ASSERT_EQ(11, test_tracked_payload_at(test_tracked_slot_at(value.u8)));
    value = cqueue->back(queue);
    TEST_ASSERT_NOT_NULL(value.u8);
    TEST_ASSERT_EQ(22, test_tracked_payload_at(test_tracked_slot_at(value.u8)));

    cqueue->pop(queue);
    value = cqueue->front(queue);
    TEST_ASSERT_NOT_NULL(value.u8);
    TEST_ASSERT_EQ(22, test_tracked_payload_at(test_tracked_slot_at(value.u8)));

    TEST_ASSERT_EQ(1, cqueue->clear(queue));
    TEST_ASSERT_EQ(0, test_tracked_live());
}

TEST_CASE(unit, queue_backend_fifo)
{
    const test_adaptor_desc_t* descs[] = {
        queue_desc_deque(), queue_desc_list()
    };
    size_t d;

    for (d = 0; d < sizeof(descs) / sizeof(descs[0]); ++d) {
        const test_adaptor_desc_t* desc = descs[d];
        void* self = desc->create();
        int32_t front = 0;
        int32_t back = 0;
        size_t i;

        TEST_ASSERT_NOT_NULL(self);
        test_cleanup_push(queue_free, self);

        for (i = 0; i < 10; ++i)
            TEST_ASSERT_TRUE(desc->push(self, (int32_t)i));
        TEST_ASSERT_EQ(10, desc->size(self));
        TEST_ASSERT_TRUE(desc->peek(self, &front));
        TEST_ASSERT_TRUE(desc->peek_back(self, &back));
        TEST_ASSERT_EQ(0, front);
        TEST_ASSERT_EQ(9, back);

        for (i = 0; i < 3; ++i)
            desc->pop(self);
        TEST_ASSERT_EQ(7, desc->size(self));
        TEST_ASSERT_TRUE(desc->peek(self, &front));
        TEST_ASSERT_TRUE(desc->peek_back(self, &back));
        TEST_ASSERT_EQ(3, front);
        TEST_ASSERT_EQ(9, back);
        TEST_ASSERT_EQ(7, desc->clear(self));
        TEST_ASSERT_TRUE(desc->empty(self));

        TEST_ASSERT_TRUE(desc->push(self, 99));
        TEST_ASSERT_TRUE(desc->peek(self, &front));
        TEST_ASSERT_TRUE(desc->peek_back(self, &back));
        TEST_ASSERT_EQ(99, front);
        TEST_ASSERT_EQ(99, back);

        test_cleanup_pop(true);
    }
}

TEST_CASE(unit, queue_numeric_backends)
{
    queue_t* f_deque = QUEUE_NEW_T(float, DEQUE);
    queue_t* f_list = QUEUE_NEW_T(float, LIST);
    queue_t* d_deque = QUEUE_NEW_T(double, DEQUE);
    queue_t* d_list = QUEUE_NEW_T(double, LIST);
    queue_t* i_deque = QUEUE_NEW_T(int64_t, DEQUE);
    queue_t* i_list = QUEUE_NEW_T(int64_t, LIST);

    TEST_ASSERT_NOT_NULL(f_deque);
    TEST_ASSERT_NOT_NULL(f_list);
    TEST_ASSERT_NOT_NULL(d_deque);
    TEST_ASSERT_NOT_NULL(d_list);
    TEST_ASSERT_NOT_NULL(i_deque);
    TEST_ASSERT_NOT_NULL(i_list);
    test_cleanup_push(queue_free, f_deque);
    test_cleanup_push(queue_free, f_list);
    test_cleanup_push(queue_free, d_deque);
    test_cleanup_push(queue_free, d_list);
    test_cleanup_push(queue_free, i_deque);
    test_cleanup_push(queue_free, i_list);

    queue_check_float(f_deque);
    queue_check_float(f_list);
    queue_check_double(d_deque);
    queue_check_double(d_list);
    queue_check_int64(i_deque);
    queue_check_int64(i_list);
}

TEST_CASE(unit, queue_inline_strings)
{
    queue_t* sso_deque = QUEUE_NEW_SSO_T(DEQUE);
    queue_t* sso_list = QUEUE_NEW_SSO_T(LIST);
    queue_t* chars_deque = QUEUE_NEW_CHAR_T(DEQUE);
    queue_t* chars_list = QUEUE_NEW_CHAR_T(LIST);

    TEST_ASSERT_NOT_NULL(sso_deque);
    TEST_ASSERT_NOT_NULL(sso_list);
    TEST_ASSERT_NOT_NULL(chars_deque);
    TEST_ASSERT_NOT_NULL(chars_list);
    test_cleanup_push(queue_free, sso_deque);
    test_cleanup_push(queue_free, sso_list);
    test_cleanup_push(queue_free, chars_deque);
    test_cleanup_push(queue_free, chars_list);

    queue_check_strings(sso_deque, chars_deque);
    queue_check_strings(sso_list, chars_list);
}

TEST_CASE(unit, queue_tracked_ops)
{
    queue_t* deque = QUEUE_NEW_OPS_COMPLATE(test_ops_tracked(),
                                            SEQ_CNTR_DTYPE_STRUCT,
                                            test_tracked_t, DEQUE);
    queue_t* list = QUEUE_NEW_OPS_COMPLATE(test_ops_tracked(),
                                           SEQ_CNTR_DTYPE_STRUCT,
                                           test_tracked_t, LIST);

    TEST_ASSERT_NOT_NULL(deque);
    TEST_ASSERT_NOT_NULL(list);
    test_cleanup_push(queue_free, deque);
    test_cleanup_push(queue_free, list);

    queue_check_tracked(deque);
    queue_check_tracked(list);
}

TEST_CASE(unit, queue_swap_and_null_guards)
{
    const test_adaptor_desc_t* descs[] = {
        queue_desc_deque(), queue_desc_list()
    };
    size_t d;

    for (d = 0; d < sizeof(descs) / sizeof(descs[0]); ++d) {
        const test_adaptor_desc_t* desc = descs[d];
        void* first = desc->create();
        void* second = desc->create();
        int32_t front = 0;
        int32_t back = 0;

        TEST_ASSERT_NOT_NULL(first);
        TEST_ASSERT_NOT_NULL(second);
        test_cleanup_push(queue_free, first);
        test_cleanup_push(queue_free, second);

        TEST_ASSERT_TRUE(desc->push(first, 1));
        TEST_ASSERT_TRUE(desc->push(first, 2));
        TEST_ASSERT_TRUE(desc->push(second, 3));
        desc->swap(first, second);
        TEST_ASSERT_EQ(1, desc->size(first));
        TEST_ASSERT_EQ(2, desc->size(second));
        TEST_ASSERT_TRUE(desc->peek(first, &front));
        TEST_ASSERT_TRUE(desc->peek_back(first, &back));
        TEST_ASSERT_EQ(3, front);
        TEST_ASSERT_EQ(3, back);
        TEST_ASSERT_TRUE(desc->peek(second, &front));
        TEST_ASSERT_TRUE(desc->peek_back(second, &back));
        TEST_ASSERT_EQ(1, front);
        TEST_ASSERT_EQ(2, back);

        test_cleanup_pop(true);
        test_cleanup_pop(true);
    }

    TEST_ASSERT_EQ(-1, cqueue->size(NULL));
    TEST_ASSERT_FALSE(cqueue->push(NULL, __ds_s32data(1)));
    TEST_ASSERT_NULL(cqueue->front(NULL).u8);
    TEST_ASSERT_NULL(cqueue->back(NULL).u8);
    cqueue->pop(NULL);
    cqueue->swap(NULL, NULL);
    TEST_ASSERT_EQ(-1, cqueue->clear(NULL));
}
