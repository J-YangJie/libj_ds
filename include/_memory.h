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
#undef p_aligned_malloc
#undef p_aligned_realloc
#undef p_aligned_free
#undef is_null
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <_compiler_inter.h>
JDSC_MEMFUNC_PREFIX     void* _p_malloc(size_t size)                { return malloc(size); }
JDSC_MEMFUNC_PREFIX     void* _p_calloc(size_t nmemb, size_t size)  { return calloc(nmemb, size); }
JDSC_MEMFUNC_PREFIX     void* _p_realloc(void* ptr, size_t size)    { return realloc(ptr, size); }
JDSC_MEMFUNC_PREFIX     void  _p_free(void** ptr)                   { if (NULL != ptr && NULL != *ptr) { free(*ptr); *ptr = NULL; } }
#define p_malloc(size)        _p_malloc((size))
#define p_calloc(nmemb, size) _p_calloc((nmemb), (size))
#define p_realloc(pold, size) _p_realloc((void*)(pold), (size))
#define p_free(ptr)           do { _p_free((void**)(&(ptr))); } while (0)



#ifndef JDSC_ALIGN
#define JDSC_ALIGN 0
#endif

#define JDSC_ALIGN_DEFAULT  (sizeof(void*) * 2)

#define p_aligned_free(ptr)            p_free((ptr))


#if JDSC_ALIGN
JDSC_MEMFUNC_PREFIX void* _p_aligned_malloc(size_t alignment, size_t size)
{
    void* p = NULL;

    if (alignment <= JDSC_ALIGN_DEFAULT)
        return _p_malloc(size);

    if (0 != posix_memalign(&p, alignment, size))
        return NULL;
    return p;
}
#define p_aligned_malloc(align, size)  _p_aligned_malloc((align), (size))
#else
#define p_aligned_malloc(align, size)  p_malloc((size))
#endif


#if JDSC_ALIGN
JDSC_MEMFUNC_PREFIX void* _p_aligned_realloc(void* ptr, size_t alignment, size_t size, size_t old_size)
{
    void* p;

    if (alignment <= JDSC_ALIGN_DEFAULT)
        return _p_realloc(ptr, size);

    p = _p_aligned_malloc(alignment, size);
    if (NULL == p)
        return NULL;

    if (NULL != ptr) {
        memcpy(p, ptr, old_size < size ? old_size : size);
        p_aligned_free(ptr);
    }
    return p;
}
#define p_aligned_realloc(ptr, align, size, old_size)  _p_aligned_realloc((ptr), (align), (size), (old_size))
#else
#define p_aligned_realloc(ptr, align, size, old_size)  p_realloc((ptr), (size))
#endif



static inline bool _is_null(const void* ptr)
{
    return NULL == ptr;
}
#define is_null(ptr) _is_null((void*)(ptr))

#endif /* __J_MEMORY_H */
