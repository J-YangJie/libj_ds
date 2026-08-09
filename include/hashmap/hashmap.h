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

#include <hashmap/hashmap_ops.h>

typedef struct hashmap_iterator {
    union {
        hashmap_key_t key;
        char* skey;
    };
    union {
        hashmap_value_t value;
        char* svalue;
    };
    hashmap_hash_t hash;
} hashmap_iterator_t;

typedef struct hashmap_reverse_iterator {
    union {
        hashmap_key_t key;
        char* skey;
    };
    union {
        hashmap_value_t value;
        char* svalue;
    };
    hashmap_hash_t hash;
} hashmap_reverse_iterator_t;
typedef hashmap_reverse_iterator_t hashmap_r_iterator_t;

typedef union hashmap_config {
    struct {
        uint32_t b_bkt_only_l : 1;
        uint32_t b_bkt_only_r : 1; /* TODO: Currently invalid */
        uint32_t b_bkt_l_to_r : 1;
    } c;
    uint32_t d;
} hashmap_config_t;

typedef struct hashmap hashmap_t;

/* Method 1 */
hashmap_size_t        chashmap_size(const hashmap_t* _this);
hashmap_bcount_t      chashmap_bucket_count(const hashmap_t* _this);
hashmap_bcount_t      chashmap_bucket_valid_count(const hashmap_t* _this);
hashmap_count_t       chashmap_count(const hashmap_t* _this, hashmap_key_t key);
hashmap_iterator_t*   chashmap_end(const hashmap_t* _this);
hashmap_iterator_t*   chashmap_begin(const hashmap_t* _this);
hashmap_iterator_t*   chashmap_next(const hashmap_t* _this, const hashmap_iterator_t* iterator);
hashmap_iterator_t*   chashmap_prev(const hashmap_t* _this, const hashmap_iterator_t* iterator);
hashmap_r_iterator_t* chashmap_rend(const hashmap_t* _this);
hashmap_r_iterator_t* chashmap_rbegin(const hashmap_t* _this);
hashmap_r_iterator_t* chashmap_rnext(const hashmap_t* _this, const hashmap_r_iterator_t* r_iterator);
hashmap_r_iterator_t* chashmap_rprev(const hashmap_t* _this, const hashmap_r_iterator_t* r_iterator);
hashmap_iterator_t*   chashmap_find(const hashmap_t* _this, hashmap_key_t key);
hashmap_iterator_t*   chashmap_insert(hashmap_t* _this, hashmap_key_t key, hashmap_value_t value);
hashmap_iterator_t*   chashmap_insert_replace(hashmap_t* _this, hashmap_key_t key, hashmap_value_t value);
hashmap_iterator_t*   chashmap_erase(hashmap_t* _this, hashmap_iterator_t* iterator);
hashmap_size_t        chashmap_remove(hashmap_t* _this, hashmap_key_t key);
hashmap_size_t        chashmap_clear(hashmap_t* _this);

/* Method 2 */
typedef struct class_hashmap {
    hashmap_size_t (*size)(const hashmap_t* _this);
    hashmap_bcount_t (*bucket_count)(const hashmap_t* _this);
    hashmap_bcount_t (*bucket_valid_count)(const hashmap_t* _this);
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

hashmap_t* __hashmap_new(const class_hashmap_ops_t* ops,
                         hashmap_bcount_t bucket_count_init,
                         hashmap_bcount_t bucket_count_max,
                         float            load_factor,
                         hashmap_config_t* config);
void       __hashmap_delete(hashmap_t** _this);
const class_hashmap_t* class_hashmap_ins(void);
#define g_class_hashmap()      class_hashmap_ins()
#define chashmap               g_class_hashmap()
#define HASHMAP_NEW()          __hashmap_new(NULL, 0, 0, 0.0f, NULL)
#define HASHMAP_NEW_OPS(_ops)  __hashmap_new((_ops), 0, 0, 0.0f, NULL)
#define HASHMAP_DELETE(_pptr)  do { __hashmap_delete((_pptr)); } while(0)

#define HASHMAP_NEW_1(_bucket_count_init) \
        __hashmap_new(NULL, (_bucket_count_init), 0, 0.0f, NULL)
#define HASHMAP_NEW_2(_bucket_count_init, _bucket_count_max) \
        __hashmap_new(NULL, (_bucket_count_init), (_bucket_count_max), 0.0f, NULL)
#define HASHMAP_NEW_3(_bucket_count_init, _bucket_count_max, _load_factor) \
        __hashmap_new(NULL, (_bucket_count_init), (_bucket_count_max), (_load_factor), NULL)
#define HASHMAP_NEW_4(_bucket_count_init, _bucket_count_max, _load_factor, _config) \
        __hashmap_new(NULL, (_bucket_count_init), (_bucket_count_max), (_load_factor), (_config))

#define HASHMAP_NEW_OPS_1(_ops, _bucket_count_init) \
        __hashmap_new((_ops), (_bucket_count_init), 0, 0.0f, NULL)
#define HASHMAP_NEW_OPS_2(_ops, _bucket_count_init, _bucket_count_max) \
        __hashmap_new((_ops), (_bucket_count_init), (_bucket_count_max), 0.0f, NULL)
#define HASHMAP_NEW_OPS_3(_ops, _bucket_count_init, _bucket_count_max, _load_factor) \
        __hashmap_new((_ops), (_bucket_count_init), (_bucket_count_max), (_load_factor), NULL)
#define HASHMAP_NEW_OPS_4(_ops, _bucket_count_init, _bucket_count_max, _load_factor, _config) \
        __hashmap_new((_ops), (_bucket_count_init), (_bucket_count_max), (_load_factor), (_config))

#endif /* __J_HASH_MAP_H */
