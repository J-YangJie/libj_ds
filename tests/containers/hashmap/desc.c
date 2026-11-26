/*
  libj_ds permanent tests hashmap descriptor
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

#include <hashmap/hashmap.h>

#include "desc.h"

static void* hashmap_int32_create(void)
{
    return HASHMAP_NEW_KV(int32_t, int32_t);
}

static void hashmap_int32_destroy(void* self)
{
    hashmap_t* hashmap = self;

    HASHMAP_DELETE(&hashmap);
}

static size_t hashmap_int32_size(const void* self)
{
    hashmap_size_t size = chashmap->size(self);

    return size < 0 ? 0 : (size_t)size;
}

static size_t hashmap_int32_count(const void* self, int32_t key)
{
    hashmap_count_t count = chashmap->count(self, __ds_s32key(key));

    return count < 0 ? 0 : (size_t)count;
}

static bool hashmap_int32_insert(void* self, int32_t key, int32_t value,
                                 bool* out_inserted)
{
    hashmap_iterator_t it = chashmap->insert(self, __ds_s32key(key),
                                             __ds_s32value(value));

    *out_inserted = it.d != NULL;
    return true;
}

static bool hashmap_int32_insert_replace(void* self, int32_t key,
                                         int32_t value)
{
    hashmap_iterator_t it = chashmap->insert_replace(self, __ds_s32key(key),
                                                     __ds_s32value(value));

    return it.d != NULL;
}

static bool hashmap_int32_erase_key(void* self, int32_t key,
                                    int32_t* out_next_key,
                                    bool* out_next_found)
{
    hashmap_iterator_t end = chashmap->end(self);
    hashmap_iterator_t it = chashmap->find(self, __ds_s32key(key));

    if (it_eq(it, end)) {
        *out_next_found = false;
        return true;
    }

    it = chashmap->erase(self, it);
    if (it_eq(it, end)) {
        *out_next_found = false;
    } else {
        *out_next_key = __ds_token_to_s32(it_key(it));
        *out_next_found = true;
    }
    return true;
}

static size_t hashmap_int32_remove(void* self, int32_t key)
{
    hashmap_size_t count = chashmap->remove(self, __ds_s32key(key));

    return count < 0 ? 0 : (size_t)count;
}

static bool hashmap_int32_find(const void* self, int32_t key,
                               int32_t* out_value)
{
    hashmap_iterator_t it = chashmap->find(self, __ds_s32key(key));

    if (it_eq(it, chashmap->end(self)))
        return false;
    *out_value = __ds_token_to_s32(it_value(it));
    return true;
}

static bool hashmap_int32_foreach(const void* self, bool reverse,
                                  test_assoc_visit_fn fn, void* ctx)
{
    if (!reverse) {
        hashmap_iterator_t it = chashmap->begin(self);
        hashmap_iterator_t end = chashmap->end(self);

        while (it_ne(it, end)) {
            if (!fn(ctx, __ds_token_to_s32(it_key(it)),
                    __ds_token_to_s32(it_value(it))))
                return false;
            it = chashmap->next(it);
        }
    } else {
        hashmap_r_iterator_t it = chashmap->rbegin(self);
        hashmap_r_iterator_t end = chashmap->rend(self);

        while (it_ne(it, end)) {
            if (!fn(ctx, __ds_token_to_s32(it_key(it)),
                    __ds_token_to_s32(it_value(it))))
                return false;
            it = chashmap->rnext(it);
        }
    }
    return true;
}

static size_t hashmap_int32_clear(void* self)
{
    hashmap_size_t count = chashmap->clear(self);

    return count < 0 ? 0 : (size_t)count;
}

static const test_assoc_desc_t hashmap_int32_desc = {
    "assoc.hashmap.int32",
    TEST_ASSOC_HASHED | TEST_ASSOC_HAS_VALUE | TEST_ASSOC_INSERT_REPLACE,
    hashmap_int32_create,
    hashmap_int32_destroy,
    hashmap_int32_size,
    hashmap_int32_count,
    hashmap_int32_insert,
    hashmap_int32_insert_replace,
    hashmap_int32_erase_key,
    hashmap_int32_remove,
    hashmap_int32_find,
    NULL,
    NULL,
    hashmap_int32_foreach,
    hashmap_int32_clear,
    NULL
};

const test_assoc_desc_t* hashmap_desc_int32(void)
{
    return &hashmap_int32_desc;
}
