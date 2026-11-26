/*
  libj_ds permanent tests multimap descriptor
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

#include <multimap/multimap.h>

#include "desc.h"

static void* multimap_int32_create(void)
{
    return MULTIMAP_NEW_KV(int32_t, int32_t);
}

static void multimap_int32_destroy(void* self)
{
    multimap_t* multimap = self;

    MULTIMAP_DELETE(&multimap);
}

static size_t multimap_int32_size(const void* self)
{
    multimap_size_t size = cmultimap->size(self);

    return size < 0 ? 0 : (size_t)size;
}

static size_t multimap_int32_count(const void* self, int32_t key)
{
    multimap_count_t count = cmultimap->count(self, __ds_s32key(key));

    return count < 0 ? 0 : (size_t)count;
}

static bool multimap_int32_insert(void* self, int32_t key, int32_t value,
                                  bool* out_inserted)
{
    multimap_iterator_t it = cmultimap->insert(self, __ds_s32key(key),
                                               __ds_s32value(value));

    *out_inserted = it.d != NULL;
    return true;
}

static bool multimap_int32_erase_key(void* self, int32_t key,
                                     int32_t* out_next_key,
                                     bool* out_next_found)
{
    multimap_iterator_t end = cmultimap->end(self);
    multimap_iterator_t it = cmultimap->find(self, __ds_s32key(key));

    if (it_eq(it, end)) {
        *out_next_found = false;
        return true;
    }

    it = cmultimap->erase(self, it);
    if (it_eq(it, end)) {
        *out_next_found = false;
    } else {
        *out_next_key = __ds_token_to_s32(it_key(it));
        *out_next_found = true;
    }
    return true;
}

static size_t multimap_int32_remove(void* self, int32_t key)
{
    multimap_size_t count = cmultimap->remove(self, __ds_s32key(key));

    return count < 0 ? 0 : (size_t)count;
}

static bool multimap_int32_find(const void* self, int32_t key,
                                int32_t* out_value)
{
    multimap_iterator_t it = cmultimap->find(self, __ds_s32key(key));

    if (it_eq(it, cmultimap->end(self)))
        return false;
    *out_value = __ds_token_to_s32(it_value(it));
    return true;
}

static bool multimap_int32_lower_bound(const void* self, int32_t key,
                                       int32_t* out_key, int32_t* out_value)
{
    multimap_iterator_t it = cmultimap->lower_bound(self, __ds_s32key(key));

    if (it_eq(it, cmultimap->end(self)))
        return false;
    *out_key = __ds_token_to_s32(it_key(it));
    *out_value = __ds_token_to_s32(it_value(it));
    return true;
}

static bool multimap_int32_upper_bound(const void* self, int32_t key,
                                       int32_t* out_key, int32_t* out_value)
{
    multimap_iterator_t it = cmultimap->upper_bound(self, __ds_s32key(key));

    if (it_eq(it, cmultimap->end(self)))
        return false;
    *out_key = __ds_token_to_s32(it_key(it));
    *out_value = __ds_token_to_s32(it_value(it));
    return true;
}

static bool multimap_int32_foreach(const void* self, bool reverse,
                                   test_assoc_visit_fn fn, void* ctx)
{
    if (!reverse) {
        multimap_iterator_t it = cmultimap->begin(self);
        multimap_iterator_t end = cmultimap->end(self);

        while (it_ne(it, end)) {
            if (!fn(ctx, __ds_token_to_s32(it_key(it)),
                    __ds_token_to_s32(it_value(it))))
                return false;
            it = cmultimap->next(it);
        }
    } else {
        multimap_r_iterator_t it = cmultimap->rbegin(self);
        multimap_r_iterator_t end = cmultimap->rend(self);

        while (it_ne(it, end)) {
            if (!fn(ctx, __ds_token_to_s32(it_key(it)),
                    __ds_token_to_s32(it_value(it))))
                return false;
            it = cmultimap->rnext(it);
        }
    }
    return true;
}

static size_t multimap_int32_clear(void* self)
{
    multimap_size_t count = cmultimap->clear(self);

    return count < 0 ? 0 : (size_t)count;
}

static const test_assoc_desc_t multimap_int32_desc = {
    "assoc.multimap.int32",
    TEST_ASSOC_ORDERED | TEST_ASSOC_MULTI | TEST_ASSOC_HAS_VALUE
        | TEST_ASSOC_BOUNDS,
    multimap_int32_create,
    multimap_int32_destroy,
    multimap_int32_size,
    multimap_int32_count,
    multimap_int32_insert,
    NULL,
    multimap_int32_erase_key,
    multimap_int32_remove,
    multimap_int32_find,
    multimap_int32_lower_bound,
    multimap_int32_upper_bound,
    multimap_int32_foreach,
    multimap_int32_clear,
    NULL
};

const test_assoc_desc_t* multimap_desc_int32(void)
{
    return &multimap_int32_desc;
}
