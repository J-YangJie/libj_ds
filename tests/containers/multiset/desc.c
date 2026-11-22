/*
  libj_ds permanent tests multiset descriptor
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

#include <multiset/multiset.h>

#include "desc.h"

static void* multiset_int32_create(void)
{
    return MULTISET_NEW_T(int32_t);
}

static void multiset_int32_destroy(void* self)
{
    multiset_t* multiset = self;

    MULTISET_DELETE(&multiset);
}

static size_t multiset_int32_size(const void* self)
{
    multiset_size_t size = cmultiset->size(self);

    return size < 0 ? 0 : (size_t)size;
}

static size_t multiset_int32_count(const void* self, int32_t key)
{
    multiset_count_t count = cmultiset->count(self, __ds_s32key(key));

    return count < 0 ? 0 : (size_t)count;
}

static bool multiset_int32_insert(void* self, int32_t key, int32_t value,
                                  bool* out_inserted)
{
    multiset_iterator_t it = cmultiset->insert(self, __ds_s32key(key));

    (void)value;
    *out_inserted = it.d != NULL;
    return true;
}

static bool multiset_int32_erase_key(void* self, int32_t key,
                                     int32_t* out_next_key,
                                     bool* out_next_found)
{
    multiset_iterator_t end = cmultiset->end(self);
    multiset_iterator_t it = cmultiset->find(self, __ds_s32key(key));

    if (it_eq(it, end)) {
        *out_next_found = false;
        return true;
    }

    it = cmultiset->erase(self, it);
    if (it_eq(it, end)) {
        *out_next_found = false;
    } else {
        *out_next_key = __ds_token_to_s32(it_data(it));
        *out_next_found = true;
    }
    return true;
}

static size_t multiset_int32_remove(void* self, int32_t key)
{
    multiset_size_t count = cmultiset->remove(self, __ds_s32key(key));

    return count < 0 ? 0 : (size_t)count;
}

static bool multiset_int32_find(const void* self, int32_t key,
                                int32_t* out_value)
{
    multiset_iterator_t it = cmultiset->find(self, __ds_s32key(key));

    if (it_eq(it, cmultiset->end(self)))
        return false;
    *out_value = __ds_token_to_s32(it_data(it));
    return true;
}

static bool multiset_int32_lower_bound(const void* self, int32_t key,
                                       int32_t* out_key, int32_t* out_value)
{
    multiset_iterator_t it = cmultiset->lower_bound(self, __ds_s32key(key));

    if (it_eq(it, cmultiset->end(self)))
        return false;
    *out_key = __ds_token_to_s32(it_data(it));
    *out_value = *out_key;
    return true;
}

static bool multiset_int32_upper_bound(const void* self, int32_t key,
                                       int32_t* out_key, int32_t* out_value)
{
    multiset_iterator_t it = cmultiset->upper_bound(self, __ds_s32key(key));

    if (it_eq(it, cmultiset->end(self)))
        return false;
    *out_key = __ds_token_to_s32(it_data(it));
    *out_value = *out_key;
    return true;
}

static bool multiset_int32_foreach(const void* self, bool reverse,
                                   test_assoc_visit_fn fn, void* ctx)
{
    if (!reverse) {
        multiset_iterator_t it = cmultiset->begin(self);
        multiset_iterator_t end = cmultiset->end(self);

        while (it_ne(it, end)) {
            int32_t key = __ds_token_to_s32(it_data(it));

            if (!fn(ctx, key, key))
                return false;
            it = cmultiset->next(it);
        }
    } else {
        multiset_r_iterator_t it = cmultiset->rbegin(self);
        multiset_r_iterator_t end = cmultiset->rend(self);

        while (it_ne(it, end)) {
            int32_t key = __ds_token_to_s32(it_data(it));

            if (!fn(ctx, key, key))
                return false;
            it = cmultiset->rnext(it);
        }
    }
    return true;
}

static size_t multiset_int32_clear(void* self)
{
    multiset_size_t count = cmultiset->clear(self);

    return count < 0 ? 0 : (size_t)count;
}

static const test_assoc_desc_t multiset_int32_desc = {
    "assoc.multiset.int32",
    TEST_ASSOC_ORDERED | TEST_ASSOC_MULTI | TEST_ASSOC_BOUNDS,
    multiset_int32_create,
    multiset_int32_destroy,
    multiset_int32_size,
    multiset_int32_count,
    multiset_int32_insert,
    NULL,
    multiset_int32_erase_key,
    multiset_int32_remove,
    multiset_int32_find,
    multiset_int32_lower_bound,
    multiset_int32_upper_bound,
    multiset_int32_foreach,
    multiset_int32_clear,
    NULL
};

const test_assoc_desc_t* multiset_desc_int32(void)
{
    return &multiset_int32_desc;
}
