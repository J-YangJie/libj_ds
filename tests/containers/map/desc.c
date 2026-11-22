/*
  libj_ds permanent tests map descriptor
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

#include <map/map.h>

#include "desc.h"

static void* map_int32_create(void)
{
    return MAP_NEW_KV(int32_t, int32_t);
}

static void map_int32_destroy(void* self)
{
    map_t* map = self;

    MAP_DELETE(&map);
}

static size_t map_int32_size(const void* self)
{
    map_size_t size = cmap->size(self);

    return size < 0 ? 0 : (size_t)size;
}

static size_t map_int32_count(const void* self, int32_t key)
{
    map_count_t count = cmap->count(self, __ds_s32key(key));

    return count < 0 ? 0 : (size_t)count;
}

static bool map_int32_insert(void* self, int32_t key, int32_t value,
                             bool* out_inserted)
{
    map_iterator_t it = cmap->insert(self, __ds_s32key(key),
                                     __ds_s32value(value));

    *out_inserted = it.d != NULL;
    return true;
}

static bool map_int32_insert_replace(void* self, int32_t key, int32_t value)
{
    map_iterator_t it = cmap->insert_replace(self, __ds_s32key(key),
                                             __ds_s32value(value));

    return it.d != NULL;
}

static bool map_int32_erase_key(void* self, int32_t key, int32_t* out_next_key,
                                bool* out_next_found)
{
    map_iterator_t end = cmap->end(self);
    map_iterator_t it = cmap->find(self, __ds_s32key(key));

    if (it_eq(it, end)) {
        *out_next_found = false;
        return true;
    }

    it = cmap->erase(self, it);
    if (it_eq(it, end)) {
        *out_next_found = false;
    } else {
        *out_next_key = __ds_token_to_s32(it_key(it));
        *out_next_found = true;
    }
    return true;
}

static size_t map_int32_remove(void* self, int32_t key)
{
    map_size_t count = cmap->remove(self, __ds_s32key(key));

    return count < 0 ? 0 : (size_t)count;
}

static bool map_int32_find(const void* self, int32_t key, int32_t* out_value)
{
    map_iterator_t it = cmap->find(self, __ds_s32key(key));

    if (it_eq(it, cmap->end(self)))
        return false;
    *out_value = __ds_token_to_s32(it_value(it));
    return true;
}

static bool map_int32_lower_bound(const void* self, int32_t key,
                                  int32_t* out_key, int32_t* out_value)
{
    map_iterator_t it = cmap->lower_bound(self, __ds_s32key(key));

    if (it_eq(it, cmap->end(self)))
        return false;
    *out_key = __ds_token_to_s32(it_key(it));
    *out_value = __ds_token_to_s32(it_value(it));
    return true;
}

static bool map_int32_upper_bound(const void* self, int32_t key,
                                  int32_t* out_key, int32_t* out_value)
{
    map_iterator_t it = cmap->upper_bound(self, __ds_s32key(key));

    if (it_eq(it, cmap->end(self)))
        return false;
    *out_key = __ds_token_to_s32(it_key(it));
    *out_value = __ds_token_to_s32(it_value(it));
    return true;
}

static bool map_int32_foreach(const void* self, bool reverse,
                              test_assoc_visit_fn fn, void* ctx)
{
    map_iterator_t it;
    map_iterator_t end;

    if (!reverse) {
        it = cmap->begin(self);
        end = cmap->end(self);
        while (it_ne(it, end)) {
            if (!fn(ctx, __ds_token_to_s32(it_key(it)),
                    __ds_token_to_s32(it_value(it))))
                return false;
            it = cmap->next(it);
        }
    } else {
        map_r_iterator_t rit = cmap->rbegin(self);
        map_r_iterator_t rend = cmap->rend(self);

        while (it_ne(rit, rend)) {
            if (!fn(ctx, __ds_token_to_s32(it_key(rit)),
                    __ds_token_to_s32(it_value(rit))))
                return false;
            rit = cmap->rnext(rit);
        }
    }
    return true;
}

static size_t map_int32_clear(void* self)
{
    map_size_t count = cmap->clear(self);

    return count < 0 ? 0 : (size_t)count;
}

static const test_assoc_desc_t map_int32_desc = {
    "assoc.map.int32",
    TEST_ASSOC_ORDERED | TEST_ASSOC_HAS_VALUE | TEST_ASSOC_INSERT_REPLACE
        | TEST_ASSOC_BOUNDS,
    map_int32_create,
    map_int32_destroy,
    map_int32_size,
    map_int32_count,
    map_int32_insert,
    map_int32_insert_replace,
    map_int32_erase_key,
    map_int32_remove,
    map_int32_find,
    map_int32_lower_bound,
    map_int32_upper_bound,
    map_int32_foreach,
    map_int32_clear,
    NULL
};

const test_assoc_desc_t* map_desc_int32(void)
{
    return &map_int32_desc;
}
