/*
  Sequence Containers Operations Defines
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

#ifndef __J_SEQUENCE_CONTAINER_OPS_H
#define __J_SEQUENCE_CONTAINER_OPS_H

#include <_types.h>
#include <_token.h>
#include <sequence_container/operations/ds_ops_sso.h>

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
    void (*__move)(ds_data_t dest, ds_data_t src);   /* Move semantics, default to using `memmove` if not provided. */
} class_sequence_container_ops_t;

#define __CLASS_SEQUENCE_CONTAINER_OPS_DECL(_t) \
    const class_sequence_container_ops_t* class_sequence_container_ops_##_t##_ins(void);

__CLASS_SEQUENCE_CONTAINER_OPS_DECL(float)
__CLASS_SEQUENCE_CONTAINER_OPS_DECL(double)
__CLASS_SEQUENCE_CONTAINER_OPS_DECL(chars)
__CLASS_SEQUENCE_CONTAINER_OPS_DECL(sso)

#undef __CLASS_SEQUENCE_CONTAINER_OPS_DECL

#define g_class_sequence_container_ops_uint8_t()    NULL
#define g_class_sequence_container_ops_uint16_t()   NULL
#define g_class_sequence_container_ops_uint32_t()   NULL
#define g_class_sequence_container_ops_uint64_t()   NULL
#define g_class_sequence_container_ops_int8_t()     NULL
#define g_class_sequence_container_ops_int16_t()    NULL
#define g_class_sequence_container_ops_int32_t()    NULL
#define g_class_sequence_container_ops_int64_t()    NULL
#define g_class_sequence_container_ops_uintptr_t()  NULL
#define g_class_sequence_container_ops_intptr_t()   NULL
#define g_class_sequence_container_ops_size_t()     NULL
#define g_class_sequence_container_ops_ssize_t()    NULL
#define g_class_sequence_container_ops_float()      class_sequence_container_ops_float_ins()
#define g_class_sequence_container_ops_double()     class_sequence_container_ops_double_ins()
#define g_class_sequence_container_ops_chars()      class_sequence_container_ops_chars_ins()
#define g_class_sequence_container_ops_sso()        class_sequence_container_ops_sso_ins()
#define g_class_sequence_container_ops_string()     g_class_sequence_container_ops_sso()
#define g_class_sequence_container_ops_ds_sso_t()   g_class_sequence_container_ops_sso()

#ifndef __DS_DATA_CTORS_DEFINED
#define __DS_DATA_CTORS_DEFINED
#define ds_data(v)       __ds_data(v)
#define ds_sdata(v)      __ds_sdata(v)

#define ds_u8data(v)     __ds_u8data(v)
#define ds_u16data(v)    __ds_u16data(v)
#define ds_u32data(v)    __ds_u32data(v)
#define ds_u64data(v)    __ds_u64data(v)
#define ds_s8data(v)     __ds_s8data(v)
#define ds_s16data(v)    __ds_s16data(v)
#define ds_s32data(v)    __ds_s32data(v)
#define ds_s64data(v)    __ds_s64data(v)
#define ds_uptrdata(v)   __ds_uptrdata(v)
#define ds_iptrdata(v)   __ds_iptrdata(v)
#define ds_sizedata(v)   __ds_sizedata(v)
#define ds_ssizedata(v)  __ds_ssizedata(v)
#define ds_fdata(v)      __ds_fdata(v)
#define ds_lfdata(v)     __ds_lfdata(v)
#endif /* __DS_DATA_CTORS_DEFINED */


#endif /* __J_SEQUENCE_CONTAINER_OPS_H */
