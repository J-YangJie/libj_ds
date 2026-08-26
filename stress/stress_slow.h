/*
  Sorting Algorithms Implementations
  Copyright (c) 2021 YangJie.

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
    void* d;      /* element pointer */
    void* ptr1;   /* reserved */
    void* ptr2;   /* reserved */
    void* ptr3;   /* reserved */
} stress_iterator_t;

static inline bool stress_it_null(stress_iterator_t it)   { return NULL == it.d; }
static inline bool stress_it_eq(stress_iterator_t a, stress_iterator_t b)  { return a.d == b.d; }
static inline bool stress_it_ne(stress_iterator_t a, stress_iterator_t b)  { return a.d != b.d; }
#define stress_it_data(it)   (*((ds_data_t*)((it).d)))
#define stress_it_sdata(it)  (*((char**)((it).d)))

typedef struct {
    ds_size_t (*size)(void* _this);
    ds_count_t (*count)(void* _this, ds_data_t data);
    stress_iterator_t (*end)(void* _this);
    stress_iterator_t (*begin)(void* _this);
    stress_iterator_t (*next)(void* _this, stress_iterator_t iterator);
    stress_iterator_t (*prev)(void* _this, stress_iterator_t iterator);
    stress_iterator_t (*rend)(void* _this);
    stress_iterator_t (*rbegin)(void* _this);
    stress_iterator_t (*rnext)(void* _this, stress_iterator_t r_iterator);
    stress_iterator_t (*rprev)(void* _this, stress_iterator_t r_iterator);
    ds_data_t (*first)(void* _this, ds_data_t default_data);
    ds_data_t (*last)(void* _this, ds_data_t default_data);
    stress_iterator_t (*find)(void* _this, ds_data_t data);
    stress_iterator_t (*push_back)(void* _this, ds_data_t data);
    stress_iterator_t (*push_front)(void* _this, ds_data_t data);
    stress_iterator_t (*insert)(void* _this, stress_iterator_t iterator, ds_data_t data);                            /* insert data before `iterator` */
    stress_iterator_t (*erase)(void* _this, stress_iterator_t iterator);
    stress_iterator_t (*erase_range)(void* _this, stress_iterator_t iterator_begin, stress_iterator_t iterator_end); /* [iterator_begin, iterator_end) */
    void (*pop_back)(void* _this);
    void (*pop_front)(void* _this);
    ds_size_t (*remove)(void* _this, ds_data_t data);
    ds_size_t (*remove_if)(void* _this, remove_if_condition cond);
    void (*sort)(void* _this, __cmp __cmp);
    ds_size_t (*clear)(void* _this);
} class_stress_slow_interface_t;

#endif /* __J_STRESS_SLOW_H */
