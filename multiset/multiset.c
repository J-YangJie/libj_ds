/*
  Multiset Implementations
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

#include <multiset/multiset.h>

#include <_memory.h>
#include <linux/rbtree.h>
#include <linux/_compiler.h>
#include <iterator/iterator.h>

#define multiset_entry(ptr) rb_entry((ptr), struct multiset_node, node)

static __always_inline multiset_node_t* multiset_find(const multiset_t* _this, multiset_value_t value);
static __always_inline multiset_node_t* __multiset_end(const multiset_t* _this);
static __always_inline multiset_node_t* __multiset_next(const multiset_t* _this, const multiset_node_t* node);

static __always_inline multiset_size_t __multiset_size(const multiset_t* _this)
{
    return _this->size;
}

static __always_inline multiset_size_t _multiset_size(const multiset_t* _this)
{
    if (unlikely(is_null(_this)))
        return -1;
    return __multiset_size(_this);
}

static inline multiset_count_t multiset_count(const multiset_t* _this, multiset_value_t value)
{
    multiset_count_t ret = 0;
    multiset_node_t* t = multiset_find(_this, value);

    if (is_null(t))
        return -1;

    if (__multiset_end(_this) == t)
        return 0;

    /* TODO: The current way of writing code will result in low performance. It's 
             necessary to balance the memory usage and performance for optimization. */
    if (is_null(_this->ops) || is_null(_this->ops->__lt_value)) {
        for (; __multiset_end(_this) != t; t = __multiset_next(_this, t)) {
            if (value != t->value)
                break;

            ret++;
        }
    } else {
        for (; __multiset_end(_this) != t; t = __multiset_next(_this, t)) {
            if (_this->ops->__lt_value(value, t->value) || _this->ops->__lt_value(t->value, value))
                break;

            ret++;
        }
    }
    return ret;
}

static __always_inline multiset_node_t* __multiset_first(const multiset_t* _this)
{
    struct rb_node* t = rb_first(&_this->root);
    return is_null(t) ? NULL : multiset_entry(t);
}

static __always_inline multiset_node_t* __multiset_last(const multiset_t* _this)
{
    struct rb_node* t = rb_last(&_this->root);
    return is_null(t) ? NULL : multiset_entry(t);
}

static __always_inline multiset_node_t* __multiset_end(const multiset_t* _this)
{
    return (multiset_node_t*)iterator_end();
}

static __always_inline multiset_node_t* __multiset_begin(const multiset_t* _this)
{
    multiset_node_t* t = __multiset_first(_this);
    return is_null(t) ? __multiset_end(_this) : t;
}

static __always_inline multiset_node_t* _multiset_begin(const multiset_t* _this)
{
    if (unlikely(is_null(_this)))
        return NULL;
    return __multiset_begin(_this);
}

static __always_inline multiset_node_t* __multiset_next(const multiset_t* _this, const multiset_node_t* node)
{
    struct rb_node* t = NULL;

    if (RB_EMPTY_ROOT(&_this->root) || __multiset_end(_this) == node)
        return __multiset_end(_this);

    /* The input parameter is `iterator`, and there's no need 
       to check whether it equals `rend` */

    /* This check should come after `end` or `rend`.
       This is a pre-judgment condition for `rb_next` or `rb_prev` */
    if (unlikely(RB_EMPTY_NODE(&node->node)))
        return NULL;

    t = rb_next(&node->node);
    return is_null(t) ? __multiset_end(_this) : multiset_entry(t);
}

static __always_inline multiset_node_t* _multiset_next(const multiset_t* _this, const multiset_node_t* node)
{
    if (unlikely(is_null(_this) || is_null(node)))
        return NULL;
    return __multiset_next(_this, node);
}

static __always_inline multiset_node_t* __multiset_prev(const multiset_t* _this, const multiset_node_t* node)
{
    struct rb_node* t = NULL;

    if (RB_EMPTY_ROOT(&_this->root))
        return __multiset_end(_this);

    if (__multiset_end(_this) == node)
        return __multiset_last(_this); /* Err: since the `ds` is non-empty, 
                                               the return value includes the error case of `NULL` */

    /* This check should come after `end` or `rend`.
       This is a pre-judgment condition for `rb_next` or `rb_prev` */
    if (unlikely(RB_EMPTY_NODE(&node->node)))
        return NULL;

    t = rb_prev(&node->node);
    return is_null(t) ? __multiset_end(_this) : multiset_entry(t);
}

static __always_inline multiset_node_t* _multiset_prev(const multiset_t* _this, const multiset_node_t* node)
{
    if (unlikely(is_null(_this) || is_null(node)))
        return NULL;
    return __multiset_prev(_this, node);
}

static __always_inline multiset_node_t* __multiset_rend(const multiset_t* _this)
{
    return (multiset_node_t*)iterator_rend();
}

static __always_inline multiset_node_t* __multiset_rbegin(const multiset_t* _this)
{
    multiset_node_t* t = __multiset_last(_this);
    return is_null(t) ? __multiset_rend(_this) : t;
}

static __always_inline multiset_node_t* _multiset_rbegin(const multiset_t* _this)
{
    if (unlikely(is_null(_this)))
        return NULL;
    return __multiset_rbegin(_this);
}

static __always_inline multiset_node_t* __multiset_rnext(const multiset_t* _this, const multiset_node_t* node)
{
    struct rb_node* t = NULL;

    if (RB_EMPTY_ROOT(&_this->root) || __multiset_rend(_this) == node)
        return __multiset_rend(_this);

    /* The input parameter is `reverse_iterator`, and there's no need 
       to check whether it equals `end` */

    /* This check should come after `end` or `rend`.
       This is a pre-judgment condition for `rb_next` or `rb_prev` */
    if (unlikely(RB_EMPTY_NODE(&node->node)))
        return NULL;

    t = rb_prev(&node->node);
    return is_null(t) ? __multiset_rend(_this) : multiset_entry(t);
}

static __always_inline multiset_node_t* _multiset_rnext(const multiset_t* _this, const multiset_node_t* node)
{
    if (unlikely(is_null(_this) || is_null(node)))
        return NULL;
    return __multiset_rnext(_this, node);
}

static __always_inline multiset_node_t* __multiset_rprev(const multiset_t* _this, const multiset_node_t* node)
{
    struct rb_node* t = NULL;

    if (RB_EMPTY_ROOT(&_this->root))
        return __multiset_rend(_this);

    if (__multiset_rend(_this) == node)
        return __multiset_first(_this); /* Err: since the `ds` is non-empty, 
                                                the return value includes the error case of `NULL` */

    /* This check should come after `end` or `rend`.
       This is a pre-judgment condition for `rb_next` or `rb_prev` */
    if (unlikely(RB_EMPTY_NODE(&node->node)))
        return NULL;

    t = rb_next(&node->node);
    return is_null(t) ? __multiset_rend(_this) : multiset_entry(t);
}

static __always_inline multiset_node_t* _multiset_rprev(const multiset_t* _this, const multiset_node_t* node)
{
    if (unlikely(is_null(_this) || is_null(node)))
        return NULL;
    return __multiset_rprev(_this, node);
}

static inline multiset_node_t* __multiset_find(const multiset_t* _this, multiset_value_t value)
{
    struct rb_node* n = _this->root.rb_node;
    multiset_node_t* t = NULL;
    multiset_node_t* ret = NULL;

    if (is_null(_this->ops) || is_null(_this->ops->__lt_value)) {
        while (!is_null(n)) {
            t = multiset_entry(n);

            if (value < t->value) {
                n = n->rb_left;
            } else if (value > t->value) {
                n = n->rb_right;
            } else {
                n = n->rb_left;
                ret = t;
            }
        }
    } else {
        while (!is_null(n)) {
            t = multiset_entry(n);

            if (_this->ops->__lt_value(value, t->value)) {
                n = n->rb_left;
            } else if (_this->ops->__lt_value(t->value, value)) {
                n = n->rb_right;
            } else {
                n = n->rb_left;
                ret = t;
            }
        }
    }

    return ret;
}

static __always_inline multiset_node_t* multiset_find(const multiset_t* _this, multiset_value_t value)
{
    multiset_node_t* t = NULL;

    if (unlikely(is_null(_this)))
        return NULL;

    if (!is_null(_this->ops) && !is_null(_this->ops->valid_value) && !_this->ops->valid_value(value))
        return NULL;

    t = __multiset_find(_this, value);
    return is_null(t) ? __multiset_end(_this) : t;
}

static inline multiset_node_t* __multiset_lower_bound(const multiset_t* _this, multiset_value_t value)
{
    struct rb_node* n = _this->root.rb_node;
    multiset_node_t* t = NULL;
    multiset_node_t* eq = NULL;
    multiset_node_t* gt = NULL;

    if (is_null(_this->ops) || is_null(_this->ops->__lt_value)) {
        while (!is_null(n)) {
            t = multiset_entry(n);

            if (value < t->value) {
                n = n->rb_left;
                gt = t;
            } else if (value > t->value) {
                n = n->rb_right;
            } else {
                n = n->rb_left;
                eq = t;
            }
        }
    } else {
        while (!is_null(n)) {
            t = multiset_entry(n);

            if (_this->ops->__lt_value(value, t->value)) {
                n = n->rb_left;
                gt = t;
            } else if (_this->ops->__lt_value(t->value, value)) {
                n = n->rb_right;
            } else {
                n = n->rb_left;
                eq = t;
            }
        }
    }

    return is_null(eq) ? gt : eq;
}

static __always_inline multiset_node_t* multiset_lower_bound(const multiset_t* _this, multiset_value_t value)
{
    multiset_node_t* t = NULL;

    if (unlikely(is_null(_this)))
        return NULL;

    if (!is_null(_this->ops) && !is_null(_this->ops->valid_value) && !_this->ops->valid_value(value))
        return NULL;

    t = __multiset_lower_bound(_this, value);
    return is_null(t) ? __multiset_end(_this) : t;
}

static inline multiset_node_t* __multiset_upper_bound(const multiset_t* _this, multiset_value_t value)
{
    struct rb_node* n = _this->root.rb_node;
    multiset_node_t* t = NULL;
    multiset_node_t* ret = NULL;

    if (is_null(_this->ops) || is_null(_this->ops->__lt_value)) {
        while (!is_null(n)) {
            t = multiset_entry(n);

            if (value < t->value) {
                n = n->rb_left;
                ret = t;
            } else if (value > t->value) {
                n = n->rb_right;
            } else {
                n = n->rb_right;
            }
        }
    } else {
        while (!is_null(n)) {
            t = multiset_entry(n);

            if (_this->ops->__lt_value(value, t->value)) {
                n = n->rb_left;
                ret = t;
            } else if (_this->ops->__lt_value(t->value, value)) {
                n = n->rb_right;
            } else {
                n = n->rb_right;
            }
        }
    }

    return ret;
}

static __always_inline multiset_node_t* multiset_upper_bound(const multiset_t* _this, multiset_value_t value)
{
    multiset_node_t* t = NULL;

    if (unlikely(is_null(_this)))
        return NULL;

    if (!is_null(_this->ops) && !is_null(_this->ops->valid_value) && !_this->ops->valid_value(value))
        return NULL;

    t = __multiset_upper_bound(_this, value);
    return is_null(t) ? __multiset_end(_this) : t;
}

static inline multiset_node_t* __multiset_insert(multiset_t* _this, multiset_node_t* node)
{
    struct rb_node** n = &_this->root.rb_node;
    struct rb_node* parent = NULL;
    multiset_node_t* t = NULL;

    if (is_null(_this->ops) || is_null(_this->ops->__lt_value)) {
        while (!is_null(*n)) {
            parent = *n;
            t = multiset_entry(parent);

            if (node->value < t->value)
                n = &parent->rb_left;
            else if (node->value > t->value)
                n = &parent->rb_right;
            else
                n = &parent->rb_right;
        }
    } else {
        while (!is_null(*n)) {
            parent = *n;
            t = multiset_entry(parent);

            if (_this->ops->__lt_value(node->value, t->value))
                n = &parent->rb_left;
            else if (_this->ops->__lt_value(t->value, node->value))
                n = &parent->rb_right;
            else
                n = &parent->rb_right;
        }
    }

    rb_link_node(&node->node, parent, n);
    rb_insert_color(&node->node, &_this->root);
    _this->size++;
    return node;
}

static inline multiset_node_t* multiset_insert(multiset_t* _this, multiset_value_t value)
{
    multiset_node_t* t = NULL;
    multiset_node_t* ret = NULL;

    if (unlikely(is_null(_this)))
        return NULL;

    if (!is_null(_this->ops) && !is_null(_this->ops->valid_value) && !_this->ops->valid_value(value))
        return NULL;

    t = (multiset_node_t*)p_calloc(1, sizeof(multiset_node_t));
    if (unlikely(is_null(t)))
        return NULL;

    if (is_null(_this->ops) || is_null(_this->ops->copy_value)) {
        t->value = value;
    } else {
        if (!_this->ops->copy_value(value, &t->value))
            goto err;
    }

    ret = __multiset_insert(_this, t);
    if (t != ret)
        goto err;
    return t;

err:
    if (!is_null(_this->ops) && !is_null(_this->ops->free_value))
        _this->ops->free_value(&t->value);

    p_free(t);
    return NULL;
}

static __always_inline multiset_node_t* __multiset_erase(multiset_t* _this, multiset_node_t* pos)
{
    rb_erase(&pos->node, &_this->root);
    _this->size--;
    return pos;
}

static inline multiset_node_t* multiset_erase(multiset_t* _this, multiset_node_t* pos)
{
    multiset_node_t* t = NULL;

    if (unlikely(is_null(_this) || is_null(pos)))
        return NULL;

    /* The input parameter is `iterator`, and there's no need 
       to check whether it equals `rend` */
    if (__multiset_size(_this) <= 0 || __multiset_end(_this) == pos/* || __multiset_rend(_this) == pos*/)
        return NULL;

    t = __multiset_next(_this, pos);
    if (is_null(t))
        return NULL;

    __multiset_erase(_this, pos);

    if (!is_null(_this->ops) && !is_null(_this->ops->free_value))
        _this->ops->free_value(&pos->value);

    p_free(pos);

    return t;
}

static inline multiset_size_t multiset_remove(multiset_t* _this, multiset_value_t value)
{
    multiset_size_t ret = 0;
    multiset_node_t* t = NULL;

    if (unlikely(is_null(_this)))
        return -1;

    t = multiset_find(_this, value);
    if (is_null(t))
        return -1;

    if (__multiset_end(_this) == t)
        return 0;

    if (is_null(_this->ops) || is_null(_this->ops->__lt_value)) {
        for (; __multiset_end(_this) != t; ) {
            if (value != t->value)
                break;

            t = multiset_erase(_this, t);
            ret++;
        }
    } else {
        for (; __multiset_end(_this) != t; ) {
            if (_this->ops->__lt_value(value, t->value) || _this->ops->__lt_value(t->value, value))
                break;

            t = multiset_erase(_this, t);
            ret++;
        }
    }

    return ret;
}

static inline multiset_size_t multiset_remove_if(multiset_t* _this, remove_if_condition_v cond)
{
    multiset_size_t ret = 0;
    multiset_node_t* t = NULL;

    if (unlikely(is_null(_this) || is_null(cond)))
        return -1;

    for (t = __multiset_begin(_this); __multiset_end(_this) != t; ) {
        if (!cond(t->value)) {
            t = __multiset_next(_this, t);
            continue;
        }

        t = multiset_erase(_this, t);
        ret++;
    }

    return ret;
}

static inline multiset_size_t multiset_clear(multiset_t* _this)
{
    multiset_size_t ret = 0;
    multiset_node_t* t = NULL;

    if (unlikely(is_null(_this)))
        return -1;

    for (t = __multiset_begin(_this); __multiset_end(_this) != t; ) {
        t = multiset_erase(_this, t);
        ret++;
    }

    return ret;
}

__always_inline void __multiset_init(multiset_t* multiset)
{
    multiset->root = RB_ROOT;
}

__always_inline void __multiset_deinit(multiset_t* multiset)
{
    multiset_clear(multiset);

    multiset->ops = NULL;
    multiset->root = RB_ROOT;
    multiset->size = 0;
}

typedef multiset_iterator_t* (*fp_end)(const multiset_t* _this);
typedef multiset_iterator_t* (*fp_begin)(const multiset_t* _this);
typedef multiset_iterator_t* (*fp_next)(const multiset_t* _this, const multiset_iterator_t* iterator);
typedef multiset_iterator_t* (*fp_prev)(const multiset_t* _this, const multiset_iterator_t* iterator);
typedef multiset_r_iterator_t* (*fp_rend)(const multiset_t* _this);
typedef multiset_r_iterator_t* (*fp_rbegin)(const multiset_t* _this);
typedef multiset_r_iterator_t* (*fp_rnext)(const multiset_t* _this, const multiset_r_iterator_t* r_iterator);
typedef multiset_r_iterator_t* (*fp_rprev)(const multiset_t* _this, const multiset_r_iterator_t* r_iterator);
typedef multiset_iterator_t* (*fp_find)(const multiset_t* _this, multiset_value_t value);
typedef multiset_iterator_t* (*fp_lower_bound)(const multiset_t* _this, multiset_value_t value);
typedef multiset_iterator_t* (*fp_upper_bound)(const multiset_t* _this, multiset_value_t value);
typedef multiset_iterator_t* (*fp_insert)(multiset_t* _this, multiset_value_t value);
typedef multiset_iterator_t* (*fp_erase)(multiset_t* _this, multiset_iterator_t* iterator);

const class_multiset_t* class_multiset_ins(void)
{
    static const class_multiset_t ins = {
        .size        = _multiset_size,
        .count       = multiset_count,
        .end         = (fp_end)__multiset_end,
        .begin       = (fp_begin)_multiset_begin,
        .next        = (fp_next)_multiset_next,
        .prev        = (fp_prev)_multiset_prev,
        .rend        = (fp_rend)__multiset_rend,
        .rbegin      = (fp_rbegin)_multiset_rbegin,
        .rnext       = (fp_rnext)_multiset_rnext,
        .rprev       = (fp_rprev)_multiset_rprev,
        .find        = (fp_find)multiset_find,
        .lower_bound = (fp_lower_bound)multiset_lower_bound,
        .upper_bound = (fp_upper_bound)multiset_upper_bound,
        .insert      = (fp_insert)multiset_insert,
        .erase       = (fp_erase)multiset_erase,
        .remove      = multiset_remove,
        .remove_if   = multiset_remove_if,
        .clear       = multiset_clear,
    };
    return &ins;
}
