/*
  Multimap Implementations
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

#include <multimap/multimap.h>

#include <_memory.h>
#include <linux/rbtree.h>
#include <linux/_compiler.h>
#include <iterator/iterator.h>

#define multimap_entry(ptr) rb_entry((ptr), struct multimap_node, node)

static __always_inline multimap_node_t* multimap_find(const multimap_t* _this, multimap_key_t key);
static __always_inline multimap_node_t* __multimap_end(const multimap_t* _this);
static __always_inline multimap_node_t* __multimap_next(const multimap_t* _this, const multimap_node_t* node);

static __always_inline multimap_size_t __multimap_size(const multimap_t* _this)
{
    return _this->size;
}

static __always_inline multimap_size_t _multimap_size(const multimap_t* _this)
{
    if (unlikely(is_null(_this)))
        return -1;
    return __multimap_size(_this);
}

static inline multimap_count_t multimap_count(const multimap_t* _this, multimap_key_t key)
{
    multimap_count_t ret = 0;
    multimap_node_t* t = multimap_find(_this, key);

    if (is_null(t))
        return -1;

    if (__multimap_end(_this) == t)
        return 0;

    /* TODO: The current way of writing code will result in low performance. It's 
             necessary to balance the memory usage and performance for optimization. */
    if (is_null(_this->ops) || is_null(_this->ops->__lt)) {
        for (; __multimap_end(_this) != t; t = __multimap_next(_this, t)) {
            if (key != t->key)
                break;

            ret++;
        }
    } else {
        for (; __multimap_end(_this) != t; t = __multimap_next(_this, t)) {
            if (_this->ops->__lt(key, t->key) || _this->ops->__lt(t->key, key))
                break;

            ret++;
        }
    }
    return ret;
}

static __always_inline multimap_node_t* __multimap_first(const multimap_t* _this)
{
    struct rb_node* t = rb_first(&_this->root);
    return is_null(t) ? NULL : multimap_entry(t);
}

static __always_inline multimap_node_t* __multimap_last(const multimap_t* _this)
{
    struct rb_node* t = rb_last(&_this->root);
    return is_null(t) ? NULL : multimap_entry(t);
}

static __always_inline multimap_node_t* __multimap_end(const multimap_t* _this)
{
    return (multimap_node_t*)iterator_end();
}

static __always_inline multimap_node_t* __multimap_begin(const multimap_t* _this)
{
    multimap_node_t* t = __multimap_first(_this);
    return is_null(t) ? __multimap_end(_this) : t;
}

static __always_inline multimap_node_t* _multimap_begin(const multimap_t* _this)
{
    if (unlikely(is_null(_this)))
        return NULL;
    return __multimap_begin(_this);
}

static __always_inline multimap_node_t* __multimap_next(const multimap_t* _this, const multimap_node_t* node)
{
    struct rb_node* t = NULL;

    if (RB_EMPTY_ROOT(&_this->root) || __multimap_end(_this) == node)
        return __multimap_end(_this);

    /* The input parameter is `iterator`, and there's no need 
       to check whether it equals `rend` */

    /* This check should come after `end` or `rend`.
       This is a pre-judgment condition for `rb_next` or `rb_prev` */
    if (unlikely(RB_EMPTY_NODE(&node->node)))
        return NULL;

    t = rb_next(&node->node);
    return is_null(t) ? __multimap_end(_this) : multimap_entry(t);
}

static __always_inline multimap_node_t* _multimap_next(const multimap_t* _this, const multimap_node_t* node)
{
    if (unlikely(is_null(_this) || is_null(node)))
        return NULL;
    return __multimap_next(_this, node);
}

static __always_inline multimap_node_t* __multimap_prev(const multimap_t* _this, const multimap_node_t* node)
{
    struct rb_node* t = NULL;

    if (RB_EMPTY_ROOT(&_this->root))
        return __multimap_end(_this);

    if (__multimap_end(_this) == node)
        return __multimap_last(_this); /* Err: since the `ds` is non-empty, 
                                               the return value includes the error case of `NULL` */

    /* This check should come after `end` or `rend`.
       This is a pre-judgment condition for `rb_next` or `rb_prev` */
    if (unlikely(RB_EMPTY_NODE(&node->node)))
        return NULL;

    t = rb_prev(&node->node);
    return is_null(t) ? __multimap_end(_this) : multimap_entry(t);
}

static __always_inline multimap_node_t* _multimap_prev(const multimap_t* _this, const multimap_node_t* node)
{
    if (unlikely(is_null(_this) || is_null(node)))
        return NULL;
    return __multimap_prev(_this, node);
}

static __always_inline multimap_node_t* __multimap_rend(const multimap_t* _this)
{
    return (multimap_node_t*)iterator_rend();
}

static __always_inline multimap_node_t* __multimap_rbegin(const multimap_t* _this)
{
    multimap_node_t* t = __multimap_last(_this);
    return is_null(t) ? __multimap_rend(_this) : t;
}

static __always_inline multimap_node_t* _multimap_rbegin(const multimap_t* _this)
{
    if (unlikely(is_null(_this)))
        return NULL;
    return __multimap_rbegin(_this);
}

static __always_inline multimap_node_t* __multimap_rnext(const multimap_t* _this, const multimap_node_t* node)
{
    struct rb_node* t = NULL;

    if (RB_EMPTY_ROOT(&_this->root) || __multimap_rend(_this) == node)
        return __multimap_rend(_this);

    /* The input parameter is `reverse_iterator`, and there's no need 
       to check whether it equals `end` */

    /* This check should come after `end` or `rend`.
       This is a pre-judgment condition for `rb_next` or `rb_prev` */
    if (unlikely(RB_EMPTY_NODE(&node->node)))
        return NULL;

    t = rb_prev(&node->node);
    return is_null(t) ? __multimap_rend(_this) : multimap_entry(t);
}

static __always_inline multimap_node_t* _multimap_rnext(const multimap_t* _this, const multimap_node_t* node)
{
    if (unlikely(is_null(_this) || is_null(node)))
        return NULL;
    return __multimap_rnext(_this, node);
}

static __always_inline multimap_node_t* __multimap_rprev(const multimap_t* _this, const multimap_node_t* node)
{
    struct rb_node* t = NULL;

    if (RB_EMPTY_ROOT(&_this->root))
        return __multimap_rend(_this);

    if (__multimap_rend(_this) == node)
        return __multimap_first(_this); /* Err: since the `ds` is non-empty, 
                                                the return value includes the error case of `NULL` */

    /* This check should come after `end` or `rend`.
       This is a pre-judgment condition for `rb_next` or `rb_prev` */
    if (unlikely(RB_EMPTY_NODE(&node->node)))
        return NULL;

    t = rb_next(&node->node);
    return is_null(t) ? __multimap_rend(_this) : multimap_entry(t);
}

static __always_inline multimap_node_t* _multimap_rprev(const multimap_t* _this, const multimap_node_t* node)
{
    if (unlikely(is_null(_this) || is_null(node)))
        return NULL;
    return __multimap_rprev(_this, node);
}

static inline multimap_node_t* __multimap_find(const multimap_t* _this, multimap_key_t key)
{
    struct rb_node* n = _this->root.rb_node;
    multimap_node_t* t = NULL;
    multimap_node_t* ret = NULL;

    if (is_null(_this->ops) || is_null(_this->ops->__lt)) {
        while (!is_null(n)) {
            t = multimap_entry(n);

            if (key < t->key) {
                n = n->rb_left;
            } else if (key > t->key) {
                n = n->rb_right;
            } else {
                n = n->rb_left;
                ret = t;
            }
        }
    } else {
        while (!is_null(n)) {
            t = multimap_entry(n);

            if (_this->ops->__lt(key, t->key)) {
                n = n->rb_left;
            } else if (_this->ops->__lt(t->key, key)) {
                n = n->rb_right;
            } else {
                n = n->rb_left;
                ret = t;
            }
        }
    }

    return ret;
}

static __always_inline multimap_node_t* multimap_find(const multimap_t* _this, multimap_key_t key)
{
    multimap_node_t* t = NULL;

    if (unlikely(is_null(_this)))
        return NULL;

    if (!is_null(_this->ops) && !is_null(_this->ops->valid_key) && !_this->ops->valid_key(key))
        return NULL;

    t = __multimap_find(_this, key);
    return is_null(t) ? __multimap_end(_this) : t;
}

static inline multimap_node_t* __multimap_lower_bound(const multimap_t* _this, multimap_key_t key)
{
    struct rb_node* n = _this->root.rb_node;
    multimap_node_t* t = NULL;
    multimap_node_t* eq = NULL;
    multimap_node_t* gt = NULL;

    if (is_null(_this->ops) || is_null(_this->ops->__lt)) {
        while (!is_null(n)) {
            t = multimap_entry(n);

            if (key < t->key) {
                n = n->rb_left;
                gt = t;
            } else if (key > t->key) {
                n = n->rb_right;
            } else {
                n = n->rb_left;
                eq = t;
            }
        }
    } else {
        while (!is_null(n)) {
            t = multimap_entry(n);

            if (_this->ops->__lt(key, t->key)) {
                n = n->rb_left;
                gt = t;
            } else if (_this->ops->__lt(t->key, key)) {
                n = n->rb_right;
            } else {
                n = n->rb_left;
                eq = t;
            }
        }
    }

    return is_null(eq) ? gt : eq;
}

static __always_inline multimap_node_t* multimap_lower_bound(const multimap_t* _this, multimap_key_t key)
{
    multimap_node_t* t = NULL;

    if (unlikely(is_null(_this)))
        return NULL;

    if (!is_null(_this->ops) && !is_null(_this->ops->valid_key) && !_this->ops->valid_key(key))
        return NULL;

    t = __multimap_lower_bound(_this, key);
    return is_null(t) ? __multimap_end(_this) : t;
}

static inline multimap_node_t* __multimap_upper_bound(const multimap_t* _this, multimap_key_t key)
{
    struct rb_node* n = _this->root.rb_node;
    multimap_node_t* t = NULL;
    multimap_node_t* ret = NULL;

    if (is_null(_this->ops) || is_null(_this->ops->__lt)) {
        while (!is_null(n)) {
            t = multimap_entry(n);

            if (key < t->key) {
                n = n->rb_left;
                ret = t;
            } else if (key > t->key) {
                n = n->rb_right;
            } else {
                n = n->rb_right;
            }
        }
    } else {
        while (!is_null(n)) {
            t = multimap_entry(n);

            if (_this->ops->__lt(key, t->key)) {
                n = n->rb_left;
                ret = t;
            } else if (_this->ops->__lt(t->key, key)) {
                n = n->rb_right;
            } else {
                n = n->rb_right;
            }
        }
    }

    return ret;
}

static __always_inline multimap_node_t* multimap_upper_bound(const multimap_t* _this, multimap_key_t key)
{
    multimap_node_t* t = NULL;

    if (unlikely(is_null(_this)))
        return NULL;

    if (!is_null(_this->ops) && !is_null(_this->ops->valid_key) && !_this->ops->valid_key(key))
        return NULL;

    t = __multimap_upper_bound(_this, key);
    return is_null(t) ? __multimap_end(_this) : t;
}

static inline multimap_node_t* __multimap_insert(multimap_t* _this, multimap_node_t* node)
{
    struct rb_node** n = &_this->root.rb_node;
    struct rb_node* parent = NULL;
    multimap_node_t* t = NULL;

    if (is_null(_this->ops) || is_null(_this->ops->__lt)) {
        while (!is_null(*n)) {
            parent = *n;
            t = multimap_entry(parent);

            if (node->key < t->key)
                n = &parent->rb_left;
            else if (node->key > t->key)
                n = &parent->rb_right;
            else
                n = &parent->rb_right;
        }
    } else {
        while (!is_null(*n)) {
            parent = *n;
            t = multimap_entry(parent);

            if (_this->ops->__lt(node->key, t->key))
                n = &parent->rb_left;
            else if (_this->ops->__lt(t->key, node->key))
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

static inline multimap_node_t* multimap_insert(multimap_t* _this, multimap_key_t key, multimap_value_t value)
{
    multimap_node_t* t = NULL;
    multimap_node_t* ret = NULL;

    if (unlikely(is_null(_this)))
        return NULL;

    if (!is_null(_this->ops) && !is_null(_this->ops->valid_value) && !_this->ops->valid_value(value))
        return NULL;

    if (!is_null(_this->ops) && !is_null(_this->ops->valid_key) && !_this->ops->valid_key(key))
        return NULL;

    t = (multimap_node_t*)p_calloc(1, sizeof(multimap_node_t));
    if (unlikely(is_null(t)))
        return NULL;

    if (is_null(_this->ops) || is_null(_this->ops->copy_key)) {
        t->key = key;
    } else {
        if (!_this->ops->copy_key(key, &t->key))
            goto err;
    }

    if (is_null(_this->ops) || is_null(_this->ops->copy_value)) {
        t->value = value;
    } else {
        if (!_this->ops->copy_value(value, &t->value))
            goto err;
    }

    ret = __multimap_insert(_this, t);
    if (t != ret)
        goto err;
    return t;

err:
    if (!is_null(_this->ops) && !is_null(_this->ops->free_value))
        _this->ops->free_value(&t->value);

    if (!is_null(_this->ops) && !is_null(_this->ops->free_key))
        _this->ops->free_key(&t->key);

    p_free(t);
    return NULL;
}

static __always_inline multimap_node_t* __multimap_erase(multimap_t* _this, multimap_node_t* pos)
{
    rb_erase(&pos->node, &_this->root);
    _this->size--;
    return pos;
}

static inline multimap_node_t* multimap_erase(multimap_t* _this, multimap_node_t* pos)
{
    multimap_node_t* t = NULL;

    if (unlikely(is_null(_this) || is_null(pos)))
        return NULL;

    /* The input parameter is `iterator`, and there's no need 
       to check whether it equals `rend` */
    if (__multimap_size(_this) <= 0 || __multimap_end(_this) == pos/* || __multimap_rend(_this) == pos*/)
        return NULL;

    t = __multimap_next(_this, pos);
    if (is_null(t))
        return NULL;

    __multimap_erase(_this, pos);

    if (!is_null(_this->ops) && !is_null(_this->ops->free_key))
        _this->ops->free_key(&pos->key);

    if (!is_null(_this->ops) && !is_null(_this->ops->free_value))
        _this->ops->free_value(&pos->value);

    p_free(pos);

    return t;
}

static inline multimap_size_t multimap_remove(multimap_t* _this, multimap_key_t key)
{
    multimap_size_t ret = 0;
    multimap_node_t* t = NULL;

    if (unlikely(is_null(_this)))
        return -1;

    t = multimap_find(_this, key);
    if (is_null(t))
        return -1;

    if (__multimap_end(_this) == t)
        return 0;

    if (is_null(_this->ops) || is_null(_this->ops->__lt)) {
        for (; __multimap_end(_this) != t; ) {
            if (key != t->key)
                break;

            t = multimap_erase(_this, t);
            ret++;
        }
    } else {
        for (; __multimap_end(_this) != t; ) {
            if (_this->ops->__lt(key, t->key) || _this->ops->__lt(t->key, key))
                break;

            t = multimap_erase(_this, t);
            ret++;
        }
    }

    return ret;
}

static inline multimap_size_t multimap_remove_if(multimap_t* _this, remove_if_condition_kv cond)
{
    multimap_size_t ret = 0;
    multimap_node_t* t = NULL;

    if (unlikely(is_null(_this) || is_null(cond)))
        return -1;

    for (t = __multimap_begin(_this); __multimap_end(_this) != t; ) {
        if (!cond(t->key, t->value)) {
            t = __multimap_next(_this, t);
            continue;
        }

        t = multimap_erase(_this, t);
        ret++;
    }

    return ret;
}

static inline multimap_size_t multimap_clear(multimap_t* _this)
{
    multimap_size_t ret = 0;
    multimap_node_t* t = NULL;

    if (unlikely(is_null(_this)))
        return -1;

    for (t = __multimap_begin(_this); __multimap_end(_this) != t; ) {
        t = multimap_erase(_this, t);
        ret++;
    }

    return ret;
}

__always_inline void __multimap_init(multimap_t* multimap)
{
    multimap->root = RB_ROOT;
}

__always_inline void __multimap_deinit(multimap_t* multimap)
{
    multimap_clear(multimap);

    multimap->ops = NULL;
    multimap->root = RB_ROOT;
    multimap->size = 0;
}

typedef multimap_iterator_t* (*fp_end)(const multimap_t* _this);
typedef multimap_iterator_t* (*fp_begin)(const multimap_t* _this);
typedef multimap_iterator_t* (*fp_next)(const multimap_t* _this, const multimap_iterator_t* iterator);
typedef multimap_iterator_t* (*fp_prev)(const multimap_t* _this, const multimap_iterator_t* iterator);
typedef multimap_r_iterator_t* (*fp_rend)(const multimap_t* _this);
typedef multimap_r_iterator_t* (*fp_rbegin)(const multimap_t* _this);
typedef multimap_r_iterator_t* (*fp_rnext)(const multimap_t* _this, const multimap_r_iterator_t* r_iterator);
typedef multimap_r_iterator_t* (*fp_rprev)(const multimap_t* _this, const multimap_r_iterator_t* r_iterator);
typedef multimap_iterator_t* (*fp_find)(const multimap_t* _this, multimap_key_t key);
typedef multimap_iterator_t* (*fp_lower_bound)(const multimap_t* _this, multimap_key_t key);
typedef multimap_iterator_t* (*fp_upper_bound)(const multimap_t* _this, multimap_key_t key);
typedef multimap_iterator_t* (*fp_insert)(multimap_t* _this, multimap_key_t key, multimap_value_t value);
typedef multimap_iterator_t* (*fp_erase)(multimap_t* _this, multimap_iterator_t* iterator);

const class_multimap_t* class_multimap_ins(void)
{
    static const class_multimap_t ins = {
        .size        = _multimap_size,
        .count       = multimap_count,
        .end         = (fp_end)__multimap_end,
        .begin       = (fp_begin)_multimap_begin,
        .next        = (fp_next)_multimap_next,
        .prev        = (fp_prev)_multimap_prev,
        .rend        = (fp_rend)__multimap_rend,
        .rbegin      = (fp_rbegin)_multimap_rbegin,
        .rnext       = (fp_rnext)_multimap_rnext,
        .rprev       = (fp_rprev)_multimap_rprev,
        .find        = (fp_find)multimap_find,
        .lower_bound = (fp_lower_bound)multimap_lower_bound,
        .upper_bound = (fp_upper_bound)multimap_upper_bound,
        .insert      = (fp_insert)multimap_insert,
        .erase       = (fp_erase)multimap_erase,
        .remove      = multimap_remove,
        .remove_if   = multimap_remove_if,
        .clear       = multimap_clear,
    };
    return &ins;
}
