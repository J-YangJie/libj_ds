/*
  libj_ds permanent tests deque debug death cases
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

#include <deque/deque.h>

#include "../../core/test.h"

#ifdef JDSC_DEBUG

static void deque_free(void* self)
{
    deque_t* deque = self;

    DEQUE_DELETE(&deque);
}

static void death_pop_front(void* arg)
{
    cdeque->pop_front(arg);
}

static void death_pop_back(void* arg)
{
    cdeque->pop_back(arg);
}

static void death_front(void* arg)
{
    deque_value_t value = cdeque->front(arg);

    (void)value;
}

static void death_swap_incompatible(void* arg)
{
    deque_t** deques = arg;

    cdeque->swap(deques[0], deques[1]);
}

static void death_next_null(void* arg)
{
    deque_iterator_t it = i_deque_null_iterator();

    (void)arg;
    (void)cdeque->next(it);
}

typedef struct death_struct {
    int32_t value;
} death_struct_t;

static bool death_struct_valid(ds_data_t data)
{
    return data != 0;
}

static bool death_struct_copy(ds_data_t in, ds_data_t* out)
{
    const death_struct_t* src = (const death_struct_t*)(uintptr_t)in;
    death_struct_t* dst = (death_struct_t*)out;

    *dst = *src;
    return true;
}

static void death_struct_free(ds_data_t* data)
{
    (void)data;
}

static const class_deque_ops_t death_struct_no_eq_ops = {
    .valid_data = death_struct_valid,
    .copy_data = death_struct_copy,
    .free_data = death_struct_free,
};

static void death_find_no_eq(void* arg)
{
    death_struct_t needle = { 7 };
    deque_iterator_t it = cdeque->find(arg, ds_cstdata(&needle));

    (void)it;
}

TEST_CASE(death, deque_pop_front_empty)
{
    deque_t* deque = DEQUE_NEW_T(int32_t);

    TEST_ASSERT_NOT_NULL(deque);
    test_cleanup_push(deque_free, deque);
    test_death_expect_abort(death_pop_front, deque,
                            "`pop_front` on an empty deque");
}

TEST_CASE(death, deque_pop_back_empty)
{
    deque_t* deque = DEQUE_NEW_T(int32_t);

    TEST_ASSERT_NOT_NULL(deque);
    test_cleanup_push(deque_free, deque);
    test_death_expect_abort(death_pop_back, deque,
                            "`pop_back` on an empty deque");
}

TEST_CASE(death, deque_front_empty)
{
    deque_t* deque = DEQUE_NEW_T(int32_t);

    TEST_ASSERT_NOT_NULL(deque);
    test_cleanup_push(deque_free, deque);
    test_death_expect_abort(death_front, deque,
                            "`front` on an empty deque");
}

TEST_CASE(death, deque_swap_incompatible)
{
    deque_t* first = DEQUE_NEW_T(int32_t);
    deque_t* second = DEQUE_NEW_T(int64_t);
    deque_t* pair[2];

    TEST_ASSERT_NOT_NULL(first);
    TEST_ASSERT_NOT_NULL(second);
    test_cleanup_push(deque_free, first);
    test_cleanup_push(deque_free, second);
    pair[0] = first;
    pair[1] = second;
    test_death_expect_abort(death_swap_incompatible, pair,
                            "`swap` with incompatible element layouts");
}

TEST_CASE(death, deque_next_null)
{
    test_death_expect_abort(death_next_null, NULL,
                            "`next` on a null iterator");
}

TEST_CASE(death, deque_find_without_eq)
{
    deque_t* deque = DEQUE_NEW_COMPLATE(&death_struct_no_eq_ops,
                                        SEQ_CNTR_DTYPE_STRUCT,
                                        death_struct_t);
    death_struct_t element = { 7 };

    TEST_ASSERT_NOT_NULL(deque);
    test_cleanup_push(deque_free, deque);
    TEST_ASSERT_TRUE(cdeque->push_back(deque, ds_cstdata(&element)));
    test_death_expect_abort(death_find_no_eq, deque,
                            "`find` without `__eq` on a STUN type");
}

#endif /* JDSC_DEBUG */
