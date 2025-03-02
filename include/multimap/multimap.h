/*
  Multimap Interfaces
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

#ifndef __J_MULTIMAP_H
#define __J_MULTIMAP_H

#include <linux/_types.h>
#include <multimap/multimap_ops.h>

typedef struct multimap_node {
    multimap_key_t key;
    multimap_value_t value;
    struct rb_node node;
} multimap_node_t;

typedef struct multimap_iterator {
    multimap_key_t key;
    multimap_value_t value;
} multimap_iterator_t;

typedef struct multimap_reverse_iterator {
    multimap_key_t key;
    multimap_value_t value;
} multimap_reverse_iterator_t;
typedef multimap_reverse_iterator_t multimap_r_iterator_t;

typedef struct multimap {
    const class_multimap_ops_t* ops;
    struct rb_root root;
    multimap_size_t size;
} multimap_t;

typedef struct class_multimap {
    multimap_size_t (*size)(const multimap_t* _this);
    multimap_count_t (*count)(const multimap_t* _this, multimap_key_t key);
    multimap_iterator_t* (*end)(const multimap_t* _this);
    multimap_iterator_t* (*begin)(const multimap_t* _this);
    multimap_iterator_t* (*next)(const multimap_t* _this, const multimap_iterator_t* iterator);
    multimap_iterator_t* (*prev)(const multimap_t* _this, const multimap_iterator_t* iterator);
    multimap_r_iterator_t* (*rend)(const multimap_t* _this);
    multimap_r_iterator_t* (*rbegin)(const multimap_t* _this);
    multimap_r_iterator_t* (*rnext)(const multimap_t* _this, const multimap_r_iterator_t* r_iterator);
    multimap_r_iterator_t* (*rprev)(const multimap_t* _this, const multimap_r_iterator_t* r_iterator);
    multimap_iterator_t* (*find)(const multimap_t* _this, multimap_key_t key);
    multimap_iterator_t* (*lower_bound)(const multimap_t* _this, multimap_key_t key);              /* >= key */
    multimap_iterator_t* (*upper_bound)(const multimap_t* _this, multimap_key_t key);              /*  > key */
    multimap_iterator_t* (*insert)(multimap_t* _this, multimap_key_t key, multimap_value_t value); /* if input key doesn't match -> insert | if input key match -> return NULL */
    multimap_iterator_t* (*erase)(multimap_t* _this, multimap_iterator_t* iterator);
    multimap_size_t (*remove)(multimap_t* _this, multimap_key_t key);
    multimap_size_t (*remove_if)(multimap_t* _this, remove_if_condition_kv cond);
    multimap_size_t (*clear)(multimap_t* _this);
} class_multimap_t;

void __multimap_init(multimap_t* multimap);
void __multimap_deinit(multimap_t* multimap);
const class_multimap_t* class_multimap_ins(void);
#define g_class_multimap()            class_multimap_ins()
#define cmultimap                     g_class_multimap()
#define MULTIMAP_INIT(_ptr)           (multimap_t) { .ops = NULL, .size = 0, }; __multimap_init((_ptr))
#define MULTIMAP_INIT_OPS(_ptr, _ops) (multimap_t) { .ops = _ops, .size = 0, }; __multimap_init((_ptr))
#define MULTIMAP_DEINIT(_ptr)         do { __multimap_deinit((_ptr)); } while(0)

#endif /* __J_MULTIMAP_H */
