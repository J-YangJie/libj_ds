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

#include <_log.h>
#include <_memory.h>
#include <_compiler.h>
#include <linux/_types.h>
#include <linux/rbtree.h>
#include <linux/_compiler.h>
#include <iterator/iterator_inter.h>

typedef struct set_node {
    set_key_t key;
    struct rb_node node;
} set_node_t;

JDSC_STATIC_ASSERT(offsetof(set_node_t, key) == 0,          "set node key vs iterator data offset mismatch");
JDSC_STATIC_ASSERT(sizeof(set_key_t) == sizeof(set_data_t), "set node key width vs iterator data width mismatch");
JDSC_STATIC_ASSERT(sizeof(set_data_t) == sizeof(char*),     "iterator data/sdata union width mismatch");

struct set {
    const class_set_ops_t* ops;
    struct rb_root root;
    set_size_t size;
};

#define TAG "[set]"

#define set_entry(ptr) rb_entry((ptr), struct set_node, node)

static JDSC_INLINE_FORCE_POLICY set_node_t* set_find(const set_t* _this, set_key_t key);
static JDSC_INLINE_FORCE set_node_t* __set_end(const set_t* _this);

static JDSC_INLINE_FORCE
set_size_t __set_size(const set_t* _this)
{
    return _this->size;
}

static JDSC_INLINE_FORCE
set_size_t _set_size(const set_t* _this)
{
    if (unlikely(is_null(_this)))
        return -1;
    return __set_size(_this);
}

static JDSC_INLINE_FORCE_POLICY
set_count_t set_count(const set_t* _this, set_key_t key)
{
    set_node_t* t = set_find(_this, key);
    return is_null(t) ? -1 : (__set_end(_this) == t ? 0 : 1);
}

static JDSC_INLINE_FORCE JDSC_ONLY_WRAPPER JDSC_NO_ITERATOR
set_node_t* ___set_first(const set_t* _this)
{
    struct rb_node* t = rb_first(&_this->root);
    return is_null(t) ? NULL : set_entry(t);
}

static JDSC_INLINE_FORCE JDSC_ONLY_WRAPPER JDSC_NO_ITERATOR
set_node_t* ___set_last(const set_t* _this)
{
    struct rb_node* t = rb_last(&_this->root);
    return is_null(t) ? NULL : set_entry(t);
}

static JDSC_INLINE_FORCE
set_node_t* __set_end(const set_t* _this)
{
    return (set_node_t*)iterator_end();
}

static JDSC_INLINE_FORCE
set_node_t* __set_begin(const set_t* _this)
{
    set_node_t* t = ___set_first(_this);
    return is_null(t) ? __set_end(_this) : t;
}

static JDSC_INLINE_FORCE
set_node_t* _set_begin(const set_t* _this)
{
    if (unlikely(is_null(_this)))
        return NULL;
    return __set_begin(_this);
}

static JDSC_INLINE_FORCE
set_node_t* __set_next(const set_t* _this, const set_node_t* node)
{
    struct rb_node* t;

    JDSC_ASSERT(!RB_EMPTY_ROOT(&_this->root));
    JDSC_ASSERT(__set_end(_this) != node);

#if JDSC_ITERATOR_ERR_NULL
    if (unlikely(__set_end(_this) == node))
        return NULL;
#endif /* JDSC_ITERATOR_ERR_NULL */

    /* The input parameter is `iterator`, and there's no need 
       to check whether it equals `rend` */

    /* This check should come after `end` or `rend`.
       This is a pre-judgment condition for `rb_next` or `rb_prev` */
    JDSC_ASSERT(!RB_EMPTY_NODE(&node->node));

    t = rb_next(&node->node);
    return is_null(t) ? __set_end(_this) : set_entry(t);
}

static JDSC_INLINE_FORCE
set_node_t* _set_next(const set_t* _this, const set_node_t* node)
{
    if (unlikely(is_null(_this) || is_null(node)))
        return NULL;
    return __set_next(_this, node);
}

static JDSC_INLINE_FORCE_POLICY
set_node_t* __set_prev(const set_t* _this, const set_node_t* node)
{
    struct rb_node* t;

    JDSC_ASSERT(!RB_EMPTY_ROOT(&_this->root));

    if (unlikely(__set_end(_this) == node))
        return ___set_last(_this); /* Err: since the `ds` is non-empty, 
                                            the return value includes the error case of `NULL` */

    /* This check should come after `end` or `rend`.
       This is a pre-judgment condition for `rb_next` or `rb_prev` */
    JDSC_ASSERT(!RB_EMPTY_NODE(&node->node));

    t = rb_prev(&node->node);
    JDSC_ASSERT(!is_null(t));

#if JDSC_ITERATOR_ERR_NULL
    return is_null(t) ? NULL : set_entry(t);
#else
    return set_entry(t);
#endif /* JDSC_ITERATOR_ERR_NULL */

}

static JDSC_INLINE_FORCE
set_node_t* _set_prev(const set_t* _this, const set_node_t* node)
{
    if (unlikely(is_null(_this) || is_null(node)))
        return NULL;
    return __set_prev(_this, node);
}

static JDSC_INLINE_FORCE
set_node_t* __set_rend(const set_t* _this)
{
    return (set_node_t*)iterator_rend();
}

static JDSC_INLINE_FORCE
set_node_t* __set_rbegin(const set_t* _this)
{
    set_node_t* t = ___set_last(_this);
    return is_null(t) ? __set_rend(_this) : t;
}

static JDSC_INLINE_FORCE
set_node_t* _set_rbegin(const set_t* _this)
{
    if (unlikely(is_null(_this)))
        return NULL;
    return __set_rbegin(_this);
}

static JDSC_INLINE_FORCE
set_node_t* __set_rnext(const set_t* _this, const set_node_t* node)
{
    struct rb_node* t;

    JDSC_ASSERT(!RB_EMPTY_ROOT(&_this->root));
    JDSC_ASSERT(__set_rend(_this) != node);

#if JDSC_ITERATOR_ERR_NULL
    if (unlikely(__set_rend(_this) == node))
        return NULL;
#endif /* JDSC_ITERATOR_ERR_NULL */

    /* The input parameter is `reverse_iterator`, and there's no need 
       to check whether it equals `end` */

    /* This check should come after `end` or `rend`.
       This is a pre-judgment condition for `rb_next` or `rb_prev` */
    JDSC_ASSERT(!RB_EMPTY_NODE(&node->node));

    t = rb_prev(&node->node);
    return is_null(t) ? __set_rend(_this) : set_entry(t);
}

static JDSC_INLINE_FORCE
set_node_t* _set_rnext(const set_t* _this, const set_node_t* node)
{
    if (unlikely(is_null(_this) || is_null(node)))
        return NULL;
    return __set_rnext(_this, node);
}

static JDSC_INLINE_FORCE_POLICY
set_node_t* __set_rprev(const set_t* _this, const set_node_t* node)
{
    struct rb_node* t;

    JDSC_ASSERT(!RB_EMPTY_ROOT(&_this->root));

    if (unlikely(__set_rend(_this) == node))
        return ___set_first(_this); /* Err: since the `ds` is non-empty, 
                                            the return value includes the error case of `NULL` */

    /* This check should come after `end` or `rend`.
       This is a pre-judgment condition for `rb_next` or `rb_prev` */
    JDSC_ASSERT(!RB_EMPTY_NODE(&node->node));

    t = rb_next(&node->node);
    JDSC_ASSERT(!is_null(t));

#if JDSC_ITERATOR_ERR_NULL
    return is_null(t) ? NULL : set_entry(t);
#else
    return set_entry(t);
#endif /* JDSC_ITERATOR_ERR_NULL */

}

static JDSC_INLINE_FORCE
set_node_t* _set_rprev(const set_t* _this, const set_node_t* node)
{
    if (unlikely(is_null(_this) || is_null(node)))
        return NULL;
    return __set_rprev(_this, node);
}

static JDSC_ONLY_WRAPPER JDSC_NO_ITERATOR
set_node_t* ___set_find(const set_t* _this, set_key_t key)
{
    struct rb_node* n = _this->root.rb_node;
    set_node_t* t;

    if (is_null(_this->ops) || is_null(_this->ops->__lt)) {
        while (!is_null(n)) {
            t = set_entry(n);

            if (key < t->key)
                n = n->rb_left;
            else if (key > t->key)
                n = n->rb_right;
            else
                return t;
        }
    } else {
        while (!is_null(n)) {
            t = set_entry(n);

            if (_this->ops->__lt(key, t->key))
                n = n->rb_left;
            else if (_this->ops->__lt(t->key, key))
                n = n->rb_right;
            else
                return t;
        }
    }

    return NULL;
}

static JDSC_INLINE_FORCE_POLICY
set_node_t* set_find(const set_t* _this, set_key_t key)
{
    set_node_t* t;

    if (unlikely(is_null(_this)))
        return NULL;

    if (unlikely(!is_null(_this->ops) && !is_null(_this->ops->valid_key) && !_this->ops->valid_key(key)))
        return NULL;

    t = ___set_find(_this, key);
    return is_null(t) ? __set_end(_this) : t;
}

static JDSC_ONLY_WRAPPER JDSC_NO_ITERATOR
set_node_t* ___set_lower_bound(const set_t* _this, set_key_t key)
{
    struct rb_node* n = _this->root.rb_node;
    set_node_t* t;
    set_node_t* ret = NULL;

    if (is_null(_this->ops) || is_null(_this->ops->__lt)) {
        while (!is_null(n)) {
            t = set_entry(n);

            if (key < t->key) {
                n = n->rb_left;
                ret = t;
            } else if (key > t->key) {
                n = n->rb_right;
            } else {
                return t;
            }
        }
    } else {
        while (!is_null(n)) {
            t = set_entry(n);

            if (_this->ops->__lt(key, t->key)) {
                n = n->rb_left;
                ret = t;
            } else if (_this->ops->__lt(t->key, key)) {
                n = n->rb_right;
            } else {
                return t;
            }
        }
    }

    return ret;
}

static JDSC_INLINE_FORCE_POLICY
set_node_t* set_lower_bound(const set_t* _this, set_key_t key)
{
    set_node_t* t;

    if (unlikely(is_null(_this)))
        return NULL;

    if (unlikely(!is_null(_this->ops) && !is_null(_this->ops->valid_key) && !_this->ops->valid_key(key)))
        return NULL;

    t = ___set_lower_bound(_this, key);
    return is_null(t) ? __set_end(_this) : t;
}

static JDSC_ONLY_WRAPPER JDSC_NO_ITERATOR
set_node_t* ___set_upper_bound(const set_t* _this, set_key_t key)
{
    struct rb_node* n = _this->root.rb_node;
    set_node_t* t;
    set_node_t* ret = NULL;

    if (is_null(_this->ops) || is_null(_this->ops->__lt)) {
        while (!is_null(n)) {
            t = set_entry(n);

            if (key < t->key) {
                n = n->rb_left;
                ret = t;
            } else if (key > t->key) {
                n = n->rb_right;
            } else {
                n = rb_next(n);
                return is_null(n) ? NULL : set_entry(n);
            }
        }
    } else {
        while (!is_null(n)) {
            t = set_entry(n);

            if (_this->ops->__lt(key, t->key)) {
                n = n->rb_left;
                ret = t;
            } else if (_this->ops->__lt(t->key, key)) {
                n = n->rb_right;
            } else {
                n = rb_next(n);
                return is_null(n) ? NULL : set_entry(n);
            }
        }
    }

    return ret;
}

static JDSC_INLINE_FORCE_POLICY
set_node_t* set_upper_bound(const set_t* _this, set_key_t key)
{
    set_node_t* t;

    if (unlikely(is_null(_this)))
        return NULL;

    if (unlikely(!is_null(_this->ops) && !is_null(_this->ops->valid_key) && !_this->ops->valid_key(key)))
        return NULL;

    t = ___set_upper_bound(_this, key);
    return is_null(t) ? __set_end(_this) : t;
}

static JDSC_ONLY_WRAPPER JDSC_NO_ITERATOR
set_node_t* ___set_insert(set_t* _this, set_node_t* node)
{
    struct rb_node** n = &_this->root.rb_node;
    struct rb_node* parent = NULL;
    set_node_t* t;

    if (is_null(_this->ops) || is_null(_this->ops->__lt)) {
        while (!is_null(*n)) {
            parent = *n;
            t = set_entry(parent);

            if (node->key < t->key)
                n = &parent->rb_left;
            else if (node->key > t->key)
                n = &parent->rb_right;
            else
                return t;
        }
    } else {
        while (!is_null(*n)) {
            parent = *n;
            t = set_entry(parent);

            if (_this->ops->__lt(node->key, t->key))
                n = &parent->rb_left;
            else if (_this->ops->__lt(t->key, node->key))
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

static
set_node_t* set_insert(set_t* _this, set_key_t key)
{
    set_node_t* t;
    set_node_t* ret;

    if (unlikely(is_null(_this)))
        return NULL;

    if (unlikely(!is_null(_this->ops) && !is_null(_this->ops->valid_key) && !_this->ops->valid_key(key)))
        return NULL;

    t = (set_node_t*)p_calloc(1, sizeof(set_node_t));
    if (unlikely(is_null(t)))
        return NULL;

    if (is_null(_this->ops) || is_null(_this->ops->copy_key)) {
        t->key = key;
    } else {
        if (unlikely(!_this->ops->copy_key(key, &t->key)))
            goto err_key;
    }

    ret = ___set_insert(_this, t);
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
set_node_t* ___set_erase(set_t* _this, set_node_t* pos)
{
    rb_erase(&pos->node, &_this->root);
    _this->size--;
    return pos;
}

static
set_node_t* set_erase(set_t* _this, set_node_t* pos)
{
    set_node_t* t;

    if (unlikely(is_null(_this) || is_null(pos)))
        return NULL;

    /* The input parameter is `iterator`, and there's no need 
       to check whether it equals `rend` */
    if (unlikely(RB_EMPTY_ROOT(&_this->root) || __set_end(_this) == pos)/* || __set_rend(_this) == pos*/)
        return NULL;

    t = __set_next(_this, pos);
    if (unlikely(is_null(t)))
        return NULL;

    ___set_erase(_this, pos);

    if (!is_null(_this->ops) && !is_null(_this->ops->free_key))
        _this->ops->free_key(&pos->key);

    p_free(pos);

    return t;
}

static
set_size_t set_remove(set_t* _this, set_key_t key)
{
    set_node_t* t;

    if (unlikely(is_null(_this)))
        return -1;

    t = set_find(_this, key);
    if (unlikely(is_null(t)))
        return -1;

    if (__set_end(_this) == t)
        return 0;

    ___set_erase(_this, t);

    if (!is_null(_this->ops) && !is_null(_this->ops->free_key))
        _this->ops->free_key(&t->key);

    p_free(t);

    return 1;
}

static
set_size_t set_remove_if(set_t* _this, remove_if_condition_k cond)
{
    set_size_t ret = 0;
    set_node_t* t;

    if (unlikely(is_null(_this) || is_null(cond)))
        return -1;

    for (t = __set_begin(_this); __set_end(_this) != t; ) {
        if (!cond(t->key)) {
            t = __set_next(_this, t);
            continue;
        }

        t = set_erase(_this, t);
        ret++;
    }

    return ret;
}

static
set_size_t set_clear(set_t* _this)
{
    set_size_t ret = 0;
    set_node_t* t;

    if (unlikely(is_null(_this)))
        return -1;

    for (t = __set_begin(_this); __set_end(_this) != t; ) {
        t = set_erase(_this, t);
        ret++;
    }

    return ret;
}

set_t* __set_new(const class_set_ops_t* ops)
{
    set_t* set = (set_t*)p_calloc(1, sizeof(set_t));
    if (is_null(set))
        return NULL;

    if (!is_null(ops) && !is_null(ops->copy_key)) {
        JDSC_ASSERT(!is_null(ops->free_key));
        if (is_null(ops->free_key)) {
            pr_err("%s: ops->copy_key provided but ops->free_key is null!", __func__);
            goto err;
        }
    }

    set->ops  = ops;
    set->root = RB_ROOT;
    return set;

err:
    p_free(set);
    return NULL;
}

void __set_delete(set_t** _this)
{
    if (is_null(_this) || is_null(*_this))
        return;

    set_clear(*_this);
    p_free(*_this);
}

#if 0
typedef set_iterator_t* (*fp_end)(const set_t* _this);
typedef set_iterator_t* (*fp_begin)(const set_t* _this);
typedef set_iterator_t* (*fp_next)(const set_t* _this, const set_iterator_t* iterator);
typedef set_iterator_t* (*fp_prev)(const set_t* _this, const set_iterator_t* iterator);
typedef set_r_iterator_t* (*fp_rend)(const set_t* _this);
typedef set_r_iterator_t* (*fp_rbegin)(const set_t* _this);
typedef set_r_iterator_t* (*fp_rnext)(const set_t* _this, const set_r_iterator_t* r_iterator);
typedef set_r_iterator_t* (*fp_rprev)(const set_t* _this, const set_r_iterator_t* r_iterator);
typedef set_iterator_t* (*fp_find)(const set_t* _this, set_key_t key);
typedef set_iterator_t* (*fp_lower_bound)(const set_t* _this, set_key_t key);
typedef set_iterator_t* (*fp_upper_bound)(const set_t* _this, set_key_t key);
typedef set_iterator_t* (*fp_insert)(set_t* _this, set_key_t key);
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

#else

const class_set_t* class_set_ins(void)
{
    static const class_set_t ins = {
        .size            = cset_size,
        .count           = cset_count,
        .end             = cset_end,
        .begin           = cset_begin,
        .next            = cset_next,
        .prev            = cset_prev,
        .rend            = cset_rend,
        .rbegin          = cset_rbegin,
        .rnext           = cset_rnext,
        .rprev           = cset_rprev,
        .find            = cset_find,
        .lower_bound     = cset_lower_bound,
        .upper_bound     = cset_upper_bound,
        .insert          = cset_insert,
        .erase           = cset_erase,
        .remove          = cset_remove,
        .remove_if       = cset_remove_if,
        .clear           = cset_clear,
    };
    return &ins;
}
#endif /* 0 */





set_size_t cset_size(const set_t* _this)
{
    return _set_size(_this);
}

set_count_t cset_count(const set_t* _this, set_key_t key)
{
    return set_count(_this, key);
}

set_iterator_t cset_end(const set_t* _this)
{
    set_iterator_t it;
    it.d = (set_data_t*)__set_end(_this);
    return it;
}

set_iterator_t cset_begin(const set_t* _this)
{
    set_iterator_t it;
    it.d = (set_data_t*)_set_begin(_this);
    return it;
}

set_iterator_t cset_next(const set_t* _this, const set_iterator_t iterator)
{
    set_iterator_t it;
    it.d = (set_data_t*)_set_next(_this, (const set_node_t*)iterator.d);
    return it;
}

set_iterator_t cset_prev(const set_t* _this, const set_iterator_t iterator)
{
    set_iterator_t it;
    it.d = (set_data_t*)_set_prev(_this, (const set_node_t*)iterator.d);
    return it;
}

set_r_iterator_t cset_rend(const set_t* _this)
{
    set_r_iterator_t it;
    it.d = (set_data_t*)__set_rend(_this);
    return it;
}

set_r_iterator_t cset_rbegin(const set_t* _this)
{
    set_r_iterator_t it;
    it.d = (set_data_t*)_set_rbegin(_this);
    return it;
}

set_r_iterator_t cset_rnext(const set_t* _this, const set_r_iterator_t r_iterator)
{
    set_r_iterator_t it;
    it.d = (set_data_t*)_set_rnext(_this, (const set_node_t*)r_iterator.d);
    return it;
}

set_r_iterator_t cset_rprev(const set_t* _this, const set_r_iterator_t r_iterator)
{
    set_r_iterator_t it;
    it.d = (set_data_t*)_set_rprev(_this, (const set_node_t*)r_iterator.d);
    return it;
}

set_iterator_t cset_find(const set_t* _this, set_key_t key)
{
    set_iterator_t it;
    it.d = (set_data_t*)set_find(_this, key);
    return it;
}

set_iterator_t cset_lower_bound(const set_t* _this, set_key_t key)
{
    set_iterator_t it;
    it.d = (set_data_t*)set_lower_bound(_this, key);
    return it;
}

set_iterator_t cset_upper_bound(const set_t* _this, set_key_t key)
{
    set_iterator_t it;
    it.d = (set_data_t*)set_upper_bound(_this, key);
    return it;
}

set_iterator_t cset_insert(set_t* _this, set_key_t key)
{
    set_iterator_t it;
    it.d = (set_data_t*)set_insert(_this, key);
    return it;
}

set_iterator_t cset_erase(set_t* _this, set_iterator_t iterator)
{
    set_iterator_t it;
    it.d = (set_data_t*)set_erase(_this, (set_node_t*)iterator.d);
    return it;
}

set_size_t cset_remove(set_t* _this, set_key_t key)
{
    return set_remove(_this, key);
}

set_size_t cset_remove_if(set_t* _this, remove_if_condition_k cond)
{
    return set_remove_if(_this, cond);
}

set_size_t cset_clear(set_t* _this)
{
    return set_clear(_this);
}

