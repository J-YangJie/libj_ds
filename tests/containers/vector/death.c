/*
  libj_ds permanent tests vector debug death cases
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

#include <vector/vector.h>

#include "../../core/test.h"

#ifdef JDSC_DEBUG

static void vector_free(void* self)
{
    vector_t* vector = self;

    VECTOR_DELETE(&vector);
}

static void death_pop_back(void* arg)
{
    cvector->pop_back(arg);
}

static void death_front(void* arg)
{
    vector_value_t value = cvector->front(arg);

    (void)value;
}

static void death_back(void* arg)
{
    vector_value_t value = cvector->back(arg);

    (void)value;
}

static void death_swap_incompatible(void* arg)
{
    vector_t** vectors = arg;

    cvector->swap(vectors[0], vectors[1]);
}

static void death_next_null(void* arg)
{
    vector_iterator_t it = i_vector_null_iterator();

    (void)arg;
    (void)cvector->next(it);
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

static const class_vector_ops_t death_struct_no_eq_ops = {
    .valid_data = death_struct_valid,
    .copy_data = death_struct_copy,
    .free_data = death_struct_free,
};

static void death_find_no_eq(void* arg)
{
    death_struct_t needle = { 7 };
    vector_iterator_t it = cvector->find(arg, ds_cstdata(&needle));

    (void)it;
}

static void death_new_invalid(void* arg)
{
    vector_t* vector = __vector_new(NULL, SEQ_CNTR_DTYPE_S32, 0, 0, 0);

    (void)arg;
    (void)vector;
}

TEST_CASE(death, vector_pop_back_empty)
{
    vector_t* vector = VECTOR_NEW_T(int32_t);

    TEST_ASSERT_NOT_NULL(vector);
    test_cleanup_push(vector_free, vector);
    test_death_expect_abort(death_pop_back, vector,
                            "`pop_back` on an empty vector");
}

TEST_CASE(death, vector_front_empty)
{
    vector_t* vector = VECTOR_NEW_T(int32_t);

    TEST_ASSERT_NOT_NULL(vector);
    test_cleanup_push(vector_free, vector);
    test_death_expect_abort(death_front, vector,
                            "`front` on an empty vector");
}

TEST_CASE(death, vector_back_empty)
{
    vector_t* vector = VECTOR_NEW_T(int32_t);

    TEST_ASSERT_NOT_NULL(vector);
    test_cleanup_push(vector_free, vector);
    test_death_expect_abort(death_back, vector,
                            "`back` on an empty vector");
}

TEST_CASE(death, vector_swap_incompatible)
{
    vector_t* first = VECTOR_NEW_T(int32_t);
    vector_t* second = VECTOR_NEW_T(int64_t);
    vector_t* pair[2];

    TEST_ASSERT_NOT_NULL(first);
    TEST_ASSERT_NOT_NULL(second);
    test_cleanup_push(vector_free, first);
    test_cleanup_push(vector_free, second);
    pair[0] = first;
    pair[1] = second;
    test_death_expect_abort(death_swap_incompatible, pair,
                            "`swap` with incompatible element layouts");
}

TEST_CASE(death, vector_next_null)
{
    test_death_expect_abort(death_next_null, NULL,
                            "`next` on a null iterator");
}

TEST_CASE(death, vector_find_without_eq)
{
    vector_t* vector = VECTOR_NEW_COMPLATE(&death_struct_no_eq_ops,
                                           SEQ_CNTR_DTYPE_STRUCT,
                                           death_struct_t, 0);
    death_struct_t element = { 7 };

    TEST_ASSERT_NOT_NULL(vector);
    test_cleanup_push(vector_free, vector);
    TEST_ASSERT_TRUE(cvector->push_back(vector, ds_cstdata(&element)));
    test_death_expect_abort(death_find_no_eq, vector,
                            "`find` without `__eq` on a STUN type");
}

TEST_CASE(death, vector_constructor_invalid)
{
    test_death_expect_abort(death_new_invalid, NULL,
                            "a zero `step` in `__vector_new`");
}

#endif /* JDSC_DEBUG */
