/*
  Hashmap Interfaces
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

#ifndef __J_HASH_MAP_H
#define __J_HASH_MAP_H

#include <stdint.h>
#include <linux/_types.h>
#include <bucket/bucket.h>
#include <hashmap/hashmap_ops.h>

typedef struct hashmap_node {
    bucket_t* bucket;
} hashmap_node_t;
typedef bucket_node_t hashmap_bnode_t;

typedef struct hashmap_iterator {
    hashmap_key_t key;
    hashmap_value_t value;
    hashmap_hash_t hash;
} hashmap_iterator_t;

typedef struct hashmap_reverse_iterator {
    hashmap_key_t key;
    hashmap_value_t value;
    hashmap_hash_t hash;
} hashmap_reverse_iterator_t;
typedef hashmap_reverse_iterator_t hashmap_r_iterator_t;

typedef union hashmap_config {
    struct {
        uint32_t b_bkt_only_l : 1;
        uint32_t b_bkt_only_r : 1;
        uint32_t b_bkt_l_to_r : 1;
    } c;
    uint32_t d;
} hashmap_config_t;

typedef struct hashmap {
    const class_hashmap_ops_t* ops;
    hashmap_node_t*  head;
    hashmap_node_t*  head_e;
    hashmap_size_t   size;
    hashmap_size_t   capacity;
    hashmap_size_t   capacity_init;
    hashmap_size_t   capacity_max;
    float            capacity_load_factor;
    hashmap_config_t config;
    hashmap_bcount_t bucket_count;
    hashmap_size_t   pi_s;
    hashmap_size_t   pi_e;
} hashmap_t;

typedef struct class_hashmap {
    hashmap_size_t (*size)(const hashmap_t* _this);
    hashmap_size_t (*capacity)(const hashmap_t* _this);
    hashmap_bcount_t (*bucket_count)(const hashmap_t* _this);
    hashmap_count_t (*count)(const hashmap_t* _this, hashmap_key_t key);
    hashmap_iterator_t* (*end)(const hashmap_t* _this);
    hashmap_iterator_t* (*begin)(const hashmap_t* _this);
    hashmap_iterator_t* (*next)(const hashmap_t* _this, const hashmap_iterator_t* iterator);
    hashmap_iterator_t* (*prev)(const hashmap_t* _this, const hashmap_iterator_t* iterator);
    hashmap_r_iterator_t* (*rend)(const hashmap_t* _this);
    hashmap_r_iterator_t* (*rbegin)(const hashmap_t* _this);
    hashmap_r_iterator_t* (*rnext)(const hashmap_t* _this, const hashmap_r_iterator_t* r_iterator);
    hashmap_r_iterator_t* (*rprev)(const hashmap_t* _this, const hashmap_r_iterator_t* r_iterator);
    hashmap_iterator_t* (*find)(const hashmap_t* _this, hashmap_key_t key);
    hashmap_iterator_t* (*insert)(hashmap_t* _this, hashmap_key_t key, hashmap_value_t value);         /* if input key doesn't match -> insert | if input key match -> return NULL */
    hashmap_iterator_t* (*insert_replace)(hashmap_t* _this, hashmap_key_t key, hashmap_value_t value); /* if input key doesn't match -> insert | if input key match -> replace value (Refer to C++11 a[key] = value) */
    hashmap_iterator_t* (*erase)(hashmap_t* _this, hashmap_iterator_t* iterator);
    hashmap_size_t (*remove)(hashmap_t* _this, hashmap_key_t key);
    hashmap_size_t (*clear)(hashmap_t* _this);
} class_hashmap_t;

void __hashmap_init(hashmap_t* hashmap);
void __hashmap_init_arg(hashmap_t* hashmap, int num_arg, ...);
void __hashmap_deinit(hashmap_t* hashmap);
const class_hashmap_t* class_hashmap_ins(void);
#define g_class_hashmap()            class_hashmap_ins()
#define chashmap                     g_class_hashmap()
#define HASHMAP_INIT(_ptr)           (hashmap_t) { .ops = NULL, .size = 0, }; __hashmap_init((_ptr))
#define HASHMAP_INIT_OPS(_ptr, _ops) (hashmap_t) { .ops = _ops, .size = 0, }; __hashmap_init((_ptr))
#define HASHMAP_DEINIT(_ptr)         do { __hashmap_deinit((_ptr)); } while(0)

#define HASHMAP_INIT_1(_ptr, _capacity_init) \
        (hashmap_t) { .ops = NULL, .size = 0, }; __hashmap_init_arg((_ptr), 1, (_capacity_init))
#define HASHMAP_INIT_2(_ptr, _capacity_init, _capacity_max) \
        (hashmap_t) { .ops = NULL, .size = 0, }; __hashmap_init_arg((_ptr), 2, (_capacity_init), (_capacity_max))
#define HASHMAP_INIT_3(_ptr, _capacity_init, _capacity_max, _load_factor) \
        (hashmap_t) { .ops = NULL, .size = 0, }; __hashmap_init_arg((_ptr), 3, (_capacity_init), (_capacity_max), (_load_factor))
#define HASHMAP_INIT_4(_ptr, _capacity_init, _capacity_max, _load_factor, _config) \
        (hashmap_t) { .ops = NULL, .size = 0, }; __hashmap_init_arg((_ptr), 4, (_capacity_init), (_capacity_max), (_load_factor), (_config))

#define HASHMAP_INIT_OPS_1(_ptr, _ops, _capacity_init) \
        (hashmap_t) { .ops = _ops, .size = 0, }; __hashmap_init_arg((_ptr), 1, (_capacity_init))
#define HASHMAP_INIT_OPS_2(_ptr, _ops, _capacity_init, _capacity_max) \
        (hashmap_t) { .ops = _ops, .size = 0, }; __hashmap_init_arg((_ptr), 2, (_capacity_init), (_capacity_max))
#define HASHMAP_INIT_OPS_3(_ptr, _ops, _capacity_init, _capacity_max, _load_factor) \
        (hashmap_t) { .ops = _ops, .size = 0, }; __hashmap_init_arg((_ptr), 3, (_capacity_init), (_capacity_max), (_load_factor))
#define HASHMAP_INIT_OPS_4(_ptr, _ops, _capacity_init, _capacity_max, _load_factor, _config) \
        (hashmap_t) { .ops = _ops, .size = 0, }; __hashmap_init_arg((_ptr), 4, (_capacity_init), (_capacity_max), (_load_factor), (_config))

#endif /* __J_HASH_MAP_H */
