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

#ifndef __J_STRESS_H
#define __J_STRESS_H

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
#define stress_it_value(it)  (((ds_value_t*)((it).d))[1])

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
    stress_iterator_t (*insert)(void* _this, ds_data_t data, ds_data_t data2);
    stress_iterator_t (*insert_replace)(void* _this, ds_data_t data, ds_data_t data2);
    stress_iterator_t (*find)(void* _this, ds_data_t data);
    stress_iterator_t (*lower_bound)(void* _this, ds_data_t data);
    stress_iterator_t (*upper_bound)(void* _this, ds_data_t data);
    stress_iterator_t (*erase)(void* _this, stress_iterator_t iterator);
    ds_size_t (*remove)(void* _this, ds_data_t data);
    ds_size_t (*clear)(void* _this);
} class_stress_interface_t;

#endif /* __J_STRESS_H */
