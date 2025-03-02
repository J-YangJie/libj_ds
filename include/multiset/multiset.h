/*
  Multiset Interfaces
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

#ifndef __J_MULTISET_H
#define __J_MULTISET_H

#include <linux/_types.h>
#include <multiset/multiset_ops.h>

typedef struct multiset_node {
    multiset_value_t value;
    struct rb_node node;
} multiset_node_t;

typedef struct multiset_iterator {
    multiset_value_t value;
} multiset_iterator_t;

typedef struct multiset_reverse_iterator {
    multiset_value_t value;
} multiset_reverse_iterator_t;
typedef multiset_reverse_iterator_t multiset_r_iterator_t;

typedef struct multiset {
    const class_multiset_ops_t* ops;
    struct rb_root root;
    multiset_size_t size;
} multiset_t;

typedef struct class_multiset {
    multiset_size_t (*size)(const multiset_t* _this);
    multiset_count_t (*count)(const multiset_t* _this, multiset_value_t value);
    multiset_iterator_t* (*end)(const multiset_t* _this);
    multiset_iterator_t* (*begin)(const multiset_t* _this);
    multiset_iterator_t* (*next)(const multiset_t* _this, const multiset_iterator_t* iterator);
    multiset_iterator_t* (*prev)(const multiset_t* _this, const multiset_iterator_t* iterator);
    multiset_r_iterator_t* (*rend)(const multiset_t* _this);
    multiset_r_iterator_t* (*rbegin)(const multiset_t* _this);
    multiset_r_iterator_t* (*rnext)(const multiset_t* _this, const multiset_r_iterator_t* r_iterator);
    multiset_r_iterator_t* (*rprev)(const multiset_t* _this, const multiset_r_iterator_t* r_iterator);
    multiset_iterator_t* (*find)(const multiset_t* _this, multiset_value_t value);
    multiset_iterator_t* (*lower_bound)(const multiset_t* _this, multiset_value_t value); /* >= value */
    multiset_iterator_t* (*upper_bound)(const multiset_t* _this, multiset_value_t value); /*  > value */
    multiset_iterator_t* (*insert)(multiset_t* _this, multiset_value_t value);            /* if input value doesn't match -> insert | if input value match -> return NULL */
    multiset_iterator_t* (*erase)(multiset_t* _this, multiset_iterator_t* iterator);
    multiset_size_t (*remove)(multiset_t* _this, multiset_value_t value);
    multiset_size_t (*remove_if)(multiset_t* _this, remove_if_condition_v cond);
    multiset_size_t (*clear)(multiset_t* _this);
} class_multiset_t;

void __multiset_init(multiset_t* multiset);
void __multiset_deinit(multiset_t* multiset);
const class_multiset_t* class_multiset_ins(void);
#define g_class_multiset()            class_multiset_ins()
#define cmultiset                     g_class_multiset()
#define MULTISET_INIT(_ptr)           (multiset_t) { .ops = NULL, .size = 0, }; __multiset_init((_ptr))
#define MULTISET_INIT_OPS(_ptr, _ops) (multiset_t) { .ops = _ops, .size = 0, }; __multiset_init((_ptr))
#define MULTISET_DEINIT(_ptr)         do { __multiset_deinit((_ptr)); } while(0)

#endif /* __J_MULTISET_H */
