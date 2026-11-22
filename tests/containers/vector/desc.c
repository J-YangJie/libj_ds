/*
  libj_ds permanent tests vector descriptor
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
#include <string.h>

#include "desc.h"

#define VECTOR_DESC_CAPS                                                     \
    (TEST_SEQ_RESERVE | TEST_SEQ_SHRINK | TEST_SEQ_ASSIGN_RANGE             \
     | TEST_SEQ_INSERT_RANGE | TEST_SEQ_REMOVE_IF | TEST_SEQ_SWAP)

static size_t vector_int32_size(const void* self)
{
    vector_size_t size = cvector->size(self);

    return size < 0 ? 0 : (size_t)size;
}

static vector_iterator_t vector_int32_position(const vector_t* self, size_t index)
{
    if (index == vector_int32_size(self))
        return cvector->end(self);
    return cvector->it(self, index);
}

static void* vector_int32_create(void)
{
    return VECTOR_NEW_T(int32_t);
}

static void vector_int32_destroy(void* self)
{
    vector_t* vector = self;

    VECTOR_DELETE(&vector);
}

static bool vector_int32_get(const void* self, size_t index, int32_t* out)
{
    vector_value_t value = cvector->at(self, index);

    if (value.u8 == NULL)
        return false;
    *out = *value.s32;
    return true;
}

static bool vector_int32_foreach(const void* self, bool reverse,
                                test_seq_visit_fn fn, void* ctx)
{
    if (!reverse) {
        vector_iterator_t it = cvector->begin(self);
        vector_iterator_t end = cvector->end(self);

        while (it.cur != end.cur) {
            int32_t value;

            memcpy(&value, it.cur, sizeof(value));
            if (!fn(ctx, value))
                return false;
            it = cvector->next(it);
        }
    } else {
        vector_r_iterator_t it = cvector->rbegin(self);
        vector_r_iterator_t end = cvector->rend(self);

        while (it.cur != end.cur) {
            int32_t value;

            memcpy(&value, it.cur, sizeof(value));
            if (!fn(ctx, value))
                return false;
            it = cvector->rnext(it);
        }
    }
    return true;
}

static bool vector_int32_push_back(void* self, int32_t value)
{
    return cvector->push_back(self, __ds_s32data(value));
}

static void vector_int32_pop_back(void* self)
{
    cvector->pop_back(self);
}

static bool vector_int32_insert(void* self, size_t index, int32_t value,
                               size_t* out_index)
{
    vector_iterator_t ret = cvector->insert(self, vector_int32_position(self, index),
                                          __ds_s32data(value));

    if (ret.cur == NULL)
        return false;
    *out_index = index;
    return true;
}

static bool vector_int32_insert_n(void* self, size_t index, size_t n,
                                 int32_t value, size_t* out_index)
{
    vector_iterator_t ret = cvector->insert_n(self, vector_int32_position(self, index),
                                            n, __ds_s32data(value));

    if (ret.cur == NULL)
        return false;
    *out_index = index;
    return true;
}

static bool vector_int32_insert_range_values(void* self, size_t index,
                                            const int32_t* values, size_t n,
                                            size_t* out_index)
{
    vector_t* source = VECTOR_NEW_T(int32_t);
    vector_iterator_t ret;
    size_t i;

    if (source == NULL)
        return false;
    for (i = 0; i < n; ++i) {
        if (!cvector->push_back(source, __ds_s32data(values[i]))) {
            VECTOR_DELETE(&source);
            return false;
        }
    }
    ret = cvector->insert_range(self, vector_int32_position(self, index),
                               cvector->begin(source), cvector->end(source));
    VECTOR_DELETE(&source);
    if (ret.cur == NULL)
        return false;
    *out_index = index;
    return true;
}

static bool vector_int32_erase(void* self, size_t index, size_t* out_next)
{
    vector_iterator_t ret = cvector->erase(self, cvector->it(self, index));

    if (ret.cur == NULL)
        return false;
    *out_next = (size_t)__i_vector_ptr_diff(ret.cur, cvector->begin(self).cur, ret.step);
    return true;
}

static bool vector_int32_erase_range(void* self, size_t first, size_t last,
                                    size_t* out_next)
{
    vector_iterator_t ret = cvector->erase_range(self,
                                               vector_int32_position(self, first),
                                               vector_int32_position(self, last));

    if (ret.cur == NULL)
        return false;
    *out_next = (size_t)__i_vector_ptr_diff(ret.cur, cvector->begin(self).cur, ret.step);
    return true;
}

static bool vector_int32_find(const void* self, int32_t value, size_t* out_index)
{
    vector_iterator_t it = cvector->find(self, __ds_s32data(value));
    vector_iterator_t end = cvector->end(self);

    if (it.cur == NULL || it.cur == end.cur)
        return false;
    *out_index = (size_t)__i_vector_ptr_diff(it.cur, cvector->begin(self).cur, it.step);
    return true;
}

static size_t vector_int32_count(const void* self, int32_t value)
{
    vector_count_t count = cvector->count(self, __ds_s32data(value));

    return count < 0 ? 0 : (size_t)count;
}

static size_t vector_int32_remove(void* self, int32_t value)
{
    vector_count_t count = cvector->remove(self, __ds_s32data(value));

    return count < 0 ? 0 : (size_t)count;
}

static test_seq_pred_fn vector_remove_pred;
static void* vector_remove_ctx;

static bool vector_int32_remove_bridge(ds_data_t data)
{
    return vector_remove_pred(__ds_token_to_s32(data), vector_remove_ctx);
}

static size_t vector_int32_remove_if(void* self, test_seq_pred_fn pred, void* ctx)
{
    vector_count_t count;

    vector_remove_pred = pred;
    vector_remove_ctx = ctx;
    count = cvector->remove_if(self, vector_int32_remove_bridge);
    vector_remove_pred = NULL;
    vector_remove_ctx = NULL;
    return count < 0 ? 0 : (size_t)count;
}

static bool vector_int32_assign(void* self, size_t n, int32_t value)
{
    return cvector->assign(self, n, __ds_s32data(value));
}

static bool vector_int32_assign_range_values(void* self, const int32_t* values,
                                            size_t n)
{
    vector_t* source = VECTOR_NEW_T(int32_t);
    bool ok;
    size_t i;

    if (source == NULL)
        return false;
    for (i = 0; i < n; ++i) {
        if (!cvector->push_back(source, __ds_s32data(values[i]))) {
            VECTOR_DELETE(&source);
            return false;
        }
    }
    ok = cvector->assign_range(self, cvector->begin(source), cvector->end(source));
    VECTOR_DELETE(&source);
    return ok;
}

static bool vector_int32_resize(void* self, size_t n, int32_t value)
{
    return cvector->resize(self, n, __ds_s32data(value));
}

static bool vector_int32_reserve(void* self, size_t n)
{
    return cvector->reserve(self, (vector_size_t)n);
}

static void vector_int32_shrink_to_fit(void* self)
{
    cvector->shrink_to_fit(self);
}

static size_t vector_int32_clear(void* self)
{
    vector_size_t count = cvector->clear(self);

    return count < 0 ? 0 : (size_t)count;
}

static void vector_int32_swap(void* self, void* other)
{
    cvector->swap(self, other);
}

static const test_seq_desc_t vector_int32_desc = {
    .name = "vector.int32",
    .caps = VECTOR_DESC_CAPS,
    .create = vector_int32_create,
    .destroy = vector_int32_destroy,
    .size = vector_int32_size,
    .get = vector_int32_get,
    .foreach = vector_int32_foreach,
    .push_back = vector_int32_push_back,
    .pop_back = vector_int32_pop_back,
    .insert = vector_int32_insert,
    .insert_n = vector_int32_insert_n,
    .insert_range_values = vector_int32_insert_range_values,
    .erase = vector_int32_erase,
    .erase_range = vector_int32_erase_range,
    .find = vector_int32_find,
    .count = vector_int32_count,
    .remove = vector_int32_remove,
    .remove_if = vector_int32_remove_if,
    .assign = vector_int32_assign,
    .assign_range_values = vector_int32_assign_range_values,
    .resize = vector_int32_resize,
    .reserve = vector_int32_reserve,
    .shrink_to_fit = vector_int32_shrink_to_fit,
    .clear = vector_int32_clear,
    .swap = vector_int32_swap,
};

const test_seq_desc_t* vector_desc_int32(void)
{
    return &vector_int32_desc;
}

static size_t vector_string_size(const void* self)
{
    vector_size_t size = cvector->size(self);

    return size < 0 ? 0 : (size_t)size;
}

static vector_iterator_t vector_string_position(const vector_t* self, size_t index)
{
    if (index == vector_string_size(self))
        return cvector->end(self);
    return cvector->it(self, index);
}

static void* vector_string_create(void)
{
    return VECTOR_NEW_SSO();
}

static void vector_string_destroy(void* self)
{
    vector_t* vector = self;

    VECTOR_DELETE(&vector);
}

static bool vector_string_get(const void* self, size_t index, char* buf,
                             size_t cap)
{
    vector_value_t value = cvector->at(self, index);
    size_t len;

    if (value.u8 == NULL)
        return false;
    len = value.ssd->len;
    if (len + 1 > cap)
        return false;
    memcpy(buf, value.ssd->p, len + 1);
    return true;
}

static bool vector_string_foreach(const void* self, bool reverse,
                                 test_seq_str_visit_fn fn, void* ctx)
{
    if (!reverse) {
        vector_iterator_t it = cvector->begin(self);
        vector_iterator_t end = cvector->end(self);

        while (it.cur != end.cur) {
            if (!fn(ctx, it.ssd->p))
                return false;
            it = cvector->next(it);
        }
    } else {
        vector_r_iterator_t it = cvector->rbegin(self);
        vector_r_iterator_t end = cvector->rend(self);

        while (it.cur != end.cur) {
            if (!fn(ctx, it.ssd->p))
                return false;
            it = cvector->rnext(it);
        }
    }
    return true;
}

static bool vector_string_push_back(void* self, const char* value)
{
    return cvector->push_back(self, ds_sdata(value));
}

static bool vector_string_push_front(void* self, const char* value)
{
    vector_iterator_t ret = cvector->insert(self, cvector->begin(self),
                                            ds_sdata(value));

    return ret.cur != NULL;
}

static bool vector_string_insert(void* self, size_t index, const char* value,
                                size_t* out_index)
{
    vector_iterator_t ret = cvector->insert(self, vector_string_position(self, index),
                                          ds_sdata(value));

    if (ret.cur == NULL)
        return false;
    *out_index = index;
    return true;
}

static bool vector_string_erase(void* self, size_t index, size_t* out_next)
{
    vector_iterator_t ret = cvector->erase(self, cvector->it(self, index));

    if (ret.cur == NULL)
        return false;
    *out_next = (size_t)__i_vector_ptr_diff(ret.cur, cvector->begin(self).cur, ret.step);
    return true;
}

static size_t vector_string_clear(void* self)
{
    vector_size_t count = cvector->clear(self);

    return count < 0 ? 0 : (size_t)count;
}

static void vector_string_swap(void* self, void* other)
{
    cvector->swap(self, other);
}

static const test_seq_str_desc_t vector_string_desc = {
    .name = "vector.string",
    .create = vector_string_create,
    .destroy = vector_string_destroy,
    .size = vector_string_size,
    .get = vector_string_get,
    .foreach = vector_string_foreach,
    .push_back = vector_string_push_back,
    .push_front = vector_string_push_front,
    .insert = vector_string_insert,
    .erase = vector_string_erase,
    .clear = vector_string_clear,
    .swap = vector_string_swap,
};

const test_seq_str_desc_t* vector_desc_string(void)
{
    return &vector_string_desc;
}
