/*
  Default Implementations Of Sequence Containers Custom Operation Interfaces
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

#include <sequence_container/sequence_container_ops.h>

#include <_token.h>

/* Floating-point equality is numeric, so 0.0 and -0.0 compare equal.
 * The container default comparison is bitwise, hence this callback.
 * U64/S64 need no callback: the step-8 comparison is exact on both widths. */

static bool __ds_ops_eq_seq_float(ds_data_t left, ds_data_t right)  { return __ds_token_to_f32(left) == __ds_token_to_f32(right); }
static bool __ds_ops_eq_seq_double(ds_data_t left, ds_data_t right) { return __ds_token_to_f64(left) == __ds_token_to_f64(right); }

#define __CLASS_SEQUENCE_CONTAINER_OPS(_t, __func_eq) \
const class_sequence_container_ops_t* class_sequence_container_ops_##_t##_ins(void) \
{ \
    static const class_sequence_container_ops_t ins = { \
        .valid_data = NULL,         \
        .__eq       = __func_eq,    \
        .copy_data  = NULL,         \
        .free_data  = NULL,         \
        .__move     = NULL,         \
    }; \
    return &ins; \
}

__CLASS_SEQUENCE_CONTAINER_OPS(float,  __ds_ops_eq_seq_float)
__CLASS_SEQUENCE_CONTAINER_OPS(double, __ds_ops_eq_seq_double)





#include <operations/ds_ops_string.h>

const class_sequence_container_ops_t* class_sequence_container_ops_chars_ins(void)
{
    static const class_sequence_container_ops_t ins = {
        .valid_data = ds_ops_valid_data_default_string,
        .__eq       = __ds_ops_eq_default_string,
        .copy_data  = ds_ops_copy_data_default_string,
        .free_data  = ds_ops_free_data_default_string,
        .__move     = NULL,
    };
    return &ins;
}

const class_sequence_container_ops_t* class_sequence_container_ops_sso_ins(void)
{
    static const class_sequence_container_ops_t ins = {
        .valid_data = ds_ops_valid_data_default_sso,
        .__eq       = __ds_ops_eq_diff_sso,
        .copy_data  = ds_ops_copy_data_diff_sso,
        .free_data  = ds_ops_free_data_default_sso,
        .__move     = NULL,
    };
    return &ins;
}
