/*
  Data Structures Memory
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

#ifndef __J_MEMORY_H
#define __J_MEMORY_H

#undef p_malloc
#undef p_calloc
#undef p_realloc
#undef p_free
#undef is_null
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <sys/types.h>
__attribute__((weak))   void* _p_malloc(size_t size)                { return malloc(size); }
__attribute__((weak))   void* _p_calloc(size_t nmemb, size_t size)  { return calloc(nmemb, size); }
__attribute__((weak))   void* _p_realloc(void* ptr, size_t size)    { return realloc(ptr, size); }
__attribute__((weak))   void  _p_free(void** ptr)                   { if (NULL != ptr && NULL != *ptr) { free(*ptr); *ptr = NULL; } }
#define p_malloc(size)        _p_malloc((size))
#define p_calloc(nmemb, size) _p_calloc((nmemb), (size))
#define p_realloc(pold, size) _p_realloc((void*)(pold), (size))
#define p_free(ptr)           do { _p_free((void**)(&(ptr))); } while (0)



static inline bool _is_null(const void* ptr)
{
    return NULL == ptr;
}
#define is_null(ptr) _is_null((void*)(ptr))

#endif /* __J_MEMORY_H */
