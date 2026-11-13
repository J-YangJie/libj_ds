/*
  Default Implementations Of Associative Containers Custom Operation Interfaces
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

#include <associative_container/associative_container_ops.h>

#include <_token.h>
#include <_memory.h>
#include <linux/_compiler.h>

#ifndef JDSC_64BIT

static bool __ds_ops_lt_default_int64_t(ds_key_t left, ds_key_t right)   { return __ds_token_to_s64(left)     < __ds_token_to_s64(right); }

static
bool ds_ops_copy_default_box8(ds_key_t in, ds_key_t* out)
{
    uint64_t* box;

    /* is_null(in/out) */

    box = (uint64_t*)p_malloc(8);
    if (unlikely(is_null(box)))
        return false;

    *box = *(const uint64_t*)(uintptr_t)in;
    *out = (ds_key_t)(uintptr_t)box;
    return true;
}

static
void ds_ops_free_default_box8(ds_key_t* key)
{
    p_free(*key);
}

#endif /* JDSC_64BIT */

static bool __ds_ops_lt_default_uint8_t(ds_key_t left, ds_key_t right)   { return __ds_token_to_u8(left)      < __ds_token_to_u8(right);  }
static bool __ds_ops_lt_default_uint16_t(ds_key_t left, ds_key_t right)  { return __ds_token_to_u16(left)     < __ds_token_to_u16(right); }
static bool __ds_ops_lt_default_uint32_t(ds_key_t left, ds_key_t right)  { return __ds_token_to_u32(left)     < __ds_token_to_u32(right); }
static bool __ds_ops_lt_default_uint64_t(ds_key_t left, ds_key_t right)  { return __ds_token_to_u64(left)     < __ds_token_to_u64(right); }
static bool __ds_ops_lt_default_uintptr_t(ds_key_t left, ds_key_t right) { return __ds_token_to_uintptr(left) < __ds_token_to_uintptr(right); }
static bool __ds_ops_lt_default_size_t(ds_key_t left, ds_key_t right)    { return __ds_token_to_size(left)    < __ds_token_to_size(right);    }
static bool __ds_ops_lt_default_float(ds_key_t left, ds_key_t right)     { return __ds_token_to_f32(left)     < __ds_token_to_f32(right); }
static bool __ds_ops_lt_default_double(ds_key_t left, ds_key_t right)    { return __ds_token_to_f64(left)     < __ds_token_to_f64(right); }

#define __CLASS_ASSOCIATIVE_CONTAINER_KEY_OPS(_t, _lt, _copy, _free) \
const class_associative_container_key_ops_t* class_associative_container_key_ops_##_t##_ins(void) \
{ \
    static const class_associative_container_key_ops_t ins = { \
        .valid_key = NULL,   \
        .__lt      = _lt,    \
        .copy_key  = _copy,  \
        .free_key  = _free,  \
    }; \
    return &ins; \
}

#define __CLASS_ASSOCIATIVE_CONTAINER_VALUE_OPS(_t, _copy, _free) \
const class_associative_container_value_ops_t* class_associative_container_value_ops_##_t##_ins(void) \
{ \
    static const class_associative_container_value_ops_t ins = { \
        .valid_value = NULL,   \
        .copy_value  = _copy,  \
        .free_value  = _free,  \
    }; \
    return &ins; \
}

#define __CLASS_ASSOCIATIVE_CONTAINER_KV_OPS(_t, _lt, _kc, _kf, _vc, _vf) \
    __CLASS_ASSOCIATIVE_CONTAINER_KEY_OPS(_t, _lt, _kc, _kf) \
    __CLASS_ASSOCIATIVE_CONTAINER_VALUE_OPS(_t, _vc, _vf)

__CLASS_ASSOCIATIVE_CONTAINER_KV_OPS(uint8_t,   __ds_ops_lt_default_uint8_t,   NULL, NULL, NULL, NULL)
__CLASS_ASSOCIATIVE_CONTAINER_KV_OPS(uint16_t,  __ds_ops_lt_default_uint16_t,  NULL, NULL, NULL, NULL)
__CLASS_ASSOCIATIVE_CONTAINER_KV_OPS(uint32_t,  __ds_ops_lt_default_uint32_t,  NULL, NULL, NULL, NULL)
__CLASS_ASSOCIATIVE_CONTAINER_KV_OPS(int8_t,    NULL,                          NULL, NULL, NULL, NULL)
__CLASS_ASSOCIATIVE_CONTAINER_KV_OPS(int16_t,   NULL,                          NULL, NULL, NULL, NULL)
__CLASS_ASSOCIATIVE_CONTAINER_KV_OPS(int32_t,   NULL,                          NULL, NULL, NULL, NULL)
__CLASS_ASSOCIATIVE_CONTAINER_KV_OPS(uintptr_t, __ds_ops_lt_default_uintptr_t, NULL, NULL, NULL, NULL)
__CLASS_ASSOCIATIVE_CONTAINER_KV_OPS(intptr_t,  NULL,                          NULL, NULL, NULL, NULL)
__CLASS_ASSOCIATIVE_CONTAINER_KV_OPS(size_t,    __ds_ops_lt_default_size_t,    NULL, NULL, NULL, NULL)
__CLASS_ASSOCIATIVE_CONTAINER_KV_OPS(ssize_t,   NULL,                          NULL, NULL, NULL, NULL)
__CLASS_ASSOCIATIVE_CONTAINER_KV_OPS(float,     __ds_ops_lt_default_float,     NULL, NULL, NULL, NULL)

#ifdef JDSC_64BIT
__CLASS_ASSOCIATIVE_CONTAINER_KV_OPS(uint64_t,  __ds_ops_lt_default_uint64_t,  NULL, NULL, NULL, NULL)
__CLASS_ASSOCIATIVE_CONTAINER_KV_OPS(int64_t,   NULL,                          NULL, NULL, NULL, NULL)
__CLASS_ASSOCIATIVE_CONTAINER_KV_OPS(double,    __ds_ops_lt_default_double,    NULL, NULL, NULL, NULL)
#else
__CLASS_ASSOCIATIVE_CONTAINER_KV_OPS(uint64_t,  __ds_ops_lt_default_uint64_t,  ds_ops_copy_default_box8, ds_ops_free_default_box8, 
                                                                               ds_ops_copy_default_box8, ds_ops_free_default_box8)
__CLASS_ASSOCIATIVE_CONTAINER_KV_OPS(int64_t,   __ds_ops_lt_default_int64_t,   ds_ops_copy_default_box8, ds_ops_free_default_box8, 
                                                                               ds_ops_copy_default_box8, ds_ops_free_default_box8)
__CLASS_ASSOCIATIVE_CONTAINER_KV_OPS(double,    __ds_ops_lt_default_double,    ds_ops_copy_default_box8, ds_ops_free_default_box8, 
                                                                               ds_ops_copy_default_box8, ds_ops_free_default_box8)
#endif /* JDSC_64BIT */





/* Greater */

#define __DS_OPS_DEFINE_GT(_t, _tok) \
static bool __ds_ops_gt_default_##_t(ds_key_t left, ds_key_t right) \
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

#define __CLASS_ASSOCIATIVE_CONTAINER_KEY_OPS_GREATER(_t, _lt, _copy, _free) \
const class_associative_container_key_ops_t* class_associative_container_key_ops_##_t##_greater_ins(void) \
{ \
    static const class_associative_container_key_ops_t ins = { \
        .valid_key = NULL,   \
        .__lt      = _lt,    \
        .copy_key  = _copy,  \
        .free_key  = _free,  \
    }; \
    return &ins; \
}

__CLASS_ASSOCIATIVE_CONTAINER_KEY_OPS_GREATER(uint8_t,   __ds_ops_gt_default_uint8_t,   NULL, NULL)
__CLASS_ASSOCIATIVE_CONTAINER_KEY_OPS_GREATER(uint16_t,  __ds_ops_gt_default_uint16_t,  NULL, NULL)
__CLASS_ASSOCIATIVE_CONTAINER_KEY_OPS_GREATER(uint32_t,  __ds_ops_gt_default_uint32_t,  NULL, NULL)
__CLASS_ASSOCIATIVE_CONTAINER_KEY_OPS_GREATER(int8_t,    __ds_ops_gt_default_int8_t,    NULL, NULL)
__CLASS_ASSOCIATIVE_CONTAINER_KEY_OPS_GREATER(int16_t,   __ds_ops_gt_default_int16_t,   NULL, NULL)
__CLASS_ASSOCIATIVE_CONTAINER_KEY_OPS_GREATER(int32_t,   __ds_ops_gt_default_int32_t,   NULL, NULL)
__CLASS_ASSOCIATIVE_CONTAINER_KEY_OPS_GREATER(uintptr_t, __ds_ops_gt_default_uintptr_t, NULL, NULL)
__CLASS_ASSOCIATIVE_CONTAINER_KEY_OPS_GREATER(intptr_t,  __ds_ops_gt_default_intptr_t,  NULL, NULL)
__CLASS_ASSOCIATIVE_CONTAINER_KEY_OPS_GREATER(size_t,    __ds_ops_gt_default_size_t,    NULL, NULL)
__CLASS_ASSOCIATIVE_CONTAINER_KEY_OPS_GREATER(ssize_t,   __ds_ops_gt_default_ssize_t,   NULL, NULL)
__CLASS_ASSOCIATIVE_CONTAINER_KEY_OPS_GREATER(float,     __ds_ops_gt_default_float,     NULL, NULL)

#ifdef JDSC_64BIT
__CLASS_ASSOCIATIVE_CONTAINER_KEY_OPS_GREATER(uint64_t,  __ds_ops_gt_default_uint64_t,  NULL, NULL)
__CLASS_ASSOCIATIVE_CONTAINER_KEY_OPS_GREATER(int64_t,   __ds_ops_gt_default_int64_t,   NULL, NULL)
__CLASS_ASSOCIATIVE_CONTAINER_KEY_OPS_GREATER(double,    __ds_ops_gt_default_double,    NULL, NULL)
#else
__CLASS_ASSOCIATIVE_CONTAINER_KEY_OPS_GREATER(uint64_t,  __ds_ops_gt_default_uint64_t,  ds_ops_copy_default_box8, ds_ops_free_default_box8)
__CLASS_ASSOCIATIVE_CONTAINER_KEY_OPS_GREATER(int64_t,   __ds_ops_gt_default_int64_t,   ds_ops_copy_default_box8, ds_ops_free_default_box8)
__CLASS_ASSOCIATIVE_CONTAINER_KEY_OPS_GREATER(double,    __ds_ops_gt_default_double,    ds_ops_copy_default_box8, ds_ops_free_default_box8)
#endif /* JDSC_64BIT */





/* String */

#include <operations/ds_ops_string.h>

const class_associative_container_key_ops_t* class_associative_container_key_ops_chars_ins(void)
{
    static const class_associative_container_key_ops_t ins = {
        .valid_key = ds_ops_valid_data_default_string,
        .__lt      = __ds_ops_lt_default_string,
        .copy_key  = ds_ops_copy_data_default_string,
        .free_key  = ds_ops_free_data_default_string,
    };
    return &ins;
}

const class_associative_container_key_ops_t* class_associative_container_key_ops_chars_greater_ins(void)
{
    static const class_associative_container_key_ops_t ins = {
        .valid_key = ds_ops_valid_data_default_string,
        .__lt      = __ds_ops_gt_default_string,
        .copy_key  = ds_ops_copy_data_default_string,
        .free_key  = ds_ops_free_data_default_string,
    };
    return &ins;
}

const class_associative_container_value_ops_t* class_associative_container_value_ops_chars_ins(void)
{
    static const class_associative_container_value_ops_t ins = {
        .valid_value = ds_ops_valid_data_default_string,
        .copy_value  = ds_ops_copy_data_default_string,
        .free_value  = ds_ops_free_data_default_string,
    };
    return &ins;
}
