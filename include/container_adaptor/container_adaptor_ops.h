/*
  Container Adaptors Common Operations Defines
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

#ifndef __J_CONTAINER_ADAPTOR_OPS_H
#define __J_CONTAINER_ADAPTOR_OPS_H

#include <_token.h>
#include <sequence_container/sequence_container.h>
#include <sequence_container/operations/ds_ops_sso.h>

typedef sequence_container_value_t     container_adaptor_value_t;
typedef class_sequence_container_ops_t class_container_adaptor_ops_t;

#define g_class_container_adaptor_ops_chars()  g_class_sequence_container_ops_chars()
#define g_class_container_adaptor_ops_sso()    g_class_sequence_container_ops_sso()

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

HEADER_STATIC_INLINE
bool __container_adaptor_dtype_swap_compatible(ds_dtype_t left, ds_dtype_t right)
{
    return __sequence_container_dtype_swap_compatible(left, right);
}

#endif /* __J_CONTAINER_ADAPTOR_OPS_H */
