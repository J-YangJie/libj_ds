/*
  Sequence Containers Common Operations Defines
  Copyright (C) 2026  YangJie <yangjie98765@yeah.net>

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

#ifndef __J_SEQUENCE_CONTAINER_H
#define __J_SEQUENCE_CONTAINER_H

#include <_types.h>
#include <sequence_container/operations/ds_ops_sso.h>

#define __SEQUENCE_CONTAINER_BASE_VECTOR 1
#define __SEQUENCE_CONTAINER_BASE_DEQUE  2
#define __SEQUENCE_CONTAINER_BASE_LIST   3

typedef struct sequence_container_value {
    union {
        ds_data_t* d;
        char**     sd;
        ds_sso_t*  ssd;
        uint8_t*   u8;
        uint16_t*  u16;
        uint32_t*  u32;
        int8_t*    s8;
        int16_t*   s16;
        int32_t*   s32;
    };
} sequence_container_value_t;

typedef struct class_sequence_container_ops {
    bool (*valid_data)(ds_data_t data);              /* Return true if `data` is valid */
    bool (*__eq)(ds_data_t left, ds_data_t right);   /* Return true if [ `left` == `right` ] */
    bool (*copy_data)(ds_data_t in, ds_data_t* out); /* The function pointer can be null and manages memory on its own. However, if this function is implemented, `free_data` must also be implemented */
    void (*free_data)(ds_data_t* data);              /* The function pointer can be null and manages memory on its own */
} class_sequence_container_ops_t;

const class_sequence_container_ops_t* class_sequence_container_ops_string_ins(void);
#define g_class_sequence_container_ops_string()  class_sequence_container_ops_string_ins()

const class_sequence_container_ops_t* class_sequence_container_ops_sso_ins(void);
#define g_class_sequence_container_ops_sso()     class_sequence_container_ops_sso_ins()

#endif /* __J_SEQUENCE_CONTAINER_H */
