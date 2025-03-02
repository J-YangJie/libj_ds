/*
  Hashmap Custom Operation Interfaces
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

#ifndef __J_HASH_MAP_OPS_H
#define __J_HASH_MAP_OPS_H

#include <_types.h>

typedef struct class_hashmap_ops {
    hashmap_hash_t (*__hash)(hashmap_key_t key);                  /* Caculate and return a hash value based on `key` */
    bool (*valid_key)(hashmap_key_t key);                         /* Return true if `key` is valid */
    bool (*__lt)(hashmap_key_t left, hashmap_key_t right);        /* Return true if [ `left` < `right` ] */
    bool (*copy_key)(hashmap_key_t in, hashmap_key_t* out);       /* The function pointer can be null and manages memory on its own. However, if this function is implemented, `free_key` must also be implemented */
    void (*free_key)(hashmap_key_t* key);                         /* The function pointer can be null and manages memory on its own */
    bool (*valid_value)(hashmap_value_t value);                   /* Return true if `valid` is valid */
    bool (*copy_value)(hashmap_value_t in, hashmap_value_t* out); /* The function pointer can be null and manages memory on its own. However, if this function is implemented, `free_value` must also be implemented */
    void (*free_value)(hashmap_value_t* value);                   /* The function pointer can be null and manages memory on its own */
} class_hashmap_ops_t;

#endif /* __J_HASH_MAP_OPS_H */
