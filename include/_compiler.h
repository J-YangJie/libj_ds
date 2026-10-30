/*
  About compiler
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

#ifndef __J_COMPILER_H
#define __J_COMPILER_H


#define JDSC_ITERATOR_ERR_NULL  0
#define JDSC_ITERATOR_END       1UL
#define JDSC_ITERATOR_REND      2UL


#if defined(__SIZEOF_POINTER__)
#   if __SIZEOF_POINTER__ == 4
#       define JDSC_32BIT 1
#   elif __SIZEOF_POINTER__ == 8
#       define JDSC_64BIT 1
#   else
#       error "Unsupported pointer size!"
#   endif
#elif defined(_WIN64) || defined(__x86_64__) || defined(__aarch64__) || defined(__LP64__)
#   define JDSC_64BIT 1
#elif defined(_WIN32) || defined(__i386__) || defined(__arm__) || defined(__ILP32__)
#   define JDSC_32BIT 1
#else
#   error "Unknown pointer size!"
#endif


#if defined(_MSC_VER)
#   define JDSC_INLINE              __inline
#   define JDSC_INLINE_FORCE        __forceinline
#elif defined(__ARMCC_VERSION) || defined(__CC_ARM)
#   if defined(__ARMCC_VERSION) && __ARMCC_VERSION >= 6010050
#       define JDSC_INLINE          inline
#       define JDSC_INLINE_FORCE    __attribute__((always_inline)) inline
#   else
#       define JDSC_INLINE          __inline
#       define JDSC_INLINE_FORCE    __forceinline
#   endif
#elif defined(__ICCARM__)
#   define JDSC_INLINE              inline
#   define JDSC_INLINE_FORCE        inline
#elif defined(__GNUC__) || defined(__clang__)
#   define JDSC_INLINE              inline
#   define JDSC_INLINE_FORCE        __inline __attribute__((__always_inline__))
#else
#   define JDSC_INLINE              inline
#   define JDSC_INLINE_FORCE        inline
#endif

#define HEADER_STATIC_INLINE  static JDSC_INLINE


#if defined(JDSC_DEBUG)
#include <assert.h>
#define JDSC_ASSERT(expr)                assert(expr)
#define JDSC_ASSERT_NEW(expr, fmt, ...)  do { if (!(expr)) { pr_err(fmt, ##__VA_ARGS__); assert(0); } } while (0)
#else
#define JDSC_ASSERT(expr)                do { (void)0; } while(0)
#define JDSC_ASSERT_NEW(expr, fmt, ...)  do { if (!(expr)) { pr_err(fmt, ##__VA_ARGS__); } } while(0)
#endif /* JDSC_DEBUG */


#include <assert.h>
#if defined(__cplusplus)
#   define JDSC_STATIC_ASSERT(cond, msg)  static_assert((cond), msg)
#elif defined(__STDC_VERSION__) && __STDC_VERSION__ >= 201112L \
    && !defined(__ICCARM__) \
    && (!defined(__ARMCC_VERSION) || __ARMCC_VERSION >= 6010050)
#   define JDSC_STATIC_ASSERT(cond, msg)  _Static_assert((cond), msg)
#elif defined(__COUNTER__)
#   define JDSC_STATIC_ASSERT_CAT_(a, b)  a##b
#   define JDSC_STATIC_ASSERT_CAT(a, b)   JDSC_STATIC_ASSERT_CAT_(a, b)
#   define JDSC_STATIC_ASSERT(cond, msg)  \
        typedef char JDSC_STATIC_ASSERT_CAT(jdsc_static_assert_, __COUNTER__)[(cond) ? 1 : -1]
#else
#   define JDSC_STATIC_ASSERT_CAT_(a, b)  a##b
#   define JDSC_STATIC_ASSERT_CAT(a, b)   JDSC_STATIC_ASSERT_CAT_(a, b)
#   define JDSC_STATIC_ASSERT(cond, msg)  \
        extern void JDSC_STATIC_ASSERT_CAT(jdsc_static_assert_, __LINE__)(int x[(cond) ? 1 : -1])
#endif


#if defined(__ICCARM__)
#   define JDSC_ALIGNOF(_type)      __ALIGNOF__(_type)
#   define JDSC_ALIGN_DEFAULT       __ALIGNOF__(long double)
#elif defined(__STDC_VERSION__) && __STDC_VERSION__ >= 201112L
#   include <stddef.h>              /* size_t, max_align_t (C11) */
#   define JDSC_ALIGNOF(_type)      _Alignof(_type)
#   define JDSC_ALIGN_DEFAULT       _Alignof(max_align_t)
#elif defined(__GNUC__) || defined(__clang__)
#   define JDSC_ALIGNOF(_type)      __alignof__(_type)
#   define JDSC_ALIGN_DEFAULT       __alignof__(long double)
#elif defined(_MSC_VER)
#   define JDSC_ALIGNOF(_type)      __alignof(_type)
#   define JDSC_ALIGN_DEFAULT       (sizeof(void*) * 2)
#elif defined(__ARMCC_VERSION) || defined(__CC_ARM)
#   define JDSC_ALIGNOF(_type)      __alignof__(_type)
#   define JDSC_ALIGN_DEFAULT       (sizeof(void*) * 2)
#else
#   define JDSC_ALIGNOF(_type)      (0)
#   define JDSC_ALIGN_DEFAULT       (sizeof(void*) * 2)
#endif


#ifndef JDSC_ALIGN_TYPE_N
#   if defined(_MSC_VER)
#       define JDSC_ALIGN_TYPE_N(n)  __declspec(align(n))
#   elif defined(__ARMCC_VERSION) && __ARMCC_VERSION >= 6010050
#       define JDSC_ALIGN_TYPE_N(n)  __attribute__((aligned(n)))
#   elif defined(__CC_ARM) || defined(__ARMCC_VERSION)
#       define JDSC_ALIGN_TYPE_N(n)  __align(n)
#   elif defined(__GNUC__) || defined(__clang__)
#       define JDSC_ALIGN_TYPE_N(n)  __attribute__((aligned(n)))
#   elif defined(__ICCARM__) && (__VER__ >= 7080000)
#       define JDSC_ALIGN_TYPE_N(n)  __attribute__((aligned(n)))
#   elif defined(__STDC_VERSION__) && __STDC_VERSION__ >= 201112L
#       define JDSC_ALIGN_TYPE_N(n)  _Alignas(n)
#   else
#       define JDSC_ALIGN_TYPE_N(n)
#   endif
#endif


#endif /* __J_COMPILER_H */
