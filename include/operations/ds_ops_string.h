/*
  Data Structures Default Operations
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

#ifndef __J_DS_OPS_STRING_H
#define __J_DS_OPS_STRING_H

#include <_types.h>

/* hash */
ds_hash_t __ds_ops_hash_default_string(ds_key_t key);               /* String type: don't judge the validity of `key`(otherwise there will be ambiguity), 
                                                                                    caculate the hash value of `key` */

/* key */
bool ds_ops_valid_key_default_string_max_2(ds_key_t key);           /* String type: judge whether the `key` of string type is valid, with a length limit of 2 bytes */
bool ds_ops_valid_key_default_string_max_4(ds_key_t key);           /* String type: judge whether the `key` of string type is valid, with a length limit of 4 bytes */
bool ds_ops_valid_key_default_string_max_8(ds_key_t key);           /* String type: judge whether the `key` of string type is valid, with a length limit of 8 bytes */
bool ds_ops_valid_key_default_string_max_16(ds_key_t key);          /* String type: judge whether the `key` of string type is valid, with a length limit of 16 bytes */
bool ds_ops_valid_key_default_string_max_32(ds_key_t key);          /* String type: judge whether the `key` of string type is valid, with a length limit of 32 bytes */
bool ds_ops_valid_key_default_string_max_64(ds_key_t key);          /* String type: judge whether the `key` of string type is valid, with a length limit of 64 bytes */
bool ds_ops_valid_key_default_string_max_128(ds_key_t key);         /* String type: judge whether the `key` of string type is valid, with a length limit of 128 bytes */
bool ds_ops_valid_key_default_string_max_256(ds_key_t key);         /* String type: judge whether the `key` of string type is valid, with a length limit of 256 bytes */
bool ds_ops_valid_key_default_string_max_512(ds_key_t key);         /* String type: judge whether the `key` of string type is valid, with a length limit of 512 bytes */

/* value or data */
bool ds_ops_valid_data_default_string(ds_data_t data);              /* String type: judge the validity of the `data`(whether the pointer is null and whether the length is greater than 0), 
                                                                                    without limiting the length */
bool __ds_ops_lt_default_string(ds_data_t left, ds_data_t right);   /* String type: return true if [ `left` < `right` ] */
bool __ds_ops_gt_default_string(ds_data_t left, ds_data_t right);   /* String type: return true if [ `left` > `right` ] */
bool ds_ops_copy_data_default_string(ds_data_t in, ds_data_t* out); /* String type: deep copy `in` and use `out` to receive the copied memory */
void ds_ops_free_data_default_string(ds_data_t* data);              /* String type: release the `data` and set `data` to `NULL` */

/* value or data */
bool __ds_ops_gt_default(ds_data_t left, ds_data_t right);          /* Numeric type: return true if [ `left` > `right` ] */

#endif /* __J_DS_OPS_STRING_H */
