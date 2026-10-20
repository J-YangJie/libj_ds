/*
  Sequence Containers Common Operations Defines
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
#include <sequence_container/operations/ds_ops_sso.h>

#ifndef BIT
#define BIT(n) (1UL << (n))
#endif /* BIT */

#define SEQ_CNTR_DTYPE_U8                BIT(2)
#define SEQ_CNTR_DTYPE_U16               BIT(3)
#define SEQ_CNTR_DTYPE_U32               BIT(4)
#define SEQ_CNTR_DTYPE_S8                BIT(6)
#define SEQ_CNTR_DTYPE_S16               BIT(7)
#define SEQ_CNTR_DTYPE_S32               BIT(8)
#ifdef JDSC_64BIT
#define SEQ_CNTR_DTYPE_U64               BIT(5)
#define SEQ_CNTR_DTYPE_S64               BIT(9)
#endif /* JDSC_64BIT */
#define SEQ_CNTR_DTYPE_UINTPTR           BIT(10)
#define SEQ_CNTR_DTYPE_INTPTR            BIT(11)
#define SEQ_CNTR_DTYPE_STRUCT            BIT(12)
#define SEQ_CNTR_DTYPE_UNION             BIT(13)
#define SEQ_CNTR_DTYPE_PTR               SEQ_CNTR_DTYPE_INTPTR
#define SEQ_CNTR_DTYPE_SSO               SEQ_CNTR_DTYPE_STRUCT

#ifdef JDSC_64BIT
#define SEQ_CNTR_DTYPE_MASK_U64          SEQ_CNTR_DTYPE_U64
#define SEQ_CNTR_DTYPE_MASK_S64          SEQ_CNTR_DTYPE_S64
#else
#define SEQ_CNTR_DTYPE_MASK_U64          0
#define SEQ_CNTR_DTYPE_MASK_S64          0
#endif /* JDSC_64BIT */

#define SEQ_CNTR_DTYPE_MASK_UINT         (SEQ_CNTR_DTYPE_U8  | SEQ_CNTR_DTYPE_U16 | SEQ_CNTR_DTYPE_U32 | SEQ_CNTR_DTYPE_UINTPTR | SEQ_CNTR_DTYPE_MASK_U64)
#define SEQ_CNTR_DTYPE_MASK_SINT         (SEQ_CNTR_DTYPE_S8  | SEQ_CNTR_DTYPE_S16 | SEQ_CNTR_DTYPE_S32 | SEQ_CNTR_DTYPE_INTPTR | SEQ_CNTR_DTYPE_MASK_S64)
#define SEQ_CNTR_DTYPE_MASK_NUM          (SEQ_CNTR_DTYPE_MASK_UINT | SEQ_CNTR_DTYPE_MASK_SINT)
#define SEQ_CNTR_DTYPE_MASK_STUN         (SEQ_CNTR_DTYPE_STRUCT | SEQ_CNTR_DTYPE_UNION)
#define SEQ_CNTR_DTYPE_MASK_ALL          (SEQ_CNTR_DTYPE_MASK_NUM | SEQ_CNTR_DTYPE_MASK_STUN)

#define __SEQUENCE_CONTAINER_DTYPE_uint8_t                SEQ_CNTR_DTYPE_U8
#define __SEQUENCE_CONTAINER_DTYPE_uint16_t               SEQ_CNTR_DTYPE_U16
#define __SEQUENCE_CONTAINER_DTYPE_uint32_t               SEQ_CNTR_DTYPE_U32
#ifdef JDSC_64BIT
#define __SEQUENCE_CONTAINER_DTYPE_uint64_t               SEQ_CNTR_DTYPE_U64
#endif /* JDSC_64BIT */
#define __SEQUENCE_CONTAINER_DTYPE_int8_t                 SEQ_CNTR_DTYPE_S8
#define __SEQUENCE_CONTAINER_DTYPE_int16_t                SEQ_CNTR_DTYPE_S16
#define __SEQUENCE_CONTAINER_DTYPE_int32_t                SEQ_CNTR_DTYPE_S32
#ifdef JDSC_64BIT
#define __SEQUENCE_CONTAINER_DTYPE_int64_t                SEQ_CNTR_DTYPE_S64
#endif /* JDSC_64BIT */
#define __SEQUENCE_CONTAINER_DTYPE_uintptr_t              SEQ_CNTR_DTYPE_UINTPTR
#define __SEQUENCE_CONTAINER_DTYPE_intptr_t               SEQ_CNTR_DTYPE_INTPTR
#define __SEQUENCE_CONTAINER_DTYPE_ds_data_t              SEQ_CNTR_DTYPE_PTR
#define __SEQUENCE_CONTAINER_DTYPE_deque_data_t           __SEQUENCE_CONTAINER_DTYPE_ds_data_t
#define __SEQUENCE_CONTAINER_DTYPE_vector_data_t          __SEQUENCE_CONTAINER_DTYPE_ds_data_t
#define __SEQUENCE_CONTAINER_DTYPE_list_data_t            __SEQUENCE_CONTAINER_DTYPE_ds_data_t
#define __SEQUENCE_CONTAINER_DTYPE_stack_data_t           __SEQUENCE_CONTAINER_DTYPE_ds_data_t
#define __SEQUENCE_CONTAINER_DTYPE_queue_data_t           __SEQUENCE_CONTAINER_DTYPE_ds_data_t
#define __SEQUENCE_CONTAINER_DTYPE_priority_queue_data_t  __SEQUENCE_CONTAINER_DTYPE_ds_data_t
#define __SEQUENCE_CONTAINER_DTYPE_ds_sso_t               SEQ_CNTR_DTYPE_SSO

static inline
bool __sequence_container_dtype_valid(ds_dtype_t dtype)
{
    return 0 != dtype && 0 == (dtype & (dtype - 1)) && 0 == (dtype & ~(ds_dtype_t)SEQ_CNTR_DTYPE_MASK_ALL);
}

static inline
ds_step_t __sequence_container_dtype_width(ds_dtype_t dtype)
{
    switch (dtype) {
    case SEQ_CNTR_DTYPE_U8:  case SEQ_CNTR_DTYPE_S8:  return 1;
    case SEQ_CNTR_DTYPE_U16: case SEQ_CNTR_DTYPE_S16: return 2;
    case SEQ_CNTR_DTYPE_U32: case SEQ_CNTR_DTYPE_S32: return 4;
#ifdef JDSC_64BIT
    case SEQ_CNTR_DTYPE_U64: case SEQ_CNTR_DTYPE_S64: return 8;
#endif /* JDSC_64BIT */
    case SEQ_CNTR_DTYPE_UINTPTR:                      return sizeof(uintptr_t);
    case SEQ_CNTR_DTYPE_INTPTR:                       return sizeof(intptr_t);
    default:                                          return 0; /* struct/union: step carries the size */
    }
}

static inline
bool __sequence_container_dtype_swap_compatible(ds_dtype_t a, ds_dtype_t b)
{
    if (a == b)
        return true;

    if ((a & SEQ_CNTR_DTYPE_MASK_STUN) || (b & SEQ_CNTR_DTYPE_MASK_STUN))
        return false;

    {
        const size_t wa = __sequence_container_dtype_width(a);
        const size_t wb = __sequence_container_dtype_width(b);
        const bool   ua = 0 != (a & SEQ_CNTR_DTYPE_MASK_UINT);
        const bool   ub = 0 != (b & SEQ_CNTR_DTYPE_MASK_UINT);

        return 0 != wa && wa == wb && ua == ub;
    }
}

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

const class_sequence_container_ops_t* class_sequence_container_ops_string_ins(void);
#define g_class_sequence_container_ops_string()  class_sequence_container_ops_string_ins()

const class_sequence_container_ops_t* class_sequence_container_ops_sso_ins(void);
#define g_class_sequence_container_ops_sso()     class_sequence_container_ops_sso_ins()

#endif /* __J_SEQUENCE_CONTAINER_H */
