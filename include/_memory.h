/*
  Data Structures Memory
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
#include <stdint.h>     /* uintptr_t, SIZE_MAX */
#include <string.h>     /* memcpy */
#include <sys/types.h>
#include <_compiler_inter.h>
JDSC_MEMFUNC_PREFIX      void* _p_malloc(size_t size)                { return malloc(size); }
JDSC_MEMFUNC_PREFIX      void* _p_calloc(size_t nmemb, size_t size)  { return calloc(nmemb, size); }
JDSC_MEMFUNC_PREFIX      void* _p_realloc(void* ptr, size_t size)    { return realloc(ptr, size); }
JDSC_MEMFUNC_PREFIX      void  _p_free(void** ptr)                   { if (NULL != ptr && NULL != *ptr) { free(*ptr); *ptr = NULL; } }

#define p_malloc(size)         _p_malloc((size))
#define p_calloc(nmemb, size)  _p_calloc((nmemb), (size))
#define p_realloc(pold, size)  _p_realloc((void*)(pold), (size))
#define p_free(ptr)            do { _p_free((void**)(&(ptr))); } while (0)





#ifndef JDSC_ALIGNMEM
#define JDSC_ALIGNMEM 1
#endif

#if JDSC_ALIGNMEM

#if defined(_WIN32) || defined(_MSC_VER)
#   include <malloc.h>
#   define JDSC_ALIGN_IMPL_MSVC  1
#   define JDSC_ALIGN_IMPL_POSIX 0
#   define JDSC_ALIGN_IMPL_ANDROID 0
#   define JDSC_ALIGN_IMPL_FALLBACK 0
#elif defined(__ANDROID__)
#   include <malloc.h>
#   define JDSC_ALIGN_IMPL_MSVC  0
#   define JDSC_ALIGN_IMPL_POSIX 0
#   define JDSC_ALIGN_IMPL_ANDROID 1
#   define JDSC_ALIGN_IMPL_FALLBACK 0
#elif defined(__unix__) || defined(__unix) || defined(__APPLE__) || \
      (defined(__STDC_HOSTED__) && __STDC_HOSTED__ && !defined(_WIN32))
   /*
    * posix_memalign() is declared in <stdlib.h> only when
    * _POSIX_C_SOURCE >= 200112L (or _XOPEN_SOURCE >= 600).
    * Under strict -std=c99 / -std=c11 the build system MUST define:
    *     -D_POSIX_C_SOURCE=200112L
    */
#   define JDSC_ALIGN_IMPL_MSVC  0
#   define JDSC_ALIGN_IMPL_POSIX 1
#   define JDSC_ALIGN_IMPL_ANDROID 0
#   define JDSC_ALIGN_IMPL_FALLBACK 0
#elif defined(__ICCARM__) || defined(__ARMCC_VERSION) || defined(__CC_ARM)
#   define JDSC_ALIGN_IMPL_MSVC  0
#   define JDSC_ALIGN_IMPL_POSIX 0
#   define JDSC_ALIGN_IMPL_ANDROID 0
#   define JDSC_ALIGN_IMPL_FALLBACK 1
#else
#   error "JDSC_ALIGNMEM=1 but no aligned allocation primitive for this platform. \
Define JDSC_ALIGNMEM=0 to disable, or add a platform adapter."
#endif

#define JDSC_ALIGN_IS_VALID(a)  \
    (0 != (a) && 0 == ((a) & ((a) - 1)))

JDSC_MEMFUNC_PREFIX
void _p_aligned_free(void** ptr, size_t alignment)
{
    if (alignment <= JDSC_ALIGN_DEFAULT) {
        _p_free(ptr);
        return ;
    }

    JDSC_ASSERT(JDSC_ALIGN_IS_VALID(alignment));

    if (NULL != ptr && NULL != *ptr) {
#if JDSC_ALIGN_IMPL_MSVC
        _aligned_free(*ptr);
#elif JDSC_ALIGN_IMPL_ANDROID || JDSC_ALIGN_IMPL_POSIX
        free(*ptr);
#else
        free(((void**)*ptr)[-1]);
#endif
        *ptr = NULL;
    }
}
#define p_aligned_free(ptr, align)  do { _p_aligned_free((void**)(&(ptr)), (align)); } while (0)


JDSC_MEMFUNC_PREFIX
void* _p_aligned_malloc(size_t alignment, size_t size)
{
    if (alignment <= JDSC_ALIGN_DEFAULT)
        return _p_malloc(size);

    JDSC_ASSERT(JDSC_ALIGN_IS_VALID(alignment));

#if JDSC_ALIGN_IMPL_MSVC
    return _aligned_malloc(size, alignment);
#elif JDSC_ALIGN_IMPL_ANDROID
    return memalign(alignment, size);
#elif JDSC_ALIGN_IMPL_POSIX
    {
        void* p = NULL;
        if (0 != posix_memalign(&p, alignment, size))
            return NULL;
        return p;
    }
#else
    {
        void* ptr = malloc(sizeof(void*) + alignment - 1 + size);
        if (NULL != ptr) {
            void* orig_ptr = ptr;
            ptr = (void*)(((uintptr_t)ptr + sizeof(void*) + alignment - 1)
                          & ~(uintptr_t)(alignment - 1));
            ((void**)ptr)[-1] = orig_ptr;
        }
        return ptr;
    }
#endif
}
#define p_aligned_malloc(align, size)  _p_aligned_malloc((align), (size))


JDSC_MEMFUNC_PREFIX
void* _p_aligned_realloc(void* ptr, size_t alignment, size_t size, size_t old_size)
{
    if (0 != size) {
        if (alignment <= JDSC_ALIGN_DEFAULT)
            return _p_realloc(ptr, size);

        JDSC_ASSERT(JDSC_ALIGN_IS_VALID(alignment));

#if JDSC_ALIGN_IMPL_MSVC
        /* _aligned_realloc() does not document NULL handling; route NULL
           through _p_aligned_malloc() so it behaves like realloc(NULL, n). */
        if (NULL == ptr)
            return _p_aligned_malloc(alignment, size);
        return _aligned_realloc(ptr, size, alignment);
#else
        void* p = _p_aligned_malloc(alignment, size);
        if (NULL == p)
            return NULL;

        if (NULL != ptr) {
            size_t copy_size = old_size < size ? old_size : size;
            if (copy_size > 0) memcpy(p, ptr, copy_size);
            p_aligned_free(ptr, alignment);
        }
        return p;
#endif
    }

    /* size == 0: free the block and return NULL.  This matches the common
       realloc(ptr, 0) behavior (glibc frees and returns NULL); C11/C17 leave
       it implementation-defined, and C23 makes it undefined. */
    p_aligned_free(ptr, alignment);
    return NULL;
}
#define p_aligned_realloc(ptr, align, size, old_size) \
    _p_aligned_realloc((ptr), (align), (size), (old_size))

#else /* JDSC_ALIGNMEM == 0 */

#define p_aligned_malloc(align, size)                  p_malloc((size))
#define p_aligned_realloc(ptr, align, size, old_size)  p_realloc((ptr), (size))
#define p_aligned_free(ptr, align)                     p_free((ptr))

#endif /* JDSC_ALIGNMEM */





HEADER_STATIC_INLINE
bool _is_null(const void* ptr)
{
    return NULL == ptr;
}
#define is_null(ptr) _is_null((void*)(ptr))


#endif /* __J_MEMORY_H */
