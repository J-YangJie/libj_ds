/*
  Iterator macro
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

#ifndef __J_ITERATOR_H
#define __J_ITERATOR_H

#include <_token.h>

#define it_hash(it_obj)            ((it_obj).d->hash)
#define it_key(it_obj)             ((it_obj).d->key)
#define it_skey(it_obj)            ((it_obj).d->skey)
#define it_value(it_obj)           ((it_obj).d->value)
#define it_svalue(it_obj)          ((it_obj).d->svalue)
#define it_data(it_obj)            (*(it_obj).d)
#define it_sdata(it_obj)           (*(it_obj).sd)

#define it_u8key(it_obj)           __ds_token_to_u8(it_key(it_obj))
#define it_u16key(it_obj)          __ds_token_to_u16(it_key(it_obj))
#define it_u32key(it_obj)          __ds_token_to_u32(it_key(it_obj))
#define it_u64key(it_obj)          __ds_token_to_u64(it_key(it_obj))
#define it_s8key(it_obj)           __ds_token_to_s8(it_key(it_obj))
#define it_s16key(it_obj)          __ds_token_to_s16(it_key(it_obj))
#define it_s32key(it_obj)          __ds_token_to_s32(it_key(it_obj))
#define it_s64key(it_obj)          __ds_token_to_s64(it_key(it_obj))
#define it_uptrkey(it_obj)         __ds_token_to_uintptr(it_key(it_obj))
#define it_iptrkey(it_obj)         __ds_token_to_intptr(it_key(it_obj))
#define it_sizekey(it_obj)         __ds_token_to_size(it_key(it_obj))
#define it_ssizekey(it_obj)        __ds_token_to_ssize(it_key(it_obj))
#define it_fkey(it_obj)            __ds_token_to_f32(it_key(it_obj))
#define it_lfkey(it_obj)           __ds_token_to_f64(it_key(it_obj))

#define it_u8value(it_obj)         __ds_token_to_u8(it_value(it_obj))
#define it_u16value(it_obj)        __ds_token_to_u16(it_value(it_obj))
#define it_u32value(it_obj)        __ds_token_to_u32(it_value(it_obj))
#define it_u64value(it_obj)        __ds_token_to_u64(it_value(it_obj))
#define it_s8value(it_obj)         __ds_token_to_s8(it_value(it_obj))
#define it_s16value(it_obj)        __ds_token_to_s16(it_value(it_obj))
#define it_s32value(it_obj)        __ds_token_to_s32(it_value(it_obj))
#define it_s64value(it_obj)        __ds_token_to_s64(it_value(it_obj))
#define it_uptrvalue(it_obj)       __ds_token_to_uintptr(it_value(it_obj))
#define it_iptrvalue(it_obj)       __ds_token_to_intptr(it_value(it_obj))
#define it_sizevalue(it_obj)       __ds_token_to_size(it_value(it_obj))
#define it_ssizevalue(it_obj)      __ds_token_to_ssize(it_value(it_obj))
#define it_fvalue(it_obj)          __ds_token_to_f32(it_value(it_obj))
#define it_lfvalue(it_obj)         __ds_token_to_f64(it_value(it_obj))

#define it_u8data(it_obj)          (*(const uint8_t*)(it_obj).d)
#define it_u16data(it_obj)         (*(const uint16_t*)(it_obj).d)
#define it_u32data(it_obj)         (*(const uint32_t*)(it_obj).d)
#define it_u64data(it_obj)         (*(const uint64_t*)(it_obj).d)
#define it_s8data(it_obj)          (*(const int8_t*)(it_obj).d)
#define it_s16data(it_obj)         (*(const int16_t*)(it_obj).d)
#define it_s32data(it_obj)         (*(const int32_t*)(it_obj).d)
#define it_s64data(it_obj)         (*(const int64_t*)(it_obj).d)
#define it_uptrdata(it_obj)        (*(const uintptr_t*)(it_obj).d)
#define it_iptrdata(it_obj)        (*(const intptr_t*)(it_obj).d)
#define it_sizedata(it_obj)        (*(const size_t*)(it_obj).d)
#define it_ssizedata(it_obj)       (*(const ssize_t*)(it_obj).d)
#define it_fdata(it_obj)           (*(const float*)(it_obj).d)
#define it_lfdata(it_obj)          (*(const double*)(it_obj).d)

#define it_hash_safe(it_obj)       ((it_obj).d                      ? it_hash(it_obj)   : 0)
#define it_key_safe(it_obj)        ((it_obj).d                      ? it_key(it_obj)    : 0)
#define it_skey_safe(it_obj)       ((it_obj).d && it_skey(it_obj)   ? it_skey(it_obj)   : "null")
#define it_value_safe(it_obj)      ((it_obj).d                      ? it_value(it_obj)  : 0)
#define it_svalue_safe(it_obj)     ((it_obj).d && it_svalue(it_obj) ? it_svalue(it_obj) : "null")
#define it_data_safe(it_obj)       ((it_obj).d                      ? it_data(it_obj)   : 0)
#define it_sdata_safe(it_obj)      ((it_obj).d && it_sdata(it_obj)  ? it_sdata(it_obj)  : "null")

#define it_u8key_safe(it_obj)      ((it_obj).d ? it_u8key(it_obj)      : 0)
#define it_u16key_safe(it_obj)     ((it_obj).d ? it_u16key(it_obj)     : 0)
#define it_u32key_safe(it_obj)     ((it_obj).d ? it_u32key(it_obj)     : 0)
#define it_u64key_safe(it_obj)     ((it_obj).d ? it_u64key(it_obj)     : 0)
#define it_s8key_safe(it_obj)      ((it_obj).d ? it_s8key(it_obj)      : 0)
#define it_s16key_safe(it_obj)     ((it_obj).d ? it_s16key(it_obj)     : 0)
#define it_s32key_safe(it_obj)     ((it_obj).d ? it_s32key(it_obj)     : 0)
#define it_s64key_safe(it_obj)     ((it_obj).d ? it_s64key(it_obj)     : 0)
#define it_uptrkey_safe(it_obj)    ((it_obj).d ? it_uptrkey(it_obj)    : 0)
#define it_iptrkey_safe(it_obj)    ((it_obj).d ? it_iptrkey(it_obj)    : 0)
#define it_sizekey_safe(it_obj)    ((it_obj).d ? it_sizekey(it_obj)    : 0)
#define it_ssizekey_safe(it_obj)   ((it_obj).d ? it_ssizekey(it_obj)   : 0)
#define it_fkey_safe(it_obj)       ((it_obj).d ? it_fkey(it_obj)       : 0.0f)
#define it_lfkey_safe(it_obj)      ((it_obj).d ? it_lfkey(it_obj)      : 0.0)

#define it_u8value_safe(it_obj)    ((it_obj).d ? it_u8value(it_obj)    : 0)
#define it_u16value_safe(it_obj)   ((it_obj).d ? it_u16value(it_obj)   : 0)
#define it_u32value_safe(it_obj)   ((it_obj).d ? it_u32value(it_obj)   : 0)
#define it_u64value_safe(it_obj)   ((it_obj).d ? it_u64value(it_obj)   : 0)
#define it_s8value_safe(it_obj)    ((it_obj).d ? it_s8value(it_obj)    : 0)
#define it_s16value_safe(it_obj)   ((it_obj).d ? it_s16value(it_obj)   : 0)
#define it_s32value_safe(it_obj)   ((it_obj).d ? it_s32value(it_obj)   : 0)
#define it_s64value_safe(it_obj)   ((it_obj).d ? it_s64value(it_obj)   : 0)
#define it_uptrvalue_safe(it_obj)  ((it_obj).d ? it_uptrvalue(it_obj)  : 0)
#define it_iptrvalue_safe(it_obj)  ((it_obj).d ? it_iptrvalue(it_obj)  : 0)
#define it_sizevalue_safe(it_obj)  ((it_obj).d ? it_sizevalue(it_obj)  : 0)
#define it_ssizevalue_safe(it_obj) ((it_obj).d ? it_ssizevalue(it_obj) : 0)
#define it_fvalue_safe(it_obj)     ((it_obj).d ? it_fvalue(it_obj)     : 0.0f)
#define it_lfvalue_safe(it_obj)    ((it_obj).d ? it_lfvalue(it_obj)    : 0.0)

#define it_u8data_safe(it_obj)     ((it_obj).d ? it_u8data(it_obj)     : 0)
#define it_u16data_safe(it_obj)    ((it_obj).d ? it_u16data(it_obj)    : 0)
#define it_u32data_safe(it_obj)    ((it_obj).d ? it_u32data(it_obj)    : 0)
#define it_u64data_safe(it_obj)    ((it_obj).d ? it_u64data(it_obj)    : 0)
#define it_s8data_safe(it_obj)     ((it_obj).d ? it_s8data(it_obj)     : 0)
#define it_s16data_safe(it_obj)    ((it_obj).d ? it_s16data(it_obj)    : 0)
#define it_s32data_safe(it_obj)    ((it_obj).d ? it_s32data(it_obj)    : 0)
#define it_s64data_safe(it_obj)    ((it_obj).d ? it_s64data(it_obj)    : 0)
#define it_uptrdata_safe(it_obj)   ((it_obj).d ? it_uptrdata(it_obj)   : 0)
#define it_iptrdata_safe(it_obj)   ((it_obj).d ? it_iptrdata(it_obj)   : 0)
#define it_sizedata_safe(it_obj)   ((it_obj).d ? it_sizedata(it_obj)   : 0)
#define it_ssizedata_safe(it_obj)  ((it_obj).d ? it_ssizedata(it_obj)  : 0)
#define it_fdata_safe(it_obj)      ((it_obj).d ? it_fdata(it_obj)      : 0.0f)
#define it_lfdata_safe(it_obj)     ((it_obj).d ? it_lfdata(it_obj)     : 0.0)


HEADER_STATIC_INLINE
bool __it_eq(void* _d1, void* _d2)
{
    return _d1 == _d2;
}

HEADER_STATIC_INLINE
bool __it_ne(void* _d1, void* _d2)
{
    return _d1 != _d2;
}

#define it_eq(_it1, _it2)  (__it_eq((_it1).d, (_it2).d))
#define it_ne(_it1, _it2)  (__it_ne((_it1).d, (_it2).d))


#endif /* __J_ITERATOR_H */
