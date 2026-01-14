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
    ds_size_t (*size)(void* _this);
    ds_count_t (*count)(void* _this, ds_data_t data);
    void* (*end)(void* _this);
    void* (*begin)(void* _this);
    void* (*next)(void* _this, void* iterator);
    void* (*prev)(void* _this, void* iterator);
    void* (*rend)(void* _this);
    void* (*rbegin)(void* _this);
    void* (*rnext)(void* _this, void* r_iterator);
    void* (*rprev)(void* _this, void* r_iterator);
    void* (*insert)(void* _this, ds_data_t data, ds_data_t data2);
    void* (*insert_replace)(void* _this, ds_data_t data, ds_data_t data2);
    void* (*find)(void* _this, ds_data_t data);
    void* (*lower_bound)(void* _this, ds_data_t data);
    void* (*upper_bound)(void* _this, ds_data_t data);
    void* (*erase)(void* _this, void* iterator);
    ds_size_t (*remove)(void* _this, ds_data_t data);
    ds_size_t (*clear)(void* _this);
} class_stress_interface_t;

#endif /* __J_STRESS_H */
