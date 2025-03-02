/*
  Default Implementations Of Vector Custom Operation Interfaces
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

#include <vector/vector_ops.h>

#include <operations/ds_ops_string.h>

__always_inline const class_vector_ops_t* class_vector_ops_string_ins(void)
{
    static const class_vector_ops_t ins = {
        .valid_data = ds_ops_valid_data_default_string,
        .__lt       = __ds_ops_lt_default_string,
        .copy_data  = ds_ops_copy_data_default_string,
        .free_data  = ds_ops_free_data_default_string,
    };
    return &ins;
}
