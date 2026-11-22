/*
  libj_ds permanent tests list descriptor
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

#include <string.h>

#include "desc.h"

#define LIST_DESC_CAPS                                                       \
    (TEST_SEQ_PUSH_FRONT | TEST_SEQ_POP_FRONT | TEST_SEQ_ASSIGN_RANGE        \
     | TEST_SEQ_INSERT_RANGE | TEST_SEQ_REMOVE_IF | TEST_SEQ_SWAP)

static list_iterator_t list_position(const list_t* self, size_t index)
{
    if (index == (size_t)clist->size(self))
        return clist->end(self);
    return clist->it(self, index);
}

static size_t list_index_of(const list_t* self, list_iterator_t target)
{
    list_iterator_t it;
    list_iterator_t end = clist->end(self);
    size_t index = 0;

    for (it = clist->begin(self); it.d != end.d; it = clist->next(it)) {
        if (it.d == target.d)
            return index;
        ++index;
    }
    return index;
}

static size_t list_int32_size(const void* self)
{
    list_size_t size = clist->size(self);

    return size < 0 ? 0 : (size_t)size;
}

static void* list_int32_create(void)
{
    return LIST_NEW_T(int32_t);
}

static void list_int32_destroy(void* self)
{
    list_t* list = self;

    LIST_DELETE(&list);
}

static bool list_int32_get(const void* self, size_t index, int32_t* out)
{
    list_value_t value = clist->at(self, index);

    if (value.u8 == NULL)
        return false;
    *out = *value.s32;
    return true;
}

static bool list_int32_foreach(const void* self, bool reverse,
                               test_seq_visit_fn fn, void* ctx)
{
    if (!reverse) {
        list_iterator_t it = clist->begin(self);
        list_iterator_t end = clist->end(self);

        while (it.d != end.d) {
            if (!fn(ctx, it_s32data(it)))
                return false;
            it = clist->next(it);
        }
    } else {
        list_r_iterator_t it = clist->rbegin(self);
        list_r_iterator_t end = clist->rend(self);

        while (it.d != end.d) {
            if (!fn(ctx, it_s32data(it)))
                return false;
            it = clist->rnext(it);
        }
    }
    return true;
}

static bool list_int32_push_back(void* self, int32_t value)
{
    return clist->push_back(self, __ds_s32data(value));
}

static bool list_int32_push_front(void* self, int32_t value)
{
    return clist->push_front(self, __ds_s32data(value));
}

static void list_int32_pop_back(void* self)
{
    clist->pop_back(self);
}

static void list_int32_pop_front(void* self)
{
    clist->pop_front(self);
}

static bool list_int32_insert(void* self, size_t index, int32_t value,
                              size_t* out_index)
{
    list_iterator_t ret = clist->insert(self, list_position(self, index),
                                        __ds_s32data(value));

    if (ret.d == NULL)
        return false;
    *out_index = index;
    return true;
}

static bool list_int32_insert_n(void* self, size_t index, size_t n,
                                int32_t value, size_t* out_index)
{
    list_iterator_t ret = clist->insert_n(self, list_position(self, index),
                                          (list_size_t)n, __ds_s32data(value));

    if (ret.d == NULL)
        return false;
    *out_index = index;
    return true;
}

static bool list_int32_insert_range_values(void* self, size_t index,
                                           const int32_t* values, size_t n,
                                           size_t* out_index)
{
    list_t* source = LIST_NEW_T(int32_t);
    list_iterator_t ret;
    size_t i;

    if (source == NULL)
        return false;
    for (i = 0; i < n; ++i) {
        if (!clist->push_back(source, __ds_s32data(values[i]))) {
            LIST_DELETE(&source);
            return false;
        }
    }
    ret = clist->insert_range(self, list_position(self, index),
                              clist->begin(source), clist->end(source));
    LIST_DELETE(&source);
    if (ret.d == NULL)
        return false;
    *out_index = index;
    return true;
}

static bool list_int32_erase(void* self, size_t index, size_t* out_next)
{
    list_iterator_t ret = clist->erase(self, clist->it(self, index));

    if (ret.d == NULL)
        return false;
    *out_next = index;
    return true;
}

static bool list_int32_erase_range(void* self, size_t first, size_t last,
                                   size_t* out_next)
{
    list_iterator_t ret = clist->erase_range(self, list_position(self, first),
                                             list_position(self, last));

    if (ret.d == NULL)
        return false;
    *out_next = first;
    return true;
}

static bool list_int32_find(const void* self, int32_t value, size_t* out_index)
{
    list_iterator_t it = clist->find(self, __ds_s32data(value));
    list_iterator_t end = clist->end(self);

    if (it.d == NULL || it.d == end.d)
        return false;
    *out_index = list_index_of(self, it);
    return true;
}

static size_t list_int32_count(const void* self, int32_t value)
{
    list_count_t count = clist->count(self, __ds_s32data(value));

    return count < 0 ? 0 : (size_t)count;
}

static size_t list_int32_remove(void* self, int32_t value)
{
    list_size_t count = clist->remove(self, __ds_s32data(value));

    return count < 0 ? 0 : (size_t)count;
}

static test_seq_pred_fn list_remove_pred;
static void* list_remove_ctx;

static bool list_int32_remove_bridge(ds_data_t data)
{
    return list_remove_pred(__ds_token_to_s32(data), list_remove_ctx);
}

static size_t list_int32_remove_if(void* self, test_seq_pred_fn pred, void* ctx)
{
    list_size_t count;

    list_remove_pred = pred;
    list_remove_ctx = ctx;
    count = clist->remove_if(self, list_int32_remove_bridge);
    list_remove_pred = NULL;
    list_remove_ctx = NULL;
    return count < 0 ? 0 : (size_t)count;
}

static bool list_int32_assign(void* self, size_t n, int32_t value)
{
    return clist->assign(self, (list_size_t)n, __ds_s32data(value));
}

static bool list_int32_assign_range_values(void* self, const int32_t* values,
                                           size_t n)
{
    list_t* source = LIST_NEW_T(int32_t);
    bool ok;
    size_t i;

    if (source == NULL)
        return false;
    for (i = 0; i < n; ++i) {
        if (!clist->push_back(source, __ds_s32data(values[i]))) {
            LIST_DELETE(&source);
            return false;
        }
    }
    ok = clist->assign_range(self, clist->begin(source), clist->end(source));
    LIST_DELETE(&source);
    return ok;
}

static bool list_int32_resize(void* self, size_t n, int32_t value)
{
    return clist->resize(self, (list_size_t)n, __ds_s32data(value));
}

static size_t list_int32_clear(void* self)
{
    list_size_t count = clist->clear(self);

    return count < 0 ? 0 : (size_t)count;
}

static void list_int32_swap(void* self, void* other)
{
    clist->swap(self, other);
}

static const test_seq_desc_t list_int32_desc = {
    .name = "list.int32",
    .caps = LIST_DESC_CAPS,
    .create = list_int32_create,
    .destroy = list_int32_destroy,
    .size = list_int32_size,
    .get = list_int32_get,
    .foreach = list_int32_foreach,
    .push_back = list_int32_push_back,
    .push_front = list_int32_push_front,
    .pop_back = list_int32_pop_back,
    .pop_front = list_int32_pop_front,
    .insert = list_int32_insert,
    .insert_n = list_int32_insert_n,
    .insert_range_values = list_int32_insert_range_values,
    .erase = list_int32_erase,
    .erase_range = list_int32_erase_range,
    .find = list_int32_find,
    .count = list_int32_count,
    .remove = list_int32_remove,
    .remove_if = list_int32_remove_if,
    .assign = list_int32_assign,
    .assign_range_values = list_int32_assign_range_values,
    .resize = list_int32_resize,
    .reserve = NULL,
    .shrink_to_fit = NULL,
    .clear = list_int32_clear,
    .swap = list_int32_swap,
};

const test_seq_desc_t* list_desc_int32(void)
{
    return &list_int32_desc;
}

static size_t list_string_size(const void* self)
{
    list_size_t size = clist->size(self);

    return size < 0 ? 0 : (size_t)size;
}

static void* list_string_create(void)
{
    return LIST_NEW_SSO();
}

static void list_string_destroy(void* self)
{
    list_t* list = self;

    LIST_DELETE(&list);
}

static bool list_string_get(const void* self, size_t index, char* buf,
                            size_t cap)
{
    list_value_t value = clist->at(self, index);
    size_t len;

    if (value.u8 == NULL)
        return false;
    len = value.ssd->len;
    if (len + 1 > cap)
        return false;
    memcpy(buf, value.ssd->p, len + 1);
    return true;
}

static bool list_string_foreach(const void* self, bool reverse,
                                test_seq_str_visit_fn fn, void* ctx)
{
    if (!reverse) {
        list_iterator_t it = clist->begin(self);
        list_iterator_t end = clist->end(self);

        while (it.d != end.d) {
            if (!fn(ctx, it.ssd->p))
                return false;
            it = clist->next(it);
        }
    } else {
        list_r_iterator_t it = clist->rbegin(self);
        list_r_iterator_t end = clist->rend(self);

        while (it.d != end.d) {
            if (!fn(ctx, it.ssd->p))
                return false;
            it = clist->rnext(it);
        }
    }
    return true;
}

static bool list_string_push_back(void* self, const char* value)
{
    return clist->push_back(self, ds_sdata(value));
}

static bool list_string_push_front(void* self, const char* value)
{
    return clist->push_front(self, ds_sdata(value));
}

static bool list_string_insert(void* self, size_t index, const char* value,
                               size_t* out_index)
{
    list_iterator_t ret = clist->insert(self, list_position(self, index),
                                        ds_sdata(value));

    if (ret.d == NULL)
        return false;
    *out_index = index;
    return true;
}

static bool list_string_erase(void* self, size_t index, size_t* out_next)
{
    list_iterator_t ret = clist->erase(self, clist->it(self, index));

    if (ret.d == NULL)
        return false;
    *out_next = index;
    return true;
}

static size_t list_string_clear(void* self)
{
    list_size_t count = clist->clear(self);

    return count < 0 ? 0 : (size_t)count;
}

static void list_string_swap(void* self, void* other)
{
    clist->swap(self, other);
}

static const test_seq_str_desc_t list_string_desc = {
    .name = "list.string",
    .create = list_string_create,
    .destroy = list_string_destroy,
    .size = list_string_size,
    .get = list_string_get,
    .foreach = list_string_foreach,
    .push_back = list_string_push_back,
    .push_front = list_string_push_front,
    .insert = list_string_insert,
    .erase = list_string_erase,
    .clear = list_string_clear,
    .swap = list_string_swap,
};

const test_seq_str_desc_t* list_desc_string(void)
{
    return &list_string_desc;
}

static void list_ext_int32_sort(void* self, bool ascending)
{
    clist->sort(self, ascending ? __list_lt_default : __list_gt_default);
}

static bool list_ext_int32_sorted(const void* self, bool ascending)
{
    return clist_sorted_default(self, ascending);
}

static void list_ext_int32_reverse(void* self)
{
    clist->reverse(self);
}

static size_t list_ext_int32_unique(void* self)
{
    list_size_t count = clist->unique(self);

    return count < 0 ? 0 : (size_t)count;
}

static bool list_ext_int32_replace(void* self, size_t index, int32_t value)
{
    return clist->replace_index(self, (list_index_t)index,
                                __ds_s32data(value));
}

static void list_ext_int32_splice(void* self, size_t index, void* source)
{
    clist->splice(self, list_position(self, index), source);
}

static void list_ext_int32_splice_range(void* self, size_t index,
                                        void* source, size_t first,
                                        size_t last)
{
    clist->splice_range(self, list_position(self, index), source,
                        list_position(source, first),
                        list_position(source, last));
}

static void list_ext_int32_merge(void* self, void* source, bool ascending)
{
    clist->merge(self, source,
                 ascending ? __list_lt_default : __list_gt_default);
}

static const test_list_ext_desc_t list_int32_ext_desc = {
    .sort = list_ext_int32_sort,
    .sorted = list_ext_int32_sorted,
    .reverse = list_ext_int32_reverse,
    .unique = list_ext_int32_unique,
    .replace = list_ext_int32_replace,
    .splice = list_ext_int32_splice,
    .splice_range = list_ext_int32_splice_range,
    .merge = list_ext_int32_merge,
};

const test_list_ext_desc_t* list_desc_ext_int32(void)
{
    return &list_int32_ext_desc;
}

static void list_ext_string_sort(void* self, bool ascending)
{
    clist->sort(self, ascending ? __list_lt_sso : __list_gt_sso);
}

static bool list_ext_string_sorted(const void* self, bool ascending)
{
    return clist_sorted_sso(self, ascending);
}

static void list_ext_string_reverse(void* self)
{
    clist->reverse(self);
}

static size_t list_ext_string_unique(void* self)
{
    list_size_t count = clist->unique(self);

    return count < 0 ? 0 : (size_t)count;
}

static bool list_ext_string_replace(void* self, size_t index,
                                    const char* value)
{
    return clist->replace_index(self, (list_index_t)index, ds_sdata(value));
}

static void list_ext_string_splice(void* self, size_t index, void* source)
{
    clist->splice(self, list_position(self, index), source);
}

static void list_ext_string_splice_range(void* self, size_t index,
                                         void* source, size_t first,
                                         size_t last)
{
    clist->splice_range(self, list_position(self, index), source,
                        list_position(source, first),
                        list_position(source, last));
}

static void list_ext_string_merge(void* self, void* source, bool ascending)
{
    clist->merge(self, source, ascending ? __list_lt_sso : __list_gt_sso);
}

static const test_list_str_ext_desc_t list_string_ext_desc = {
    .sort = list_ext_string_sort,
    .sorted = list_ext_string_sorted,
    .reverse = list_ext_string_reverse,
    .unique = list_ext_string_unique,
    .replace = list_ext_string_replace,
    .splice = list_ext_string_splice,
    .splice_range = list_ext_string_splice_range,
    .merge = list_ext_string_merge,
};

const test_list_str_ext_desc_t* list_desc_ext_string(void)
{
    return &list_string_ext_desc;
}
