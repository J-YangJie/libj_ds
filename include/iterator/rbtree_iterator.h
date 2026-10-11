/*
  Red Black Trees Iterator
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

#ifndef __J_RBTREE_ITERATOR_H
#define __J_RBTREE_ITERATOR_H

#include <stdint.h>
#include <_compiler.h>
#include <linux/rbtree.h>

#define __RBTREE_ITERATOR_END   JDSC_ITERATOR_END
#define __RBTREE_ITERATOR_REND  JDSC_ITERATOR_REND
#define __RBTREE_ITERATOR_MASK  (JDSC_ITERATOR_END | JDSC_ITERATOR_REND)

#ifndef unlikely
#define unlikely(x) (x)
#endif /* unlikely */

typedef       struct rb_node* rbtree_iterator_t;
typedef const struct rb_node* rbtree_const_iterator_t;

#define __rbtree_is_end(_rbtree_iterator)    (__RBTREE_ITERATOR_END  == (unsigned long)_rbtree_iterator)
#define __rbtree_is_rend(_rbtree_r_iterator) (__RBTREE_ITERATOR_REND == (unsigned long)_rbtree_r_iterator)

static inline
rbtree_iterator_t __rbtree_return_end(void)
{
    return (rbtree_iterator_t)(uintptr_t)__RBTREE_ITERATOR_END;
}

static inline
rbtree_iterator_t __rbtree_return_rend(void)
{
    return (rbtree_iterator_t)(uintptr_t)__RBTREE_ITERATOR_REND;
}

static inline
rbtree_iterator_t __red_rb_leftmost(rbtree_const_iterator_t _it)
{
    rbtree_iterator_t it = (rbtree_iterator_t)_it;

    while (it->rb_left)
        it = it->rb_left;
    return it;
}

static inline
rbtree_iterator_t __red_rb_rightmost(rbtree_const_iterator_t _it)
{
    rbtree_iterator_t it = (rbtree_iterator_t)_it;

    while (it->rb_right)
        it = it->rb_right;
    return it;
}

static inline
rbtree_iterator_t __red_rb_next(rbtree_const_iterator_t _it)
{
    rbtree_iterator_t p, it = (rbtree_iterator_t)_it;

    if (rb_parent(it) == it)
        return NULL;

    if (it->rb_right)
        return __red_rb_leftmost(it->rb_right);

    while ((p = rb_parent(it)) && it == p->rb_right)
        it = p;

    return p ? p : __rbtree_return_end();
}

static inline
rbtree_iterator_t __red_rb_prev(rbtree_const_iterator_t _it)
{
    rbtree_iterator_t p, it = (rbtree_iterator_t)_it;

    if (rb_parent(it) == it)
        return NULL;

    if (it->rb_left)
        return __red_rb_rightmost(it->rb_left);

    while ((p = rb_parent(it)) && it == p->rb_left)
        it = p;

    return p ? p : __rbtree_return_rend();
}

static inline
rbtree_iterator_t __rbtree_end(const struct rb_root *root)
{
    (void)root;
    return __rbtree_return_end();
}

static inline
rbtree_iterator_t __rbtree_begin(const struct rb_root *root)
{
    return !root->rb_node ? __rbtree_end(root) : __red_rb_leftmost(root->rb_node);
}

static inline
rbtree_iterator_t __rbtree_next(const struct rb_root *root, rbtree_const_iterator_t it)
{
    JDSC_ASSERT(!RB_EMPTY_ROOT(root));
    JDSC_ASSERT(!__rbtree_is_end(it));

#if JDSC_ITERATOR_ERR_NULL
    if (unlikely(__rbtree_is_end(it)))
        return NULL;
#endif /* JDSC_ITERATOR_ERR_NULL */

    /* The input parameter is `iterator`, and there's no need 
       to check whether it equals `rend` */

    /* This check should come after `end` or `rend`.
       This is a pre-judgment condition for `rb_next` or `rb_prev` */
    JDSC_ASSERT(!RB_EMPTY_NODE(it));
    return __red_rb_next(it);
}

static inline
rbtree_iterator_t __rbtree_prev(const struct rb_root *root, rbtree_const_iterator_t it)
{
    rbtree_iterator_t t;

    JDSC_ASSERT(!RB_EMPTY_ROOT(root));

    if (unlikely(__rbtree_is_end(it)))
        return rb_last(root); /* Err: since the `ds` is non-empty, 
                                        the return value includes the error case of `NULL` */

    /* This check should come after `end` or `rend`.
       This is a pre-judgment condition for `rb_next` or `rb_prev` */
    JDSC_ASSERT(!RB_EMPTY_NODE(it));

    t = rb_prev(it);
    JDSC_ASSERT(t);
    return t;
}

static inline
rbtree_iterator_t __rbtree_rend(const struct rb_root *root)
{
    (void)root;
    return __rbtree_return_rend();
}

static inline
rbtree_iterator_t __rbtree_rbegin(const struct rb_root *root)
{
    return !root->rb_node ? __rbtree_rend(root) : __red_rb_rightmost(root->rb_node);
}

static inline
rbtree_iterator_t __rbtree_rnext(const struct rb_root *root, rbtree_const_iterator_t it)
{
    JDSC_ASSERT(!RB_EMPTY_ROOT(root));
    JDSC_ASSERT(!__rbtree_is_rend(it));

#if JDSC_ITERATOR_ERR_NULL
    if (unlikely(__rbtree_is_rend(it)))
        return NULL;
#endif /* JDSC_ITERATOR_ERR_NULL */

    /* The input parameter is `reverse_iterator`, and there's no need 
       to check whether it equals `end` */

    /* This check should come after `end` or `rend`.
       This is a pre-judgment condition for `rb_next` or `rb_prev` */
    JDSC_ASSERT(!RB_EMPTY_NODE(it));
    return __red_rb_prev(it);
}

static inline
rbtree_iterator_t __rbtree_rprev(const struct rb_root *root, rbtree_const_iterator_t it)
{
    rbtree_iterator_t t;

    JDSC_ASSERT(!RB_EMPTY_ROOT(root));

    if (unlikely(__rbtree_is_rend(it)))
        return rb_first(root); /* Err: since the `ds` is non-empty, 
                                        the return value includes the error case of `NULL` */

    /* This check should come after `end` or `rend`.
       This is a pre-judgment condition for `rb_next` or `rb_prev` */
    JDSC_ASSERT(!RB_EMPTY_NODE(it));

    t = rb_next(it);
    JDSC_ASSERT(t);
    return t;
}





#define __RBTREE_ITERATOR_TEMPLATE(T) \
static JDSC_INLINE_FORCE \
T##_node_t* __##T##_end(const T##_t* _this) \
{ \
    return (T##_node_t*)__rbtree_end(&_this->root); \
} \
\
static JDSC_INLINE_FORCE \
T##_node_t* __##T##_begin(const T##_t* _this) \
{ \
    rbtree_iterator_t it = __rbtree_begin(&_this->root); \
    return __rbtree_is_end(it) ? (T##_node_t*)it : T##_entry(it); \
} \
\
static JDSC_INLINE_FORCE \
T##_node_t* _##T##_begin(const T##_t* _this) \
{ \
    if (unlikely(is_null(_this))) \
        return NULL; \
    return __##T##_begin(_this); \
} \
\
static JDSC_INLINE_FORCE \
T##_node_t* __##T##_next(const T##_t* _this, const T##_node_t* node) \
{ \
    rbtree_const_iterator_t it = __rbtree_is_end(node) ? (rbtree_const_iterator_t)node : &node->node; \
    it = __rbtree_next(&_this->root, it); \
    return is_null(it) || __rbtree_is_end(it) ? (T##_node_t*)it : T##_entry(it); \
} \
\
static JDSC_INLINE_FORCE \
T##_node_t* _##T##_next(const T##_t* _this, const T##_node_t* node) \
{ \
    if (unlikely(is_null(_this) || is_null(node))) \
        return NULL; \
    return __##T##_next(_this, node); \
} \
\
static JDSC_INLINE_FORCE_POLICY \
T##_node_t* __##T##_prev(const T##_t* _this, const T##_node_t* node) \
{ \
    rbtree_const_iterator_t it = __rbtree_is_end(node) ? (rbtree_const_iterator_t)node : &node->node; \
    it = __rbtree_prev(&_this->root, it); \
    return is_null(it) ? (T##_node_t*)it : T##_entry(it); \
} \
\
static JDSC_INLINE_FORCE \
T##_node_t* _##T##_prev(const T##_t* _this, const T##_node_t* node) \
{ \
    if (unlikely(is_null(_this) || is_null(node))) \
        return NULL; \
    return __##T##_prev(_this, node); \
} \
\
static JDSC_INLINE_FORCE \
T##_node_t* __##T##_rend(const T##_t* _this) \
{ \
    return (T##_node_t*)__rbtree_rend(&_this->root); \
} \
\
static JDSC_INLINE_FORCE \
T##_node_t* __##T##_rbegin(const T##_t* _this) \
{ \
    rbtree_iterator_t it = __rbtree_rbegin(&_this->root); \
    return __rbtree_is_rend(it) ? (T##_node_t*)it : T##_entry(it); \
} \
\
static JDSC_INLINE_FORCE \
T##_node_t* _##T##_rbegin(const T##_t* _this) \
{ \
    if (unlikely(is_null(_this))) \
        return NULL; \
    return __##T##_rbegin(_this); \
} \
\
static JDSC_INLINE_FORCE \
T##_node_t* __##T##_rnext(const T##_t* _this, const T##_node_t* node) \
{ \
    rbtree_const_iterator_t it = __rbtree_is_rend(node) ? (rbtree_const_iterator_t)node : &node->node; \
    it = __rbtree_rnext(&_this->root, it); \
    return is_null(it) || __rbtree_is_rend(it) ? (T##_node_t*)it : T##_entry(it); \
} \
\
static JDSC_INLINE_FORCE \
T##_node_t* _##T##_rnext(const T##_t* _this, const T##_node_t* node) \
{ \
    if (unlikely(is_null(_this) || is_null(node))) \
        return NULL; \
    return __##T##_rnext(_this, node); \
} \
\
static JDSC_INLINE_FORCE_POLICY \
T##_node_t* __##T##_rprev(const T##_t* _this, const T##_node_t* node) \
{ \
    rbtree_const_iterator_t it = __rbtree_is_rend(node) ? (rbtree_const_iterator_t)node : &node->node; \
    it = __rbtree_rprev(&_this->root, it); \
    return is_null(it) ? (T##_node_t*)it : T##_entry(it); \
} \
\
static JDSC_INLINE_FORCE \
T##_node_t* _##T##_rprev(const T##_t* _this, const T##_node_t* node) \
{ \
    if (unlikely(is_null(_this) || is_null(node))) \
        return NULL; \
    return __##T##_rprev(_this, node); \
}

#endif /* __J_RBTREE_ITERATOR_H */
