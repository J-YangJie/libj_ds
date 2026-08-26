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

#include "test_multimap.h"

#include <multimap/multimap.h>

#define cds cmultimap

static ds_size_t size(void* _this)
{
    multimap_t* this = (multimap_t*)_this;
    return cds->size(this);
}

static ds_count_t count(void* _this, ds_data_t data)
{
    multimap_t* this = (multimap_t*)_this;
    return cds->count(this, data);
}

static stress_iterator_t end(void* _this)
{
    multimap_t* this = (multimap_t*)_this;
    multimap_iterator_t it = cds->end(this);
    stress_iterator_t r = { .d = it.d, .ptr1 = NULL, .ptr2 = NULL, .ptr3 = NULL };
    return r;
}

static stress_iterator_t begin(void* _this)
{
    multimap_t* this = (multimap_t*)_this;
    multimap_iterator_t it = cds->begin(this);
    stress_iterator_t r = { .d = it.d, .ptr1 = NULL, .ptr2 = NULL, .ptr3 = NULL };
    return r;
}

static stress_iterator_t next(void* _this, stress_iterator_t iterator)
{
    multimap_t* this = (multimap_t*)_this;
    multimap_iterator_t it;
    it.d = (multimap_iterator_kv_t*)iterator.d;
    it = cds->next(this, it);
    stress_iterator_t r = { .d = it.d, .ptr1 = NULL, .ptr2 = NULL, .ptr3 = NULL };
    return r;
}

static stress_iterator_t prev(void* _this, stress_iterator_t iterator)
{
    multimap_t* this = (multimap_t*)_this;
    multimap_iterator_t it;
    it.d = (multimap_iterator_kv_t*)iterator.d;
    it = cds->prev(this, it);
    stress_iterator_t r = { .d = it.d, .ptr1 = NULL, .ptr2 = NULL, .ptr3 = NULL };
    return r;
}

static stress_iterator_t rend(void* _this)
{
    multimap_t* this = (multimap_t*)_this;
    multimap_r_iterator_t it = cds->rend(this);
    stress_iterator_t r = { .d = it.d, .ptr1 = NULL, .ptr2 = NULL, .ptr3 = NULL };
    return r;
}

static stress_iterator_t rbegin(void* _this)
{
    multimap_t* this = (multimap_t*)_this;
    multimap_r_iterator_t it = cds->rbegin(this);
    stress_iterator_t r = { .d = it.d, .ptr1 = NULL, .ptr2 = NULL, .ptr3 = NULL };
    return r;
}

static stress_iterator_t rnext(void* _this, stress_iterator_t r_iterator)
{
    multimap_t* this = (multimap_t*)_this;
    multimap_r_iterator_t it;
    it.d = (multimap_r_iterator_kv_t*)r_iterator.d;
    it = cds->rnext(this, it);
    stress_iterator_t r = { .d = it.d, .ptr1 = NULL, .ptr2 = NULL, .ptr3 = NULL };
    return r;
}

static stress_iterator_t rprev(void* _this, stress_iterator_t r_iterator)
{
    multimap_t* this = (multimap_t*)_this;
    multimap_r_iterator_t it;
    it.d = (multimap_r_iterator_kv_t*)r_iterator.d;
    it = cds->rprev(this, it);
    stress_iterator_t r = { .d = it.d, .ptr1 = NULL, .ptr2 = NULL, .ptr3 = NULL };
    return r;
}

static stress_iterator_t insert(void* _this, ds_data_t data, ds_data_t data2)
{
    multimap_t* this = (multimap_t*)_this;
    multimap_iterator_t it = cds->insert(this, data, data2);
    stress_iterator_t r = { .d = it.d, .ptr1 = NULL, .ptr2 = NULL, .ptr3 = NULL };
    return r;
}

static stress_iterator_t find(void* _this, ds_data_t data)
{
    multimap_t* this = (multimap_t*)_this;
    multimap_iterator_t it = cds->find(this, data);
    stress_iterator_t r = { .d = it.d, .ptr1 = NULL, .ptr2 = NULL, .ptr3 = NULL };
    return r;
}

static stress_iterator_t lower_bound(void* _this, ds_data_t data)
{
    multimap_t* this = (multimap_t*)_this;
    multimap_iterator_t it = cds->lower_bound(this, data);
    stress_iterator_t r = { .d = it.d, .ptr1 = NULL, .ptr2 = NULL, .ptr3 = NULL };
    return r;
}

static stress_iterator_t upper_bound(void* _this, ds_data_t data)
{
    multimap_t* this = (multimap_t*)_this;
    multimap_iterator_t it = cds->upper_bound(this, data);
    stress_iterator_t r = { .d = it.d, .ptr1 = NULL, .ptr2 = NULL, .ptr3 = NULL };
    return r;
}

static stress_iterator_t erase(void* _this, stress_iterator_t iterator)
{
    multimap_t* this = (multimap_t*)_this;
    multimap_iterator_t it;
    it.d = (multimap_iterator_kv_t*)iterator.d;
    it = cds->erase(this, it);
    stress_iterator_t r = { .d = it.d, .ptr1 = NULL, .ptr2 = NULL, .ptr3 = NULL };
    return r;
}

static ds_size_t remove(void* _this, ds_data_t data)
{
    multimap_t* this = (multimap_t*)_this;
    return cds->remove(this, data);
}

static ds_size_t clear(void* _this)
{
    multimap_t* this = (multimap_t*)_this;
    return cds->clear(this);
}

const class_stress_interface_t* class_stress_multimap_ins(void)
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
        .find           = find,
        .lower_bound    = lower_bound,
        .upper_bound    = upper_bound,
        .erase          = erase,
        .remove         = remove,
        .clear          = clear,
    };
    return &ins;
}

