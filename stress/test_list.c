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

#include "test_list.h"

#include <list/list.h>

#define cds clist

static ds_size_t size(void* _this)
{
    list_t* this = (list_t*)_this;
    return cds->size(this);
}

static ds_count_t count(void* _this, ds_data_t data)
{
    list_t* this = (list_t*)_this;
    return cds->count(this, data);
}

static void* end(void* _this)
{
    list_t* this = (list_t*)_this;
    return cds->end(this);
}

static void* begin(void* _this)
{
    list_t* this = (list_t*)_this;
    return cds->begin(this);
}

static void* next(void* _this, void* iterator)
{
    list_t* this = (list_t*)_this;
    return cds->next(this, iterator);
}

static void* prev(void* _this, void* iterator)
{
    list_t* this = (list_t*)_this;
    return cds->prev(this, iterator);
}

static void* rend(void* _this)
{
    list_t* this = (list_t*)_this;
    return cds->rend(this);
}

static void* rbegin(void* _this)
{
    list_t* this = (list_t*)_this;
    return cds->rbegin(this);
}

static void* rnext(void* _this, void* r_iterator)
{
    list_t* this = (list_t*)_this;
    return cds->rnext(this, r_iterator);
}

static void* rprev(void* _this, void* r_iterator)
{
    list_t* this = (list_t*)_this;
    return cds->rprev(this, r_iterator);
}

static ds_data_t first(void* _this, ds_data_t default_data)
{
    list_t* this = (list_t*)_this;
    return cds->first(this, default_data);
}

static ds_data_t last(void* _this, ds_data_t default_data)
{
    list_t* this = (list_t*)_this;
    return cds->last(this, default_data);
}

static void* find(void* _this, ds_data_t data)
{
    list_t* this = (list_t*)_this;
    return cds->find(this, data);
}

static void* push_back(void* _this, ds_data_t data)
{
    list_t* this = (list_t*)_this;
    return cds->push_back(this, data);
}

static void* push_front(void* _this, ds_data_t data)
{
    list_t* this = (list_t*)_this;
    return cds->push_front(this, data);
}

static void* insert(void* _this, void* iterator, ds_data_t data)
{
    list_t* this = (list_t*)_this;
    return cds->insert(this, iterator, data);
}

static void* erase(void* _this, void* iterator)
{
    list_t* this = (list_t*)_this;
    return cds->erase(this, iterator);
}

static void* erase_range(void* _this, void* iterator_begin, void* iterator_end)
{
    list_t* this = (list_t*)_this;
    return cds->erase_range(this, iterator_begin, iterator_end);
}

static void pop_back(void* _this)
{
    list_t* this = (list_t*)_this;
    return cds->pop_back(this);
}

static void pop_front(void* _this)
{
    list_t* this = (list_t*)_this;
    return cds->pop_front(this);
}

static ds_size_t remove(void* _this, ds_data_t data)
{
    list_t* this = (list_t*)_this;
    return cds->remove(this, data);
}

static ds_size_t remove_if(void* _this, remove_if_condition cond)
{
    list_t* this = (list_t*)_this;
    return cds->remove_if(this, cond);
}

static ds_size_t clear(void* _this)
{
    list_t* this = (list_t*)_this;
    return cds->clear(this);
}

const class_stress_slow_interface_t* class_stress_slow_list_ins(void)
{
    static class_stress_slow_interface_t ins = {
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
        .first          = first,
        .last           = last,
        .find           = find,
        .push_back      = push_back,
        .push_front     = push_front,
        .insert         = insert,
        .erase          = erase,
        .erase_range    = erase_range,
        .pop_back       = pop_back,
        .pop_front      = pop_front,
        .remove         = remove,
        .remove_if      = remove_if,
        .sort           = NULL,
        .clear          = clear,
    };
    return &ins;
}
