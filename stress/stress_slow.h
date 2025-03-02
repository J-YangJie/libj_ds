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
    ds_data_t (*first)(void* _this, ds_data_t default_data);
    ds_data_t (*last)(void* _this, ds_data_t default_data);
    void* (*find)(void* _this, ds_data_t data);
    void* (*push_back)(void* _this, ds_data_t data);
    void* (*push_front)(void* _this, ds_data_t data);
    void* (*insert)(void* _this, void* iterator, ds_data_t data);                /* insert data before `iterator` */
    void* (*erase)(void* _this, void* iterator);
    void* (*erase_range)(void* _this, void* iterator_begin, void* iterator_end); /* [iterator_begin, iterator_end) */
    void (*pop_back)(void* _this);
    void (*pop_front)(void* _this);
    ds_size_t (*remove)(void* _this, ds_data_t data);
    ds_size_t (*remove_if)(void* _this, remove_if_condition cond);
    void (*sort)(void* _this, __comp __comp);
    ds_size_t (*clear)(void* _this);
} class_stress_slow_interface_t;

#endif /* __J_STRESS_SLOW_H */
