/*
  libj_ds permanent tests deque descriptor
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
#include <string.h>

#include "desc.h"

#define DEQUE_DESC_CAPS                                                     \
    (TEST_SEQ_PUSH_FRONT | TEST_SEQ_POP_FRONT | TEST_SEQ_SHRINK             \
     | TEST_SEQ_ASSIGN_RANGE | TEST_SEQ_INSERT_RANGE | TEST_SEQ_REMOVE_IF   \
     | TEST_SEQ_SWAP)

static size_t deque_int32_size(const void* self)
{
    deque_size_t size = cdeque->size(self);

    return size < 0 ? 0 : (size_t)size;
}

static deque_iterator_t deque_int32_position(const deque_t* self, size_t index)
{
    if (index == deque_int32_size(self))
        return cdeque->end(self);
    return cdeque->it(self, index);
}

static void* deque_int32_create(void)
{
    return DEQUE_NEW_T(int32_t);
}

static void deque_int32_destroy(void* self)
{
    deque_t* deque = self;

    DEQUE_DELETE(&deque);
}

static bool deque_int32_get(const void* self, size_t index, int32_t* out)
{
    deque_value_t value = cdeque->at(self, index);

    if (value.u8 == NULL)
        return false;
    *out = *value.s32;
    return true;
}

static bool deque_int32_foreach(const void* self, bool reverse,
                                test_seq_visit_fn fn, void* ctx)
{
    if (!reverse) {
        deque_iterator_t it = cdeque->begin(self);
        deque_iterator_t end = cdeque->end(self);

        while (it.cur != end.cur) {
            int32_t value;

            memcpy(&value, it.cur, sizeof(value));
            if (!fn(ctx, value))
                return false;
            it = cdeque->next(it);
        }
    } else {
        deque_r_iterator_t it = cdeque->rbegin(self);
        deque_r_iterator_t end = cdeque->rend(self);

        while (it.cur != end.cur) {
            int32_t value;

            memcpy(&value, it.cur, sizeof(value));
            if (!fn(ctx, value))
                return false;
            it = cdeque->rnext(it);
        }
    }
    return true;
}

static bool deque_int32_push_back(void* self, int32_t value)
{
    return cdeque->push_back(self, __ds_s32data(value));
}

static bool deque_int32_push_front(void* self, int32_t value)
{
    return cdeque->push_front(self, __ds_s32data(value));
}

static void deque_int32_pop_back(void* self)
{
    cdeque->pop_back(self);
}

static void deque_int32_pop_front(void* self)
{
    cdeque->pop_front(self);
}

static bool deque_int32_insert(void* self, size_t index, int32_t value,
                               size_t* out_index)
{
    deque_iterator_t ret = cdeque->insert(self, deque_int32_position(self, index),
                                          __ds_s32data(value));

    if (ret.cur == NULL)
        return false;
    *out_index = index;
    return true;
}

static bool deque_int32_insert_n(void* self, size_t index, size_t n,
                                 int32_t value, size_t* out_index)
{
    deque_iterator_t ret = cdeque->insert_n(self, deque_int32_position(self, index),
                                            n, __ds_s32data(value));

    if (ret.cur == NULL)
        return false;
    *out_index = index;
    return true;
}

static bool deque_int32_insert_range_values(void* self, size_t index,
                                            const int32_t* values, size_t n,
                                            size_t* out_index)
{
    deque_t* source = DEQUE_NEW_T(int32_t);
    deque_iterator_t ret;
    size_t i;

    if (source == NULL)
        return false;
    for (i = 0; i < n; ++i) {
        if (!cdeque->push_back(source, __ds_s32data(values[i]))) {
            DEQUE_DELETE(&source);
            return false;
        }
    }
    ret = cdeque->insert_range(self, deque_int32_position(self, index),
                               cdeque->begin(source), cdeque->end(source));
    DEQUE_DELETE(&source);
    if (ret.cur == NULL)
        return false;
    *out_index = index;
    return true;
}

static bool deque_int32_erase(void* self, size_t index, size_t* out_next)
{
    deque_iterator_t ret = cdeque->erase(self, cdeque->it(self, index));

    if (ret.cur == NULL)
        return false;
    *out_next = (size_t)__i_deque_iterator_distance(cdeque->begin(self), ret);
    return true;
}

static bool deque_int32_erase_range(void* self, size_t first, size_t last,
                                    size_t* out_next)
{
    deque_iterator_t ret = cdeque->erase_range(self,
                                               deque_int32_position(self, first),
                                               deque_int32_position(self, last));

    if (ret.cur == NULL)
        return false;
    *out_next = (size_t)__i_deque_iterator_distance(cdeque->begin(self), ret);
    return true;
}

static bool deque_int32_find(const void* self, int32_t value, size_t* out_index)
{
    deque_iterator_t it = cdeque->find(self, __ds_s32data(value));
    deque_iterator_t end = cdeque->end(self);

    if (it.cur == NULL || it.cur == end.cur)
        return false;
    *out_index = (size_t)__i_deque_iterator_distance(cdeque->begin(self), it);
    return true;
}

static size_t deque_int32_count(const void* self, int32_t value)
{
    deque_count_t count = cdeque->count(self, __ds_s32data(value));

    return count < 0 ? 0 : (size_t)count;
}

static size_t deque_int32_remove(void* self, int32_t value)
{
    deque_count_t count = cdeque->remove(self, __ds_s32data(value));

    return count < 0 ? 0 : (size_t)count;
}

static test_seq_pred_fn deque_remove_pred;
static void* deque_remove_ctx;

static bool deque_int32_remove_bridge(ds_data_t data)
{
    return deque_remove_pred(__ds_token_to_s32(data), deque_remove_ctx);
}

static size_t deque_int32_remove_if(void* self, test_seq_pred_fn pred, void* ctx)
{
    deque_count_t count;

    deque_remove_pred = pred;
    deque_remove_ctx = ctx;
    count = cdeque->remove_if(self, deque_int32_remove_bridge);
    deque_remove_pred = NULL;
    deque_remove_ctx = NULL;
    return count < 0 ? 0 : (size_t)count;
}

static bool deque_int32_assign(void* self, size_t n, int32_t value)
{
    return cdeque->assign(self, n, __ds_s32data(value));
}

static bool deque_int32_assign_range_values(void* self, const int32_t* values,
                                            size_t n)
{
    deque_t* source = DEQUE_NEW_T(int32_t);
    bool ok;
    size_t i;

    if (source == NULL)
        return false;
    for (i = 0; i < n; ++i) {
        if (!cdeque->push_back(source, __ds_s32data(values[i]))) {
            DEQUE_DELETE(&source);
            return false;
        }
    }
    ok = cdeque->assign_range(self, cdeque->begin(source), cdeque->end(source));
    DEQUE_DELETE(&source);
    return ok;
}

static bool deque_int32_resize(void* self, size_t n, int32_t value)
{
    return cdeque->resize(self, n, __ds_s32data(value));
}

static void deque_int32_shrink_to_fit(void* self)
{
    cdeque->shrink_to_fit(self);
}

static size_t deque_int32_clear(void* self)
{
    deque_size_t count = cdeque->clear(self);

    return count < 0 ? 0 : (size_t)count;
}

static void deque_int32_swap(void* self, void* other)
{
    cdeque->swap(self, other);
}

static const test_seq_desc_t deque_int32_desc = {
    .name = "deque.int32",
    .caps = DEQUE_DESC_CAPS,
    .create = deque_int32_create,
    .destroy = deque_int32_destroy,
    .size = deque_int32_size,
    .get = deque_int32_get,
    .foreach = deque_int32_foreach,
    .push_back = deque_int32_push_back,
    .push_front = deque_int32_push_front,
    .pop_back = deque_int32_pop_back,
    .pop_front = deque_int32_pop_front,
    .insert = deque_int32_insert,
    .insert_n = deque_int32_insert_n,
    .insert_range_values = deque_int32_insert_range_values,
    .erase = deque_int32_erase,
    .erase_range = deque_int32_erase_range,
    .find = deque_int32_find,
    .count = deque_int32_count,
    .remove = deque_int32_remove,
    .remove_if = deque_int32_remove_if,
    .assign = deque_int32_assign,
    .assign_range_values = deque_int32_assign_range_values,
    .resize = deque_int32_resize,
    .reserve = NULL,
    .shrink_to_fit = deque_int32_shrink_to_fit,
    .clear = deque_int32_clear,
    .swap = deque_int32_swap,
};

const test_seq_desc_t* deque_desc_int32(void)
{
    return &deque_int32_desc;
}

static size_t deque_string_size(const void* self)
{
    deque_size_t size = cdeque->size(self);

    return size < 0 ? 0 : (size_t)size;
}

static deque_iterator_t deque_string_position(const deque_t* self, size_t index)
{
    if (index == deque_string_size(self))
        return cdeque->end(self);
    return cdeque->it(self, index);
}

static void* deque_string_create(void)
{
    return DEQUE_NEW_SSO();
}

static void deque_string_destroy(void* self)
{
    deque_t* deque = self;

    DEQUE_DELETE(&deque);
}

static bool deque_string_get(const void* self, size_t index, char* buf,
                             size_t cap)
{
    deque_value_t value = cdeque->at(self, index);
    size_t len;

    if (value.u8 == NULL)
        return false;
    len = value.ssd->len;
    if (len + 1 > cap)
        return false;
    memcpy(buf, value.ssd->p, len + 1);
    return true;
}

static bool deque_string_foreach(const void* self, bool reverse,
                                 test_seq_str_visit_fn fn, void* ctx)
{
    if (!reverse) {
        deque_iterator_t it = cdeque->begin(self);
        deque_iterator_t end = cdeque->end(self);

        while (it.cur != end.cur) {
            if (!fn(ctx, it.ssd->p))
                return false;
            it = cdeque->next(it);
        }
    } else {
        deque_r_iterator_t it = cdeque->rbegin(self);
        deque_r_iterator_t end = cdeque->rend(self);

        while (it.cur != end.cur) {
            if (!fn(ctx, it.ssd->p))
                return false;
            it = cdeque->rnext(it);
        }
    }
    return true;
}

static bool deque_string_push_back(void* self, const char* value)
{
    return cdeque->push_back(self, ds_sdata(value));
}

static bool deque_string_push_front(void* self, const char* value)
{
    return cdeque->push_front(self, ds_sdata(value));
}

static bool deque_string_insert(void* self, size_t index, const char* value,
                                size_t* out_index)
{
    deque_iterator_t ret = cdeque->insert(self, deque_string_position(self, index),
                                          ds_sdata(value));

    if (ret.cur == NULL)
        return false;
    *out_index = index;
    return true;
}

static bool deque_string_erase(void* self, size_t index, size_t* out_next)
{
    deque_iterator_t ret = cdeque->erase(self, cdeque->it(self, index));

    if (ret.cur == NULL)
        return false;
    *out_next = (size_t)__i_deque_iterator_distance(cdeque->begin(self), ret);
    return true;
}

static size_t deque_string_clear(void* self)
{
    deque_size_t count = cdeque->clear(self);

    return count < 0 ? 0 : (size_t)count;
}

static void deque_string_swap(void* self, void* other)
{
    cdeque->swap(self, other);
}

static const test_seq_str_desc_t deque_string_desc = {
    .name = "deque.string",
    .create = deque_string_create,
    .destroy = deque_string_destroy,
    .size = deque_string_size,
    .get = deque_string_get,
    .foreach = deque_string_foreach,
    .push_back = deque_string_push_back,
    .push_front = deque_string_push_front,
    .insert = deque_string_insert,
    .erase = deque_string_erase,
    .clear = deque_string_clear,
    .swap = deque_string_swap,
};

const test_seq_str_desc_t* deque_desc_string(void)
{
    return &deque_string_desc;
}
