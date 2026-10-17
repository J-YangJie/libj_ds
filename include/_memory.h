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

/* ---------------------------------------------------------------------------
 * 对齐分配（默认关闭）
 *
 * 关闭（JDSC_ALIGN = 0）：
 *     p_aligned_malloc 就是 p_malloc，只保证 alignof(max_align_t)（x86-64 上 16）
 *     —— 和引入这个开关之前的行为完全一致，等于 C++17 之前的 std::allocator。
 * 打开（-DWITH_ALIGN=y，Makefile 会加 -DJDSC_ALIGN=1）：
 *     对齐要求超过默认档时走 posix_memalign
 *     —— 等于 C++17 的 aligned new；STL 也是在 allocator 里做的同一件事。
 *
 * 释放必须走 p_aligned_free，名字要单独留着：glibc 上它和 free 等价，但换平台时
 * （例如 Windows 的 _aligned_malloc 必须配 _aligned_free、不能用 free）只改这一处，
 * 所有调用点不用动。
 * ------------------------------------------------------------------------- */
#ifndef JDSC_ALIGN
#define JDSC_ALIGN 0
#endif

#define JDSC_ALIGN_DEFAULT  (sizeof(void*) * 2)     /* x86-64 上 = 16，即 max_align_t 那一档 */

#if JDSC_ALIGN
static inline void* _p_aligned_malloc(size_t align, size_t size)
{
    void* p = NULL;

    if (align <= JDSC_ALIGN_DEFAULT)
        return _p_malloc(size);
    if (0 != posix_memalign(&p, align, size))
        return NULL;
    return p;
}
#else
static inline void* _p_aligned_malloc(size_t align, size_t size)
{
    (void)align;                                    /* 关闭时忽略对齐要求，行为与改动前一致 */
    return _p_malloc(size);
}
#endif

#define p_aligned_malloc(align, size)  _p_aligned_malloc((align), (size))

/* ---------------------------------------------------------------------------
 * 对齐版 realloc
 *
 * 没有平台提供「对齐 realloc」原语（glibc / Windows 都没有，C++ 也没有 ——
 * std::vector 扩容同样是分配 + 搬移）。所以实现只能是 allocate + copy + free。
 *
 * 不能写成「先 realloc 再检查对齐」：realloc 成功后旧块就已经被接管，
 * 万一新地址不对齐、对齐分配又失败，旧数据就没了，无法兑现
 * 「失败时旧缓冲区原样保留」的约定。
 *
 * old_size 必须由调用方给（自由函数拿不到旧块大小）。
 * 默认档及以下（align <= JDSC_ALIGN_DEFAULT）直接转发 p_realloc —— 和改动前一致。
 * ------------------------------------------------------------------------- */
#if JDSC_ALIGN
static inline void* _p_aligned_realloc(void* ptr, size_t align, size_t size, size_t old_size)
{
    void*  p;
    size_t n;

    if (align <= JDSC_ALIGN_DEFAULT)
        return _p_realloc(ptr, size);

    p = _p_aligned_malloc(align, size);
    if (NULL == p)
        return NULL;

    if (NULL != ptr) {
        n = old_size < size ? old_size : size;
        memcpy(p, ptr, n);
        _p_free(&ptr);
    }
    return p;
}
#define p_aligned_realloc(ptr, align, size, old_size) \
        _p_aligned_realloc((ptr), (align), (size), (old_size))
#else
/* 默认模式：就是 realloc，align / old_size 不参与计算 */
#define p_aligned_realloc(ptr, align, size, old_size)  p_realloc((ptr), (size))
#endif

#define p_aligned_free(ptr)            p_free((ptr))



static inline bool _is_null(const void* ptr)
{
    return NULL == ptr;
}
#define is_null(ptr) _is_null((void*)(ptr))

#endif /* __J_MEMORY_H */
