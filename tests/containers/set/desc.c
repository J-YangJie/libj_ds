/*
  libj_ds permanent tests set descriptor
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

#include <set/set.h>

#include "desc.h"

static void* set_int32_create(void)
{
    return SET_NEW_T(int32_t);
}

static void set_int32_destroy(void* self)
{
    set_t* set = self;

    SET_DELETE(&set);
}

static size_t set_int32_size(const void* self)
{
    set_size_t size = cset->size(self);

    return size < 0 ? 0 : (size_t)size;
}

static size_t set_int32_count(const void* self, int32_t key)
{
    set_count_t count = cset->count(self, __ds_s32key(key));

    return count < 0 ? 0 : (size_t)count;
}

static bool set_int32_insert(void* self, int32_t key, int32_t value,
                             bool* out_inserted)
{
    set_iterator_t it = cset->insert(self, __ds_s32key(key));

    (void)value;
    *out_inserted = it.d != NULL;
    return true;
}

static bool set_int32_erase_key(void* self, int32_t key, int32_t* out_next_key,
                                bool* out_next_found)
{
    set_iterator_t end = cset->end(self);
    set_iterator_t it = cset->find(self, __ds_s32key(key));

    if (it_eq(it, end)) {
        *out_next_found = false;
        return true;
    }

    it = cset->erase(self, it);
    if (it_eq(it, end)) {
        *out_next_found = false;
    } else {
        *out_next_key = __ds_token_to_s32(it_data(it));
        *out_next_found = true;
    }
    return true;
}

static size_t set_int32_remove(void* self, int32_t key)
{
    set_size_t count = cset->remove(self, __ds_s32key(key));

    return count < 0 ? 0 : (size_t)count;
}

static bool set_int32_find(const void* self, int32_t key, int32_t* out_value)
{
    set_iterator_t it = cset->find(self, __ds_s32key(key));

    if (it_eq(it, cset->end(self)))
        return false;
    *out_value = __ds_token_to_s32(it_data(it));
    return true;
}

static bool set_int32_lower_bound(const void* self, int32_t key,
                                  int32_t* out_key, int32_t* out_value)
{
    set_iterator_t it = cset->lower_bound(self, __ds_s32key(key));

    if (it_eq(it, cset->end(self)))
        return false;
    *out_key = __ds_token_to_s32(it_data(it));
    *out_value = *out_key;
    return true;
}

static bool set_int32_upper_bound(const void* self, int32_t key,
                                  int32_t* out_key, int32_t* out_value)
{
    set_iterator_t it = cset->upper_bound(self, __ds_s32key(key));

    if (it_eq(it, cset->end(self)))
        return false;
    *out_key = __ds_token_to_s32(it_data(it));
    *out_value = *out_key;
    return true;
}

static bool set_int32_foreach(const void* self, bool reverse,
                              test_assoc_visit_fn fn, void* ctx)
{
    if (!reverse) {
        set_iterator_t it = cset->begin(self);
        set_iterator_t end = cset->end(self);

        while (it_ne(it, end)) {
            int32_t key = __ds_token_to_s32(it_data(it));

            if (!fn(ctx, key, key))
                return false;
            it = cset->next(it);
        }
    } else {
        set_r_iterator_t it = cset->rbegin(self);
        set_r_iterator_t end = cset->rend(self);

        while (it_ne(it, end)) {
            int32_t key = __ds_token_to_s32(it_data(it));

            if (!fn(ctx, key, key))
                return false;
            it = cset->rnext(it);
        }
    }
    return true;
}

static size_t set_int32_clear(void* self)
{
    set_size_t count = cset->clear(self);

    return count < 0 ? 0 : (size_t)count;
}

static const test_assoc_desc_t set_int32_desc = {
    "assoc.set.int32",
    TEST_ASSOC_ORDERED | TEST_ASSOC_BOUNDS,
    set_int32_create,
    set_int32_destroy,
    set_int32_size,
    set_int32_count,
    set_int32_insert,
    NULL,
    set_int32_erase_key,
    set_int32_remove,
    set_int32_find,
    set_int32_lower_bound,
    set_int32_upper_bound,
    set_int32_foreach,
    set_int32_clear,
    NULL
};

const test_assoc_desc_t* set_desc_int32(void)
{
    return &set_int32_desc;
}
