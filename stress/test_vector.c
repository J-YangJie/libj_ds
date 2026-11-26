/*
  Sorting Algorithms Implementations
  Copyright (C) 2021  YangJie <yangjie98765@yeah.net>
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

#include "test_vector.h"

#include <vector/vector.h>
#include "demo_ops.h"

#define cds cvector

#define STRESS_VECTOR_SORT_LESS(_l, _r)     __demo_cmp_lt((ds_data_t)(_l), (ds_data_t)(_r))
#define STRESS_VECTOR_SORT_GREATER(_l, _r)  __demo_cmp_gt((ds_data_t)(_l), (ds_data_t)(_r))
vector_sort_define(stress_vector_sort_less, ds_data_t, STRESS_VECTOR_SORT_LESS)
vector_sort_define(stress_vector_sort_greater, ds_data_t, STRESS_VECTOR_SORT_GREATER)
#undef STRESS_VECTOR_SORT_LESS
#undef STRESS_VECTOR_SORT_GREATER

static stress_iterator_t stress_it(vector_iterator_t it)
{
    stress_iterator_t r = { .d = it.d, .ptr1 = NULL, .ptr2 = NULL, .ptr3 = NULL, .ptr4 = NULL, .step = it.step };
    return r;
}

static vector_iterator_t vector_it(stress_iterator_t it)
{
    vector_iterator_t r;
    r.d = (vector_data_t*)it.d;
    r.step = it.step;
    return r;
}

static stress_iterator_t stress_rit(vector_r_iterator_t it)
{
    stress_iterator_t r = { .d = it.d, .ptr1 = NULL, .ptr2 = NULL, .ptr3 = NULL, .ptr4 = NULL, .step = it.step };
    return r;
}

static vector_r_iterator_t vector_rit(stress_iterator_t it)
{
    vector_r_iterator_t r;
    r.d = (vector_data_t*)it.d;
    r.step = it.step;
    return r;
}

static ds_size_t size(void* _this)
{
    vector_t* this = (vector_t*)_this;
    return cds->size(this);
}

static ds_size_t max_size(void* _this)
{
    vector_t* this = (vector_t*)_this;
    return cds->max_size(this);
}

static bool empty(void* _this)
{
    vector_t* this = (vector_t*)_this;
    return cds->__empty(this);
}

static ds_count_t count(void* _this, ds_data_t data)
{
    vector_t* this = (vector_t*)_this;
    return cds->count(this, data);
}

static stress_iterator_t end(void* _this)
{
    vector_t* this = (vector_t*)_this;
    return stress_it(cds->end(this));
}

static stress_iterator_t begin(void* _this)
{
    vector_t* this = (vector_t*)_this;
    return stress_it(cds->begin(this));
}

static stress_iterator_t next(void* _this, stress_iterator_t iterator)
{
    (void)_this;
    return stress_it(cds->next(vector_it(iterator)));
}

static stress_iterator_t prev(void* _this, stress_iterator_t iterator)
{
    (void)_this;
    return stress_it(cds->prev(vector_it(iterator)));
}

static stress_iterator_t rend(void* _this)
{
    vector_t* this = (vector_t*)_this;
    return stress_rit(cds->rend(this));
}

static stress_iterator_t rbegin(void* _this)
{
    vector_t* this = (vector_t*)_this;
    return stress_rit(cds->rbegin(this));
}

static stress_iterator_t rnext(void* _this, stress_iterator_t r_iterator)
{
    (void)_this;
    return stress_rit(cds->rnext(vector_rit(r_iterator)));
}

static stress_iterator_t rprev(void* _this, stress_iterator_t r_iterator)
{
    (void)_this;
    return stress_rit(cds->rprev(vector_rit(r_iterator)));
}

static stress_iterator_t it(void* _this, ds_index_t index)
{
    vector_t* this = (vector_t*)_this;
    return stress_it(cds->it(this, index));
}

static ds_data_t at(void* _this, ds_index_t index, ds_data_t default_data)
{
    vector_t* this = (vector_t*)_this;
    vector_value_t value = cds->at(this, index);
    return value.d ? *(ds_data_t*)value.d : default_data;
}

static ds_data_t front(void* _this, ds_data_t default_data)
{
    vector_t* this = (vector_t*)_this;
    vector_value_t value = cds->front(this);
    return value.d ? *(ds_data_t*)value.d : default_data;
}

static ds_data_t back(void* _this, ds_data_t default_data)
{
    vector_t* this = (vector_t*)_this;
    vector_value_t value = cds->back(this);
    return value.d ? *(ds_data_t*)value.d : default_data;
}

static bool replace_it(void* _this, stress_iterator_t iterator, ds_data_t data)
{
    vector_t* this = (vector_t*)_this;
    return cds->replace_it(this, vector_it(iterator), data);
}

static bool replace_index(void* _this, ds_index_t index, ds_data_t data)
{
    vector_t* this = (vector_t*)_this;
    return cds->replace_index(this, index, data);
}

static stress_iterator_t find(void* _this, ds_data_t data)
{
    vector_t* this = (vector_t*)_this;
    return stress_it(cds->find(this, data));
}

static stress_iterator_t push_back(void* _this, ds_data_t data)
{
    vector_t* this = (vector_t*)_this;
    if (!cds->push_back(this, data))
        return (stress_iterator_t){ 0 };
    return stress_it(cds->prev(cds->end(this)));
}

static stress_iterator_t insert(void* _this, stress_iterator_t iterator, ds_data_t data)
{
    vector_t* this = (vector_t*)_this;
    return stress_it(cds->insert(this, vector_it(iterator), data));
}

static stress_iterator_t insert_n(void* _this, stress_iterator_t iterator, ds_size_t n, ds_data_t data)
{
    vector_t* this = (vector_t*)_this;
    return stress_it(cds->insert_n(this, vector_it(iterator), n, data));
}

static stress_iterator_t insert_range(void* _this, stress_iterator_t iterator_pos, stress_iterator_t iterator_begin,
                                      stress_iterator_t iterator_end)
{
    vector_t* this = (vector_t*)_this;
    return stress_it(cds->insert_range(this, vector_it(iterator_pos), vector_it(iterator_begin), vector_it(iterator_end)));
}

static stress_iterator_t erase(void* _this, stress_iterator_t iterator)
{
    vector_t* this = (vector_t*)_this;
    return stress_it(cds->erase(this, vector_it(iterator)));
}

static stress_iterator_t erase_range(void* _this, stress_iterator_t iterator_begin, stress_iterator_t iterator_end)
{
    vector_t* this = (vector_t*)_this;
    return stress_it(cds->erase_range(this, vector_it(iterator_begin), vector_it(iterator_end)));
}

static void pop_back(void* _this)
{
    vector_t* this = (vector_t*)_this;
    cds->pop_back(this);
}

static ds_size_t remove(void* _this, ds_data_t data)
{
    vector_t* this = (vector_t*)_this;
    return cds->remove(this, data);
}

static ds_size_t remove_if(void* _this, remove_if_condition cond)
{
    vector_t* this = (vector_t*)_this;
    return cds->remove_if(this, cond);
}

static bool assign(void* _this, ds_size_t n, ds_data_t data)
{
    vector_t* this = (vector_t*)_this;
    return cds->assign(this, n, data);
}

static bool assign_range(void* _this, stress_iterator_t iterator_begin, stress_iterator_t iterator_end)
{
    vector_t* this = (vector_t*)_this;
    return cds->assign_range(this, vector_it(iterator_begin), vector_it(iterator_end));
}

static bool resize(void* _this, ds_size_t n, ds_data_t default_data)
{
    vector_t* this = (vector_t*)_this;
    return cds->resize(this, n, default_data);
}

static void sort(void* _this, bool ascending)
{
    vector_t* this = (vector_t*)_this;
    if (ascending)
        stress_vector_sort_less(cds->begin(this), cds->end(this));
    else
        stress_vector_sort_greater(cds->begin(this), cds->end(this));
}

static void swap(void* _this, void* other)
{
    vector_t* this = (vector_t*)_this;
    cds->swap(this, (vector_t*)other);
}

static ds_size_t capacity(void* _this)
{
    vector_t* this = (vector_t*)_this;
    return cds->capacity(this);
}

static bool reserve(void* _this, ds_size_t n)
{
    vector_t* this = (vector_t*)_this;
    return cds->reserve(this, n);
}

static void shrink_to_fit(void* _this)
{
    vector_t* this = (vector_t*)_this;
    cds->shrink_to_fit(this);
}

static ds_size_t clear(void* _this)
{
    vector_t* this = (vector_t*)_this;
    return cds->clear(this);
}

const class_stress_slow_interface_t* class_stress_slow_vector_ins(void)
{
    static class_stress_slow_interface_t ins = {
        .size           = size,
        .max_size       = max_size,
        .empty          = empty,
        .count          = count,
        .end            = end,
        .begin          = begin,
        .next           = next,
        .prev           = prev,
        .rend           = rend,
        .rbegin         = rbegin,
        .rnext          = rnext,
        .rprev          = rprev,
        .it             = it,
        .at             = at,
        .front          = front,
        .back           = back,
        .replace_it     = replace_it,
        .replace_index  = replace_index,
        .find           = find,
        .push_back      = push_back,
        .insert         = insert,
        .insert_n       = insert_n,
        .insert_range   = insert_range,
        .erase          = erase,
        .erase_range    = erase_range,
        .pop_back       = pop_back,
        .remove         = remove,
        .remove_if      = remove_if,
        .assign         = assign,
        .assign_range   = assign_range,
        .resize         = resize,
        .sort           = sort,
        .swap           = swap,
        .capacity       = capacity,
        .reserve        = reserve,
        .shrink_to_fit  = shrink_to_fit,
        .clear          = clear,
    };
    return &ins;
}
