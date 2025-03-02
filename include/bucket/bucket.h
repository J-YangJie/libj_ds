/*
  Bucket Interfaces
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

#ifndef __J_BUCKET_H
#define __J_BUCKET_H

#include <linux/_types.h>
#include <bucket/bucket_ops.h>

typedef struct bucket_node {
    bucket_key_t key;
    bucket_value_t value;
    bucket_hash_t hash;
    union {
        struct rb_node rb_node;
#ifdef BUCKET_USE_DL
        struct list_head dl_node;
#endif /* BUCKET_USE_DL */
        struct hlist_node hl_node;
    } ds_node;
} bucket_node_t;

typedef struct bucket_iterator {
    bucket_key_t key;
    bucket_value_t value;
    bucket_hash_t hash;
} bucket_iterator_t;

typedef struct bucket_reverse_iterator {
    bucket_key_t key;
    bucket_value_t value;
    bucket_hash_t hash;
} bucket_reverse_iterator_t;
typedef bucket_reverse_iterator_t bucket_r_iterator_t;

typedef struct bucket {
    bucket_size_t size;
    union {
        struct rb_root rb;
#ifdef BUCKET_USE_DL
        struct list_head dl;
#endif /* BUCKET_USE_DL */
        struct hlist_head hl;
    } ds;
} bucket_t;

typedef enum bucket_ds {
    BUCKET_DS_MIN   = 0,
    BUCKET_DS_HLIST,
    BUCKET_DS_DLIST,
    BUCKET_DS_RBTREE,
    // BUCKET_DS_SLIST,
    BUCKET_DS_MAX,
} bucket_ds_t;

typedef struct class_bucket {
    bucket_size_t  (*size)(const bucket_t* _this);
    bucket_iterator_t* (*end)(const bucket_t* _this);
    bucket_iterator_t* (*begin)(const bucket_t* _this, bucket_ds_t type);
    bucket_iterator_t* (*next)(const bucket_t* _this, bucket_ds_t type, const bucket_iterator_t* iterator);
    bucket_iterator_t* (*prev)(const bucket_t* _this, bucket_ds_t type, const bucket_iterator_t* iterator);
    bucket_r_iterator_t* (*rend)(const bucket_t* _this);
    bucket_r_iterator_t* (*rbegin)(const bucket_t* _this, bucket_ds_t type);
    bucket_r_iterator_t* (*rnext)(const bucket_t* _this, bucket_ds_t type, const bucket_r_iterator_t* r_iterator);
    bucket_r_iterator_t* (*rprev)(const bucket_t* _this, bucket_ds_t type, const bucket_r_iterator_t* r_iterator);
    bucket_iterator_t* (*find)(const bucket_t* _this, const class_bucket_ops_t* ops, bucket_ds_t type, bucket_key_t key);
    bucket_iterator_t* (*insert)(bucket_t* _this, const class_bucket_ops_t* ops, bucket_ds_t type, bucket_hash_t hash, bucket_key_t key, bucket_value_t value);         /* if input key doesn't match -> insert | if input key match -> return NULL */
    bucket_iterator_t* (*insert_replace)(bucket_t* _this, const class_bucket_ops_t* ops, bucket_ds_t type, bucket_hash_t hash, bucket_key_t key, bucket_value_t value); /* if input key doesn't match -> insert | if input key match -> replace value (Refer to C++11 a[key] = value) */
    bucket_iterator_t* (*erase)(bucket_t* _this, const class_bucket_ops_t* ops, bucket_ds_t type, bucket_iterator_t* iterator);
    bucket_size_t  (*remove)(bucket_t* _this, const class_bucket_ops_t* ops, bucket_ds_t type, bucket_key_t key);
    bucket_size_t  (*clear)(bucket_t* _this, const class_bucket_ops_t* ops, bucket_ds_t type);
} class_bucket_t;

/* Private functions */
typedef struct class_bucket_priv {
    /* iterator */
    bucket_node_t* (*end)(const bucket_t* _this);
    bucket_node_t* (*begin)(const bucket_t* _this, bucket_ds_t type);
    bucket_node_t* (*next)(const bucket_t* _this, bucket_ds_t type, const bucket_node_t* node);
    bucket_node_t* (*prev)(const bucket_t* _this, bucket_ds_t type, const bucket_node_t* node);
    bucket_node_t* (*rend)(const bucket_t* _this);
    bucket_node_t* (*rbegin)(const bucket_t* _this, bucket_ds_t type);
    bucket_node_t* (*rnext)(const bucket_t* _this, bucket_ds_t type, const bucket_node_t* node);
    bucket_node_t* (*rprev)(const bucket_t* _this, bucket_ds_t type, const bucket_node_t* node);
    bucket_node_t* (*find)(const bucket_t* _this, const class_bucket_ops_t* ops, bucket_ds_t type, bucket_key_t key);
    bucket_node_t* (*insert)(bucket_t* _this, const class_bucket_ops_t* ops, bucket_ds_t type, bucket_hash_t hash, bucket_key_t key, bucket_value_t value);         /* if input key doesn't match -> insert | if input key match -> return NULL */
    bucket_node_t* (*insert_replace)(bucket_t* _this, const class_bucket_ops_t* ops, bucket_ds_t type, bucket_hash_t hash, bucket_key_t key, bucket_value_t value); /* if input key doesn't match -> insert | if input key match -> replace value (Refer to C++11 a[key] = value) */
    bucket_node_t* (*erase)(bucket_t* _this, const class_bucket_ops_t* ops, bucket_ds_t type, bucket_node_t* pos);
    /* hashmap */
    bucket_node_t* (*_first)(const bucket_t* _this, bucket_ds_t type);
    bucket_node_t* (*_last)(const bucket_t* _this, bucket_ds_t type);
    bucket_node_t* (*find_has_checked_valid)(const bucket_t* _this, const class_bucket_ops_t* ops, bucket_ds_t type, bucket_key_t key);
    bucket_node_t* (*insert_has_checked_same)(bucket_t* _this, const class_bucket_ops_t* ops, bucket_ds_t type, bucket_node_t* node);
    bucket_node_t* (*insert_has_checked_valid)(bucket_t* _this, const class_bucket_ops_t* ops, bucket_ds_t type, bucket_hash_t hash, bucket_key_t key, bucket_value_t value);         /* if input key doesn't match -> insert | if input key match -> return NULL */
    bucket_node_t* (*insert_replace_has_checked_valid)(bucket_t* _this, const class_bucket_ops_t* ops, bucket_ds_t type, bucket_hash_t hash, bucket_key_t key, bucket_value_t value); /* if input key doesn't match -> insert | if input key match -> replace value (Refer to C++11 a[key] = value) */
    bucket_size_t  (*remove_has_checked_valid)(bucket_t* _this, const class_bucket_ops_t* ops, bucket_ds_t type, bucket_key_t key);
    bucket_node_t* (*pop)(bucket_t* _this, bucket_ds_t type, bucket_node_t* pos);
    void           (*__switch)(bucket_t* _this, const class_bucket_ops_t* ops, bucket_ds_t type_from, bucket_ds_t type_to);
} class_bucket_priv_t;

void __bucket_init(bucket_t* bucket, bucket_ds_t type);
void __bucket_deinit(bucket_t* bucket, const class_bucket_ops_t* ops, bucket_ds_t type);
const class_bucket_t*      class_bucket_ins(void);
const class_bucket_priv_t* class_bucket_priv_ins(void);
#define g_class_bucket()                 class_bucket_ins()
#define g_class_bucket_priv()            class_bucket_priv_ins()
#define cbucket                          g_class_bucket()
#define cbucket_priv                     g_class_bucket_priv()
#define BUCKET_INIT(_ptr, _type)         (bucket_t) { .size = 0, }; __bucket_init((_ptr), (_type))
#define BUCKET_DEINIT(_ptr, _ops, _type) do { __bucket_deinit((_ptr), (_ops), (_type)); } while(0)

#endif /* __J_BUCKET_H */
