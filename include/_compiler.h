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

#define JDSC_ITERATOR_ERR_NULL  0
#define JDSC_ITERATOR_END       1UL
#define JDSC_ITERATOR_REND      2UL

#if defined(JDSC_DEBUG)
#include <assert.h>
#define JDSC_ASSERT(expr) assert(expr)
#else
#define JDSC_ASSERT(expr) do { (void)0; } while(0)
#endif /* JDSC_DEBUG */

/*
 * Exactly one of JDSC_32BIT / JDSC_64BIT is defined. The slot code keeps the
 * 8-byte paths only for JDSC_64BIT, where a 64-bit value fits in ds_data_t.
 */
#if defined(__SIZEOF_POINTER__)
#   if 4 == __SIZEOF_POINTER__
#       define JDSC_32BIT 1
#   endif
#elif defined(_WIN32) && !defined(_WIN64)
#   define JDSC_32BIT 1
#elif defined(__i386__) || defined(__arm__) || defined(__ILP32__)
#   define JDSC_32BIT 1
#endif

#if !defined(JDSC_32BIT)
#   define JDSC_64BIT 1
#endif

#endif /* __J_COMPILER_H */
