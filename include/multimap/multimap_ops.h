/*
  Multimap Custom Operation Interfaces
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

#ifndef __J_MULTIMAP_OPS_H
#define __J_MULTIMAP_OPS_H

#include <_types.h>

typedef struct class_multimap_ops {
    bool (*valid_key)(multimap_key_t key);                             /* Return true if `key` is valid */
    bool (*__lt)(multimap_key_t left, multimap_key_t right);           /* Return true if [ `left` < `right` ] */
    bool (*copy_key)(multimap_key_t in, multimap_key_t* out);          /* The function pointer can be null and manages memory on its own. However, if this function is implemented, `free_key` must also be implemented */
    void (*free_key)(multimap_key_t* key);                             /* The function pointer can be null and manages memory on its own */
    bool (*valid_value)(multimap_value_t value);                       /* Return true if `value` is valid */
    bool (*__lt_value)(multimap_value_t left, multimap_value_t right); /* Return true if [ `left` != `right` ], or if [ `left` < `right` ] */
    bool (*copy_value)(multimap_value_t in, multimap_value_t* out);    /* The function pointer can be null and manages memory on its own. However, if this function is implemented, `free_value` must also be implemented */
    void (*free_value)(multimap_value_t* value);                       /* The function pointer can be null and manages memory on its own */
} class_multimap_ops_t;

#endif /* __J_MULTIMAP_OPS_H */
