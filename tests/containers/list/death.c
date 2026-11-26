/*
  libj_ds permanent tests list debug death cases
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

#include <list/list.h>

#include "../../core/test.h"

#ifdef JDSC_DEBUG

static void list_free(void* self)
{
    list_t* list = self;

    LIST_DELETE(&list);
}

static void death_pop_back(void* arg)
{
    clist->pop_back(arg);
}

static void death_pop_front(void* arg)
{
    clist->pop_front(arg);
}

static void death_front(void* arg)
{
    list_value_t value = clist->front(arg);

    (void)value;
}

static void death_back(void* arg)
{
    list_value_t value = clist->back(arg);

    (void)value;
}

static void death_swap_incompatible(void* arg)
{
    list_t** lists = arg;

    clist->swap(lists[0], lists[1]);
}

static void death_splice_incompatible(void* arg)
{
    list_t** lists = arg;

    clist->splice(lists[0], clist->begin(lists[0]), lists[1]);
}

static void death_merge_incompatible(void* arg)
{
    list_t** lists = arg;

    clist->merge(lists[0], lists[1], __list_lt_default);
}

static void death_splice_range_incompatible(void* arg)
{
    list_t** lists = arg;

    clist->splice_range(lists[0], clist->begin(lists[0]), lists[1],
                        clist->begin(lists[1]), clist->end(lists[1]));
}

static void death_splice_range_self_inside(void* arg)
{
    list_t* list = arg;

    clist->splice_range(list, clist->it(list, 1), list, clist->it(list, 0),
                        clist->it(list, 2));
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

static const class_list_ops_t death_struct_no_eq_ops = {
    .valid_data = death_struct_valid,
    .copy_data = death_struct_copy,
    .free_data = death_struct_free,
};

static void death_find_no_eq(void* arg)
{
    death_struct_t needle = { 7 };
    list_iterator_t it = clist->find(arg, ds_cstdata(&needle));

    (void)it;
}

static void death_new_invalid_step(void* arg)
{
    list_t* list = __list_new(NULL, SEQ_CNTR_DTYPE_S32, 0, 0);

    (void)arg;
    (void)list;
}

static void death_new_invalid_sso(void* arg)
{
    list_t* list = __list_new(g_class_list_ops_sso(), SEQ_CNTR_DTYPE_S32,
                              sizeof(int32_t), 0);

    (void)arg;
    (void)list;
}

TEST_CASE(death, list_pop_back_empty)
{
    list_t* list = LIST_NEW_T(int32_t);

    TEST_ASSERT_NOT_NULL(list);
    test_cleanup_push(list_free, list);
    test_death_expect_abort(death_pop_back, list,
                            "`pop_back` on an empty list");
}

TEST_CASE(death, list_pop_front_empty)
{
    list_t* list = LIST_NEW_T(int32_t);

    TEST_ASSERT_NOT_NULL(list);
    test_cleanup_push(list_free, list);
    test_death_expect_abort(death_pop_front, list,
                            "`pop_front` on an empty list");
}

TEST_CASE(death, list_front_empty)
{
    list_t* list = LIST_NEW_T(int32_t);

    TEST_ASSERT_NOT_NULL(list);
    test_cleanup_push(list_free, list);
    test_death_expect_abort(death_front, list, "`front` on an empty list");
}

TEST_CASE(death, list_back_empty)
{
    list_t* list = LIST_NEW_T(int32_t);

    TEST_ASSERT_NOT_NULL(list);
    test_cleanup_push(list_free, list);
    test_death_expect_abort(death_back, list, "`back` on an empty list");
}

TEST_CASE(death, list_swap_incompatible)
{
    list_t* first = LIST_NEW_T(int32_t);
    list_t* second = LIST_NEW_T(int64_t);
    list_t* pair[2];

    TEST_ASSERT_NOT_NULL(first);
    TEST_ASSERT_NOT_NULL(second);
    test_cleanup_push(list_free, first);
    test_cleanup_push(list_free, second);
    pair[0] = first;
    pair[1] = second;
    test_death_expect_abort(death_swap_incompatible, pair,
                            "`swap` with incompatible element layouts");
}

TEST_CASE(death, list_splice_incompatible)
{
    list_t* first = LIST_NEW_T(int32_t);
    list_t* second = LIST_NEW_T(int64_t);
    list_t* pair[2];

    TEST_ASSERT_NOT_NULL(first);
    TEST_ASSERT_NOT_NULL(second);
    test_cleanup_push(list_free, first);
    test_cleanup_push(list_free, second);
    TEST_ASSERT_TRUE(clist->push_back(first, __ds_s32data(1)));
    TEST_ASSERT_TRUE(clist->push_back(second, __ds_s64data(2)));
    pair[0] = first;
    pair[1] = second;
    test_death_expect_abort(death_splice_incompatible, pair,
                            "`splice` with incompatible element layouts");
}

TEST_CASE(death, list_merge_incompatible)
{
    list_t* first = LIST_NEW_T(int32_t);
    list_t* second = LIST_NEW_T(int64_t);
    list_t* pair[2];

    TEST_ASSERT_NOT_NULL(first);
    TEST_ASSERT_NOT_NULL(second);
    test_cleanup_push(list_free, first);
    test_cleanup_push(list_free, second);
    TEST_ASSERT_TRUE(clist->push_back(first, __ds_s32data(1)));
    TEST_ASSERT_TRUE(clist->push_back(second, __ds_s64data(2)));
    pair[0] = first;
    pair[1] = second;
    test_death_expect_abort(death_merge_incompatible, pair,
                            "`merge` with incompatible element layouts");
}

TEST_CASE(death, list_splice_range_incompatible)
{
    list_t* first = LIST_NEW_T(int32_t);
    list_t* second = LIST_NEW_T(int64_t);
    list_t* pair[2];

    TEST_ASSERT_NOT_NULL(first);
    TEST_ASSERT_NOT_NULL(second);
    test_cleanup_push(list_free, first);
    test_cleanup_push(list_free, second);
    TEST_ASSERT_TRUE(clist->push_back(first, __ds_s32data(1)));
    TEST_ASSERT_TRUE(clist->push_back(second, __ds_s64data(2)));
    pair[0] = first;
    pair[1] = second;
    test_death_expect_abort(death_splice_range_incompatible, pair,
                            "`splice_range` with incompatible layouts");
}

TEST_CASE(death, list_splice_range_self_inside)
{
    list_t* list = LIST_NEW_T(int32_t);

    TEST_ASSERT_NOT_NULL(list);
    test_cleanup_push(list_free, list);
    TEST_ASSERT_TRUE(clist->push_back(list, __ds_s32data(1)));
    TEST_ASSERT_TRUE(clist->push_back(list, __ds_s32data(2)));
    TEST_ASSERT_TRUE(clist->push_back(list, __ds_s32data(3)));
    test_death_expect_abort(death_splice_range_self_inside, list,
                            "self `splice_range` with `pos` inside the range");
}

TEST_CASE(death, list_find_without_eq)
{
    list_t* list = LIST_NEW_COMPLATE(&death_struct_no_eq_ops,
                                     SEQ_CNTR_DTYPE_STRUCT,
                                     death_struct_t);
    death_struct_t element = { 7 };

    TEST_ASSERT_NOT_NULL(list);
    test_cleanup_push(list_free, list);
    TEST_ASSERT_TRUE(clist->push_back(list, ds_cstdata(&element)));
    test_death_expect_abort(death_find_no_eq, list,
                            "`find` without `__eq` on a STUN type");
}

TEST_CASE(death, list_constructor_invalid_step)
{
    test_death_expect_abort(death_new_invalid_step, NULL,
                            "a zero `step` in `__list_new`");
}

TEST_CASE(death, list_constructor_invalid_sso)
{
    test_death_expect_abort(death_new_invalid_sso, NULL,
                            "SSO ops with a non-SSO data type");
}

#endif /* JDSC_DEBUG */
