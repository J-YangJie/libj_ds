/*
  List Custom Operation Interfaces
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

#ifndef __J_LIST_OPS_H
#define __J_LIST_OPS_H

#include <_types.h>

typedef struct class_list_ops {
    bool (*valid_data)(list_data_t data);                /* Return true if `data` is valid */
    bool (*__lt)(list_data_t left, list_data_t right);   /* Return true if [ `left` != `right` ], or if [ `left` < `right` ] */
    bool (*copy_data)(list_data_t in, list_data_t* out); /* The function pointer can be null and manages memory on its own. However, if this function is implemented, `free_data` must also be implemented */
    void (*free_data)(list_data_t* data);                /* The function pointer can be null and manages memory on its own */
} class_list_ops_t;

const class_list_ops_t* class_list_ops_string_ins(void);
#define g_class_list_ops_string()  class_list_ops_string_ins()

#endif /* __J_LIST_OPS_H */
