/*
  About compiler internal
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

#ifndef __J_COMPILER_INTER_H
#define __J_COMPILER_INTER_H

#include <_compiler.h>

#define JDSC_INLINE_POLICY        JDSC_INLINE
#define JDSC_INLINE_FORCE_POLICY  JDSC_INLINE


#define JDSC_NO_ITERATOR
#define JDSC_ONLY_WRAPPER


#if defined(JDSC_MEMPOOL)
#   if defined(__GNUC__) || defined(__clang__)
#       define JDSC_MEMFUNC_PREFIX __attribute__((weak))
#   elif defined(__ARMCC_VERSION) && __ARMCC_VERSION >= 6010050
#       define JDSC_MEMFUNC_PREFIX __attribute__((weak))
#   elif defined(__CC_ARM) || defined(__ARMCC_VERSION) || defined(__ICCARM__)
#       define JDSC_MEMFUNC_PREFIX __weak
#   else
#       define JDSC_MEMFUNC_PREFIX static JDSC_INLINE
#   endif
#else
#   define JDSC_MEMFUNC_PREFIX static JDSC_INLINE
#endif


#endif /* __J_COMPILER_INTER_H */
