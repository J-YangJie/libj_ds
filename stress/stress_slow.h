/*
  Sorting Algorithms Implementations
  Copyright (c) 2021 YangJie.
  Copyright (C) 2026  YangJie <yangjie98765@yeah.net>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.
*/

#ifndef __J_STRESS_SLOW_H
#define __J_STRESS_SLOW_H

#include <_types.h>

typedef struct {
    void* d;        /* element pointer */
    void* ptr1;     /* iterator state 1 */
    void* ptr2;     /* iterator state 2 */
    void* ptr3;     /* iterator state 3 */
    void* ptr4;     /* iterator state 4 */
    ds_step_t step; /* element stride */
} stress_iterator_t;

static inline bool stress_it_null(stress_iterator_t it)   { return NULL == it.d; }
static inline bool stress_it_eq(stress_iterator_t a, stress_iterator_t b)  { return a.d == b.d; }
static inline bool stress_it_ne(stress_iterator_t a, stress_iterator_t b)  { return a.d != b.d; }
#define stress_it_data(it)   (*((ds_data_t*)((it).d)))
#define stress_it_sdata(it)  (*((char**)((it).d)))

typedef struct {
    ds_size_t (*size)(void* _this);
    ds_size_t (*max_size)(void* _this);
    bool (*empty)(void* _this);
    ds_count_t (*count)(void* _this, ds_data_t data);
    stress_iterator_t (*end)(void* _this);
    stress_iterator_t (*begin)(void* _this);
    stress_iterator_t (*next)(void* _this, stress_iterator_t iterator);
    stress_iterator_t (*prev)(void* _this, stress_iterator_t iterator);
    stress_iterator_t (*rend)(void* _this);
    stress_iterator_t (*rbegin)(void* _this);
    stress_iterator_t (*rnext)(void* _this, stress_iterator_t r_iterator);
    stress_iterator_t (*rprev)(void* _this, stress_iterator_t r_iterator);
    stress_iterator_t (*it)(void* _this, ds_index_t index);
    ds_data_t (*at)(void* _this, ds_index_t index, ds_data_t default_data);
    ds_data_t (*front)(void* _this, ds_data_t default_data);
    ds_data_t (*back)(void* _this, ds_data_t default_data);
    bool (*replace_it)(void* _this, stress_iterator_t iterator, ds_data_t data);
    bool (*replace_index)(void* _this, ds_index_t index, ds_data_t data);
    stress_iterator_t (*find)(void* _this, ds_data_t data);
    stress_iterator_t (*push_back)(void* _this, ds_data_t data);
    stress_iterator_t (*push_front)(void* _this, ds_data_t data);
    stress_iterator_t (*insert)(void* _this, stress_iterator_t iterator, ds_data_t data);                            /* insert data before `iterator` */
    stress_iterator_t (*insert_n)(void* _this, stress_iterator_t iterator, ds_size_t n, ds_data_t data);
    stress_iterator_t (*insert_range)(void* _this, stress_iterator_t iterator_pos, stress_iterator_t iterator_begin,
                                      stress_iterator_t iterator_end);                                            /* [iterator_begin, iterator_end) */
    stress_iterator_t (*erase)(void* _this, stress_iterator_t iterator);
    stress_iterator_t (*erase_range)(void* _this, stress_iterator_t iterator_begin, stress_iterator_t iterator_end); /* [iterator_begin, iterator_end) */
    void (*pop_back)(void* _this);
    void (*pop_front)(void* _this);
    ds_size_t (*remove)(void* _this, ds_data_t data);
    ds_size_t (*remove_if)(void* _this, remove_if_condition cond);
    bool (*assign)(void* _this, ds_size_t n, ds_data_t data);
    bool (*assign_range)(void* _this, stress_iterator_t iterator_begin, stress_iterator_t iterator_end);
    bool (*resize)(void* _this, ds_size_t n, ds_data_t default_data);
    void (*sort)(void* _this, bool ascending);
    void (*swap)(void* _this, void* other);
    ds_size_t (*capacity)(void* _this);
    bool (*reserve)(void* _this, ds_size_t n);
    void (*shrink_to_fit)(void* _this);
    void (*reverse)(void* _this);
    ds_size_t (*unique)(void* _this);
    ds_size_t (*unique_if)(void* _this, unique_if_condition cond);
    ds_size_t (*clear)(void* _this);
} class_stress_slow_interface_t;

#endif /* __J_STRESS_SLOW_H */
