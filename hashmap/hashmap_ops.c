/*
  Default Implementations Of Hashmap Custom Operation Interfaces
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

#include <hashmap/hashmap_ops.h>

#include <_token.h>
#include <_memory.h>
#include <linux/_compiler.h>

#ifndef JDSC_64BIT

static inline uint64_t __ds_key_box8_load(ds_key_t key)               { return *(const uint64_t*)(uintptr_t)key; }
static ds_hash_t __ds_ops_hash_default_box8(ds_key_t key)             { return (ds_hash_t)__ds_key_box8_load(key); }
static bool __ds_ops_eq_default_box8(ds_key_t left, ds_key_t right)   { return __ds_key_box8_load(left) == __ds_key_box8_load(right); }
static bool __ds_ops_lt_default_box8(ds_key_t left, ds_key_t right)   { return (int64_t)__ds_key_box8_load(left) < (int64_t)__ds_key_box8_load(right); }
static void __ds_ops_free_key_default_box8(ds_key_t* key)             { p_free(*key); }

static
bool __ds_ops_copy_key_default_box8(ds_key_t in, ds_key_t* out)
{
    uint64_t* box;

    /* is_null(in/out) */

    box = (uint64_t*)p_malloc(8);
    if (unlikely(is_null(box)))
        return false;

    *box = __ds_key_box8_load(in);
    *out = (ds_key_t)(uintptr_t)box;
    return true;
}

#endif /* JDSC_64BIT */

static
ds_hash_t __ds_ops_hash_default_float(ds_key_t key)
{
    uint32_t u = __ds_token_to_u32(key);

    if (unlikely(0x80000000U == u))
        u = 0;
    return (ds_hash_t)u;
}

static
ds_hash_t __ds_ops_hash_default_double(ds_key_t key)
{
    union {
        double   f;
        uint64_t u;
    } v;

    v.f = __ds_token_to_f64(key);
    if (unlikely(0x8000000000000000ULL == v.u))
        v.u = 0;
    return (ds_hash_t)v.u;
}

/* Floating-point keys use numeric equality: `+0.0` and `-0.0` are the same key.
 * The hash normalizes `-0.0` so that equal keys hash equally.
 * NaN is not equal to anything, so it cannot be used as a key, as in the STL. */

static bool __ds_ops_eq_default_float(ds_key_t left, ds_key_t right)  { return __ds_token_to_f32(left) == __ds_token_to_f32(right); }
static bool __ds_ops_lt_default_float(ds_key_t left, ds_key_t right)  { return __ds_token_to_f32(left) <  __ds_token_to_f32(right); }
static bool __ds_ops_eq_default_double(ds_key_t left, ds_key_t right) { return __ds_token_to_f64(left) == __ds_token_to_f64(right); }
static bool __ds_ops_lt_default_double(ds_key_t left, ds_key_t right) { return __ds_token_to_f64(left) <  __ds_token_to_f64(right); }

#define __CLASS_HASHMAP_KEY_OPS(_t, _hash, _eq, _lt, _copy, _free) \
const class_hashmap_key_ops_t* class_hashmap_key_ops_##_t##_ins(void) \
{ \
    static const class_hashmap_key_ops_t ins = { \
        .__hash    = _hash, \
        .valid_key = NULL, \
        .__lt      = _lt, \
        .__eq      = _eq, \
        .copy_key  = _copy, \
        .free_key  = _free, \
    }; \
    return &ins; \
}

__CLASS_HASHMAP_KEY_OPS(uint8_t,   NULL, NULL, NULL, NULL, NULL)
__CLASS_HASHMAP_KEY_OPS(uint16_t,  NULL, NULL, NULL, NULL, NULL)
__CLASS_HASHMAP_KEY_OPS(uint32_t,  NULL, NULL, NULL, NULL, NULL)
__CLASS_HASHMAP_KEY_OPS(int8_t,    NULL, NULL, NULL, NULL, NULL)
__CLASS_HASHMAP_KEY_OPS(int16_t,   NULL, NULL, NULL, NULL, NULL)
__CLASS_HASHMAP_KEY_OPS(int32_t,   NULL, NULL, NULL, NULL, NULL)
__CLASS_HASHMAP_KEY_OPS(uintptr_t, NULL, NULL, NULL, NULL, NULL)
__CLASS_HASHMAP_KEY_OPS(intptr_t,  NULL, NULL, NULL, NULL, NULL)
__CLASS_HASHMAP_KEY_OPS(size_t,    NULL, NULL, NULL, NULL, NULL)
__CLASS_HASHMAP_KEY_OPS(ssize_t,   NULL, NULL, NULL, NULL, NULL)
__CLASS_HASHMAP_KEY_OPS(float,     __ds_ops_hash_default_float,    __ds_ops_eq_default_float,  __ds_ops_lt_default_float, 
                                   NULL, NULL)

#ifdef JDSC_64BIT
__CLASS_HASHMAP_KEY_OPS(uint64_t,  NULL, NULL, NULL, NULL, NULL)
__CLASS_HASHMAP_KEY_OPS(int64_t,   NULL, NULL, NULL, NULL, NULL)
__CLASS_HASHMAP_KEY_OPS(double,    __ds_ops_hash_default_double,   __ds_ops_eq_default_double, __ds_ops_lt_default_double, 
                                   NULL, NULL)
#else
__CLASS_HASHMAP_KEY_OPS(uint64_t,  __ds_ops_hash_default_box8,     __ds_ops_eq_default_box8,   __ds_ops_lt_default_box8, 
                                   __ds_ops_copy_key_default_box8, __ds_ops_free_key_default_box8)
__CLASS_HASHMAP_KEY_OPS(int64_t,   __ds_ops_hash_default_box8,     __ds_ops_eq_default_box8,   __ds_ops_lt_default_box8, 
                                   __ds_ops_copy_key_default_box8, __ds_ops_free_key_default_box8)
__CLASS_HASHMAP_KEY_OPS(double,    __ds_ops_hash_default_double,   __ds_ops_eq_default_double, __ds_ops_lt_default_double, 
                                   __ds_ops_copy_key_default_box8, __ds_ops_free_key_default_box8)
#endif /* JDSC_64BIT */





#include <operations/ds_ops_string.h>

const class_hashmap_key_ops_t* class_hashmap_key_ops_chars_ins(void)
{
    static const class_hashmap_key_ops_t ins = {
        .__hash    = __ds_ops_hash_default_string,
        .valid_key = ds_ops_valid_data_default_string,
        .__lt      = __ds_ops_lt_default_string,
        .__eq      = __ds_ops_eq_default_string,
        .copy_key  = ds_ops_copy_data_default_string,
        .free_key  = ds_ops_free_data_default_string,
    };
    return &ins;
}
