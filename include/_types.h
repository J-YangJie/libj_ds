/*
  Types of Data Structures
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

#ifndef __J_TYPES_H
#define __J_TYPES_H

#include <unistd.h>
#include <stdint.h>
#include <stdbool.h>

/* common */
typedef uintptr_t  ds_uintptr_t;
typedef intptr_t   ds_data_t;
typedef intptr_t   ds_hash_t;
typedef ds_data_t  ds_key_t;
typedef ds_data_t  ds_value_t;
typedef ssize_t    ds_size_t;
typedef ssize_t    ds_count_t;
typedef bool (*remove_if_condition)(ds_data_t data);
typedef bool (*remove_if_condition_k)(ds_key_t key);
typedef bool (*remove_if_condition_v)(ds_value_t value);
typedef bool (*remove_if_condition_kv)(ds_key_t key, ds_value_t value);
typedef bool (*__comp)(ds_data_t left, ds_data_t right);

/* list */
typedef ds_data_t  list_data_t;
typedef ds_size_t  list_size_t;
typedef ds_count_t list_count_t;

/* vector */
typedef ds_data_t  vector_data_t;
typedef ds_size_t  vector_size_t;
typedef ds_count_t vector_count_t;

/* map */
typedef ds_key_t   map_key_t;
typedef ds_value_t map_value_t;
typedef ds_size_t  map_size_t;
typedef ds_count_t map_count_t;

/* multimap */
typedef ds_key_t   multimap_key_t;
typedef ds_value_t multimap_value_t;
typedef ds_size_t  multimap_size_t;
typedef ds_count_t multimap_count_t;

/* set */
typedef ds_value_t set_value_t;
typedef ds_size_t  set_size_t;
typedef ds_count_t set_count_t;

/* multiset */
typedef ds_value_t multiset_value_t;
typedef ds_size_t  multiset_size_t;
typedef ds_count_t multiset_count_t;

/* bucket */
typedef ds_hash_t  bucket_hash_t;
typedef ds_key_t   bucket_key_t;
typedef ds_value_t bucket_value_t;
typedef ds_size_t  bucket_size_t;
typedef ds_count_t bucket_count_t;

/* hashmap */
typedef ds_hash_t  hashmap_hash_t;
typedef ds_key_t   hashmap_key_t;
typedef ds_value_t hashmap_value_t;
typedef ds_size_t  hashmap_size_t;
typedef ds_count_t hashmap_count_t;
typedef ds_count_t hashmap_bcount_t;

/* priority_queue */
typedef ds_data_t  priority_queue_data_t;
typedef ds_size_t  priority_queue_size_t;
typedef ds_count_t priority_queue_count_t;

#endif /* __J_TYPES_H */
