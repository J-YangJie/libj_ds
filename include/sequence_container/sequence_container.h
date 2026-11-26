/*
  Sequence Containers Common Defines
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

#ifndef __J_SEQUENCE_CONTAINER_H
#define __J_SEQUENCE_CONTAINER_H

#include <_types.h>
#include <_compiler.h>
#include <sequence_container/sequence_container_ops.h>

#ifndef BIT
#define BIT(n) (1UL << (n))
#endif /* BIT */

typedef enum sequence_container_data_type {
    SEQ_CNTR_DTYPE_STRUCT  = BIT(2),
    SEQ_CNTR_DTYPE_UNION   = BIT(3),
    SEQ_CNTR_DTYPE_U8      = BIT(4),
    SEQ_CNTR_DTYPE_U16     = BIT(5),
    SEQ_CNTR_DTYPE_U32     = BIT(6),
    SEQ_CNTR_DTYPE_S8      = BIT(7),
    SEQ_CNTR_DTYPE_S16     = BIT(8),
    SEQ_CNTR_DTYPE_S32     = BIT(9),
    SEQ_CNTR_DTYPE_F32     = BIT(10),
    SEQ_CNTR_DTYPE_U64     = BIT(11),
    SEQ_CNTR_DTYPE_S64     = BIT(12),
    SEQ_CNTR_DTYPE_F64     = BIT(13),
    SEQ_CNTR_DTYPE_UINTPTR = BIT(14),
    SEQ_CNTR_DTYPE_INTPTR  = BIT(15),
    SEQ_CNTR_DTYPE_F       = SEQ_CNTR_DTYPE_F32,
    SEQ_CNTR_DTYPE_LF      = SEQ_CNTR_DTYPE_F64,
    SEQ_CNTR_DTYPE_PTR     = SEQ_CNTR_DTYPE_INTPTR,
    SEQ_CNTR_DTYPE_SSO     = SEQ_CNTR_DTYPE_STRUCT,
} sequence_container_data_type_t;

#define SEQ_CNTR_DTYPE_MASK_SINT   (SEQ_CNTR_DTYPE_S8  | SEQ_CNTR_DTYPE_S16 | SEQ_CNTR_DTYPE_S32 | SEQ_CNTR_DTYPE_S64 | SEQ_CNTR_DTYPE_INTPTR)
#define SEQ_CNTR_DTYPE_MASK_UINT   (SEQ_CNTR_DTYPE_U8  | SEQ_CNTR_DTYPE_U16 | SEQ_CNTR_DTYPE_U32 | SEQ_CNTR_DTYPE_U64 | SEQ_CNTR_DTYPE_UINTPTR)
#define SEQ_CNTR_DTYPE_MASK_INT    (SEQ_CNTR_DTYPE_MASK_SINT | SEQ_CNTR_DTYPE_MASK_UINT)
#define SEQ_CNTR_DTYPE_MASK_FLOAT  (SEQ_CNTR_DTYPE_F32 | SEQ_CNTR_DTYPE_F64)
#define SEQ_CNTR_DTYPE_MASK_NUM    (SEQ_CNTR_DTYPE_MASK_INT | SEQ_CNTR_DTYPE_MASK_FLOAT)

#define SEQ_CNTR_DTYPE_MASK_W1     (SEQ_CNTR_DTYPE_S8  | SEQ_CNTR_DTYPE_U8)
#define SEQ_CNTR_DTYPE_MASK_W2     (SEQ_CNTR_DTYPE_S16 | SEQ_CNTR_DTYPE_U16)
#define SEQ_CNTR_DTYPE_MASK_W4     (SEQ_CNTR_DTYPE_S32 | SEQ_CNTR_DTYPE_U32 | SEQ_CNTR_DTYPE_F32)
#define SEQ_CNTR_DTYPE_MASK_W8     (SEQ_CNTR_DTYPE_S64 | SEQ_CNTR_DTYPE_U64 | SEQ_CNTR_DTYPE_F64)
#define SEQ_CNTR_DTYPE_MASK_PTR    (SEQ_CNTR_DTYPE_UINTPTR | SEQ_CNTR_DTYPE_INTPTR)

#define SEQ_CNTR_DTYPE_MASK_STUN   (SEQ_CNTR_DTYPE_STRUCT | SEQ_CNTR_DTYPE_UNION)
#define SEQ_CNTR_DTYPE_MASK_OPS    (SEQ_CNTR_DTYPE_MASK_STUN)
#define SEQ_CNTR_DTYPE_MASK_ALL    (SEQ_CNTR_DTYPE_MASK_NUM | SEQ_CNTR_DTYPE_MASK_OPS)

#ifdef JDSC_64BIT
#define SEQ_CNTR_DTYPE_MASK_TOKEN_ADDR  (SEQ_CNTR_DTYPE_MASK_OPS)
#else
#define SEQ_CNTR_DTYPE_MASK_TOKEN_ADDR  (SEQ_CNTR_DTYPE_MASK_OPS | SEQ_CNTR_DTYPE_MASK_W8)
#endif /* JDSC_64BIT */

#define __SEQUENCE_CONTAINER_DTYPE_uint8_t                SEQ_CNTR_DTYPE_U8
#define __SEQUENCE_CONTAINER_DTYPE_uint16_t               SEQ_CNTR_DTYPE_U16
#define __SEQUENCE_CONTAINER_DTYPE_uint32_t               SEQ_CNTR_DTYPE_U32
#define __SEQUENCE_CONTAINER_DTYPE_int8_t                 SEQ_CNTR_DTYPE_S8
#define __SEQUENCE_CONTAINER_DTYPE_int16_t                SEQ_CNTR_DTYPE_S16
#define __SEQUENCE_CONTAINER_DTYPE_int32_t                SEQ_CNTR_DTYPE_S32
#define __SEQUENCE_CONTAINER_DTYPE_float                  SEQ_CNTR_DTYPE_F
#define __SEQUENCE_CONTAINER_DTYPE_uint64_t               SEQ_CNTR_DTYPE_U64
#define __SEQUENCE_CONTAINER_DTYPE_int64_t                SEQ_CNTR_DTYPE_S64
#define __SEQUENCE_CONTAINER_DTYPE_double                 SEQ_CNTR_DTYPE_LF
#define __SEQUENCE_CONTAINER_DTYPE_uintptr_t              SEQ_CNTR_DTYPE_UINTPTR
#define __SEQUENCE_CONTAINER_DTYPE_intptr_t               SEQ_CNTR_DTYPE_INTPTR
#define __SEQUENCE_CONTAINER_DTYPE_size_t                 SEQ_CNTR_DTYPE_UINTPTR
#define __SEQUENCE_CONTAINER_DTYPE_ssize_t                SEQ_CNTR_DTYPE_INTPTR
#define __SEQUENCE_CONTAINER_DTYPE_ds_sso_t               SEQ_CNTR_DTYPE_SSO

HEADER_STATIC_INLINE
bool __sequence_container_dtype_valid(ds_dtype_t dtype)
{
    return 0 != dtype && 0 == (dtype & (dtype - 1)) && 0 == (dtype & ~(ds_dtype_t)SEQ_CNTR_DTYPE_MASK_ALL);
}

HEADER_STATIC_INLINE
ds_step_t __sequence_container_dtype_width(ds_dtype_t dtype)
{
    if (dtype & SEQ_CNTR_DTYPE_MASK_W1) return 1;
    if (dtype & SEQ_CNTR_DTYPE_MASK_W2) return 2;
    if (dtype & SEQ_CNTR_DTYPE_MASK_W4) return 4;
    if (dtype & SEQ_CNTR_DTYPE_MASK_W8) return 8;
    if (dtype & SEQ_CNTR_DTYPE_MASK_PTR) return sizeof(uintptr_t);
    return 0; /* struct/union: step carries the size */
}

HEADER_STATIC_INLINE
bool __sequence_container_dtype_swap_compatible(ds_dtype_t a, ds_dtype_t b)
{
    const ds_step_t wa = __sequence_container_dtype_width(a);
    const ds_step_t wb = __sequence_container_dtype_width(b);

    if (a == b)
        return true;

    return 0 != wa && wa == wb
        && (a & SEQ_CNTR_DTYPE_MASK_TOKEN_ADDR) == (b & SEQ_CNTR_DTYPE_MASK_TOKEN_ADDR)
        && (0 != (a & SEQ_CNTR_DTYPE_MASK_SINT)) == (0 != (b & SEQ_CNTR_DTYPE_MASK_SINT));
}

#endif /* __J_SEQUENCE_CONTAINER_H */
