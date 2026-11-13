/*
  Token conversion between C types and `ds_data_t`
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

#ifndef __J_TOKEN_H
#define __J_TOKEN_H

#include <stdint.h>
#include <string.h>
#include <_types.h>
#include <_compiler.h>

HEADER_STATIC_INLINE ds_data_t __ds_token_from_u8(uint8_t value)        { return (ds_data_t)(uintptr_t)value; }
HEADER_STATIC_INLINE ds_data_t __ds_token_from_u16(uint16_t value)      { return (ds_data_t)(uintptr_t)value; }
HEADER_STATIC_INLINE ds_data_t __ds_token_from_u32(uint32_t value)      { return (ds_data_t)(uintptr_t)value; }
HEADER_STATIC_INLINE ds_data_t __ds_token_from_s8(int8_t value)         { return (ds_data_t)(intptr_t)value; }
HEADER_STATIC_INLINE ds_data_t __ds_token_from_s16(int16_t value)       { return (ds_data_t)(intptr_t)value; }
HEADER_STATIC_INLINE ds_data_t __ds_token_from_s32(int32_t value)       { return (ds_data_t)(intptr_t)value; }
HEADER_STATIC_INLINE ds_data_t __ds_token_from_str(const char* value)   { return (ds_data_t)(uintptr_t)value; }
HEADER_STATIC_INLINE ds_data_t __ds_token_from_uintptr(uintptr_t value) { return (ds_data_t)(uintptr_t)value; }
HEADER_STATIC_INLINE ds_data_t __ds_token_from_intptr(intptr_t value)   { return (ds_data_t)(intptr_t)value; }
HEADER_STATIC_INLINE ds_data_t __ds_token_from_size(size_t value)       { return (ds_data_t)(uintptr_t)value; }
HEADER_STATIC_INLINE ds_data_t __ds_token_from_ssize(ssize_t value)     { return (ds_data_t)(intptr_t)value; }

HEADER_STATIC_INLINE
ds_data_t __ds_token_from_f32(float value)
{
    union {
        float    f;
        uint32_t u;
    } bits;

    bits.f = value;
    return (ds_data_t)(uintptr_t)bits.u;
}

#ifdef JDSC_64BIT
HEADER_STATIC_INLINE ds_data_t __ds_token_from_u64(uint64_t value)  { return (ds_data_t)value; }
HEADER_STATIC_INLINE ds_data_t __ds_token_from_s64(int64_t value)   { return (ds_data_t)value; }

HEADER_STATIC_INLINE
ds_data_t __ds_token_from_f64(double value)
{
    union {
        double    d;
        ds_data_t t;
    } token;

    token.d = value;
    return token.t;
}
#else
#define __ds_token_from_u64(v)  ((ds_data_t)(uintptr_t)&(uint64_t){(v)})
#define __ds_token_from_s64(v)  ((ds_data_t)(uintptr_t)&(int64_t){(v)})
#define __ds_token_from_f64(v)  ((ds_data_t)(uintptr_t)&(double){(v)})
#endif /* JDSC_64BIT */


HEADER_STATIC_INLINE uint8_t     __ds_token_to_u8(ds_data_t token)      { return (uint8_t)(uintptr_t)token; }
HEADER_STATIC_INLINE uint16_t    __ds_token_to_u16(ds_data_t token)     { return (uint16_t)(uintptr_t)token; }
HEADER_STATIC_INLINE uint32_t    __ds_token_to_u32(ds_data_t token)     { return (uint32_t)(uintptr_t)token; }
HEADER_STATIC_INLINE int8_t      __ds_token_to_s8(ds_data_t token)      { return (int8_t)(intptr_t)token; }
HEADER_STATIC_INLINE int16_t     __ds_token_to_s16(ds_data_t token)     { return (int16_t)(intptr_t)token; }
HEADER_STATIC_INLINE int32_t     __ds_token_to_s32(ds_data_t token)     { return (int32_t)(intptr_t)token; }
HEADER_STATIC_INLINE const char* __ds_token_to_str(ds_data_t token)     { return (const char*)(uintptr_t)token; }
HEADER_STATIC_INLINE uintptr_t   __ds_token_to_uintptr(ds_data_t token) { return (uintptr_t)token; }
HEADER_STATIC_INLINE intptr_t    __ds_token_to_intptr(ds_data_t token)  { return (intptr_t)token; }
HEADER_STATIC_INLINE size_t      __ds_token_to_size(ds_data_t token)    { return (size_t)token; }
HEADER_STATIC_INLINE ssize_t     __ds_token_to_ssize(ds_data_t token)   { return (ssize_t)token; }

HEADER_STATIC_INLINE
float __ds_token_to_f32(ds_data_t token)
{
    union {
        float    f;
        uint32_t u;
    } bits;

    bits.u = (uint32_t)(uintptr_t)token;
    return bits.f;
}

HEADER_STATIC_INLINE
uint64_t __ds_token_to_u64(ds_data_t token)
{
#ifdef JDSC_64BIT
    return (uint64_t)token;
#else
    return *(const uint64_t*)(uintptr_t)token;
#endif /* JDSC_64BIT */
}

HEADER_STATIC_INLINE
int64_t __ds_token_to_s64(ds_data_t token)
{
#ifdef JDSC_64BIT
    return (int64_t)token;
#else
    return *(const int64_t*)(uintptr_t)token;
#endif /* JDSC_64BIT */
}

HEADER_STATIC_INLINE
double __ds_token_to_f64(ds_data_t token)
{
#ifdef JDSC_64BIT
    union {
        double    d;
        ds_data_t t;
    } value;

    value.t = token;
    return value.d;
#else
    return *(const double*)(uintptr_t)token;
#endif /* JDSC_64BIT */
}


/* Sequence containers */
HEADER_STATIC_INLINE
void __ds_token_store_8(void* slot, ds_data_t token)
{
#ifdef JDSC_64BIT
    *(uint64_t*)slot = (uint64_t)token;
#else
    *(uint64_t*)slot = *(const uint64_t*)(uintptr_t)token;
#endif /* JDSC_64BIT */
}

HEADER_STATIC_INLINE
ds_data_t __ds_token_load_8(const void* slot)
{
#ifdef JDSC_64BIT
    return *(const ds_data_t*)slot;
#else
    return (ds_data_t)(uintptr_t)slot;
#endif /* JDSC_64BIT */
}

HEADER_STATIC_INLINE
bool __ds_token_equal_8(ds_data_t token, const void* slot)
{
#ifdef JDSC_64BIT
    return (uint64_t)token == *(const uint64_t*)slot;
#else
    return *(const uint64_t*)(uintptr_t)token == *(const uint64_t*)slot;
#endif /* JDSC_64BIT */
}


/* Sequence element constructors. */
#define __ds_data(v)        __ds_token_from_intptr(v)
#define __ds_sdata(v)       __ds_token_from_str(v)

#define __ds_u8data(v)      __ds_token_from_u8(v)
#define __ds_u16data(v)     __ds_token_from_u16(v)
#define __ds_u32data(v)     __ds_token_from_u32(v)
#define __ds_u64data(v)     __ds_token_from_u64(v)
#define __ds_s8data(v)      __ds_token_from_s8(v)
#define __ds_s16data(v)     __ds_token_from_s16(v)
#define __ds_s32data(v)     __ds_token_from_s32(v)
#define __ds_s64data(v)     __ds_token_from_s64(v)
#define __ds_uptrdata(v)    __ds_token_from_uintptr(v)
#define __ds_iptrdata(v)    __ds_token_from_intptr(v)
#define __ds_sizedata(v)    __ds_token_from_size(v)
#define __ds_ssizedata(v)   __ds_token_from_ssize(v)
#define __ds_fdata(v)       __ds_token_from_f32(v)
#define __ds_lfdata(v)      __ds_token_from_f64(v)

/* Associative key constructors. */
#define __ds_key(v)         __ds_token_from_intptr(v)
#define __ds_skey(v)        __ds_token_from_str(v)

#define __ds_u8key(v)       __ds_token_from_u8(v)
#define __ds_u16key(v)      __ds_token_from_u16(v)
#define __ds_u32key(v)      __ds_token_from_u32(v)
#define __ds_u64key(v)      __ds_token_from_u64(v)
#define __ds_s8key(v)       __ds_token_from_s8(v)
#define __ds_s16key(v)      __ds_token_from_s16(v)
#define __ds_s32key(v)      __ds_token_from_s32(v)
#define __ds_s64key(v)      __ds_token_from_s64(v)
#define __ds_uptrkey(v)     __ds_token_from_uintptr(v)
#define __ds_iptrkey(v)     __ds_token_from_intptr(v)
#define __ds_sizekey(v)     __ds_token_from_size(v)
#define __ds_ssizekey(v)    __ds_token_from_ssize(v)
#define __ds_fkey(v)        __ds_token_from_f32(v)
#define __ds_lfkey(v)       __ds_token_from_f64(v)

/* Associative value constructors. */
#define __ds_value(v)       __ds_token_from_intptr(v)
#define __ds_svalue(v)      __ds_token_from_str(v)

#define __ds_u8value(v)     __ds_token_from_u8(v)
#define __ds_u16value(v)    __ds_token_from_u16(v)
#define __ds_u32value(v)    __ds_token_from_u32(v)
#define __ds_u64value(v)    __ds_token_from_u64(v)
#define __ds_s8value(v)     __ds_token_from_s8(v)
#define __ds_s16value(v)    __ds_token_from_s16(v)
#define __ds_s32value(v)    __ds_token_from_s32(v)
#define __ds_s64value(v)    __ds_token_from_s64(v)
#define __ds_uptrvalue(v)   __ds_token_from_uintptr(v)
#define __ds_iptrvalue(v)   __ds_token_from_intptr(v)
#define __ds_sizevalue(v)   __ds_token_from_size(v)
#define __ds_ssizevalue(v)  __ds_token_from_ssize(v)
#define __ds_fvalue(v)      __ds_token_from_f32(v)
#define __ds_lfvalue(v)     __ds_token_from_f64(v)

#endif /* __J_TOKEN_H */
