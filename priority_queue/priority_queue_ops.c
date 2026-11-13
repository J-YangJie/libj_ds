/*
  Default Implementations Of Priority Queue Custom Operation Interfaces
  Copyright (C) 2021  YangJie <yangjie98765@yeah.net>
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

#include <priority_queue/priority_queue_ops.h>

#include <_token.h>

#define __DS_OPS_DEFINE_GT(_t, _tok) \
static bool __ds_ops_gt_default_##_t(priority_queue_data_t left, priority_queue_data_t right) \
{ \
    return __ds_token_to_##_tok(left) > __ds_token_to_##_tok(right); \
}

__DS_OPS_DEFINE_GT(uint8_t,   u8)
__DS_OPS_DEFINE_GT(uint16_t,  u16)
__DS_OPS_DEFINE_GT(uint32_t,  u32)
__DS_OPS_DEFINE_GT(uint64_t,  u64)
__DS_OPS_DEFINE_GT(int8_t,    s8)
__DS_OPS_DEFINE_GT(int16_t,   s16)
__DS_OPS_DEFINE_GT(int32_t,   s32)
__DS_OPS_DEFINE_GT(int64_t,   s64)
__DS_OPS_DEFINE_GT(uintptr_t, uintptr)
__DS_OPS_DEFINE_GT(intptr_t,  intptr)
__DS_OPS_DEFINE_GT(size_t,    size)
__DS_OPS_DEFINE_GT(ssize_t,   ssize)
__DS_OPS_DEFINE_GT(float,     f32)
__DS_OPS_DEFINE_GT(double,    f64)

#undef __DS_OPS_DEFINE_GT

#define __CLASS_PRIORITY_QUEUE_OPS_GREATER(_t) \
const class_priority_queue_ops_t* class_priority_queue_ops_##_t##_greater_ins(void) \
{ \
    static const class_priority_queue_ops_t ins = { \
        .valid_data = NULL, \
        .__lt       = __ds_ops_gt_default_##_t, \
        .copy_data  = NULL, \
        .free_data  = NULL, \
        .__move     = NULL, \
    }; \
    return &ins; \
}

__CLASS_PRIORITY_QUEUE_OPS_GREATER(uint8_t)
__CLASS_PRIORITY_QUEUE_OPS_GREATER(uint16_t)
__CLASS_PRIORITY_QUEUE_OPS_GREATER(uint32_t)
__CLASS_PRIORITY_QUEUE_OPS_GREATER(uint64_t)
__CLASS_PRIORITY_QUEUE_OPS_GREATER(int8_t)
__CLASS_PRIORITY_QUEUE_OPS_GREATER(int16_t)
__CLASS_PRIORITY_QUEUE_OPS_GREATER(int32_t)
__CLASS_PRIORITY_QUEUE_OPS_GREATER(int64_t)
__CLASS_PRIORITY_QUEUE_OPS_GREATER(uintptr_t)
__CLASS_PRIORITY_QUEUE_OPS_GREATER(intptr_t)
__CLASS_PRIORITY_QUEUE_OPS_GREATER(size_t)
__CLASS_PRIORITY_QUEUE_OPS_GREATER(ssize_t)
__CLASS_PRIORITY_QUEUE_OPS_GREATER(float)
__CLASS_PRIORITY_QUEUE_OPS_GREATER(double)





#include <operations/ds_ops_string.h>

const class_priority_queue_ops_t* class_priority_queue_ops_chars_ins(void)
{
    static const class_priority_queue_ops_t ins = {
        .valid_data = ds_ops_valid_data_default_string,
        .__lt       = __ds_ops_lt_default_string,
        .copy_data  = ds_ops_copy_data_default_string,
        .free_data  = ds_ops_free_data_default_string,
        .__move     = NULL,
    };
    return &ins;
}

const class_priority_queue_ops_t* class_priority_queue_ops_sso_ins(void)
{
    static const class_priority_queue_ops_t ins = {
        .valid_data = ds_ops_valid_data_default_sso,
        .__lt       = __ds_ops_lt_same_sso,
        .copy_data  = ds_ops_copy_data_diff_sso,
        .free_data  = ds_ops_free_data_default_sso,
        .__move     = NULL,
    };
    return &ins;
}

const class_priority_queue_ops_t* class_priority_queue_ops_chars_greater_ins(void)
{
    static const class_priority_queue_ops_t ins = {
        .valid_data = ds_ops_valid_data_default_string,
        .__lt       = __ds_ops_gt_default_string,
        .copy_data  = ds_ops_copy_data_default_string,
        .free_data  = ds_ops_free_data_default_string,
        .__move     = NULL,
    };
    return &ins;
}

const class_priority_queue_ops_t* class_priority_queue_ops_sso_greater_ins(void)
{
    static const class_priority_queue_ops_t ins = {
        .valid_data = ds_ops_valid_data_default_sso,
        .__lt       = __ds_ops_gt_same_sso,
        .copy_data  = ds_ops_copy_data_diff_sso,
        .free_data  = ds_ops_free_data_default_sso,
        .__move     = NULL,
    };
    return &ins;
}
