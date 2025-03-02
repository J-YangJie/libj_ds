/*
  Set Interfaces
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

#ifndef __J_SET_H
#define __J_SET_H

#include <linux/_types.h>
#include <set/set_ops.h>

typedef struct set_node {
    set_value_t value;
    struct rb_node node;
} set_node_t;

typedef struct set_iterator {
    set_value_t value;
} set_iterator_t;

typedef struct set_reverse_iterator {
    set_value_t value;
} set_reverse_iterator_t;
typedef set_reverse_iterator_t set_r_iterator_t;

typedef struct set {
    const class_set_ops_t* ops;
    struct rb_root root;
    set_size_t size;
} set_t;

typedef struct class_set {
    set_size_t (*size)(const set_t* _this);
    set_count_t (*count)(const set_t* _this, set_value_t value);
    set_iterator_t* (*end)(const set_t* _this);
    set_iterator_t* (*begin)(const set_t* _this);
    set_iterator_t* (*next)(const set_t* _this, const set_iterator_t* iterator);
    set_iterator_t* (*prev)(const set_t* _this, const set_iterator_t* iterator);
    set_r_iterator_t* (*rend)(const set_t* _this);
    set_r_iterator_t* (*rbegin)(const set_t* _this);
    set_r_iterator_t* (*rnext)(const set_t* _this, const set_r_iterator_t* r_iterator);
    set_r_iterator_t* (*rprev)(const set_t* _this, const set_r_iterator_t* r_iterator);
    set_iterator_t* (*find)(const set_t* _this, set_value_t value);
    set_iterator_t* (*lower_bound)(const set_t* _this, set_value_t value); /* >= value */
    set_iterator_t* (*upper_bound)(const set_t* _this, set_value_t value); /*  > value */
    set_iterator_t* (*insert)(set_t* _this, set_value_t value);            /* if input value doesn't match -> insert | if input value match -> return NULL */
    set_iterator_t* (*erase)(set_t* _this, set_iterator_t* iterator);
    set_size_t (*remove)(set_t* _this, set_value_t value);
    set_size_t (*remove_if)(set_t* _this, remove_if_condition_v cond);
    set_size_t (*clear)(set_t* _this);
} class_set_t;

void __set_init(set_t* set);
void __set_deinit(set_t* set);
const class_set_t* class_set_ins(void);
#define g_class_set()            class_set_ins()
#define cset                     g_class_set()
#define SET_INIT(_ptr)           (set_t) { .ops = NULL, .size = 0, }; __set_init((_ptr))
#define SET_INIT_OPS(_ptr, _ops) (set_t) { .ops = _ops, .size = 0, }; __set_init((_ptr))
#define SET_DEINIT(_ptr)         do { __set_deinit((_ptr)); } while(0)

#endif /* __J_SET_H */
