/*
  Map Interfaces
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

#ifndef __J_MAP_H
#define __J_MAP_H

#include <linux/_types.h>
#include <map/map_ops.h>

typedef struct map_node {
    map_key_t key;
    map_value_t value;
    struct rb_node node;
} map_node_t;

typedef struct map_iterator {
    map_key_t key;
    map_value_t value;
} map_iterator_t;

typedef struct map_reverse_iterator {
    map_key_t key;
    map_value_t value;
} map_reverse_iterator_t;
typedef map_reverse_iterator_t map_r_iterator_t;

typedef struct map {
    const class_map_ops_t* ops;
    struct rb_root root;
    map_size_t size;
} map_t;

typedef struct class_map {
    map_size_t (*size)(const map_t* _this);
    map_count_t (*count)(const map_t* _this, map_key_t key);
    map_iterator_t* (*end)(const map_t* _this);
    map_iterator_t* (*begin)(const map_t* _this);
    map_iterator_t* (*next)(const map_t* _this, const map_iterator_t* iterator);
    map_iterator_t* (*prev)(const map_t* _this, const map_iterator_t* iterator);
    map_r_iterator_t* (*rend)(const map_t* _this);
    map_r_iterator_t* (*rbegin)(const map_t* _this);
    map_r_iterator_t* (*rnext)(const map_t* _this, const map_r_iterator_t* r_iterator);
    map_r_iterator_t* (*rprev)(const map_t* _this, const map_r_iterator_t* r_iterator);
    map_iterator_t* (*find)(const map_t* _this, map_key_t key);
    map_iterator_t* (*lower_bound)(const map_t* _this, map_key_t key);                 /* >= key */
    map_iterator_t* (*upper_bound)(const map_t* _this, map_key_t key);                 /*  > key */
    map_iterator_t* (*insert)(map_t* _this, map_key_t key, map_value_t value);         /* if input key doesn't match -> insert | if input key match -> return NULL */
    map_iterator_t* (*insert_replace)(map_t* _this, map_key_t key, map_value_t value); /* if input key doesn't match -> insert | if input key match -> replace value (Refer to C++11 a[key] = value) */
    map_iterator_t* (*erase)(map_t* _this, map_iterator_t* iterator);
    map_size_t (*remove)(map_t* _this, map_key_t key);
    map_size_t (*remove_if)(map_t* _this, remove_if_condition_kv cond);
    map_size_t (*clear)(map_t* _this);
} class_map_t;

void __map_init(map_t* map);
void __map_deinit(map_t* map);
const class_map_t* class_map_ins(void);
#define g_class_map()            class_map_ins()
#define cmap                     g_class_map()
#define MAP_INIT(_ptr)           (map_t) { .ops = NULL, .size = 0, }; __map_init((_ptr))
#define MAP_INIT_OPS(_ptr, _ops) (map_t) { .ops = _ops, .size = 0, }; __map_init((_ptr))
#define MAP_DEINIT(_ptr)         do { __map_deinit((_ptr)); } while(0)

#endif /* __J_MAP_H */
