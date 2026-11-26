/*
  libj_ds permanent tests stack unit
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

#include <stack/stack.h>

#include <string.h>

#include "../../core/test.h"
#include "desc.h"

static void stack_free(void* self)
{
    stack_t* stack = self;

    STACK_DELETE(&stack);
}

static void stack_check_float(stack_t* stack)
{
    stack_value_t value;

    TEST_ASSERT_TRUE(cstack->push(stack, ds_fdata(1.5f)));
    TEST_ASSERT_TRUE(cstack->push(stack, ds_fdata(-2.5f)));
    TEST_ASSERT_TRUE(cstack->push(stack, ds_fdata(3.25f)));
    TEST_ASSERT_EQ(3, cstack->size(stack));

    value = cstack->top(stack);
    TEST_ASSERT_NOT_NULL(value.f);
    TEST_ASSERT_TRUE(*value.f == 3.25f);
    cstack->pop(stack);
    value = cstack->top(stack);
    TEST_ASSERT_NOT_NULL(value.f);
    TEST_ASSERT_TRUE(*value.f == -2.5f);
    TEST_ASSERT_EQ(2, cstack->clear(stack));
}

static void stack_check_double(stack_t* stack)
{
    stack_value_t value;

    TEST_ASSERT_TRUE(cstack->push(stack, ds_lfdata(2.5)));
    TEST_ASSERT_TRUE(cstack->push(stack, ds_lfdata(-4.75)));
    value = cstack->top(stack);
    TEST_ASSERT_NOT_NULL(value.lf);
    TEST_ASSERT_TRUE(*value.lf == -4.75);
    TEST_ASSERT_EQ(2, cstack->clear(stack));
}

static void stack_check_int64(stack_t* stack)
{
    stack_value_t value;

    TEST_ASSERT_TRUE(cstack->push(stack, ds_s64data(INT64_C(0x1122334455667788))));
    TEST_ASSERT_TRUE(cstack->push(stack, ds_s64data(INT64_C(-7))));
    value = cstack->top(stack);
    TEST_ASSERT_NOT_NULL(value.s64);
    TEST_ASSERT_EQ(INT64_C(-7), *value.s64);
    cstack->pop(stack);
    value = cstack->top(stack);
    TEST_ASSERT_NOT_NULL(value.s64);
    TEST_ASSERT_EQ(INT64_C(0x1122334455667788), *value.s64);
    TEST_ASSERT_EQ(1, cstack->clear(stack));
}

static void stack_check_strings(stack_t* sso, stack_t* chars)
{
    static const char* const short15 = "abcdefghijklmno";
    static const char* const long16 = "abcdefghijklmnop";
    static const char* const long45 = "this string is longer than the inline buffer";
    stack_value_t value;

    TEST_ASSERT_TRUE(cstack->push(sso, ds_sdata(short15)));
    TEST_ASSERT_TRUE(cstack->push(sso, ds_sdata(long16)));
    TEST_ASSERT_TRUE(cstack->push(sso, ds_sdata(long45)));

    value = cstack->top(sso);
    TEST_ASSERT_NOT_NULL(value.ssd);
    TEST_ASSERT_EQ(44, value.ssd->len);
    TEST_ASSERT_STR_EQ(long45, value.ssd->p);
    cstack->pop(sso);
    value = cstack->top(sso);
    TEST_ASSERT_NOT_NULL(value.ssd);
    TEST_ASSERT_EQ(16, value.ssd->len);
    TEST_ASSERT_TRUE(value.ssd->p != value.ssd->buf);
    TEST_ASSERT_STR_EQ(long16, value.ssd->p);
    cstack->pop(sso);
    value = cstack->top(sso);
    TEST_ASSERT_NOT_NULL(value.ssd);
    TEST_ASSERT_EQ(15, value.ssd->len);
    TEST_ASSERT_TRUE(value.ssd->p == value.ssd->buf);
    TEST_ASSERT_STR_EQ(short15, value.ssd->p);
    TEST_ASSERT_EQ(1, cstack->clear(sso));

    TEST_ASSERT_TRUE(cstack->push(chars, ds_sdata(short15)));
    TEST_ASSERT_TRUE(cstack->push(chars, ds_sdata(long45)));
    value = cstack->top(chars);
    TEST_ASSERT_NOT_NULL(value.sd);
    TEST_ASSERT_STR_EQ(long45, *value.sd);
    cstack->pop(chars);
    value = cstack->top(chars);
    TEST_ASSERT_NOT_NULL(value.sd);
    TEST_ASSERT_STR_EQ(short15, *value.sd);
    TEST_ASSERT_EQ(1, cstack->clear(chars));
}

static void stack_check_tracked(stack_t* stack)
{
    test_tracked_t first = test_tracked_make(11);
    test_tracked_t second = test_tracked_make(22);
    stack_value_t value;

    TEST_ASSERT_TRUE(test_tracked_valid_at(&first));
    TEST_ASSERT_TRUE(test_tracked_valid_at(&second));

    TEST_ASSERT_TRUE(cstack->push(stack, ds_cstdata(&first)));
    test_tracked_dispose(&first);
    TEST_ASSERT_TRUE(cstack->push(stack, ds_cstdata(&second)));
    test_tracked_dispose(&second);
    TEST_ASSERT_EQ(2, cstack->size(stack));

    value = cstack->top(stack);
    TEST_ASSERT_NOT_NULL(value.u8);
    TEST_ASSERT_TRUE(test_tracked_valid_at(test_tracked_slot_at(value.u8)));
    TEST_ASSERT_EQ(22, test_tracked_payload_at(test_tracked_slot_at(value.u8)));

    cstack->pop(stack);
    value = cstack->top(stack);
    TEST_ASSERT_NOT_NULL(value.u8);
    TEST_ASSERT_EQ(11, test_tracked_payload_at(test_tracked_slot_at(value.u8)));

    TEST_ASSERT_EQ(1, cstack->clear(stack));
    TEST_ASSERT_EQ(0, test_tracked_live());
}

TEST_CASE(unit, stack_backend_lifo)
{
    const test_adaptor_desc_t* descs[] = {
        stack_desc_vector(), stack_desc_deque(), stack_desc_list()
    };
    size_t d;

    for (d = 0; d < sizeof(descs) / sizeof(descs[0]); ++d) {
        const test_adaptor_desc_t* desc = descs[d];
        void* self = desc->create();
        int32_t value = 0;
        size_t i;

        TEST_ASSERT_NOT_NULL(self);
        test_cleanup_push(stack_free, self);

        for (i = 0; i < 10; ++i)
            TEST_ASSERT_TRUE(desc->push(self, (int32_t)i));
        TEST_ASSERT_EQ(10, desc->size(self));
        TEST_ASSERT_TRUE(desc->peek(self, &value));
        TEST_ASSERT_EQ(9, value);

        for (i = 0; i < 3; ++i)
            desc->pop(self);
        TEST_ASSERT_EQ(7, desc->size(self));
        TEST_ASSERT_TRUE(desc->peek(self, &value));
        TEST_ASSERT_EQ(6, value);
        TEST_ASSERT_EQ(7, desc->clear(self));
        TEST_ASSERT_TRUE(desc->empty(self));

        TEST_ASSERT_TRUE(desc->push(self, 99));
        TEST_ASSERT_TRUE(desc->peek(self, &value));
        TEST_ASSERT_EQ(99, value);

        test_cleanup_pop(true);
    }
}

TEST_CASE(unit, stack_numeric_backends)
{
    stack_t* f_vector = STACK_NEW_T(float, VECTOR);
    stack_t* f_deque = STACK_NEW_T(float, DEQUE);
    stack_t* f_list = STACK_NEW_T(float, LIST);
    stack_t* d_vector = STACK_NEW_T(double, VECTOR);
    stack_t* d_deque = STACK_NEW_T(double, DEQUE);
    stack_t* d_list = STACK_NEW_T(double, LIST);
    stack_t* i_vector = STACK_NEW_T(int64_t, VECTOR);
    stack_t* i_deque = STACK_NEW_T(int64_t, DEQUE);
    stack_t* i_list = STACK_NEW_T(int64_t, LIST);

    TEST_ASSERT_NOT_NULL(f_vector);
    TEST_ASSERT_NOT_NULL(f_deque);
    TEST_ASSERT_NOT_NULL(f_list);
    TEST_ASSERT_NOT_NULL(d_vector);
    TEST_ASSERT_NOT_NULL(d_deque);
    TEST_ASSERT_NOT_NULL(d_list);
    TEST_ASSERT_NOT_NULL(i_vector);
    TEST_ASSERT_NOT_NULL(i_deque);
    TEST_ASSERT_NOT_NULL(i_list);
    test_cleanup_push(stack_free, f_vector);
    test_cleanup_push(stack_free, f_deque);
    test_cleanup_push(stack_free, f_list);
    test_cleanup_push(stack_free, d_vector);
    test_cleanup_push(stack_free, d_deque);
    test_cleanup_push(stack_free, d_list);
    test_cleanup_push(stack_free, i_vector);
    test_cleanup_push(stack_free, i_deque);
    test_cleanup_push(stack_free, i_list);

    stack_check_float(f_vector);
    stack_check_float(f_deque);
    stack_check_float(f_list);
    stack_check_double(d_vector);
    stack_check_double(d_deque);
    stack_check_double(d_list);
    stack_check_int64(i_vector);
    stack_check_int64(i_deque);
    stack_check_int64(i_list);
}

TEST_CASE(unit, stack_inline_strings)
{
    stack_t* sso_vector = STACK_NEW_SSO_T(VECTOR);
    stack_t* sso_deque = STACK_NEW_SSO_T(DEQUE);
    stack_t* sso_list = STACK_NEW_SSO_T(LIST);
    stack_t* chars_vector = STACK_NEW_CHAR_T(VECTOR);
    stack_t* chars_deque = STACK_NEW_CHAR_T(DEQUE);
    stack_t* chars_list = STACK_NEW_CHAR_T(LIST);

    TEST_ASSERT_NOT_NULL(sso_vector);
    TEST_ASSERT_NOT_NULL(sso_deque);
    TEST_ASSERT_NOT_NULL(sso_list);
    TEST_ASSERT_NOT_NULL(chars_vector);
    TEST_ASSERT_NOT_NULL(chars_deque);
    TEST_ASSERT_NOT_NULL(chars_list);
    test_cleanup_push(stack_free, sso_vector);
    test_cleanup_push(stack_free, sso_deque);
    test_cleanup_push(stack_free, sso_list);
    test_cleanup_push(stack_free, chars_vector);
    test_cleanup_push(stack_free, chars_deque);
    test_cleanup_push(stack_free, chars_list);

    stack_check_strings(sso_vector, chars_vector);
    stack_check_strings(sso_deque, chars_deque);
    stack_check_strings(sso_list, chars_list);
}

TEST_CASE(unit, stack_tracked_ops)
{
    stack_t* vector = STACK_NEW_OPS_COMPLATE(test_ops_tracked(),
                                             SEQ_CNTR_DTYPE_STRUCT,
                                             test_tracked_t, VECTOR, 0);
    stack_t* deque = STACK_NEW_OPS_COMPLATE(test_ops_tracked(),
                                            SEQ_CNTR_DTYPE_STRUCT,
                                            test_tracked_t, DEQUE, 0);
    stack_t* list = STACK_NEW_OPS_COMPLATE(test_ops_tracked(),
                                           SEQ_CNTR_DTYPE_STRUCT,
                                           test_tracked_t, LIST, 0);

    TEST_ASSERT_NOT_NULL(vector);
    TEST_ASSERT_NOT_NULL(deque);
    TEST_ASSERT_NOT_NULL(list);
    test_cleanup_push(stack_free, vector);
    test_cleanup_push(stack_free, deque);
    test_cleanup_push(stack_free, list);

    stack_check_tracked(vector);
    stack_check_tracked(deque);
    stack_check_tracked(list);
}

TEST_CASE(unit, stack_swap_and_null_guards)
{
    const test_adaptor_desc_t* descs[] = {
        stack_desc_vector(), stack_desc_deque(), stack_desc_list()
    };
    size_t d;

    for (d = 0; d < sizeof(descs) / sizeof(descs[0]); ++d) {
        const test_adaptor_desc_t* desc = descs[d];
        void* first = desc->create();
        void* second = desc->create();
        int32_t value = 0;

        TEST_ASSERT_NOT_NULL(first);
        TEST_ASSERT_NOT_NULL(second);
        test_cleanup_push(stack_free, first);
        test_cleanup_push(stack_free, second);

        TEST_ASSERT_TRUE(desc->push(first, 1));
        TEST_ASSERT_TRUE(desc->push(first, 2));
        TEST_ASSERT_TRUE(desc->push(second, 3));
        desc->swap(first, second);
        TEST_ASSERT_EQ(1, desc->size(first));
        TEST_ASSERT_EQ(2, desc->size(second));
        TEST_ASSERT_TRUE(desc->peek(first, &value));
        TEST_ASSERT_EQ(3, value);
        TEST_ASSERT_TRUE(desc->peek(second, &value));
        TEST_ASSERT_EQ(2, value);

        test_cleanup_pop(true);
        test_cleanup_pop(true);
    }

    TEST_ASSERT_EQ(-1, cstack->size(NULL));
    TEST_ASSERT_FALSE(cstack->push(NULL, __ds_s32data(1)));
    TEST_ASSERT_NULL(cstack->top(NULL).u8);
    cstack->pop(NULL);
    cstack->swap(NULL, NULL);
    TEST_ASSERT_EQ(-1, cstack->clear(NULL));
}
