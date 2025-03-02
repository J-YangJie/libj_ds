/*
  Vector Interfaces
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

#ifndef __J_VECTOR_H
#define __J_VECTOR_H

#include <linux/_types.h>
#include <vector/vector_ops.h>

typedef struct vector_node {
    vector_data_t data;
} vector_node_t;

typedef struct vector_iterator {
    vector_data_t data;
} vector_iterator_t;

typedef struct vector_reverse_iterator {
    vector_data_t data;
} vector_reverse_iterator_t;
typedef vector_reverse_iterator_t vector_r_iterator_t;

typedef struct vector {
    const class_vector_ops_t* ops;
    vector_node_t* head;
    vector_size_t size;
    vector_size_t capacity;
} vector_t;

typedef struct class_vector {
    vector_size_t (*size)(const vector_t* _this);
    vector_size_t (*capacity)(const vector_t* _this);
    vector_count_t (*count)(const vector_t* _this, vector_data_t data);
    vector_iterator_t* (*end)(const vector_t* _this);
    vector_iterator_t* (*begin)(const vector_t* _this);
    vector_iterator_t* (*next)(const vector_t* _this, const vector_iterator_t* iterator);
    vector_iterator_t* (*prev)(const vector_t* _this, const vector_iterator_t* iterator);
    vector_r_iterator_t* (*rend)(const vector_t* _this);
    vector_r_iterator_t* (*rbegin)(const vector_t* _this);
    vector_r_iterator_t* (*rnext)(const vector_t* _this, const vector_r_iterator_t* r_iterator);
    vector_r_iterator_t* (*rprev)(const vector_t* _this, const vector_r_iterator_t* r_iterator);
    vector_iterator_t* (*at)(const vector_t* _this, vector_size_t n);
    vector_data_t (*first)(const vector_t* _this, vector_data_t default_data);
    vector_data_t (*last)(const vector_t* _this, vector_data_t default_data);
    vector_iterator_t* (*find)(const vector_t* _this, vector_data_t data);
    vector_iterator_t* (*push_back)(vector_t* _this, vector_data_t data);
    vector_iterator_t* (*push_front)(vector_t* _this, vector_data_t data);
    vector_iterator_t* (*insert)(vector_t* _this, vector_iterator_t* iterator, vector_data_t data);                         /* insert data before `iterator` */
    vector_iterator_t* (*erase)(vector_t* _this, vector_iterator_t* iterator);
    vector_iterator_t* (*erase_range)(vector_t* _this, vector_iterator_t* iterator_begin, vector_iterator_t* iterator_end); /* [iterator_begin, iterator_end) */
    void (*pop_back)(vector_t* _this);
    void (*pop_front)(vector_t* _this);
    vector_size_t (*remove)(vector_t* _this, vector_data_t data);
    vector_size_t (*remove_if)(vector_t* _this, remove_if_condition cond);
    bool (*reserve)(vector_t* _this, vector_size_t n);
    bool (*resize)(vector_t* _this, vector_size_t n, vector_data_t default_data);
    void (*sort)(vector_t* _this, __comp __comp); /* When `__lt` is in ascending order, `__gt` is in descending order.
                                                     Use `class_vector_ops_t->__lt` as the sorting rule if `__comp` is null.
                                                     Just compare the magnitudes when `class_vector_ops_t->__lt` is null. */
    vector_size_t (*clear)(vector_t* _this);
} class_vector_t;

void __vector_init(vector_t* vector);
void __vector_deinit(vector_t* vector);
const class_vector_t* class_vector_ins(void);
#define g_class_vector()            class_vector_ins()
#define cvector                     g_class_vector()
#define VECTOR_INIT(_ptr)           (vector_t) { .ops = NULL, .size = 0, }; __vector_init((_ptr))
#define VECTOR_INIT_OPS(_ptr, _ops) (vector_t) { .ops = _ops, .size = 0, }; __vector_init((_ptr))
#define VECTOR_INIT_STRING(_ptr)    VECTOR_INIT_OPS((_ptr), g_class_vector_ops_string())
#define VECTOR_DEINIT(_ptr)         do { __vector_deinit((_ptr)); } while(0)

#endif /* __J_VECTOR_H */
