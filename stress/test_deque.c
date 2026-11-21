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

#include "test_deque.h"

#include <deque/deque.h>
#include "demo_ops.h"

#define cds cdeque

#define STRESS_DEQUE_SORT_LESS(_l, _r)      __demo_cmp_lt((ds_data_t)(_l), (ds_data_t)(_r))
#define STRESS_DEQUE_SORT_GREATER(_l, _r)   __demo_cmp_gt((ds_data_t)(_l), (ds_data_t)(_r))
deque_sort_define(stress_deque_sort_less, ds_data_t, STRESS_DEQUE_SORT_LESS)
deque_sort_define(stress_deque_sort_greater, ds_data_t, STRESS_DEQUE_SORT_GREATER)
#undef STRESS_DEQUE_SORT_LESS
#undef STRESS_DEQUE_SORT_GREATER

static stress_iterator_t stress_it(deque_iterator_t it)
{
    stress_iterator_t r = { .d = it.d, .ptr1 = it.begin, .ptr2 = it.end, .ptr3 = it.bkt,
                            .ptr4 = NULL, .step = it.step };
    return r;
}

static deque_iterator_t deque_it(stress_iterator_t it)
{
    deque_iterator_t r;
    r.d = (deque_data_t*)it.d;
    r.begin = (uint8_t*)it.ptr1;
    r.end = (uint8_t*)it.ptr2;
    r.bkt = (uint8_t**)it.ptr3;
    r.step = it.step;
    return r;
}

static stress_iterator_t stress_rit(deque_r_iterator_t it)
{
    stress_iterator_t r = { .d = it.d, .ptr1 = it.begin, .ptr2 = it.end, .ptr3 = it.bkt,
                            .ptr4 = it.begin_cur, .step = it.step };
    return r;
}

static deque_r_iterator_t deque_rit(stress_iterator_t it)
{
    deque_r_iterator_t r;
    r.d = (deque_data_t*)it.d;
    r.begin = (uint8_t*)it.ptr1;
    r.end = (uint8_t*)it.ptr2;
    r.bkt = (uint8_t**)it.ptr3;
    r.begin_cur = (uint8_t*)it.ptr4;
    r.step = it.step;
    return r;
}

static ds_size_t size(void* _this)
{
    deque_t* this = (deque_t*)_this;
    return cds->size(this);
}

static ds_size_t max_size(void* _this)
{
    deque_t* this = (deque_t*)_this;
    return cds->max_size(this);
}

static bool empty(void* _this)
{
    deque_t* this = (deque_t*)_this;
    return cds->__empty(this);
}

static ds_count_t count(void* _this, ds_data_t data)
{
    deque_t* this = (deque_t*)_this;
    return cds->count(this, data);
}

static stress_iterator_t end(void* _this)
{
    deque_t* this = (deque_t*)_this;
    return stress_it(cds->end(this));
}

static stress_iterator_t begin(void* _this)
{
    deque_t* this = (deque_t*)_this;
    return stress_it(cds->begin(this));
}

static stress_iterator_t next(void* _this, stress_iterator_t iterator)
{
    (void)_this;
    return stress_it(cds->next(deque_it(iterator)));
}

static stress_iterator_t prev(void* _this, stress_iterator_t iterator)
{
    (void)_this;
    return stress_it(cds->prev(deque_it(iterator)));
}

static stress_iterator_t rend(void* _this)
{
    deque_t* this = (deque_t*)_this;
    return stress_rit(cds->rend(this));
}

static stress_iterator_t rbegin(void* _this)
{
    deque_t* this = (deque_t*)_this;
    return stress_rit(cds->rbegin(this));
}

static stress_iterator_t rnext(void* _this, stress_iterator_t r_iterator)
{
    (void)_this;
    return stress_rit(cds->rnext(deque_rit(r_iterator)));
}

static stress_iterator_t rprev(void* _this, stress_iterator_t r_iterator)
{
    (void)_this;
    return stress_rit(cds->rprev(deque_rit(r_iterator)));
}

static stress_iterator_t it(void* _this, ds_index_t index)
{
    deque_t* this = (deque_t*)_this;
    return stress_it(cds->it(this, index));
}

static ds_data_t at(void* _this, ds_index_t index, ds_data_t default_data)
{
    deque_t* this = (deque_t*)_this;
    deque_value_t value = cds->at(this, index);
    return value.d ? *(ds_data_t*)value.d : default_data;
}

static ds_data_t front(void* _this, ds_data_t default_data)
{
    deque_t* this = (deque_t*)_this;
    deque_value_t value = cds->front(this);
    return value.d ? *(ds_data_t*)value.d : default_data;
}

static ds_data_t back(void* _this, ds_data_t default_data)
{
    deque_t* this = (deque_t*)_this;
    deque_value_t value = cds->back(this);
    return value.d ? *(ds_data_t*)value.d : default_data;
}

static bool replace_it(void* _this, stress_iterator_t iterator, ds_data_t data)
{
    deque_t* this = (deque_t*)_this;
    return cds->replace_it(this, deque_it(iterator), data);
}

static bool replace_index(void* _this, ds_index_t index, ds_data_t data)
{
    deque_t* this = (deque_t*)_this;
    return cds->replace_index(this, index, data);
}

static stress_iterator_t find(void* _this, ds_data_t data)
{
    deque_t* this = (deque_t*)_this;
    return stress_it(cds->find(this, data));
}

static stress_iterator_t push_back(void* _this, ds_data_t data)
{
    deque_t* this = (deque_t*)_this;
    if (!cds->push_back(this, data))
        return (stress_iterator_t){ 0 };
    return stress_it(cds->prev(cds->end(this)));
}

static stress_iterator_t push_front(void* _this, ds_data_t data)
{
    deque_t* this = (deque_t*)_this;
    if (!cds->push_front(this, data))
        return (stress_iterator_t){ 0 };
    return stress_it(cds->begin(this));
}

static stress_iterator_t insert(void* _this, stress_iterator_t iterator, ds_data_t data)
{
    deque_t* this = (deque_t*)_this;
    return stress_it(cds->insert(this, deque_it(iterator), data));
}

static stress_iterator_t insert_n(void* _this, stress_iterator_t iterator, ds_size_t n, ds_data_t data)
{
    deque_t* this = (deque_t*)_this;
    return stress_it(cds->insert_n(this, deque_it(iterator), n, data));
}

static stress_iterator_t insert_range(void* _this, stress_iterator_t iterator_pos, stress_iterator_t iterator_begin,
                                      stress_iterator_t iterator_end)
{
    deque_t* this = (deque_t*)_this;
    return stress_it(cds->insert_range(this, deque_it(iterator_pos), deque_it(iterator_begin), deque_it(iterator_end)));
}

static stress_iterator_t erase(void* _this, stress_iterator_t iterator)
{
    deque_t* this = (deque_t*)_this;
    return stress_it(cds->erase(this, deque_it(iterator)));
}

static stress_iterator_t erase_range(void* _this, stress_iterator_t iterator_begin, stress_iterator_t iterator_end)
{
    deque_t* this = (deque_t*)_this;
    return stress_it(cds->erase_range(this, deque_it(iterator_begin), deque_it(iterator_end)));
}

static void pop_back(void* _this)
{
    deque_t* this = (deque_t*)_this;
    cds->pop_back(this);
}

static void pop_front(void* _this)
{
    deque_t* this = (deque_t*)_this;
    cds->pop_front(this);
}

static ds_size_t remove(void* _this, ds_data_t data)
{
    deque_t* this = (deque_t*)_this;
    return cds->remove(this, data);
}

static ds_size_t remove_if(void* _this, remove_if_condition cond)
{
    deque_t* this = (deque_t*)_this;
    return cds->remove_if(this, cond);
}

static bool assign(void* _this, ds_size_t n, ds_data_t data)
{
    deque_t* this = (deque_t*)_this;
    return cds->assign(this, n, data);
}

static bool assign_range(void* _this, stress_iterator_t iterator_begin, stress_iterator_t iterator_end)
{
    deque_t* this = (deque_t*)_this;
    return cds->assign_range(this, deque_it(iterator_begin), deque_it(iterator_end));
}

static bool resize(void* _this, ds_size_t n, ds_data_t default_data)
{
    deque_t* this = (deque_t*)_this;
    return cds->resize(this, n, default_data);
}

static void sort(void* _this, bool ascending)
{
    deque_t* this = (deque_t*)_this;
    if (ascending)
        stress_deque_sort_less(cds->begin(this), cds->end(this));
    else
        stress_deque_sort_greater(cds->begin(this), cds->end(this));
}

static void swap(void* _this, void* other)
{
    deque_t* this = (deque_t*)_this;
    cds->swap(this, (deque_t*)other);
}

static void shrink_to_fit(void* _this)
{
    deque_t* this = (deque_t*)_this;
    cds->shrink_to_fit(this);
}

static ds_size_t clear(void* _this)
{
    deque_t* this = (deque_t*)_this;
    return cds->clear(this);
}

const class_stress_slow_interface_t* class_stress_slow_deque_ins(void)
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
        .push_front     = push_front,
        .insert         = insert,
        .insert_n       = insert_n,
        .insert_range   = insert_range,
        .erase          = erase,
        .erase_range    = erase_range,
        .pop_back       = pop_back,
        .pop_front      = pop_front,
        .remove         = remove,
        .remove_if      = remove_if,
        .assign         = assign,
        .assign_range   = assign_range,
        .resize         = resize,
        .sort           = sort,
        .swap           = swap,
        .shrink_to_fit  = shrink_to_fit,
        .clear          = clear,
    };
    return &ins;
}
