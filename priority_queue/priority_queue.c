/*
  Priority Queue Implementations
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

#include <priority_queue/priority_queue.h>

#include <string.h>
#include <_log.h>
#include <_memory.h>
#include <_compiler_inter.h>
#include <linux/_compiler.h>

#define TAG "[priority_queue]"

/* Common */
#define __i_priority_queue_fix_sso(p)  __ds_ops_fix_sso((priority_queue_data_t)(p))

#define __i_pq_tok_u8(p)   (*(p)) /* __ds_token_from_u8(*(const uint8_t*)(p))   */
#define __i_pq_tok_u16(p)  (*(p)) /* __ds_token_from_u16(*(const uint16_t*)(p)) */
#define __i_pq_tok_u32(p)  (*(p)) /* __ds_token_from_u32(*(const uint32_t*)(p)) */
#define __i_pq_tok_u64(p)  __ds_token_load_8(p)
#define __i_pq_tok_s8(p)   (*(p)) /* __ds_token_from_s8(*(const int8_t*)(p))    */
#define __i_pq_tok_s16(p)  (*(p)) /* __ds_token_from_s16(*(const int16_t*)(p))  */
#define __i_pq_tok_s32(p)  (*(p)) /* __ds_token_from_s32(*(const int32_t*)(p))  */
#define __i_pq_tok_s64(p)  __ds_token_load_8(p)
#define __i_pq_tok_f32(p)  __ds_token_from_f32(*(p))
#define __i_pq_tok_f64(p)  __ds_token_load_8(p)

#define __i_priority_queue_define_heap(name, type, prep, __at, __tok, size, pop_back) \
static                                                                  \
void __i_priority_queue_rise_##name(i_priority_queue_t* _this,          \
                                    priority_queue_size_t index)        \
{                                                                       \
    prep                                                                \
    const class_priority_queue_ops_t* __ops = _this->ops;               \
    bool (*__lt)(priority_queue_data_t, priority_queue_data_t) =        \
                                is_null(__ops) ? NULL : __ops->__lt;    \
    type* __hp;                                                         \
    type  __t;                                                          \
    priority_queue_size_t __i = index;                                  \
                                                                        \
    if (__i <= 0)                                                       \
        return ;                                                        \
                                                                        \
    __hp = __at(__i);                                                   \
    __t = *__hp;                                                        \
                                                                        \
    if (is_null(__lt)) {                                                \
        while (__i > 0) {                                               \
            priority_queue_size_t __parent = (__i - 1) >> 1;            \
            type* __pp = __at(__parent);                                \
                                                                        \
            if (!(*(__pp) < __t))                                       \
                break;                                                  \
                                                                        \
            *__hp = *__pp;                                              \
            __hp = __pp;                                                \
            __i = __parent;                                             \
        }                                                               \
    } else {                                                            \
        while (__i > 0) {                                               \
            priority_queue_size_t __parent = (__i - 1) >> 1;            \
            type* __pp = __at(__parent);                                \
                                                                        \
            if (!__lt(__tok(__pp), __tok(&__t)))                        \
                break;                                                  \
                                                                        \
            *__hp = *__pp;                                              \
            __hp = __pp;                                                \
            __i = __parent;                                             \
        }                                                               \
    }                                                                   \
    *__hp = __t;                                                        \
}                                                                       \
static                                                                  \
void __i_priority_queue_heapify_##name(i_priority_queue_t* _this,       \
                                       priority_queue_size_t index)     \
{                                                                       \
    prep                                                                \
    const class_priority_queue_ops_t* __ops = _this->ops;               \
    bool (*__lt)(priority_queue_data_t, priority_queue_data_t) =        \
                                is_null(__ops) ? NULL : __ops->__lt;    \
    const priority_queue_size_t __size = size;                          \
    type* __hp;                                                         \
    type  __t;                                                          \
    priority_queue_size_t __i = index;                                  \
                                                                        \
    if (__i >= __size)                                                  \
        return ;                                                        \
                                                                        \
    __hp = __at(__i);                                                   \
    __t = *__hp;                                                        \
                                                                        \
    if (is_null(__lt)) {                                                \
        while (((__i << 1) + 1) < __size) {                             \
            priority_queue_size_t __l = (__i << 1) + 1;                 \
            priority_queue_size_t __r = __l + 1;                        \
            priority_queue_size_t __m = __l;                            \
            type* __mp = __at(__l);                                     \
                                                                        \
            if (__r < __size) {                                         \
                type* __rp = __at(__r);                                 \
                                                                        \
                if (*(__mp) < *(__rp)) {                                \
                    __m  = __r;                                         \
                    __mp = __rp;                                        \
                }                                                       \
            }                                                           \
            if (!(__t < *(__mp)))                                       \
                break;                                                  \
                                                                        \
            *__hp = *__mp;                                              \
            __hp = __mp;                                                \
            __i = __m;                                                  \
        }                                                               \
    } else {                                                            \
        while (((__i << 1) + 1) < __size) {                             \
            priority_queue_size_t __l = (__i << 1) + 1;                 \
            priority_queue_size_t __r = __l + 1;                        \
            priority_queue_size_t __m = __l;                            \
            type* __mp = __at(__l);                                     \
                                                                        \
            if (__r < __size) {                                         \
                type* __rp = __at(__r);                                 \
                                                                        \
                if (__lt(__tok(__mp), __tok(__rp))) {                   \
                    __m  = __r;                                         \
                    __mp = __rp;                                        \
                }                                                       \
            }                                                           \
            if (!__lt(__tok(&__t), __tok(__mp)))                        \
                break;                                                  \
                                                                        \
            *__hp = *__mp;                                              \
            __hp = __mp;                                                \
            __i = __m;                                                  \
        }                                                               \
    }                                                                   \
    *__hp = __t;                                                        \
}                                                                       \
static                                                                  \
void __i_priority_queue_pop_##name(i_priority_queue_t* _this)           \
{                                                                       \
    prep                                                                \
    const priority_queue_size_t __size = size;                          \
                                                                        \
    if (__size <= 0)                                                    \
        return ;                                                        \
    if (__size > 1) {                                                   \
        type* __ap = __at(0);                                           \
        type* __bp = __at(__size - 1);                                  \
        type  __t = *__ap;                                              \
                                                                        \
        *__ap = *__bp;                                                  \
        *__bp = __t;                                                    \
    }                                                                   \
    pop_back;                                                           \
    __i_priority_queue_heapify_##name(_this, 0);                        \
}





/* Vector */
#define __i_pq_vec_prep       i_vector_t* __v = _this->v;



/* Vector Number */
#define __i_pq_vec_at_u8(i)   ((uint8_t*)__v->begin + (i))
#define __i_pq_vec_at_u16(i)  ((uint16_t*)__v->begin + (i))
#define __i_pq_vec_at_u32(i)  ((uint32_t*)__v->begin + (i))
#define __i_pq_vec_at_u64(i)  ((uint64_t*)__v->begin + (i))
#define __i_pq_vec_at_s8(i)   ((int8_t*)__v->begin + (i))
#define __i_pq_vec_at_s16(i)  ((int16_t*)__v->begin + (i))
#define __i_pq_vec_at_s32(i)  ((int32_t*)__v->begin + (i))
#define __i_pq_vec_at_s64(i)  ((int64_t*)__v->begin + (i))
#define __i_pq_vec_at_f32(i)  ((float*)__v->begin + (i))
#define __i_pq_vec_at_f64(i)  ((double*)__v->begin + (i))

__i_priority_queue_define_heap(vector_u8,  uint8_t,  __i_pq_vec_prep, __i_pq_vec_at_u8,  __i_pq_tok_u8,  __i_vector_size(__v), cvector_pop_back(__v))
__i_priority_queue_define_heap(vector_u16, uint16_t, __i_pq_vec_prep, __i_pq_vec_at_u16, __i_pq_tok_u16, __i_vector_size(__v), cvector_pop_back(__v))
__i_priority_queue_define_heap(vector_u32, uint32_t, __i_pq_vec_prep, __i_pq_vec_at_u32, __i_pq_tok_u32, __i_vector_size(__v), cvector_pop_back(__v))
__i_priority_queue_define_heap(vector_u64, uint64_t, __i_pq_vec_prep, __i_pq_vec_at_u64, __i_pq_tok_u64, __i_vector_size(__v), cvector_pop_back(__v))
__i_priority_queue_define_heap(vector_s8,  int8_t,   __i_pq_vec_prep, __i_pq_vec_at_s8,  __i_pq_tok_s8,  __i_vector_size(__v), cvector_pop_back(__v))
__i_priority_queue_define_heap(vector_s16, int16_t,  __i_pq_vec_prep, __i_pq_vec_at_s16, __i_pq_tok_s16, __i_vector_size(__v), cvector_pop_back(__v))
__i_priority_queue_define_heap(vector_s32, int32_t,  __i_pq_vec_prep, __i_pq_vec_at_s32, __i_pq_tok_s32, __i_vector_size(__v), cvector_pop_back(__v))
__i_priority_queue_define_heap(vector_s64, int64_t,  __i_pq_vec_prep, __i_pq_vec_at_s64, __i_pq_tok_s64, __i_vector_size(__v), cvector_pop_back(__v))
__i_priority_queue_define_heap(vector_f32, float,    __i_pq_vec_prep, __i_pq_vec_at_f32, __i_pq_tok_f32, __i_vector_size(__v), cvector_pop_back(__v))
__i_priority_queue_define_heap(vector_f64, double,   __i_pq_vec_prep, __i_pq_vec_at_f64, __i_pq_tok_f64, __i_vector_size(__v), cvector_pop_back(__v))



/* Vector SSO */
static
void __i_priority_queue_rise_vector_sso(i_priority_queue_t* _this, priority_queue_size_t index)
{
    i_vector_t* __v = _this->v;
    bool (*__lt)(priority_queue_data_t, priority_queue_data_t) = _this->ops->__lt;
    ds_sso_t* __hp;
    ds_sso_t  __t;
    priority_queue_size_t __i = index;

    if (__i <= 0)
        return ;

    __hp = ((ds_sso_t*)__v->begin + __i);
    __t  = *__hp;
    __i_priority_queue_fix_sso(&__t);

    while (__i > 0) {
        priority_queue_size_t __parent = (__i - 1) >> 1;
        ds_sso_t* __pp = ((ds_sso_t*)__v->begin + __parent);

        if (!__lt((priority_queue_data_t)__pp, (priority_queue_data_t)&__t))
            break;

        *__hp = *__pp;
        __i_priority_queue_fix_sso(__hp);
        __hp = __pp;
        __i  = __parent;
    }
    *__hp = __t;
    __i_priority_queue_fix_sso(__hp);
}

static
void __i_priority_queue_heapify_vector_sso(i_priority_queue_t* _this, priority_queue_size_t index)
{
    i_vector_t* __v = _this->v;
    bool (*__lt)(priority_queue_data_t, priority_queue_data_t) = _this->ops->__lt;
    priority_queue_size_t __size = __i_vector_size(__v);
    ds_sso_t* __hp;
    ds_sso_t  __t;
    priority_queue_size_t __i = index;

    if (__i >= __size)
        return ;

    __hp = ((ds_sso_t*)__v->begin + __i);
    __t  = *__hp;
    __i_priority_queue_fix_sso(&__t);

    while (((__i << 1) + 1) < __size) {
        priority_queue_size_t __l = (__i << 1) + 1;
        priority_queue_size_t __r = __l + 1;
        priority_queue_size_t __m = __l;
        ds_sso_t* __mp = ((ds_sso_t*)__v->begin + __l);

        if (__r < __size) {
            ds_sso_t* __rp = ((ds_sso_t*)__v->begin + __r);

            if (__lt((priority_queue_data_t)__mp, (priority_queue_data_t)__rp)) {
                __m  = __r;
                __mp = __rp;
            }
        }

        if (!__lt((priority_queue_data_t)&__t, (priority_queue_data_t)__mp))
            break;

        *__hp = *__mp;
        __i_priority_queue_fix_sso(__hp);
        __hp = __mp;
        __i  = __m;
    }
    *__hp = __t;
    __i_priority_queue_fix_sso(__hp);
}

static
void __i_priority_queue_pop_vector_sso(i_priority_queue_t* _this)
{
    i_vector_t* __v = _this->v;
    priority_queue_size_t __size = __i_vector_size(__v);

    if (__size <= 0)
        return ;

    if (__size > 1) {
        ds_sso_t* __ap = ((ds_sso_t*)__v->begin);
        ds_sso_t* __bp = ((ds_sso_t*)__v->begin + (__size - 1));
        ds_sso_t  __t  = *__ap;

        *__ap = *__bp;
        __i_priority_queue_fix_sso(__ap);
        *__bp = __t;
        __i_priority_queue_fix_sso(__bp);
    }

    cvector_pop_back(__v);
    __i_priority_queue_heapify_vector_sso(_this, 0);
}



/* Vector Struct/Union */
static
void __i_priority_queue_rise_vector_addr(i_priority_queue_t* _this, priority_queue_size_t index)
{
    i_vector_t* __v = _this->v;
    const vector_step_t __step = __v->step;
    bool (*__lt)(priority_queue_data_t, priority_queue_data_t) = _this->ops->__lt;
    void (*__move)(priority_queue_data_t, priority_queue_data_t) = is_null(__v->ops) || is_null(__v->ops->__move) ? NULL : __v->ops->__move;
    uint8_t* __hp;
    uint8_t* __t = __v->rend;
    priority_queue_size_t __i = index;

    if (__i <= 0)
        return ;

    __hp = __i_vector_ptr_add(__v->begin, __i, __step);
    if (is_null(__move)) {
        memmove(__t, __hp, __step);

        while (__i > 0) {
            priority_queue_size_t __parent = (__i - 1) >> 1;
            uint8_t* __pp = __i_vector_ptr_add(__v->begin, __parent, __step);

            if (!__lt((priority_queue_data_t)__pp, (priority_queue_data_t)__t))
                break;

            memmove(__hp, __pp, __step);
            __hp = __pp;
            __i  = __parent;
        }
        memmove(__hp, __t, __step);
    } else {
        __move((priority_queue_data_t)__t, (priority_queue_data_t)__hp);

        while (__i > 0) {
            priority_queue_size_t __parent = (__i - 1) >> 1;
            uint8_t* __pp = __i_vector_ptr_add(__v->begin, __parent, __step);

            if (!__lt((priority_queue_data_t)__pp, (priority_queue_data_t)__t))
                break;

            __move((priority_queue_data_t)__hp, (priority_queue_data_t)__pp);
            __hp = __pp;
            __i  = __parent;
        }
        __move((priority_queue_data_t)__hp, (priority_queue_data_t)__t);
    }
}

static
void __i_priority_queue_heapify_vector_addr(i_priority_queue_t* _this, priority_queue_size_t index)
{
    i_vector_t* __v = _this->v;
    const vector_step_t __step = __v->step;
    bool (*__lt)(priority_queue_data_t, priority_queue_data_t) = _this->ops->__lt;
    void (*__move)(priority_queue_data_t, priority_queue_data_t) = is_null(__v->ops) || is_null(__v->ops->__move) ? NULL : __v->ops->__move;
    priority_queue_size_t __size = __i_vector_size(__v);
    uint8_t* __hp;
    uint8_t* __t = __v->rend;
    priority_queue_size_t __i = index;

    if (__i >= __size)
        return ;

    __hp = __i_vector_ptr_add(__v->begin, __i, __step);
    if (is_null(__move)) {
        memmove(__t, __hp, __step);

        while (((__i << 1) + 1) < __size) {
            priority_queue_size_t __l = (__i << 1) + 1;
            priority_queue_size_t __r = __l + 1;
            priority_queue_size_t __m = __l;
            uint8_t* __mp = __i_vector_ptr_add(__v->begin, __l, __step);

            if (__r < __size) {
                uint8_t* __rp = __i_vector_ptr_add(__v->begin, __r, __step);

                if (__lt((priority_queue_data_t)__mp, (priority_queue_data_t)__rp)) {
                    __m  = __r;
                    __mp = __rp;
                }
            }

            if (!__lt((priority_queue_data_t)__t, (priority_queue_data_t)__mp))
                break;

            memmove(__hp, __mp, __step);
            __hp = __mp;
            __i  = __m;
        }
        memmove(__hp, __t, __step);
    } else {
        __move((priority_queue_data_t)__t, (priority_queue_data_t)__hp);

        while (((__i << 1) + 1) < __size) {
            priority_queue_size_t __l = (__i << 1) + 1;
            priority_queue_size_t __r = __l + 1;
            priority_queue_size_t __m = __l;
            uint8_t* __mp = __i_vector_ptr_add(__v->begin, __l, __step);

            if (__r < __size) {
                uint8_t* __rp = __i_vector_ptr_add(__v->begin, __r, __step);

                if (__lt((priority_queue_data_t)__mp, (priority_queue_data_t)__rp)) {
                    __m  = __r;
                    __mp = __rp;
                }
            }

            if (!__lt((priority_queue_data_t)__t, (priority_queue_data_t)__mp))
                break;

            __move((priority_queue_data_t)__hp, (priority_queue_data_t)__mp);
            __hp = __mp;
            __i  = __m;
        }
        __move((priority_queue_data_t)__hp, (priority_queue_data_t)__t);
    }
}

static
void __i_priority_queue_pop_vector_addr(i_priority_queue_t* _this)
{
    i_vector_t* __v = _this->v;
    const vector_step_t __step = __v->step;
    void (*__move)(priority_queue_data_t, priority_queue_data_t) = is_null(__v->ops) || is_null(__v->ops->__move) ? NULL : __v->ops->__move;
    priority_queue_size_t __size = __i_vector_size(__v);

    if (__size <= 0)
        return ;

    if (__size > 1) {
        uint8_t* __ap = __i_vector_ptr_add(__v->begin, 0, __step);
        uint8_t* __bp = __i_vector_ptr_add(__v->begin, __size - 1, __step);
        uint8_t* __t  = __v->rend;

        if (is_null(__move)) {
            memmove(__t,  __ap, __step);
            memmove(__ap, __bp, __step);
            memmove(__bp, __t,  __step);
        } else {
            __move((priority_queue_data_t)__t,  (priority_queue_data_t)__ap);
            __move((priority_queue_data_t)__ap, (priority_queue_data_t)__bp);
            __move((priority_queue_data_t)__bp, (priority_queue_data_t)__t);
        }
    }

    cvector_pop_back(__v);
    __i_priority_queue_heapify_vector_addr(_this, 0);
}






/* Deque */
#define __i_pq_deq_prep                                                \
    i_deque_t* __d = _this->d;                                         \
    const deque_size_t __mask = (deque_size_t)__d->bkt_size - 1;       \
    const int __shift = __builtin_ctz((unsigned)__d->bkt_size);

#define __i_pq_deq_prep_fast                                           \
    i_deque_t* __d = _this->d;                                         \
    const deque_size_t __mask = (deque_size_t)__d->bkt_size - 1;       \
    const int __shift = __builtin_ctz((unsigned)__d->bkt_size);        \
    const deque_step_t __step = __d->step;                             \
    const ptrdiff_t __off = (__d->begin.cur - __d->begin.begin) / (ptrdiff_t)__step;



/* Deque Number */
static JDSC_INLINE
uint8_t* __i_priority_queue_deque_slot_addr_pow2(const i_deque_t* deque, deque_size_t i, deque_size_t mask, int bkt_shift, int elem_shift)
{
    const ptrdiff_t off   = (deque->begin.cur - deque->begin.begin) >> elem_shift;
    const ptrdiff_t total = off + (ptrdiff_t)i;
    return deque->begin.bkt[total >> bkt_shift] + ((total & (ptrdiff_t)mask) << elem_shift);
}
#define __i_pq_deq_at_pow2(type, i, es)  ((type*)__i_priority_queue_deque_slot_addr_pow2(__d, (i), __mask, __shift, (es)))

#define __i_pq_deq_at_u8(i)   __i_pq_deq_at_pow2(uint8_t,   (i), 0)
#define __i_pq_deq_at_u16(i)  __i_pq_deq_at_pow2(uint16_t,  (i), 1)
#define __i_pq_deq_at_u32(i)  __i_pq_deq_at_pow2(uint32_t,  (i), 2)
#define __i_pq_deq_at_u64(i)  __i_pq_deq_at_pow2(uint64_t,  (i), 3)
#define __i_pq_deq_at_s8(i)   __i_pq_deq_at_pow2(int8_t,    (i), 0)
#define __i_pq_deq_at_s16(i)  __i_pq_deq_at_pow2(int16_t,   (i), 1)
#define __i_pq_deq_at_s32(i)  __i_pq_deq_at_pow2(int32_t,   (i), 2)
#define __i_pq_deq_at_s64(i)  __i_pq_deq_at_pow2(int64_t,   (i), 3)
#define __i_pq_deq_at_f32(i)  __i_pq_deq_at_pow2(float,     (i), 2)
#define __i_pq_deq_at_f64(i)  __i_pq_deq_at_pow2(double,    (i), 3)

#if 1
static JDSC_INLINE
deque_size_t __i_priority_queue_deque_size_pow2(const i_deque_t* deque, int elem_shift)
{
    const ptrdiff_t off_b = (deque->begin.cur - deque->begin.begin) >> elem_shift;
    const ptrdiff_t off_e = (deque->end.cur   - deque->end.begin)   >> elem_shift;
    const ptrdiff_t n = deque->end.bkt - deque->begin.bkt;

    return (deque_size_t)(n * (ptrdiff_t)deque->bkt_size - off_b + off_e);
}
#define __i_pq_deq_size_pow2(es) __i_priority_queue_deque_size_pow2(__d, (es))
#else
#define __i_pq_deq_size_pow2(es) __i_deque_size(__d)
#endif

__i_priority_queue_define_heap(deque_u8,  uint8_t,  __i_pq_deq_prep, __i_pq_deq_at_u8,  __i_pq_tok_u8,  __i_pq_deq_size_pow2(0), cdeque_pop_back(__d))
__i_priority_queue_define_heap(deque_u16, uint16_t, __i_pq_deq_prep, __i_pq_deq_at_u16, __i_pq_tok_u16, __i_pq_deq_size_pow2(1), cdeque_pop_back(__d))
__i_priority_queue_define_heap(deque_u32, uint32_t, __i_pq_deq_prep, __i_pq_deq_at_u32, __i_pq_tok_u32, __i_pq_deq_size_pow2(2), cdeque_pop_back(__d))
__i_priority_queue_define_heap(deque_u64, uint64_t, __i_pq_deq_prep, __i_pq_deq_at_u64, __i_pq_tok_u64, __i_pq_deq_size_pow2(3), cdeque_pop_back(__d))
__i_priority_queue_define_heap(deque_s8,  int8_t,   __i_pq_deq_prep, __i_pq_deq_at_s8,  __i_pq_tok_s8,  __i_pq_deq_size_pow2(0), cdeque_pop_back(__d))
__i_priority_queue_define_heap(deque_s16, int16_t,  __i_pq_deq_prep, __i_pq_deq_at_s16, __i_pq_tok_s16, __i_pq_deq_size_pow2(1), cdeque_pop_back(__d))
__i_priority_queue_define_heap(deque_s32, int32_t,  __i_pq_deq_prep, __i_pq_deq_at_s32, __i_pq_tok_s32, __i_pq_deq_size_pow2(2), cdeque_pop_back(__d))
__i_priority_queue_define_heap(deque_s64, int64_t,  __i_pq_deq_prep, __i_pq_deq_at_s64, __i_pq_tok_s64, __i_pq_deq_size_pow2(3), cdeque_pop_back(__d))
__i_priority_queue_define_heap(deque_f32, float,    __i_pq_deq_prep, __i_pq_deq_at_f32, __i_pq_tok_f32, __i_pq_deq_size_pow2(2), cdeque_pop_back(__d))
__i_priority_queue_define_heap(deque_f64, double,   __i_pq_deq_prep, __i_pq_deq_at_f64, __i_pq_tok_f64, __i_pq_deq_size_pow2(3), cdeque_pop_back(__d))



/* Deque SSO 64BIT */
#define __i_pq_deq_at_pow2_sso(i)  __i_pq_deq_at_pow2(ds_sso_t, (i), __es)

static
void __i_priority_queue_rise_deque_sso(i_priority_queue_t* _this, priority_queue_size_t index)
{
    __i_pq_deq_prep
    const int __es = (int)__builtin_ctz((unsigned)sizeof(ds_sso_t));
    bool (*__lt)(priority_queue_data_t, priority_queue_data_t) = _this->ops->__lt;
    ds_sso_t* __hp;
    ds_sso_t  __t;
    priority_queue_size_t __i = index;

    if (__i <= 0)
        return ;

    __hp = __i_pq_deq_at_pow2_sso(__i);
    __t  = *__hp;
    __i_priority_queue_fix_sso(&__t);

    while (__i > 0) {
        priority_queue_size_t __parent = (__i - 1) >> 1;
        ds_sso_t* __pp = __i_pq_deq_at_pow2_sso(__parent);

        if (!__lt((priority_queue_data_t)__pp, (priority_queue_data_t)&__t))
            break;

        *__hp = *__pp;
        __i_priority_queue_fix_sso(__hp);
        __hp = __pp;
        __i  = __parent;
    }
    *__hp = __t;
    __i_priority_queue_fix_sso(__hp);
}

static
void __i_priority_queue_heapify_deque_sso(i_priority_queue_t* _this, priority_queue_size_t index)
{
    __i_pq_deq_prep
    const int __es = (int)__builtin_ctz((unsigned)sizeof(ds_sso_t));
    bool (*__lt)(priority_queue_data_t, priority_queue_data_t) = _this->ops->__lt;
    priority_queue_size_t __size = __i_pq_deq_size_pow2(__es);
    ds_sso_t* __hp;
    ds_sso_t  __t;
    priority_queue_size_t __i = index;

    if (__i >= __size)
        return ;

    __hp = __i_pq_deq_at_pow2_sso(__i);
    __t  = *__hp;
    __i_priority_queue_fix_sso(&__t);

    while (((__i << 1) + 1) < __size) {
        priority_queue_size_t __l = (__i << 1) + 1;
        priority_queue_size_t __r = __l + 1;
        priority_queue_size_t __m = __l;
        ds_sso_t* __mp = __i_pq_deq_at_pow2_sso(__l);

        if (__r < __size) {
            ds_sso_t* __rp = __i_pq_deq_at_pow2_sso(__r);

            if (__lt((priority_queue_data_t)__mp, (priority_queue_data_t)__rp)) {
                __m  = __r;
                __mp = __rp;
            }
        }
        if (!__lt((priority_queue_data_t)&__t, (priority_queue_data_t)__mp))
            break;

        *__hp = *__mp;
        __i_priority_queue_fix_sso(__hp);
        __hp = __mp;
        __i  = __m;
    }
    *__hp = __t;
    __i_priority_queue_fix_sso(__hp);
}

static
void __i_priority_queue_pop_deque_sso(i_priority_queue_t* _this)
{
    __i_pq_deq_prep
    const int __es = (int)__builtin_ctz((unsigned)sizeof(ds_sso_t));
    priority_queue_size_t __size = __i_pq_deq_size_pow2(__es);

    if (__size <= 0)
        return ;

    if (__size > 1) {
        ds_sso_t* __ap = __i_pq_deq_at_pow2_sso(0);
        ds_sso_t* __bp = __i_pq_deq_at_pow2_sso(__size - 1);
        ds_sso_t  __t  = *__ap;

        *__ap = *__bp;
        __i_priority_queue_fix_sso(__ap);
        *__bp = __t;
        __i_priority_queue_fix_sso(__bp);
    }

    cdeque_pop_back(__d);
    __i_priority_queue_heapify_deque_sso(_this, 0);
}

#undef __i_pq_deq_at_pow2_sso



/* Deque SSO 32BIT */
static JDSC_INLINE
uint8_t* __i_priority_queue_deque_slot_addr_fast(const i_deque_t* deque, ptrdiff_t off, deque_size_t i, deque_size_t mask, int bkt_shift, deque_step_t step)
{
    const ptrdiff_t total = off + (ptrdiff_t)i;
    return deque->begin.bkt[total >> bkt_shift] + (size_t)(total & (ptrdiff_t)mask) * step;
}
#define __i_pq_deq_at_fast_sso(i)  (ds_sso_t*)__i_priority_queue_deque_slot_addr_fast(__d, __off, (i), __mask, __shift, (deque_step_t)sizeof(ds_sso_t))

static
void __i_priority_queue_rise_deque_sso_generic(i_priority_queue_t* _this, priority_queue_size_t index)
{
    __i_pq_deq_prep_fast
    bool (*__lt)(priority_queue_data_t, priority_queue_data_t) = _this->ops->__lt;
    ds_sso_t* __hp;
    ds_sso_t  __t;
    priority_queue_size_t __i = index;

    if (__i <= 0)
        return ;

    __hp = __i_pq_deq_at_fast_sso(__i);
    __t  = *__hp;
    __i_priority_queue_fix_sso(&__t);

    while (__i > 0) {
        priority_queue_size_t __parent = (__i - 1) >> 1;
        ds_sso_t* __pp = __i_pq_deq_at_fast_sso(__parent);

        if (!__lt((priority_queue_data_t)__pp, (priority_queue_data_t)&__t))
            break;

        *__hp = *__pp;
        __i_priority_queue_fix_sso(__hp);
        __hp = __pp;
        __i  = __parent;
    }
    *__hp = __t;
    __i_priority_queue_fix_sso(__hp);
}

static
void __i_priority_queue_heapify_deque_sso_generic(i_priority_queue_t* _this, priority_queue_size_t index)
{
    __i_pq_deq_prep_fast
    bool (*__lt)(priority_queue_data_t, priority_queue_data_t) = _this->ops->__lt;
    priority_queue_size_t __size = __i_deque_size(__d);
    ds_sso_t* __hp;
    ds_sso_t  __t;
    priority_queue_size_t __i = index;

    if (__i >= __size)
        return ;

    __hp = __i_pq_deq_at_fast_sso(__i);
    __t  = *__hp;
    __i_priority_queue_fix_sso(&__t);

    while (((__i << 1) + 1) < __size) {
        priority_queue_size_t __l = (__i << 1) + 1;
        priority_queue_size_t __r = __l + 1;
        priority_queue_size_t __m = __l;
        ds_sso_t* __mp = __i_pq_deq_at_fast_sso(__l);

        if (__r < __size) {
            ds_sso_t* __rp = __i_pq_deq_at_fast_sso(__r);

            if (__lt((priority_queue_data_t)__mp, (priority_queue_data_t)__rp)) {
                __m  = __r;
                __mp = __rp;
            }
        }
        if (!__lt((priority_queue_data_t)&__t, (priority_queue_data_t)__mp))
            break;

        *__hp = *__mp;
        __i_priority_queue_fix_sso(__hp);
        __hp = __mp;
        __i  = __m;
    }
    *__hp = __t;
    __i_priority_queue_fix_sso(__hp);
}

static
void __i_priority_queue_pop_deque_sso_generic(i_priority_queue_t* _this)
{
    __i_pq_deq_prep_fast
    priority_queue_size_t __size = __i_deque_size(__d);

    if (__size <= 0)
        return ;

    if (__size > 1) {
        ds_sso_t* __ap = __i_pq_deq_at_fast_sso(0);
        ds_sso_t* __bp = __i_pq_deq_at_fast_sso(__size - 1);
        ds_sso_t  __t  = *__ap;

        *__ap = *__bp;
        __i_priority_queue_fix_sso(__ap);
        *__bp = __t;
        __i_priority_queue_fix_sso(__bp);
    }

    cdeque_pop_back(__d);
    __i_priority_queue_heapify_deque_sso_generic(_this, 0);
}

#undef __i_pq_deq_at_fast_sso



/* Deque Struct/Union */
#define __i_pq_deq_at_fast_stun(i)  __i_priority_queue_deque_slot_addr_fast(__d, __off, (i), __mask, __shift, __step)

static
void __i_priority_queue_rise_deque_addr(i_priority_queue_t* _this, priority_queue_size_t index)
{
    __i_pq_deq_prep_fast
    bool (*__lt)(priority_queue_data_t, priority_queue_data_t) = _this->ops->__lt;
    void (*__move)(priority_queue_data_t, priority_queue_data_t) = is_null(__d->ops) || is_null(__d->ops->__move) ? NULL : __d->ops->__move;
    uint8_t* __hp;
    uint8_t* __t = __d->swap;
    priority_queue_size_t __i = index;

    if (__i <= 0)
        return ;

    __hp = __i_pq_deq_at_fast_stun(__i);
    if (is_null(__move)) {
        memmove(__t, __hp, __step);

        while (__i > 0) {
            priority_queue_size_t __parent = (__i - 1) >> 1;
            uint8_t* __pp = __i_pq_deq_at_fast_stun(__parent);

            if (!__lt((priority_queue_data_t)__pp, (priority_queue_data_t)__t))
                break;

            memmove(__hp, __pp, __step);
            __hp = __pp;
            __i  = __parent;
        }
        memmove(__hp, __t, __step);
    } else {
        __move((priority_queue_data_t)__t, (priority_queue_data_t)__hp);

        while (__i > 0) {
            priority_queue_size_t __parent = (__i - 1) >> 1;
            uint8_t* __pp = __i_pq_deq_at_fast_stun(__parent);

            if (!__lt((priority_queue_data_t)__pp, (priority_queue_data_t)__t))
                break;

            __move((priority_queue_data_t)__hp, (priority_queue_data_t)__pp);
            __hp = __pp;
            __i  = __parent;
        }
        __move((priority_queue_data_t)__hp, (priority_queue_data_t)__t);
    }

}

static
void __i_priority_queue_heapify_deque_addr(i_priority_queue_t* _this, priority_queue_size_t index)
{
    __i_pq_deq_prep_fast
    bool (*__lt)(priority_queue_data_t, priority_queue_data_t) = _this->ops->__lt;
    void (*__move)(priority_queue_data_t, priority_queue_data_t) = is_null(__d->ops) || is_null(__d->ops->__move) ? NULL : __d->ops->__move;
    priority_queue_size_t __size = __i_deque_size(__d);
    uint8_t* __hp;
    uint8_t* __t = __d->swap;
    priority_queue_size_t __i = index;

    if (__i >= __size)
        return ;

    __hp = __i_pq_deq_at_fast_stun(__i);
    if (is_null(__move)) {
        memmove(__t, __hp, __step);

        while (((__i << 1) + 1) < __size) {
            priority_queue_size_t __l = (__i << 1) + 1;
            priority_queue_size_t __r = __l + 1;
            priority_queue_size_t __m = __l;
            uint8_t* __mp = __i_pq_deq_at_fast_stun(__l);

            if (__r < __size) {
                uint8_t* __rp = __i_pq_deq_at_fast_stun(__r);

                if (__lt((priority_queue_data_t)__mp, (priority_queue_data_t)__rp)) {
                    __m  = __r;
                    __mp = __rp;
                }
            }
            if (!__lt((priority_queue_data_t)__t, (priority_queue_data_t)__mp))
                break;

            memmove(__hp, __mp, __step);
            __hp = __mp;
            __i  = __m;
        }
        memmove(__hp, __t, __step);
    } else {
        __move((priority_queue_data_t)__t, (priority_queue_data_t)__hp);

        while (((__i << 1) + 1) < __size) {
            priority_queue_size_t __l = (__i << 1) + 1;
            priority_queue_size_t __r = __l + 1;
            priority_queue_size_t __m = __l;
            uint8_t* __mp = __i_pq_deq_at_fast_stun(__l);

            if (__r < __size) {
                uint8_t* __rp = __i_pq_deq_at_fast_stun(__r);

                if (__lt((priority_queue_data_t)__mp, (priority_queue_data_t)__rp)) {
                    __m  = __r;
                    __mp = __rp;
                }
            }
            if (!__lt((priority_queue_data_t)__t, (priority_queue_data_t)__mp))
                break;

            __move((priority_queue_data_t)__hp, (priority_queue_data_t)__mp);
            __hp = __mp;
            __i  = __m;
        }
        __move((priority_queue_data_t)__hp, (priority_queue_data_t)__t);
    }

}

static
void __i_priority_queue_pop_deque_addr(i_priority_queue_t* _this)
{
    __i_pq_deq_prep_fast
    void (*__move)(priority_queue_data_t, priority_queue_data_t) = is_null(__d->ops) || is_null(__d->ops->__move) ? NULL : __d->ops->__move;
    priority_queue_size_t __size = __i_deque_size(__d);

    if (__size <= 0)
        return ;

    if (__size > 1) {
        uint8_t* __ap = __i_pq_deq_at_fast_stun(0);
        uint8_t* __bp = __i_pq_deq_at_fast_stun(__size - 1);
        uint8_t* __t = __d->swap;

        if (is_null(__move)) {
            memmove(__t,  __ap, __step);
            memmove(__ap, __bp, __step);
            memmove(__bp, __t,  __step);
        } else {
            __move((priority_queue_data_t)__t,  (priority_queue_data_t)__ap);
            __move((priority_queue_data_t)__ap, (priority_queue_data_t)__bp);
            __move((priority_queue_data_t)__bp, (priority_queue_data_t)__t);
        }

    }

    cdeque_pop_back(__d);
    __i_priority_queue_heapify_deque_addr(_this, 0);
}

#undef __i_pq_deq_at_fast_stun



static JDSC_INLINE
bool __i_priority_queue_deque_step_pow2(deque_step_t step)
{
    return step != 0 && 0 == (step & (step - 1));
}

typedef enum priority_queue_order {
    PQ_ORDER_MIN = 0,
    PQ_ORDER_SINT,
    PQ_ORDER_UINT,
    PQ_ORDER_F32,
    PQ_ORDER_F64,
    PQ_ORDER_OPS,
} pq_order_t;

static JDSC_INLINE
pq_order_t __pq_order_of(ds_dtype_t dtype)
{
    if (dtype & SEQ_CNTR_DTYPE_MASK_OPS)  return PQ_ORDER_OPS;
    if (dtype & SEQ_CNTR_DTYPE_F32)       return PQ_ORDER_F32;
    if (dtype & SEQ_CNTR_DTYPE_F64)       return PQ_ORDER_F64;
    if (dtype & SEQ_CNTR_DTYPE_MASK_SINT) return PQ_ORDER_SINT;
    if (dtype & SEQ_CNTR_DTYPE_MASK_UINT) return PQ_ORDER_UINT;
    return PQ_ORDER_MIN;
}

void i_priority_queue_pop(i_priority_queue_t* _this)
{
    if (unlikely(is_null(_this)))
        return ;

    if (__CONTAINER_ADAPTOR_BASE_VECTOR == _this->id) {
        i_vector_t* v = _this->v;

        if (v->sso) {
            __i_priority_queue_pop_vector_sso(_this);
            return ;
        }

        switch (__pq_order_of(v->dtype))
        {
        case PQ_ORDER_OPS:  __i_priority_queue_pop_vector_addr(_this); return ;
        case PQ_ORDER_F32:  __i_priority_queue_pop_vector_f32(_this);  return ;
        case PQ_ORDER_F64:  __i_priority_queue_pop_vector_f64(_this);  return ;
        case PQ_ORDER_SINT:
            switch (v->step) {
            case 1: __i_priority_queue_pop_vector_s8(_this);  return ;
            case 2: __i_priority_queue_pop_vector_s16(_this); return ;
            case 4: __i_priority_queue_pop_vector_s32(_this); return ;
            case 8: __i_priority_queue_pop_vector_s64(_this); return ;
            default: return ;
            }
        case PQ_ORDER_UINT:
            switch (v->step) {
            case 1: __i_priority_queue_pop_vector_u8(_this);  return ;
            case 2: __i_priority_queue_pop_vector_u16(_this); return ;
            case 4: __i_priority_queue_pop_vector_u32(_this); return ;
            case 8: __i_priority_queue_pop_vector_u64(_this); return ;
            default: return ;
            }
        default: return ;
        }
    } else if (__CONTAINER_ADAPTOR_BASE_DEQUE == _this->id) {
        i_deque_t* d = _this->d;

        if (d->sso) {
            if (__i_priority_queue_deque_step_pow2(d->step))
                __i_priority_queue_pop_deque_sso(_this);
            else
                __i_priority_queue_pop_deque_sso_generic(_this);
            return ;
        }

        switch (__pq_order_of(d->dtype))
        {
        case PQ_ORDER_OPS:  __i_priority_queue_pop_deque_addr(_this); return ;
        case PQ_ORDER_F32:  __i_priority_queue_pop_deque_f32(_this);  return ;
        case PQ_ORDER_F64:  __i_priority_queue_pop_deque_f64(_this);  return ;
        case PQ_ORDER_SINT:
            switch (d->step) {
            case 1: __i_priority_queue_pop_deque_s8(_this);  return ;
            case 2: __i_priority_queue_pop_deque_s16(_this); return ;
            case 4: __i_priority_queue_pop_deque_s32(_this); return ;
            case 8: __i_priority_queue_pop_deque_s64(_this); return ;
            default: return ;
            }
        case PQ_ORDER_UINT:
            switch (d->step) {
            case 1: __i_priority_queue_pop_deque_u8(_this);  return ;
            case 2: __i_priority_queue_pop_deque_u16(_this); return ;
            case 4: __i_priority_queue_pop_deque_u32(_this); return ;
            case 8: __i_priority_queue_pop_deque_u64(_this); return ;
            default: return ;
            }
        default: return ;
        }
    }
}

bool i_priority_queue_push(i_priority_queue_t* _this, priority_queue_data_t data)
{
    if (unlikely(is_null(_this)))
        return false;

    if (__CONTAINER_ADAPTOR_BASE_VECTOR == _this->id) {
        i_vector_t* v = _this->v;
        const priority_queue_size_t index = __i_vector_size(v);

        if (!(v->dtype & SEQ_CNTR_DTYPE_MASK_ALL))
            return false;

        if (!cvector_push_back(v, data))
            return false;

        if (v->sso) {
            __i_priority_queue_rise_vector_sso(_this, index);
            return true;
        }

        switch (__pq_order_of(v->dtype))
        {
        case PQ_ORDER_OPS: __i_priority_queue_rise_vector_addr(_this, index); return true;
        case PQ_ORDER_F32: __i_priority_queue_rise_vector_f32(_this, index);  return true;
        case PQ_ORDER_F64: __i_priority_queue_rise_vector_f64(_this, index);  return true;
        case PQ_ORDER_SINT:
            switch (v->step) {
            case 1: __i_priority_queue_rise_vector_s8(_this, index);  return true;
            case 2: __i_priority_queue_rise_vector_s16(_this, index); return true;
            case 4: __i_priority_queue_rise_vector_s32(_this, index); return true;
            case 8: __i_priority_queue_rise_vector_s64(_this, index); return true;
            default: return false;
            }
        case PQ_ORDER_UINT:
            switch (v->step) {
            case 1: __i_priority_queue_rise_vector_u8(_this, index);  return true;
            case 2: __i_priority_queue_rise_vector_u16(_this, index); return true;
            case 4: __i_priority_queue_rise_vector_u32(_this, index); return true;
            case 8: __i_priority_queue_rise_vector_u64(_this, index); return true;
            default: return false;
            }
        default: return false;
        }
    } else if (__CONTAINER_ADAPTOR_BASE_DEQUE == _this->id) {
        i_deque_t* d = _this->d;
        priority_queue_size_t index = __i_deque_size(d);

        if (!(d->dtype & SEQ_CNTR_DTYPE_MASK_ALL))
            return false;

        if (!cdeque_push_back(d, data))
            return false;

        if (d->sso) {
            if (__i_priority_queue_deque_step_pow2(d->step))
                __i_priority_queue_rise_deque_sso(_this, index);
            else
                __i_priority_queue_rise_deque_sso_generic(_this, index);
            return true;
        }

        switch (__pq_order_of(d->dtype))
        {
        case PQ_ORDER_OPS: __i_priority_queue_rise_deque_addr(_this, index); return true;
        case PQ_ORDER_F32: __i_priority_queue_rise_deque_f32(_this, index);  return true;
        case PQ_ORDER_F64: __i_priority_queue_rise_deque_f64(_this, index);  return true;
        case PQ_ORDER_SINT:
            switch (d->step) {
            case 1: __i_priority_queue_rise_deque_s8(_this, index);  return true;
            case 2: __i_priority_queue_rise_deque_s16(_this, index); return true;
            case 4: __i_priority_queue_rise_deque_s32(_this, index); return true;
            case 8: __i_priority_queue_rise_deque_s64(_this, index); return true;
            default: return false;
            }
        case PQ_ORDER_UINT:
            switch (d->step) {
            case 1: __i_priority_queue_rise_deque_u8(_this, index);  return true;
            case 2: __i_priority_queue_rise_deque_u16(_this, index); return true;
            case 4: __i_priority_queue_rise_deque_u32(_this, index); return true;
            case 8: __i_priority_queue_rise_deque_u64(_this, index); return true;
            default: return false;
            }
        default: return false;
        }
    }

    return false;
}

priority_queue_t* __priority_queue_new(container_adaptor_base_t id, void* pbase, const class_priority_queue_ops_t* ops)
{
    priority_queue_t* priority_queue;

    if (is_null(pbase)) {
        JDSC_ASSERT_NEW(0, "%s: The `pbase` must not be NULL!", __func__);
        return NULL;
    }

    priority_queue = (priority_queue_t*)p_calloc(1, sizeof(priority_queue_t));
    if (is_null(priority_queue)) {
        JDSC_ASSERT_NEW(0, "%s: Failed to allocate %zu bytes for priority_queue!",
                        __func__, sizeof(priority_queue_t));
        goto free_pbase;
    }

    priority_queue->p   = pbase;
    priority_queue->id  = id;
    priority_queue->ops = ops;

    {
        ds_dtype_t dtype = 0;

        switch (id) {
        case __CONTAINER_ADAPTOR_BASE_VECTOR: dtype = priority_queue->v->dtype; break;
        case __CONTAINER_ADAPTOR_BASE_DEQUE:  dtype = priority_queue->d->dtype; break;
        default:
            JDSC_ASSERT_NEW(0, "%s: Unsupported container base id %u (expected VECTOR or DEQUE)!",
                            __func__, (unsigned)id);
            goto free_priority_queue;
        }

        if ((dtype & SEQ_CNTR_DTYPE_MASK_OPS) && (is_null(ops) || is_null(ops->__lt))) {
            JDSC_ASSERT_NEW(0, "%s: The `ops->__lt` is required for struct/union element types!",
                            __func__);
            goto free_priority_queue;
        }
    }

    if (!is_null(ops) && !is_null(ops->copy_data) && is_null(ops->free_data)) {
        JDSC_ASSERT_NEW(0, "%s: The `ops->free_data` is required when `ops->copy_data` is set!",
                        __func__);
        goto free_priority_queue;
    }

    if (g_class_priority_queue_ops_sso() == priority_queue->ops ||
        g_class_priority_queue_ops_sso_greater() == priority_queue->ops) {
        priority_queue->sso = true;
        switch (id) {
        case __CONTAINER_ADAPTOR_BASE_VECTOR: priority_queue->v->sso = true; break;
        case __CONTAINER_ADAPTOR_BASE_DEQUE:  priority_queue->d->sso = true; break;
        default: break;
        }
    }

    return priority_queue;

free_priority_queue:
    p_free(priority_queue);
free_pbase:
    switch (id) {
    case __CONTAINER_ADAPTOR_BASE_VECTOR: VECTOR_DELETE((vector_t**)&pbase); break;
    case __CONTAINER_ADAPTOR_BASE_DEQUE:  DEQUE_DELETE((deque_t**)&pbase);   break;
    default: break;
    }
    return NULL;
}

void __priority_queue_delete(priority_queue_t** _this)
{
    if (unlikely(is_null(_this) || is_null(*_this)))
        return ;

    switch ((*_this)->id) {
    case __CONTAINER_ADAPTOR_BASE_VECTOR: VECTOR_DELETE(&(*_this)->v); break;
    case __CONTAINER_ADAPTOR_BASE_DEQUE:  DEQUE_DELETE(&(*_this)->d);  break;
    default: break;
    }

    p_free(*_this);
}

const class_priority_queue_t* class_priority_queue_ins(void)
{
    static const class_priority_queue_t ins = {
        .size    = cpqueue_size,
        .__empty = __cpqueue_empty,
        .push    = cpqueue_push,
        .top     = cpqueue_top,
        .pop     = cpqueue_pop,
        .swap    = cpqueue_swap,
        .clear   = cpqueue_clear,
    };
    return &ins;
}
