/*
  About compiler
  Copyright (C) 2021  YangJie <yangjie98765@yeah.net>

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

#ifndef __J_COMPILER_H
#define __J_COMPILER_H

#if defined(_MSC_VER)
#   define JDSC_INLINE         __inline
#   define JDSC_INLINE_FORCE   __forceinline
#elif defined(__GNUC__) || defined(__clang__)
#   define JDSC_INLINE         inline
#   define JDSC_INLINE_FORCE   __inline __attribute__((__always_inline__))
#else
#   define JDSC_INLINE         inline
#   define JDSC_INLINE_FORCE   inline
#endif /* _MSC_VER / __GNUC__ / __clang__ */

#define JDSC_INLINE_POLICY        JDSC_INLINE
#define JDSC_INLINE_FORCE_POLICY  JDSC_INLINE

#ifndef __always_inline
#   define __always_inline        JDSC_INLINE_FORCE
#endif /* __always_inline */


#define JDSC_NO_ITERATOR
#define JDSC_ONLY_WRAPPER


#define JDSC_ITERATOR_ERR_NULL  0


#if defined(JDSC_MEMPOOL)
#define JDSC_MEMFUNC_PREFIX __attribute__((weak))
#else
#define JDSC_MEMFUNC_PREFIX static JDSC_INLINE_FORCE
#endif /* JDSC_MEMPOOL */


#if defined(JDSC_DEBUG)
#include <assert.h>
#define JDSC_ASSERT(expr) assert(expr)
#else
#define JDSC_ASSERT(expr)
#endif /* JDSC_DEBUG */


#include <stddef.h> /* offsetof */
#if defined(__cplusplus)
#define JDSC_STATIC_ASSERT(cond, msg) static_assert((cond), msg)
#else
#define JDSC_STATIC_ASSERT(cond, msg) _Static_assert((cond), msg)
#endif /* __cplusplus */


#endif /* __J_COMPILER_H */
