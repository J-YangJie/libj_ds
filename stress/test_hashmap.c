/*
  Sorting Algorithms Implementations
  Copyright (C) 2021  YangJie <yangjie98765@yeah.net>

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

#include "test_hashmap.h"

#include <hashmap/hashmap.h>

#define cds chashmap

static ds_size_t size(void* _this)
{
    hashmap_t* this = (hashmap_t*)_this;
    return cds->size(this);
}

static ds_count_t count(void* _this, ds_data_t data)
{
    hashmap_t* this = (hashmap_t*)_this;
    return cds->count(this, data);
}

static void* end(void* _this)
{
    hashmap_t* this = (hashmap_t*)_this;
    return cds->end(this);
}

static void* begin(void* _this)
{
    hashmap_t* this = (hashmap_t*)_this;
    return cds->begin(this);
}

static void* next(void* _this, void* iterator)
{
    hashmap_t* this = (hashmap_t*)_this;
    return cds->next(this, iterator);
}

static void* prev(void* _this, void* iterator)
{
    hashmap_t* this = (hashmap_t*)_this;
    return cds->prev(this, iterator);
}

static void* rend(void* _this)
{
    hashmap_t* this = (hashmap_t*)_this;
    return cds->rend(this);
}

static void* rbegin(void* _this)
{
    hashmap_t* this = (hashmap_t*)_this;
    return cds->rbegin(this);
}

static void* rnext(void* _this, void* r_iterator)
{
    hashmap_t* this = (hashmap_t*)_this;
    return cds->rnext(this, r_iterator);
}

static void* rprev(void* _this, void* r_iterator)
{
    hashmap_t* this = (hashmap_t*)_this;
    return cds->rprev(this, r_iterator);
}

static void* insert(void* _this, ds_data_t data, ds_data_t data2)
{
    hashmap_t* this = (hashmap_t*)_this;
    return cds->insert(this, data, data2);
}

static void* insert_replace(void* _this, ds_data_t data, ds_data_t data2)
{
    hashmap_t* this = (hashmap_t*)_this;
    return cds->insert_replace(this, data, data2);
}

static void* find(void* _this, ds_data_t data)
{
    hashmap_t* this = (hashmap_t*)_this;
    return cds->find(this, data);
}

static void* erase(void* _this, void* iterator)
{
    hashmap_t* this = (hashmap_t*)_this;
    return cds->erase(this, iterator);
}

static ds_size_t remove(void* _this, ds_data_t data)
{
    hashmap_t* this = (hashmap_t*)_this;
    return cds->remove(this, data);
}

static ds_size_t clear(void* _this)
{
    hashmap_t* this = (hashmap_t*)_this;
    return cds->clear(this);
}

const class_stress_interface_t* class_stress_hashmap_ins(void)
{
    static class_stress_interface_t ins = {
        .size           = size,
        .count          = count,
        .end            = end,
        .begin          = begin,
        .next           = next,
        .prev           = prev,
        .rend           = rend,
        .rbegin         = rbegin,
        .rnext          = rnext,
        .rprev          = rprev,
        .insert         = insert,
        .insert_replace = insert_replace,
        .find           = find,
        .erase          = erase,
        .remove         = remove,
        .clear          = clear,
    };
    return &ins;
}

