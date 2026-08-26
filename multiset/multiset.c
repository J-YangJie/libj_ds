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

#include <_log.h>
#include <_memory.h>
#include <_compiler.h>
#include <linux/_types.h>
#include <linux/rbtree.h>
#include <linux/_compiler.h>
#include <iterator/iterator_inter.h>

typedef struct multiset_node {
    multiset_key_t key;
    struct rb_node node;
} multiset_node_t;

JDSC_STATIC_ASSERT(offsetof(multiset_node_t, key) == 0,               "multiset node key vs iterator data offset mismatch");
JDSC_STATIC_ASSERT(sizeof(multiset_key_t) == sizeof(multiset_data_t), "multiset node key width vs iterator data width mismatch");
JDSC_STATIC_ASSERT(sizeof(multiset_data_t) == sizeof(char*),          "iterator data/sdata union width mismatch");

struct multiset {
    const class_multiset_ops_t* ops;
    struct rb_root root;
    multiset_size_t size;
};

#define TAG "[multiset]"

#define multiset_entry(ptr) rb_entry((ptr), struct multiset_node, node)

static JDSC_INLINE_FORCE_POLICY multiset_node_t* multiset_find(const multiset_t* _this, multiset_key_t key);
static JDSC_INLINE_FORCE multiset_node_t* __multiset_end(const multiset_t* _this);
static JDSC_INLINE_FORCE multiset_node_t* __multiset_next(const multiset_t* _this, const multiset_node_t* node);

static JDSC_INLINE_FORCE
multiset_size_t __multiset_size(const multiset_t* _this)
{
    return _this->size;
}

static JDSC_INLINE_FORCE
multiset_size_t _multiset_size(const multiset_t* _this)
{
    if (unlikely(is_null(_this)))
        return -1;
    return __multiset_size(_this);
}

static
multiset_count_t multiset_count(const multiset_t* _this, multiset_key_t key)
{
    multiset_count_t ret = 0;
    multiset_node_t* t = multiset_find(_this, key);

    if (is_null(t))
        return -1;

    if (__multiset_end(_this) == t)
        return 0;

    /* TODO: The current way of writing code will result in low performance. It's 
             necessary to balance the memory usage and performance for optimization. */
    if (is_null(_this->ops) || is_null(_this->ops->__lt)) {
        for (; __multiset_end(_this) != t; t = __multiset_next(_this, t)) {
            if (key != t->key)
                break;

            ret++;
        }
    } else {
        for (; __multiset_end(_this) != t; t = __multiset_next(_this, t)) {
            if (_this->ops->__lt(key, t->key) || _this->ops->__lt(t->key, key))
                break;

            ret++;
        }
    }
    return ret;
}

static JDSC_INLINE_FORCE JDSC_ONLY_WRAPPER JDSC_NO_ITERATOR
multiset_node_t* ___multiset_first(const multiset_t* _this)
{
    struct rb_node* t = rb_first(&_this->root);
    return is_null(t) ? NULL : multiset_entry(t);
}

static JDSC_INLINE_FORCE JDSC_ONLY_WRAPPER JDSC_NO_ITERATOR
multiset_node_t* ___multiset_last(const multiset_t* _this)
{
    struct rb_node* t = rb_last(&_this->root);
    return is_null(t) ? NULL : multiset_entry(t);
}

static JDSC_INLINE_FORCE
multiset_node_t* __multiset_end(const multiset_t* _this)
{
    return (multiset_node_t*)iterator_end();
}

static JDSC_INLINE_FORCE
multiset_node_t* __multiset_begin(const multiset_t* _this)
{
    multiset_node_t* t = ___multiset_first(_this);
    return is_null(t) ? __multiset_end(_this) : t;
}

static JDSC_INLINE_FORCE
multiset_node_t* _multiset_begin(const multiset_t* _this)
{
    if (unlikely(is_null(_this)))
        return NULL;
    return __multiset_begin(_this);
}

static JDSC_INLINE_FORCE
multiset_node_t* __multiset_next(const multiset_t* _this, const multiset_node_t* node)
{
    struct rb_node* t;

    JDSC_ASSERT(!RB_EMPTY_ROOT(&_this->root));
    JDSC_ASSERT(__multiset_end(_this) != node);

#if JDSC_ITERATOR_ERR_NULL
    if (unlikely(__multiset_end(_this) == node))
        return NULL;
#endif /* JDSC_ITERATOR_ERR_NULL */

    /* The input parameter is `iterator`, and there's no need 
       to check whether it equals `rend` */

    /* This check should come after `end` or `rend`.
       This is a pre-judgment condition for `rb_next` or `rb_prev` */
    JDSC_ASSERT(!RB_EMPTY_NODE(&node->node));

    t = rb_next(&node->node);
    return is_null(t) ? __multiset_end(_this) : multiset_entry(t);
}

static JDSC_INLINE_FORCE
multiset_node_t* _multiset_next(const multiset_t* _this, const multiset_node_t* node)
{
    if (unlikely(is_null(_this) || is_null(node)))
        return NULL;
    return __multiset_next(_this, node);
}

static JDSC_INLINE_FORCE_POLICY
multiset_node_t* __multiset_prev(const multiset_t* _this, const multiset_node_t* node)
{
    struct rb_node* t;

    JDSC_ASSERT(!RB_EMPTY_ROOT(&_this->root));

    if (unlikely(__multiset_end(_this) == node))
        return ___multiset_last(_this); /* Err: since the `ds` is non-empty, 
                                                the return value includes the error case of `NULL` */

    /* This check should come after `end` or `rend`.
       This is a pre-judgment condition for `rb_next` or `rb_prev` */
    JDSC_ASSERT(!RB_EMPTY_NODE(&node->node));

    t = rb_prev(&node->node);
    JDSC_ASSERT(!is_null(t));

#if JDSC_ITERATOR_ERR_NULL
    return is_null(t) ? NULL : multiset_entry(t);
#else
    return multiset_entry(t);
#endif /* JDSC_ITERATOR_ERR_NULL */

}

static JDSC_INLINE_FORCE
multiset_node_t* _multiset_prev(const multiset_t* _this, const multiset_node_t* node)
{
    if (unlikely(is_null(_this) || is_null(node)))
        return NULL;
    return __multiset_prev(_this, node);
}

static JDSC_INLINE_FORCE
multiset_node_t* __multiset_rend(const multiset_t* _this)
{
    return (multiset_node_t*)iterator_rend();
}

static JDSC_INLINE_FORCE
multiset_node_t* __multiset_rbegin(const multiset_t* _this)
{
    multiset_node_t* t = ___multiset_last(_this);
    return is_null(t) ? __multiset_rend(_this) : t;
}

static JDSC_INLINE_FORCE
multiset_node_t* _multiset_rbegin(const multiset_t* _this)
{
    if (unlikely(is_null(_this)))
        return NULL;
    return __multiset_rbegin(_this);
}

static JDSC_INLINE_FORCE
multiset_node_t* __multiset_rnext(const multiset_t* _this, const multiset_node_t* node)
{
    struct rb_node* t;

    JDSC_ASSERT(!RB_EMPTY_ROOT(&_this->root));
    JDSC_ASSERT(__multiset_rend(_this) != node);

#if JDSC_ITERATOR_ERR_NULL
    if (unlikely(__multiset_rend(_this) == node))
        return NULL;
#endif /* JDSC_ITERATOR_ERR_NULL */

    /* The input parameter is `reverse_iterator`, and there's no need 
       to check whether it equals `end` */

    /* This check should come after `end` or `rend`.
       This is a pre-judgment condition for `rb_next` or `rb_prev` */
    JDSC_ASSERT(!RB_EMPTY_NODE(&node->node));

    t = rb_prev(&node->node);
    return is_null(t) ? __multiset_rend(_this) : multiset_entry(t);
}

static JDSC_INLINE_FORCE
multiset_node_t* _multiset_rnext(const multiset_t* _this, const multiset_node_t* node)
{
    if (unlikely(is_null(_this) || is_null(node)))
        return NULL;
    return __multiset_rnext(_this, node);
}

static JDSC_INLINE_FORCE_POLICY
multiset_node_t* __multiset_rprev(const multiset_t* _this, const multiset_node_t* node)
{
    struct rb_node* t;

    JDSC_ASSERT(!RB_EMPTY_ROOT(&_this->root));

    if (unlikely(__multiset_rend(_this) == node))
        return ___multiset_first(_this); /* Err: since the `ds` is non-empty,
                                                the return value includes the error case of `NULL` */

    /* This check should come after `end` or `rend`.
       This is a pre-judgment condition for `rb_next` or `rb_prev` */
    JDSC_ASSERT(!RB_EMPTY_NODE(&node->node));

    t = rb_next(&node->node);
    JDSC_ASSERT(!is_null(t));

#if JDSC_ITERATOR_ERR_NULL
    return is_null(t) ? NULL : multiset_entry(t);
#else
    return multiset_entry(t);
#endif /* JDSC_ITERATOR_ERR_NULL */

}

static JDSC_INLINE_FORCE
multiset_node_t* _multiset_rprev(const multiset_t* _this, const multiset_node_t* node)
{
    if (unlikely(is_null(_this) || is_null(node)))
        return NULL;
    return __multiset_rprev(_this, node);
}

static JDSC_ONLY_WRAPPER JDSC_NO_ITERATOR
multiset_node_t* ___multiset_find(const multiset_t* _this, multiset_key_t key)
{
    struct rb_node* n = _this->root.rb_node;
    multiset_node_t* t;
    multiset_node_t* ret = NULL;

    if (is_null(_this->ops) || is_null(_this->ops->__lt)) {
        while (!is_null(n)) {
            t = multiset_entry(n);

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
            t = multiset_entry(n);

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

static JDSC_INLINE_FORCE_POLICY
multiset_node_t* multiset_find(const multiset_t* _this, multiset_key_t key)
{
    multiset_node_t* t;

    if (unlikely(is_null(_this)))
        return NULL;

    if (unlikely(!is_null(_this->ops) && !is_null(_this->ops->valid_key) && !_this->ops->valid_key(key)))
        return NULL;

    t = ___multiset_find(_this, key);
    return is_null(t) ? __multiset_end(_this) : t;
}

static JDSC_ONLY_WRAPPER JDSC_NO_ITERATOR
multiset_node_t* ___multiset_lower_bound(const multiset_t* _this, multiset_key_t key)
{
    struct rb_node* n = _this->root.rb_node;
    multiset_node_t* t;
    multiset_node_t* eq = NULL;
    multiset_node_t* gt = NULL;

    if (is_null(_this->ops) || is_null(_this->ops->__lt)) {
        while (!is_null(n)) {
            t = multiset_entry(n);

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
            t = multiset_entry(n);

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

static JDSC_INLINE_FORCE_POLICY
multiset_node_t* multiset_lower_bound(const multiset_t* _this, multiset_key_t key)
{
    multiset_node_t* t;

    if (unlikely(is_null(_this)))
        return NULL;

    if (unlikely(!is_null(_this->ops) && !is_null(_this->ops->valid_key) && !_this->ops->valid_key(key)))
        return NULL;

    t = ___multiset_lower_bound(_this, key);
    return is_null(t) ? __multiset_end(_this) : t;
}

static JDSC_ONLY_WRAPPER JDSC_NO_ITERATOR
multiset_node_t* ___multiset_upper_bound(const multiset_t* _this, multiset_key_t key)
{
    struct rb_node* n = _this->root.rb_node;
    multiset_node_t* t;
    multiset_node_t* ret = NULL;

    if (is_null(_this->ops) || is_null(_this->ops->__lt)) {
        while (!is_null(n)) {
            t = multiset_entry(n);

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
            t = multiset_entry(n);

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

static JDSC_INLINE_FORCE_POLICY
multiset_node_t* multiset_upper_bound(const multiset_t* _this, multiset_key_t key)
{
    multiset_node_t* t;

    if (unlikely(is_null(_this)))
        return NULL;

    if (unlikely(!is_null(_this->ops) && !is_null(_this->ops->valid_key) && !_this->ops->valid_key(key)))
        return NULL;

    t = ___multiset_upper_bound(_this, key);
    return is_null(t) ? __multiset_end(_this) : t;
}

static JDSC_ONLY_WRAPPER JDSC_NO_ITERATOR
multiset_node_t* ___multiset_insert(multiset_t* _this, multiset_node_t* node)
{
    struct rb_node** n = &_this->root.rb_node;
    struct rb_node* parent = NULL;
    multiset_node_t* t;

    if (is_null(_this->ops) || is_null(_this->ops->__lt)) {
        while (!is_null(*n)) {
            parent = *n;
            t = multiset_entry(parent);

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
            t = multiset_entry(parent);

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

static
multiset_node_t* multiset_insert(multiset_t* _this, multiset_key_t key)
{
    multiset_node_t* t;
    multiset_node_t* ret;

    if (unlikely(is_null(_this)))
        return NULL;

    if (unlikely(!is_null(_this->ops) && !is_null(_this->ops->valid_key) && !_this->ops->valid_key(key)))
        return NULL;

    t = (multiset_node_t*)p_calloc(1, sizeof(multiset_node_t));
    if (unlikely(is_null(t)))
        return NULL;

    if (is_null(_this->ops) || is_null(_this->ops->copy_key)) {
        t->key = key;
    } else {
        if (unlikely(!_this->ops->copy_key(key, &t->key)))
            goto err_key;
    }

    ret = ___multiset_insert(_this, t);
    if (t != ret)
        goto err;
    return t;

err:
    if (!is_null(_this->ops) && !is_null(_this->ops->free_key))
        _this->ops->free_key(&t->key);
err_key:
    p_free(t);
    return NULL;
}

static JDSC_INLINE_FORCE JDSC_ONLY_WRAPPER JDSC_NO_ITERATOR
multiset_node_t* ___multiset_erase(multiset_t* _this, multiset_node_t* pos)
{
    rb_erase(&pos->node, &_this->root);
    _this->size--;
    return pos;
}

static
multiset_node_t* multiset_erase(multiset_t* _this, multiset_node_t* pos)
{
    multiset_node_t* t;

    if (unlikely(is_null(_this) || is_null(pos)))
        return NULL;

    /* The input parameter is `iterator`, and there's no need 
       to check whether it equals `rend` */
    if (unlikely(RB_EMPTY_ROOT(&_this->root) || __multiset_end(_this) == pos)/* || __multiset_rend(_this) == pos*/)
        return NULL;

    t = __multiset_next(_this, pos);
    if (unlikely(is_null(t)))
        return NULL;

    ___multiset_erase(_this, pos);

    if (!is_null(_this->ops) && !is_null(_this->ops->free_key))
        _this->ops->free_key(&pos->key);

    p_free(pos);

    return t;
}

static
multiset_size_t multiset_remove(multiset_t* _this, multiset_key_t key)
{
    multiset_size_t ret = 0;
    multiset_node_t* t;

    if (unlikely(is_null(_this)))
        return -1;

    t = multiset_find(_this, key);
    if (unlikely(is_null(t)))
        return -1;

    if (__multiset_end(_this) == t)
        return 0;

    if (is_null(_this->ops) || is_null(_this->ops->__lt)) {
        for (; __multiset_end(_this) != t; ) {
            if (key != t->key)
                break;

            t = multiset_erase(_this, t);
            ret++;
        }
    } else {
        for (; __multiset_end(_this) != t; ) {
            if (_this->ops->__lt(key, t->key) || _this->ops->__lt(t->key, key))
                break;

            t = multiset_erase(_this, t);
            ret++;
        }
    }

    return ret;
}

static
multiset_size_t multiset_remove_if(multiset_t* _this, remove_if_condition_k cond)
{
    multiset_size_t ret = 0;
    multiset_node_t* t;

    if (unlikely(is_null(_this) || is_null(cond)))
        return -1;

    for (t = __multiset_begin(_this); __multiset_end(_this) != t; ) {
        if (!cond(t->key)) {
            t = __multiset_next(_this, t);
            continue;
        }

        t = multiset_erase(_this, t);
        ret++;
    }

    return ret;
}

static
multiset_size_t multiset_clear(multiset_t* _this)
{
    multiset_size_t ret = 0;
    multiset_node_t* t;

    if (unlikely(is_null(_this)))
        return -1;

    for (t = __multiset_begin(_this); __multiset_end(_this) != t; ) {
        t = multiset_erase(_this, t);
        ret++;
    }

    return ret;
}

multiset_t* __multiset_new(const class_multiset_ops_t* ops)
{
    multiset_t* multiset = (multiset_t*)p_calloc(1, sizeof(multiset_t));
    if (is_null(multiset))
        return NULL;

    if (!is_null(ops) && !is_null(ops->copy_key)) {
        JDSC_ASSERT(!is_null(ops->free_key));
        if (is_null(ops->free_key)) {
            pr_err("%s: ops->copy_key provided but ops->free_key is null!", __func__);
            goto err;
        }
    }

    multiset->ops  = ops;
    multiset->root = RB_ROOT;
    return multiset;

err:
    p_free(multiset);
    return NULL;
}

void __multiset_delete(multiset_t** _this)
{
    if (is_null(_this) || is_null(*_this))
        return;

    multiset_clear(*_this);
    p_free(*_this);
}

#if 0
typedef multiset_iterator_t* (*fp_end)(const multiset_t* _this);
typedef multiset_iterator_t* (*fp_begin)(const multiset_t* _this);
typedef multiset_iterator_t* (*fp_next)(const multiset_t* _this, const multiset_iterator_t* iterator);
typedef multiset_iterator_t* (*fp_prev)(const multiset_t* _this, const multiset_iterator_t* iterator);
typedef multiset_r_iterator_t* (*fp_rend)(const multiset_t* _this);
typedef multiset_r_iterator_t* (*fp_rbegin)(const multiset_t* _this);
typedef multiset_r_iterator_t* (*fp_rnext)(const multiset_t* _this, const multiset_r_iterator_t* r_iterator);
typedef multiset_r_iterator_t* (*fp_rprev)(const multiset_t* _this, const multiset_r_iterator_t* r_iterator);
typedef multiset_iterator_t* (*fp_find)(const multiset_t* _this, multiset_key_t key);
typedef multiset_iterator_t* (*fp_lower_bound)(const multiset_t* _this, multiset_key_t key);
typedef multiset_iterator_t* (*fp_upper_bound)(const multiset_t* _this, multiset_key_t key);
typedef multiset_iterator_t* (*fp_insert)(multiset_t* _this, multiset_key_t key);
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

#else

const class_multiset_t* class_multiset_ins(void)
{
    static const class_multiset_t ins = {
        .size        = cmultiset_size,
        .count       = cmultiset_count,
        .end         = cmultiset_end,
        .begin       = cmultiset_begin,
        .next        = cmultiset_next,
        .prev        = cmultiset_prev,
        .rend        = cmultiset_rend,
        .rbegin      = cmultiset_rbegin,
        .rnext       = cmultiset_rnext,
        .rprev       = cmultiset_rprev,
        .find        = cmultiset_find,
        .lower_bound = cmultiset_lower_bound,
        .upper_bound = cmultiset_upper_bound,
        .insert      = cmultiset_insert,
        .erase       = cmultiset_erase,
        .remove      = cmultiset_remove,
        .remove_if   = cmultiset_remove_if,
        .clear       = cmultiset_clear,
    };
    return &ins;
}
#endif /* 0 */





multiset_size_t cmultiset_size(const multiset_t* _this)
{
    return _multiset_size(_this);
}

multiset_count_t cmultiset_count(const multiset_t* _this, multiset_key_t key)
{
    return multiset_count(_this, key);
}

multiset_iterator_t cmultiset_end(const multiset_t* _this)
{
    multiset_iterator_t it;
    it.d = (multiset_data_t*)__multiset_end(_this);
    return it;
}

multiset_iterator_t cmultiset_begin(const multiset_t* _this)
{
    multiset_iterator_t it;
    it.d = (multiset_data_t*)_multiset_begin(_this);
    return it;
}

multiset_iterator_t cmultiset_next(const multiset_t* _this, const multiset_iterator_t iterator)
{
    multiset_iterator_t it;
    it.d = (multiset_data_t*)_multiset_next(_this, (const multiset_node_t*)iterator.d);
    return it;
}

multiset_iterator_t cmultiset_prev(const multiset_t* _this, const multiset_iterator_t iterator)
{
    multiset_iterator_t it;
    it.d = (multiset_data_t*)_multiset_prev(_this, (const multiset_node_t*)iterator.d);
    return it;
}

multiset_r_iterator_t cmultiset_rend(const multiset_t* _this)
{
    multiset_r_iterator_t it;
    it.d = (multiset_data_t*)__multiset_rend(_this);
    return it;
}

multiset_r_iterator_t cmultiset_rbegin(const multiset_t* _this)
{
    multiset_r_iterator_t it;
    it.d = (multiset_data_t*)_multiset_rbegin(_this);
    return it;
}

multiset_r_iterator_t cmultiset_rnext(const multiset_t* _this, const multiset_r_iterator_t r_iterator)
{
    multiset_r_iterator_t it;
    it.d = (multiset_data_t*)_multiset_rnext(_this, (const multiset_node_t*)r_iterator.d);
    return it;
}

multiset_r_iterator_t cmultiset_rprev(const multiset_t* _this, const multiset_r_iterator_t r_iterator)
{
    multiset_r_iterator_t it;
    it.d = (multiset_data_t*)_multiset_rprev(_this, (const multiset_node_t*)r_iterator.d);
    return it;
}

multiset_iterator_t cmultiset_find(const multiset_t* _this, multiset_key_t key)
{
    multiset_iterator_t it;
    it.d = (multiset_data_t*)multiset_find(_this, key);
    return it;
}

multiset_iterator_t cmultiset_lower_bound(const multiset_t* _this, multiset_key_t key)
{
    multiset_iterator_t it;
    it.d = (multiset_data_t*)multiset_lower_bound(_this, key);
    return it;
}

multiset_iterator_t cmultiset_upper_bound(const multiset_t* _this, multiset_key_t key)
{
    multiset_iterator_t it;
    it.d = (multiset_data_t*)multiset_upper_bound(_this, key);
    return it;
}

multiset_iterator_t cmultiset_insert(multiset_t* _this, multiset_key_t key)
{
    multiset_iterator_t it;
    it.d = (multiset_data_t*)multiset_insert(_this, key);
    return it;
}

multiset_iterator_t cmultiset_erase(multiset_t* _this, multiset_iterator_t iterator)
{
    multiset_iterator_t it;
    it.d = (multiset_data_t*)multiset_erase(_this, (multiset_node_t*)iterator.d);
    return it;
}

multiset_size_t cmultiset_remove(multiset_t* _this, multiset_key_t key)
{
    return multiset_remove(_this, key);
}

multiset_size_t cmultiset_remove_if(multiset_t* _this, remove_if_condition_k cond)
{
    return multiset_remove_if(_this, cond);
}

multiset_size_t cmultiset_clear(multiset_t* _this)
{
    return multiset_clear(_this);
}

