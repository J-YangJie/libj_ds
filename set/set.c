/*
  Set Implementations
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

#include <set/set.h>

#include <_memory.h>
#include <linux/rbtree.h>
#include <linux/_compiler.h>
#include <iterator/iterator.h>

#define set_entry(ptr) rb_entry((ptr), struct set_node, node)

static __always_inline set_node_t* set_find(const set_t* _this, set_value_t value);
static __always_inline set_node_t* __set_end(const set_t* _this);

static __always_inline set_size_t __set_size(const set_t* _this)
{
    return _this->size;
}

static __always_inline set_size_t _set_size(const set_t* _this)
{
    if (unlikely(is_null(_this)))
        return -1;
    return __set_size(_this);
}

static __always_inline set_count_t set_count(const set_t* _this, set_value_t value)
{
    set_node_t* t = set_find(_this, value);
    return is_null(t) ? -1 : (__set_end(_this) == t ? 0 : 1);
}

static __always_inline set_node_t* __set_first(const set_t* _this)
{
    struct rb_node* t = rb_first(&_this->root);
    return is_null(t) ? NULL : set_entry(t);
}

static __always_inline set_node_t* __set_last(const set_t* _this)
{
    struct rb_node* t = rb_last(&_this->root);
    return is_null(t) ? NULL : set_entry(t);
}

static __always_inline set_node_t* __set_end(const set_t* _this)
{
    return (set_node_t*)iterator_end();
}

static __always_inline set_node_t* __set_begin(const set_t* _this)
{
    set_node_t* t = __set_first(_this);
    return is_null(t) ? __set_end(_this) : t;
}

static __always_inline set_node_t* _set_begin(const set_t* _this)
{
    if (unlikely(is_null(_this)))
        return NULL;
    return __set_begin(_this);
}

static __always_inline set_node_t* __set_next(const set_t* _this, const set_node_t* node)
{
    struct rb_node* t = NULL;

    if (RB_EMPTY_ROOT(&_this->root) || __set_end(_this) == node)
        return __set_end(_this);

    /* The input parameter is `iterator`, and there's no need 
       to check whether it equals `rend` */

    /* This check should come after `end` or `rend`.
       This is a pre-judgment condition for `rb_next` or `rb_prev` */
    if (unlikely(RB_EMPTY_NODE(&node->node)))
        return NULL;

    t = rb_next(&node->node);
    return is_null(t) ? __set_end(_this) : set_entry(t);
}

static __always_inline set_node_t* _set_next(const set_t* _this, const set_node_t* node)
{
    if (unlikely(is_null(_this) || is_null(node)))
        return NULL;
    return __set_next(_this, node);
}

static __always_inline set_node_t* __set_prev(const set_t* _this, const set_node_t* node)
{
    struct rb_node* t = NULL;

    if (RB_EMPTY_ROOT(&_this->root))
        return __set_end(_this);

    if (__set_end(_this) == node)
        return __set_last(_this); /* Err: since the `ds` is non-empty, 
                                          the return value includes the error case of `NULL` */

    /* This check should come after `end` or `rend`.
       This is a pre-judgment condition for `rb_next` or `rb_prev` */
    if (unlikely(RB_EMPTY_NODE(&node->node)))
        return NULL;

    t = rb_prev(&node->node);
    return is_null(t) ? __set_end(_this) : set_entry(t);
}

static __always_inline set_node_t* _set_prev(const set_t* _this, const set_node_t* node)
{
    if (unlikely(is_null(_this) || is_null(node)))
        return NULL;
    return __set_prev(_this, node);
}

static __always_inline set_node_t* __set_rend(const set_t* _this)
{
    return (set_node_t*)iterator_rend();
}

static __always_inline set_node_t* __set_rbegin(const set_t* _this)
{
    set_node_t* t = __set_last(_this);
    return is_null(t) ? __set_rend(_this) : t;
}

static __always_inline set_node_t* _set_rbegin(const set_t* _this)
{
    if (unlikely(is_null(_this)))
        return NULL;
    return __set_rbegin(_this);
}

static __always_inline set_node_t* __set_rnext(const set_t* _this, const set_node_t* node)
{
    struct rb_node* t = NULL;

    if (RB_EMPTY_ROOT(&_this->root) || __set_rend(_this) == node)
        return __set_rend(_this);

    /* The input parameter is `reverse_iterator`, and there's no need 
       to check whether it equals `end` */

    /* This check should come after `end` or `rend`.
       This is a pre-judgment condition for `rb_next` or `rb_prev` */
    if (unlikely(RB_EMPTY_NODE(&node->node)))
        return NULL;

    t = rb_prev(&node->node);
    return is_null(t) ? __set_rend(_this) : set_entry(t);
}

static __always_inline set_node_t* _set_rnext(const set_t* _this, const set_node_t* node)
{
    if (unlikely(is_null(_this) || is_null(node)))
        return NULL;
    return __set_rnext(_this, node);
}

static __always_inline set_node_t* __set_rprev(const set_t* _this, const set_node_t* node)
{
    struct rb_node* t = NULL;

    if (RB_EMPTY_ROOT(&_this->root))
        return __set_rend(_this);

    if (__set_rend(_this) == node)
        return __set_first(_this); /* Err: since the `ds` is non-empty, 
                                           the return value includes the error case of `NULL` */

    /* This check should come after `end` or `rend`.
       This is a pre-judgment condition for `rb_next` or `rb_prev` */
    if (unlikely(RB_EMPTY_NODE(&node->node)))
        return NULL;

    t = rb_next(&node->node);
    return is_null(t) ? __set_rend(_this) : set_entry(t);
}

static __always_inline set_node_t* _set_rprev(const set_t* _this, const set_node_t* node)
{
    if (unlikely(is_null(_this) || is_null(node)))
        return NULL;
    return __set_rprev(_this, node);
}

static inline set_node_t* __set_find(const set_t* _this, set_value_t value)
{
    struct rb_node* n = _this->root.rb_node;
    set_node_t* t = NULL;

    if (is_null(_this->ops) || is_null(_this->ops->__lt_value)) {
        while (!is_null(n)) {
            t = set_entry(n);

            if (value < t->value)
                n = n->rb_left;
            else if (value > t->value)
                n = n->rb_right;
            else
                return t;
        }
    } else {
        while (!is_null(n)) {
            t = set_entry(n);

            if (_this->ops->__lt_value(value, t->value))
                n = n->rb_left;
            else if (_this->ops->__lt_value(t->value, value))
                n = n->rb_right;
            else
                return t;
        }
    }

    return NULL;
}

static __always_inline set_node_t* set_find(const set_t* _this, set_value_t value)
{
    set_node_t* t = NULL;

    if (unlikely(is_null(_this)))
        return NULL;

    if (!is_null(_this->ops) && !is_null(_this->ops->valid_value) && !_this->ops->valid_value(value))
        return NULL;

    t = __set_find(_this, value);
    return is_null(t) ? __set_end(_this) : t;
}

static inline set_node_t* __set_lower_bound(const set_t* _this, set_value_t value)
{
    struct rb_node* n = _this->root.rb_node;
    set_node_t* t = NULL;
    set_node_t* ret = NULL;

    if (is_null(_this->ops) || is_null(_this->ops->__lt_value)) {
        while (!is_null(n)) {
            t = set_entry(n);

            if (value < t->value) {
                n = n->rb_left;
                ret = t;
            } else if (value > t->value) {
                n = n->rb_right;
            } else {
                return t;
            }
        }
    } else {
        while (!is_null(n)) {
            t = set_entry(n);

            if (_this->ops->__lt_value(value, t->value)) {
                n = n->rb_left;
                ret = t;
            } else if (_this->ops->__lt_value(t->value, value)) {
                n = n->rb_right;
            } else {
                return t;
            }
        }
    }

    return ret;
}

static __always_inline set_node_t* set_lower_bound(const set_t* _this, set_value_t value)
{
    set_node_t* t = NULL;

    if (unlikely(is_null(_this)))
        return NULL;

    if (!is_null(_this->ops) && !is_null(_this->ops->valid_value) && !_this->ops->valid_value(value))
        return NULL;

    t = __set_lower_bound(_this, value);
    return is_null(t) ? __set_end(_this) : t;
}

static inline set_node_t* __set_upper_bound(const set_t* _this, set_value_t value)
{
    struct rb_node* n = _this->root.rb_node;
    set_node_t* t = NULL;
    set_node_t* ret = NULL;

    if (is_null(_this->ops) || is_null(_this->ops->__lt_value)) {
        while (!is_null(n)) {
            t = set_entry(n);

            if (value < t->value) {
                n = n->rb_left;
                ret = t;
            } else if (value > t->value) {
                n = n->rb_right;
            } else {
                n = rb_next(n);
                return is_null(n) ? NULL : set_entry(n);
            }
        }
    } else {
        while (!is_null(n)) {
            t = set_entry(n);

            if (_this->ops->__lt_value(value, t->value)) {
                n = n->rb_left;
                ret = t;
            } else if (_this->ops->__lt_value(t->value, value)) {
                n = n->rb_right;
            } else {
                n = rb_next(n);
                return is_null(n) ? NULL : set_entry(n);
            }
        }
    }

    return ret;
}

static __always_inline set_node_t* set_upper_bound(const set_t* _this, set_value_t value)
{
    set_node_t* t = NULL;

    if (unlikely(is_null(_this)))
        return NULL;

    if (!is_null(_this->ops) && !is_null(_this->ops->valid_value) && !_this->ops->valid_value(value))
        return NULL;

    t = __set_upper_bound(_this, value);
    return is_null(t) ? __set_end(_this) : t;
}

static inline set_node_t* __set_insert(set_t* _this, set_node_t* node)
{
    struct rb_node** n = &_this->root.rb_node;
    struct rb_node* parent = NULL;
    set_node_t* t = NULL;

    if (is_null(_this->ops) || is_null(_this->ops->__lt_value)) {
        while (!is_null(*n)) {
            parent = *n;
            t = set_entry(parent);

            if (node->value < t->value)
                n = &parent->rb_left;
            else if (node->value > t->value)
                n = &parent->rb_right;
            else
                return t;
        }
    } else {
        while (!is_null(*n)) {
            parent = *n;
            t = set_entry(parent);

            if (_this->ops->__lt_value(node->value, t->value))
                n = &parent->rb_left;
            else if (_this->ops->__lt_value(t->value, node->value))
                n = &parent->rb_right;
            else
                return t;
        }
    }

    rb_link_node(&node->node, parent, n);
    rb_insert_color(&node->node, &_this->root);
    _this->size++;
    return node;
}

static inline set_node_t* set_insert(set_t* _this, set_value_t value)
{
    set_node_t* t = NULL;
    set_node_t* ret = NULL;

    if (unlikely(is_null(_this)))
        return NULL;

    if (!is_null(_this->ops) && !is_null(_this->ops->valid_value) && !_this->ops->valid_value(value))
        return NULL;

    t = (set_node_t*)p_calloc(1, sizeof(set_node_t));
    if (unlikely(is_null(t)))
        return NULL;

    if (is_null(_this->ops) || is_null(_this->ops->copy_value)) {
        t->value = value;
    } else {
        if (!_this->ops->copy_value(value, &t->value))
            goto err;
    }

    ret = __set_insert(_this, t);
    if (t != ret)
        goto err;
    return t;

err:
    if (!is_null(_this->ops) && !is_null(_this->ops->free_value))
        _this->ops->free_value(&t->value);

    p_free(t);
    return NULL;
}

static __always_inline set_node_t* __set_erase(set_t* _this, set_node_t* pos)
{
    rb_erase(&pos->node, &_this->root);
    _this->size--;
    return pos;
}

static inline set_node_t* set_erase(set_t* _this, set_node_t* pos)
{
    set_node_t* t = NULL;

    if (unlikely(is_null(_this) || is_null(pos)))
        return NULL;

    /* The input parameter is `iterator`, and there's no need 
       to check whether it equals `rend` */
    if (__set_size(_this) <= 0 || __set_end(_this) == pos/* || __set_rend(_this) == pos*/)
        return NULL;

    t = __set_next(_this, pos);
    if (is_null(t))
        return NULL;

    __set_erase(_this, pos);

    if (!is_null(_this->ops) && !is_null(_this->ops->free_value))
        _this->ops->free_value(&pos->value);

    p_free(pos);

    return t;
}

static inline set_size_t set_remove(set_t* _this, set_value_t value)
{
    set_node_t* t = NULL;

    if (unlikely(is_null(_this)))
        return -1;

    t = set_find(_this, value);
    if (is_null(t))
        return -1;

    if (__set_end(_this) == t)
        return 0;

    __set_erase(_this, t);

    if (!is_null(_this->ops) && !is_null(_this->ops->free_value))
        _this->ops->free_value(&t->value);

    p_free(t);

    return 1;
}

static inline set_size_t set_remove_if(set_t* _this, remove_if_condition_v cond)
{
    set_size_t ret = 0;
    set_node_t* t = NULL;

    if (unlikely(is_null(_this) || is_null(cond)))
        return -1;

    for (t = __set_begin(_this); __set_end(_this) != t; ) {
        if (!cond(t->value)) {
            t = __set_next(_this, t);
            continue;
        }

        t = set_erase(_this, t);
        ret++;
    }

    return ret;
}

static inline set_size_t set_clear(set_t* _this)
{
    set_size_t ret = 0;
    set_node_t* t = NULL;

    if (unlikely(is_null(_this)))
        return -1;

    for (t = __set_begin(_this); __set_end(_this) != t; ) {
        t = set_erase(_this, t);
        ret++;
    }

    return ret;
}

__always_inline void __set_init(set_t* set)
{
    set->root = RB_ROOT;
}

__always_inline void __set_deinit(set_t* set)
{
    set_clear(set);

    set->ops = NULL;
    set->root = RB_ROOT;
    set->size = 0;
}

typedef set_iterator_t* (*fp_end)(const set_t* _this);
typedef set_iterator_t* (*fp_begin)(const set_t* _this);
typedef set_iterator_t* (*fp_next)(const set_t* _this, const set_iterator_t* iterator);
typedef set_iterator_t* (*fp_prev)(const set_t* _this, const set_iterator_t* iterator);
typedef set_r_iterator_t* (*fp_rend)(const set_t* _this);
typedef set_r_iterator_t* (*fp_rbegin)(const set_t* _this);
typedef set_r_iterator_t* (*fp_rnext)(const set_t* _this, const set_r_iterator_t* r_iterator);
typedef set_r_iterator_t* (*fp_rprev)(const set_t* _this, const set_r_iterator_t* r_iterator);
typedef set_iterator_t* (*fp_find)(const set_t* _this, set_value_t value);
typedef set_iterator_t* (*fp_lower_bound)(const set_t* _this, set_value_t value);
typedef set_iterator_t* (*fp_upper_bound)(const set_t* _this, set_value_t value);
typedef set_iterator_t* (*fp_insert)(set_t* _this, set_value_t value);
typedef set_iterator_t* (*fp_erase)(set_t* _this, set_iterator_t* iterator);

const class_set_t* class_set_ins(void)
{
    static const class_set_t ins = {
        .size        = _set_size,
        .count       = set_count,
        .end         = (fp_end)__set_end,
        .begin       = (fp_begin)_set_begin,
        .next        = (fp_next)_set_next,
        .prev        = (fp_prev)_set_prev,
        .rend        = (fp_rend)__set_rend,
        .rbegin      = (fp_rbegin)_set_rbegin,
        .rnext       = (fp_rnext)_set_rnext,
        .rprev       = (fp_rprev)_set_rprev,
        .find        = (fp_find)set_find,
        .lower_bound = (fp_lower_bound)set_lower_bound,
        .upper_bound = (fp_upper_bound)set_upper_bound,
        .insert      = (fp_insert)set_insert,
        .erase       = (fp_erase)set_erase,
        .remove      = set_remove,
        .remove_if   = set_remove_if,
        .clear       = set_clear,
    };
    return &ins;
}
