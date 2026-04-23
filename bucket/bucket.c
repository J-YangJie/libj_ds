/*
  Bucket Implementations
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

#include <bucket/bucket.h>

#include <string.h>
#include <_log.h>
#include <_memory.h>
#include <linux/list.h>
#include <linux/rbtree.h>
#include <linux/_compiler.h>
#include <iterator/iterator.h>

#ifndef TAG
#define TAG "[hashmap]"
#endif /* TAG */

#define bucket_rb_entry(ptr)    rb_entry((ptr), struct bucket_node, ds_node.rb_node)
#define bucket_hl_entry(ptr) hlist_entry((ptr), struct bucket_node, ds_node.hl_node)

/* Size */
static __always_inline bucket_size_t __bucket_size(const bucket_t* _this)
{
    return _this->size;
}

static __always_inline bucket_size_t _bucket_size(const bucket_t* _this)
{
    if (unlikely(is_null(_this)))
        return -1;
    return __bucket_size(_this);
}



/* End */
static __always_inline bucket_node_t* __bucket_end(const bucket_t* _this)
{
    return (bucket_node_t*)iterator_end();
}

static __always_inline bucket_node_t* __bucket_rend(const bucket_t* _this)
{
    return (bucket_node_t*)iterator_rend();
}



/* iterator */
static /* __always_inline */ inline bucket_node_t* __bucket_rb_first(const bucket_t* _this)
{
    struct rb_node* t = rb_first(&_this->ds.rb);
    return is_null(t) ? NULL : bucket_rb_entry(t);
}

static /* __always_inline */ inline bucket_node_t* __bucket_rb_last(const bucket_t* _this)
{
    struct rb_node* t = rb_last(&_this->ds.rb);
    return is_null(t) ? NULL : bucket_rb_entry(t);
}

static /* __always_inline */ inline bucket_node_t* __bucket_rb_begin(const bucket_t* _this)
{
    bucket_node_t* t = __bucket_rb_first(_this);
    return is_null(t) ? __bucket_end(_this) : t;
}

static /* __always_inline */ inline bucket_node_t* _bucket_rb_begin(const bucket_t* _this)
{
    if (unlikely(is_null(_this)))
        return NULL;
    return __bucket_rb_begin(_this);
}

static /* __always_inline */ inline bucket_node_t* __bucket_rb_next(const bucket_t* _this, const bucket_node_t* node)
{
    struct rb_node* t;

    if (RB_EMPTY_ROOT(&_this->ds.rb) || __bucket_end(_this) == node)
        return __bucket_end(_this);

    /* The input parameter is `iterator`, and there's no need 
       to check whether it equals `rend` */

    if (unlikely(RB_EMPTY_NODE(&node->ds_node.rb_node)))
        return NULL; /* Err: node(empty node) doesn't belong to current tree */

    /* The checking of parameter errors in `rb_next` has been replaced by `RB_EMPTY_NODE`.
       Therefore, when `rb_next` returns `NULL`, it means there are no next node. */
    t = rb_next(&node->ds_node.rb_node);
    return is_null(t) ? __bucket_end(_this) : bucket_rb_entry(t);
}

static /* __always_inline */ inline bucket_node_t* _bucket_rb_next(const bucket_t* _this, const bucket_node_t* node)
{
    if (unlikely(is_null(_this) || is_null(node)))
        return NULL;
    return __bucket_rb_next(_this, node);
}

static /* __always_inline */ inline bucket_node_t* __bucket_rb_prev(const bucket_t* _this, const bucket_node_t* node)
{
    struct rb_node* t;

    if (RB_EMPTY_ROOT(&_this->ds.rb))
        return __bucket_end(_this);

    if (__bucket_end(_this) == node)
        return __bucket_rb_last(_this); /* Err: since the `ds` is non-empty, the return value 
                                                includes the error case of `NULL` */

    if (unlikely(RB_EMPTY_NODE(&node->ds_node.rb_node)))
        return NULL; /* Err: node(empty node) doesn't belong to current tree */

    /* The checking of parameter errors in `rb_prev` has been replaced by `RB_EMPTY_NODE`.
       Therefore, when `rb_prev` returns `NULL`, it means there are no prev node. */
    t = rb_prev(&node->ds_node.rb_node);
    return is_null(t) ? __bucket_end(_this) : bucket_rb_entry(t);
}

static /* __always_inline */ inline bucket_node_t* _bucket_rb_prev(const bucket_t* _this, const bucket_node_t* node)
{
    if (unlikely(is_null(_this) || is_null(node)))
        return NULL;
    return __bucket_rb_prev(_this, node);
}

static /* __always_inline */ inline bucket_node_t* __bucket_rb_rbegin(const bucket_t* _this)
{
    bucket_node_t* t = __bucket_rb_last(_this);
    return is_null(t) ? __bucket_rend(_this) : t;
}

static /* __always_inline */ inline bucket_node_t* _bucket_rb_rbegin(const bucket_t* _this)
{
    if (unlikely(is_null(_this)))
        return NULL;
    return __bucket_rb_rbegin(_this);
}

static /* __always_inline */ inline bucket_node_t* __bucket_rb_rnext(const bucket_t* _this, const bucket_node_t* node)
{
    struct rb_node* t;

    if (RB_EMPTY_ROOT(&_this->ds.rb) || __bucket_rend(_this) == node)
        return __bucket_rend(_this);

    /* The input parameter is `reverse_iterator`, and there's no need 
       to check whether it equals `end` */

    if (unlikely(RB_EMPTY_NODE(&node->ds_node.rb_node)))
        return NULL; /* Err: node(empty node) doesn't belong to current tree */

    /* The checking of parameter errors in `rb_prev` has been replaced by `RB_EMPTY_NODE`.
       Therefore, when `rb_prev` returns `NULL`, it means there are no next node. */
    t = rb_prev(&node->ds_node.rb_node);
    return is_null(t) ? __bucket_rend(_this) : bucket_rb_entry(t);
}

static /* __always_inline */ inline bucket_node_t* _bucket_rb_rnext(const bucket_t* _this, const bucket_node_t* node)
{
    if (unlikely(is_null(_this) || is_null(node)))
        return NULL;
    return __bucket_rb_rnext(_this, node);
}

static /* __always_inline */ inline bucket_node_t* __bucket_rb_rprev(const bucket_t* _this, const bucket_node_t* node)
{
    struct rb_node* t;

    if (RB_EMPTY_ROOT(&_this->ds.rb))
        return __bucket_rend(_this);

    if (__bucket_rend(_this) == node)
        return __bucket_rb_first(_this); /* Err: since the `ds` is non-empty, the return value 
                                                 includes the error case of `NULL` */

    if (unlikely(RB_EMPTY_NODE(&node->ds_node.rb_node)))
        return NULL; /* Err: node(empty node) doesn't belong to current tree */

    /* The checking of parameter errors in `rb_next` has been replaced by `RB_EMPTY_NODE`.
       Therefore, when `rb_next` returns `NULL`, it means there are no prev node. */
    t = rb_next(&node->ds_node.rb_node);
    return is_null(t) ? __bucket_rend(_this) : bucket_rb_entry(t);
}

static /* __always_inline */ inline bucket_node_t* _bucket_rb_rprev(const bucket_t* _this, const bucket_node_t* node)
{
    if (unlikely(is_null(_this) || is_null(node)))
        return NULL;
    return __bucket_rb_rprev(_this, node);
}


static /* __always_inline */ inline bucket_node_t* __bucket_hl_first(const bucket_t* _this)
{
    struct hlist_node* t = _this->ds.hl.first;

    if (hlist_empty(&_this->ds.hl))
        return NULL;
    return bucket_hl_entry(t);
}

static bucket_node_t* __bucket_hl_last(const bucket_t* _this)
{
    struct hlist_node* t = _this->ds.hl.first;

    if (hlist_empty(&_this->ds.hl))
        return NULL;

    /* TODO: it is not recommended to use this data structure to get rbegin when there are many nodes */
    while (!is_null(t->next)) t = t->next;
    return bucket_hl_entry(t);
}

static /* __always_inline */ inline bucket_node_t* __bucket_hl_begin(const bucket_t* _this)
{
    struct hlist_node* t = _this->ds.hl.first;

    if (hlist_empty(&_this->ds.hl))
        return __bucket_end(_this);
    return bucket_hl_entry(t);
}

static /* __always_inline */ inline bucket_node_t* _bucket_hl_begin(const bucket_t* _this)
{
    if (unlikely(is_null(_this)))
        return NULL;
    return __bucket_hl_begin(_this);
}

static /* __always_inline */ inline bucket_node_t* __bucket_hl_next(const bucket_t* _this, const bucket_node_t* node)
{
    struct hlist_node* t;

    if (hlist_empty(&_this->ds.hl) || __bucket_end(_this) == node)
        return __bucket_end(_this);

    /* The input parameter is `iterator`, and there's no need 
       to check whether it equals `rend` */

    t = node->ds_node.hl_node.next;
    return is_null(t) ? __bucket_end(_this) : bucket_hl_entry(t);
}

static /* __always_inline */ inline bucket_node_t* _bucket_hl_next(const bucket_t* _this, const bucket_node_t* node)
{
    if (unlikely(is_null(_this) || is_null(node)))
        return NULL;
    return __bucket_hl_next(_this, node);
}

static /* __always_inline */ inline bucket_node_t* __bucket_hl_prev(const bucket_t* _this, const bucket_node_t* node)
{
    struct hlist_node** t;

    if (hlist_empty(&_this->ds.hl))
        return __bucket_end(_this);

    if (__bucket_end(_this) == node)
        return __bucket_hl_last(_this); /* Err: since the `ds` is non-empty, the return value 
                                                includes the error case of `NULL` */

    t = node->ds_node.hl_node.pprev;
    if (unlikely(is_null(t) || &node->ds_node.hl_node != *t))
        return NULL; /* Err: `node` doesn't belong to current list or memory has been modified illegally */

    if (&_this->ds.hl.first == t)
        return __bucket_end(_this);
    return bucket_hl_entry(((struct hlist_node*)t));
}

static /* __always_inline */ inline bucket_node_t* _bucket_hl_prev(const bucket_t* _this, const bucket_node_t* node)
{
    if (unlikely(is_null(_this) || is_null(node)))
        return NULL;
    return __bucket_hl_prev(_this, node);
}

static bucket_node_t* __bucket_hl_rbegin(const bucket_t* _this)
{
    struct hlist_node* t = _this->ds.hl.first;

    if (hlist_empty(&_this->ds.hl))
        return __bucket_rend(_this);

    /* TODO: it is not recommended to use this data structure to get rbegin when there are many nodes */
    while (!is_null(t->next)) t = t->next;
    return bucket_hl_entry(t);
}

static /* __always_inline */ inline bucket_node_t* _bucket_hl_rbegin(const bucket_t* _this)
{
    if (unlikely(is_null(_this)))
        return NULL;
    return __bucket_hl_rbegin(_this);
}

static /* __always_inline */ inline bucket_node_t* __bucket_hl_rnext(const bucket_t* _this, const bucket_node_t* node)
{
    struct hlist_node** t;

    if (hlist_empty(&_this->ds.hl) || __bucket_rend(_this) == node)
        return __bucket_rend(_this);

    /* The input parameter is `reverse_iterator`, and there's no need 
       to check whether it equals `end` */

    t = node->ds_node.hl_node.pprev;
    if (unlikely(is_null(t) || &node->ds_node.hl_node != *t))
        return NULL; /* Err: `node` doesn't belong to current list or memory has been modified illegally */

    if (&_this->ds.hl.first == t)
        return __bucket_rend(_this);
    return bucket_hl_entry(((struct hlist_node*)t));
}

static /* __always_inline */ inline bucket_node_t* _bucket_hl_rnext(const bucket_t* _this, const bucket_node_t* node)
{
    if (unlikely(is_null(_this) || is_null(node)))
        return NULL;
    return __bucket_hl_rnext(_this, node);
}

static /* __always_inline */ inline bucket_node_t* __bucket_hl_rprev(const bucket_t* _this, const bucket_node_t* node)
{
    struct hlist_node* t;

    if (hlist_empty(&_this->ds.hl))
        return __bucket_rend(_this);

    if (__bucket_rend(_this) == node)
        return __bucket_hl_first(_this); /* Err: since the `ds` is non-empty, the return value 
                                                 includes the error case of `NULL` */

    t = node->ds_node.hl_node.next;
    return is_null(t) ? __bucket_rend(_this) : bucket_hl_entry(t);
}

static /* __always_inline */ inline bucket_node_t* _bucket_hl_rprev(const bucket_t* _this, const bucket_node_t* node)
{
    if (unlikely(is_null(_this) || is_null(node)))
        return NULL;
    return __bucket_hl_rprev(_this, node);
}


static /* __always_inline */ inline bucket_node_t* _bucket_first(const bucket_t* _this, bucket_ds_t type)
{
    if (unlikely(is_null(_this)))
        return NULL;

    switch (type)
    {
    case BKT_DS_HLIST:
        return __bucket_hl_first(_this);
    case BKT_DS_RBTREE:
        return __bucket_rb_first(_this);
    default:
        return NULL;
    }
}

static /* __always_inline */ inline bucket_node_t* _bucket_last(const bucket_t* _this, bucket_ds_t type)
{
    if (unlikely(is_null(_this)))
        return NULL;

    switch (type)
    {
    case BKT_DS_HLIST:
        return __bucket_hl_last(_this);
    case BKT_DS_RBTREE:
        return __bucket_rb_last(_this);
    default:
        return NULL;
    }
}

static /* __always_inline */ inline bucket_node_t* bucket_begin(const bucket_t* _this, bucket_ds_t type)
{
    switch (type)
    {
    case BKT_DS_HLIST:
        return _bucket_hl_begin(_this);
    case BKT_DS_RBTREE:
        return _bucket_rb_begin(_this);
    default:
        return NULL;
    }
}

static /* __always_inline */ inline bucket_node_t* bucket_next(const bucket_t* _this, bucket_ds_t type, const bucket_node_t* node)
{
    switch (type)
    {
    case BKT_DS_HLIST:
        return _bucket_hl_next(_this, node);
    case BKT_DS_RBTREE:
        return _bucket_rb_next(_this, node);
    default:
        return NULL;
    }
}

static /* __always_inline */ inline bucket_node_t* bucket_prev(const bucket_t* _this, bucket_ds_t type, const bucket_node_t* node)
{
    switch (type)
    {
    case BKT_DS_HLIST:
        return _bucket_hl_prev(_this, node);
    case BKT_DS_RBTREE:
        return _bucket_rb_prev(_this, node);
    default:
        return NULL;
    }
}

static /* __always_inline */ inline bucket_node_t* bucket_rbegin(const bucket_t* _this, bucket_ds_t type)
{
    switch (type)
    {
    case BKT_DS_HLIST:
        return _bucket_hl_rbegin(_this);
    case BKT_DS_RBTREE:
        return _bucket_rb_rbegin(_this);
    default:
        return NULL;
    }
}

static /* __always_inline */ inline bucket_node_t* bucket_rnext(const bucket_t* _this, bucket_ds_t type, const bucket_node_t* node)
{
    switch (type)
    {
    case BKT_DS_HLIST:
        return _bucket_hl_rnext(_this, node);
    case BKT_DS_RBTREE:
        return _bucket_rb_rnext(_this, node);
    default:
        return NULL;
    }
}

static /* __always_inline */ inline bucket_node_t* bucket_rprev(const bucket_t* _this, bucket_ds_t type, const bucket_node_t* node)
{
    switch (type)
    {
    case BKT_DS_HLIST:
        return _bucket_hl_rprev(_this, node);
    case BKT_DS_RBTREE:
        return _bucket_rb_rprev(_this, node);
    default:
        return NULL;
    }
}



/* Find */
static bucket_node_t* __bucket_rb_find(const bucket_t* _this, const class_bucket_ops_t* ops, bucket_key_t key)
{
    struct rb_node* n = _this->ds.rb.rb_node;
    bucket_node_t* t = NULL;

    if (is_null(ops) || is_null(ops->__lt)) {
        while (!is_null(n)) {
            t = bucket_rb_entry(n);

            if (key < t->key)
                n = n->rb_left;
            else if (key > t->key)
                n = n->rb_right;
            else
                return t;
        }
    } else {
        while (!is_null(n)) {
            t = bucket_rb_entry(n);

            if (ops->__lt(key, t->key))
                n = n->rb_left;
            else if (ops->__lt(t->key, key))
                n = n->rb_right;
            else
                return t;
        }
    }

    return NULL;
}

static bucket_node_t* __bucket_hl_find(const bucket_t* _this, const class_bucket_ops_t* ops, bucket_key_t key)
{
    bucket_node_t* t = NULL;
    struct hlist_node* thl = NULL;

    if (is_null(ops) || is_null(ops->__lt)) {
        hlist_for_each_entry(t, thl, &_this->ds.hl, ds_node.hl_node) {
            if (key == t->key)
                return t;
        }
    } else {
        hlist_for_each_entry(t, thl, &_this->ds.hl, ds_node.hl_node) {
            if (!ops->__lt(key, t->key) && !ops->__lt(t->key, key))
                return t;
        }
    }
    return NULL;
}

static /* __always_inline */ inline bucket_node_t* bucket_find(const bucket_t* _this, const class_bucket_ops_t* ops, bucket_ds_t type, bucket_key_t key)
{
    bucket_node_t* t = NULL;

    if (unlikely(is_null(_this)))
        return NULL;

    if (!is_null(ops) && !is_null(ops->valid_key) && !ops->valid_key(key))
        return NULL;

    switch (type)
    {
    case BKT_DS_HLIST:
        t = __bucket_hl_find(_this, ops, key);
        break;
    case BKT_DS_RBTREE:
        t = __bucket_rb_find(_this, ops, key);
        break;
    default:
        return NULL;
    }
    return is_null(t) ? __bucket_end(_this) : t;
}

static /* __always_inline */ inline bucket_node_t* bucket_find_has_checked_valid(const bucket_t* _this, const class_bucket_ops_t* ops, bucket_ds_t type, bucket_key_t key)
{
    bucket_node_t* t = NULL;

    if (unlikely(is_null(_this)))
        return NULL;

    switch (type)
    {
    case BKT_DS_HLIST:
        t = __bucket_hl_find(_this, ops, key);
        break;
    case BKT_DS_RBTREE:
        t = __bucket_rb_find(_this, ops, key);
        break;
    default:
        return NULL;
    }
    return is_null(t) ? __bucket_end(_this) : t;
}



/* Add */
static bucket_node_t* __bucket_rb_insert(bucket_t* _this, const class_bucket_ops_t* ops, bucket_node_t* node)
{
    struct rb_node** n = &_this->ds.rb.rb_node;
    struct rb_node* parent = NULL;
    bucket_node_t* t = NULL;

    if (is_null(ops) || is_null(ops->__lt)) {
        while (!is_null(*n)) {
            parent = *n;
            t = bucket_rb_entry(parent);

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
            t = bucket_rb_entry(parent);

            if (ops->__lt(node->key, t->key))
                n = &parent->rb_left;
            else if (ops->__lt(t->key, node->key))
                n = &parent->rb_right;
            else
                return t;
        }
    }

    rb_link_node(&node->ds_node.rb_node, parent, n);
    rb_insert_color(&node->ds_node.rb_node, &_this->ds.rb);
    _this->size++;
    return node;
}

static /* __always_inline */ inline bucket_node_t* __bucket_hl_push_front(bucket_t* _this, bucket_node_t* node)
{
    hlist_add_head(&node->ds_node.hl_node, &_this->ds.hl);
    _this->size++;
    return node;
}

static /* __always_inline */ inline bucket_node_t* bucket_insert_has_checked_same(bucket_t* _this, const class_bucket_ops_t* ops, bucket_ds_t type, bucket_node_t* node)
{
    if (unlikely(is_null(_this) || is_null(node)))
        return NULL;

    switch (type)
    {
    case BKT_DS_HLIST:
        return __bucket_hl_push_front(_this, node);
    case BKT_DS_RBTREE:
        return __bucket_rb_insert(_this, ops, node);
    default:
        return NULL;
    }
}

static inline bucket_node_t* bucket_insert(bucket_t* _this, const class_bucket_ops_t* ops, bucket_ds_t type, bucket_hash_t hash, bucket_key_t key, bucket_value_t value)
{
    bucket_node_t* t = NULL;
    bucket_node_t* ret = NULL;

    if (unlikely(is_null(_this)))
        return NULL;

    if (!is_null(ops) && !is_null(ops->valid_value) && !ops->valid_value(value))
        return NULL;

    if (BKT_DS_RBTREE != type) {
        t = bucket_find(_this, ops, type, key);
        if (is_null(t) || __bucket_end(_this) != t)
            return NULL;
    } else {
        if (!is_null(ops) && !is_null(ops->valid_key) && !ops->valid_key(key))
            return NULL;
    }

    t = (bucket_node_t*)p_calloc(1, sizeof(bucket_node_t));
    if (unlikely(is_null(t)))
        return NULL;

    if (is_null(ops) || is_null(ops->copy_key)) {
        t->key = key;
    } else {
        if (!ops->copy_key(key, &t->key))
            goto err;
    }

    if (is_null(ops) || is_null(ops->copy_value)) {
        t->value = value;
    } else {
        if (!ops->copy_value(value, &t->value))
            goto err;
    }

    t->hash = hash;

    switch (type)
    {
    case BKT_DS_HLIST:
        ret = __bucket_hl_push_front(_this, t);
        break;
    case BKT_DS_RBTREE:
        ret = __bucket_rb_insert(_this, ops, t);
        break;
    default:
        ret = NULL;
        break;
    }
    if (t != ret)
        goto err;
    return t;

err:
    if (!is_null(ops) && !is_null(ops->free_value))
        ops->free_value(&t->value);

    if (!is_null(ops) && !is_null(ops->free_key))
        ops->free_key(&t->key);

    p_free(t);
    return NULL;
}

static inline bucket_node_t* bucket_insert_has_checked_valid(bucket_t* _this, const class_bucket_ops_t* ops, bucket_ds_t type, bucket_hash_t hash, bucket_key_t key, bucket_value_t value)
{
    bucket_node_t* t = NULL;
    bucket_node_t* ret = NULL;

    if (unlikely(is_null(_this)))
        return NULL;

    if (BKT_DS_RBTREE != type) {
        t = bucket_find_has_checked_valid(_this, ops, type, key);
        if (is_null(t) || __bucket_end(_this) != t)
            return NULL;
    }

    t = (bucket_node_t*)p_calloc(1, sizeof(bucket_node_t));
    if (unlikely(is_null(t)))
        return NULL;

    if (is_null(ops) || is_null(ops->copy_key)) {
        t->key = key;
    } else {
        if (!ops->copy_key(key, &t->key))
            goto err;
    }

    if (is_null(ops) || is_null(ops->copy_value)) {
        t->value = value;
    } else {
        if (!ops->copy_value(value, &t->value))
            goto err;
    }

    t->hash = hash;

    switch (type)
    {
    case BKT_DS_HLIST:
        ret = __bucket_hl_push_front(_this, t);
        break;
    case BKT_DS_RBTREE:
        ret = __bucket_rb_insert(_this, ops, t);
        break;
    default:
        ret = NULL;
        break;
    }
    if (t != ret)
        goto err;
    return t;

err:
    if (!is_null(ops) && !is_null(ops->free_value))
        ops->free_value(&t->value);

    if (!is_null(ops) && !is_null(ops->free_key))
        ops->free_key(&t->key);

    p_free(t);
    return NULL;
}

static inline bucket_node_t* bucket_insert_replace(bucket_t* _this, const class_bucket_ops_t* ops, bucket_ds_t type, bucket_hash_t hash, bucket_key_t key, bucket_value_t value)
{
    bucket_node_t* t = NULL;
    bucket_node_t* ret = NULL;

    if (unlikely(is_null(_this)))
        return NULL;

    if (!is_null(ops) && !is_null(ops->valid_value) && !ops->valid_value(value))
        return NULL;

    t = bucket_find(_this, ops, type, key);
    if (is_null(t))
        return NULL;

    if (__bucket_end(_this) != t) {
        bucket_value_t tvalue = t->value;

        if (is_null(ops) || is_null(ops->copy_value)) {
            t->value = value;
        } else {
            if (!ops->copy_value(value, &t->value)) {
                t->value = tvalue;
                return NULL;
            }

            if (!is_null(ops) && !is_null(ops->free_value))
                ops->free_value(&tvalue);
        }

        return t;
    }

    t = (bucket_node_t*)p_calloc(1, sizeof(bucket_node_t));
    if (unlikely(is_null(t)))
        return NULL;

    if (is_null(ops) || is_null(ops->copy_key)) {
        t->key = key;
    } else {
        if (!ops->copy_key(key, &t->key))
            goto err;
    }

    if (is_null(ops) || is_null(ops->copy_value)) {
        t->value = value;
    } else {
        if (!ops->copy_value(value, &t->value))
            goto err;
    }

    t->hash = hash;

    switch (type)
    {
    case BKT_DS_HLIST:
        ret = __bucket_hl_push_front(_this, t);
        break;
    case BKT_DS_RBTREE:
        ret = __bucket_rb_insert(_this, ops, t);
        break;
    default:
        ret = NULL;
        break;
    }
    if (t != ret)
        goto err;
    return t;

err:
    if (!is_null(ops) && !is_null(ops->free_value))
        ops->free_value(&t->value);

    if (!is_null(ops) && !is_null(ops->free_key))
        ops->free_key(&t->key);

    p_free(t);
    return NULL;
}

static inline bucket_node_t* bucket_insert_replace_has_checked_valid(bucket_t* _this, const class_bucket_ops_t* ops, bucket_ds_t type, bucket_hash_t hash, bucket_key_t key, bucket_value_t value)
{
    bucket_node_t* t = NULL;
    bucket_node_t* ret = NULL;

    if (unlikely(is_null(_this)))
        return NULL;

    t = bucket_find_has_checked_valid(_this, ops, type, key);
    if (is_null(t))
        return NULL;

    if (__bucket_end(_this) != t) {
        bucket_value_t tvalue = t->value;

        if (is_null(ops) || is_null(ops->copy_value)) {
            t->value = value;
        } else {
            if (!ops->copy_value(value, &t->value)) {
                t->value = tvalue;
                return NULL;
            }

            if (!is_null(ops) && !is_null(ops->free_value))
                ops->free_value(&tvalue);
        }

        return t;
    }

    t = (bucket_node_t*)p_calloc(1, sizeof(bucket_node_t));
    if (unlikely(is_null(t)))
        return NULL;

    if (is_null(ops) || is_null(ops->copy_key)) {
        t->key = key;
    } else {
        if (!ops->copy_key(key, &t->key))
            goto err;
    }

    if (is_null(ops) || is_null(ops->copy_value)) {
        t->value = value;
    } else {
        if (!ops->copy_value(value, &t->value))
            goto err;
    }

    t->hash = hash;

    switch (type)
    {
    case BKT_DS_HLIST:
        ret = __bucket_hl_push_front(_this, t);
        break;
    case BKT_DS_RBTREE:
        ret = __bucket_rb_insert(_this, ops, t);
        break;
    default:
        ret = NULL;
        break;
    }
    if (t != ret)
        goto err;
    return t;

err:
    if (!is_null(ops) && !is_null(ops->free_value))
        ops->free_value(&t->value);

    if (!is_null(ops) && !is_null(ops->free_key))
        ops->free_key(&t->key);

    p_free(t);
    return NULL;
}



/* Remove */
static /* __always_inline */ inline bucket_node_t* __bucket_rb_erase(bucket_t* _this, bucket_node_t* pos)
{
    rb_erase(&pos->ds_node.rb_node, &_this->ds.rb);
    rb_init_node(&pos->ds_node.rb_node);
    _this->size--;
    return pos;
}

static inline bucket_node_t* bucket_rb_erase(bucket_t* _this, const class_bucket_ops_t* ops, bucket_node_t* pos)
{
    bucket_node_t* t;

    if (unlikely(is_null(_this) || is_null(pos)))
        return NULL;

    /* The input parameter is `iterator`, and there's no need 
       to check whether it equals `rend` */
    if (RB_EMPTY_ROOT(&_this->ds.rb) || __bucket_end(_this) == pos/* || __bucket_rend(_this) == pos*/)
        return NULL;

    t = __bucket_rb_next(_this, pos);
    if (is_null(t))
        return NULL;

    __bucket_rb_erase(_this, pos);

    if (!is_null(ops) && !is_null(ops->free_key))
        ops->free_key(&pos->key);

    if (!is_null(ops) && !is_null(ops->free_value))
        ops->free_value(&pos->value);

    p_free(pos);

    return t;
}

static /* __always_inline */ inline void __bucket_hl_erase(bucket_t* _this, bucket_node_t* pos)
{
    hlist_del_init(&pos->ds_node.hl_node);
    _this->size--;
}

static inline bucket_node_t* bucket_hl_erase(bucket_t* _this, const class_bucket_ops_t* ops, bucket_node_t* pos)
{
    bucket_node_t* t;

    if (unlikely(is_null(_this) || is_null(pos)))
        return NULL;

    /* The input parameter is `iterator`, and there's no need 
       to check whether it equals `rend` */
    if (hlist_empty(&_this->ds.hl) || __bucket_end(_this) == pos/* || __bucket_rend(_this) == pos*/)
        return NULL;

    t = __bucket_hl_next(_this, pos); /* Never return `NULL` */

    __bucket_hl_erase(_this, pos);

    if (!is_null(ops) && !is_null(ops->free_key))
        ops->free_key(&pos->key);

    if (!is_null(ops) && !is_null(ops->free_value))
        ops->free_value(&pos->value);

    p_free(pos);

    return t;
}

static /* __always_inline */ inline bucket_node_t* bucket_erase(bucket_t* _this, const class_bucket_ops_t* ops, bucket_ds_t type, bucket_node_t* pos)
{
    switch (type)
    {
    case BKT_DS_HLIST:
        return bucket_hl_erase(_this, ops, pos);
    case BKT_DS_RBTREE:
        return bucket_rb_erase(_this, ops, pos);    
    default:
        return NULL;
    }
}

static inline bucket_size_t bucket_remove(bucket_t* _this, const class_bucket_ops_t* ops, bucket_ds_t type, bucket_key_t key)
{
    bucket_node_t* t;

    if (unlikely(is_null(_this)))
        return -1;

    t = bucket_find(_this, ops, type, key);
    if (is_null(t))
        return -1;

    if (__bucket_end(_this) == t)
        return 0;

    switch (type)
    {
    case BKT_DS_HLIST:
        __bucket_hl_erase(_this, t); /* Always right */
        break;
    case BKT_DS_RBTREE:
        __bucket_rb_erase(_this, t); /* Always right */
        break;
    default:
        return -1;
    }

    if (!is_null(ops) && !is_null(ops->free_key))
        ops->free_key(&t->key);

    if (!is_null(ops) && !is_null(ops->free_value))
        ops->free_value(&t->value);

    p_free(t);

    return 1;
}

static inline bucket_size_t bucket_remove_has_checked_valid(bucket_t* _this, const class_bucket_ops_t* ops, bucket_ds_t type, bucket_key_t key)
{
    bucket_node_t* t;

    if (unlikely(is_null(_this)))
        return -1;

    t = bucket_find_has_checked_valid(_this, ops, type, key);
    if (is_null(t))
        return -1;

    if (__bucket_end(_this) == t)
        return 0;

    switch (type)
    {
    case BKT_DS_HLIST:
        __bucket_hl_erase(_this, t); /* Always right */
        break;
    case BKT_DS_RBTREE:
        __bucket_rb_erase(_this, t); /* Always right */
        break;
    default:
        return -1;
    }

    if (!is_null(ops) && !is_null(ops->free_key))
        ops->free_key(&t->key);

    if (!is_null(ops) && !is_null(ops->free_value))
        ops->free_value(&t->value);

    p_free(t);

    return 1;
}



/* Pop */
static /* __always_inline */ inline bucket_node_t* bucket_rb_pop(bucket_t* _this, bucket_node_t* pos)
{
    bucket_node_t* t;

    if (unlikely(is_null(_this) || is_null(pos)))
        return NULL;

    /* The input parameter is `iterator`, and there's no need 
       to check whether it equals `rend` */
    if (RB_EMPTY_ROOT(&_this->ds.rb) || __bucket_end(_this) == pos/* || __bucket_rend(_this) == pos*/)
        return NULL;

    t = __bucket_rb_next(_this, pos);
    if (is_null(t))
        return NULL;

    __bucket_rb_erase(_this, pos);

    return t;
}

static /* __always_inline */ inline bucket_node_t* bucket_hl_pop(bucket_t* _this, bucket_node_t* pos)
{
    bucket_node_t* t;

    if (unlikely(is_null(_this) || is_null(pos)))
        return NULL;

    /* The input parameter is `iterator`, and there's no need 
       to check whether it equals `rend` */
    if (hlist_empty(&_this->ds.hl) || __bucket_end(_this) == pos/* || __bucket_rend(_this) == pos*/)
        return NULL;

    t = __bucket_hl_next(_this, pos); /* Never return `NULL` */

    __bucket_hl_erase(_this, pos);

    return t;
}

static /* __always_inline */ inline bucket_node_t* bucket_pop(bucket_t* _this, bucket_ds_t type, bucket_node_t* pos)
{
    switch (type)
    {
    case BKT_DS_HLIST:
        return bucket_hl_pop(_this, pos);
    case BKT_DS_RBTREE:
        return bucket_rb_pop(_this, pos);
    default:
        return NULL;
    }
}



/* Clear */
static bucket_size_t bucket_rb_clear(bucket_t* _this, const class_bucket_ops_t* ops)
{
    bucket_size_t ret = 0;
    bucket_node_t* t = NULL;

    if (unlikely(is_null(_this)))
        return -1;

    for (t = __bucket_rb_begin(_this); __bucket_end(_this) != t; ) {
        t = bucket_rb_erase(_this, ops, t);
        ret++;
    }

    return ret;
}

static bucket_size_t bucket_hl_clear(bucket_t* _this, const class_bucket_ops_t* ops)
{
    bucket_size_t ret = 0;
    bucket_node_t* t = NULL;
    struct hlist_node* p = NULL;
    struct hlist_node* n = NULL;

    if (unlikely(is_null(_this)))
        return -1;

    hlist_for_each_entry_safe(t, p, n, &_this->ds.hl, ds_node.hl_node) {
        __bucket_hl_erase(_this, t);

        if (!is_null(ops) && !is_null(ops->free_key))
            ops->free_key(&t->key);

        if (!is_null(ops) && !is_null(ops->free_value))
            ops->free_value(&t->value);

        p_free(t);
        ret++;
    }

    return ret;
}

static /* __always_inline */ inline bucket_size_t bucket_clear(bucket_t* _this, const class_bucket_ops_t* ops, bucket_ds_t type)
{
    bucket_size_t ret;
    bucket_size_t size = _bucket_size(_this);

    switch (type)
    {
    case BKT_DS_HLIST:
        ret = bucket_hl_clear(_this, ops);
        break;
    case BKT_DS_RBTREE:
        ret = bucket_rb_clear(_this, ops);
        break;
    default:
        ret = -1;
        break;
    }

    if (unlikely(size != ret))
        pr_err("After clearing bucket, errors were discovered [ %zd | %zd ]", ret, size);
    return ret;
}

#if 1
static __always_inline bool __bucket_empty(const bucket_t* _this)
{
    return is_null(_this->ds.p); /* TODO: Current approach is for performance, only works in this version */
}

static inline void __bucket_switch(bucket_t* _this, const class_bucket_ops_t* ops, bucket_ds_t type_from, bucket_ds_t type_to)
{
    bucket_t tmp = BUCKET_INIT(&tmp, type_to);
    bucket_size_t size = __bucket_size(_this);
    bucket_node_t* bkt_node;

    while (!__bucket_empty(_this)) {
        bkt_node = bucket_begin(_this, type_from);
        bucket_pop(_this, type_from, bkt_node);
        bucket_insert_has_checked_same(&tmp, ops, type_to, bkt_node);
    }

    memset(_this, 0, sizeof(bucket_t)); /* TODO: Current approach is for performance, only works in this version */

    switch (type_to)
    {
    case BKT_DS_HLIST:
        hlist_move_list(&tmp.ds.hl, &_this->ds.hl);
        break;
    case BKT_DS_RBTREE:
        _this->ds.rb.rb_node = tmp.ds.rb.rb_node;
        break;
    default:
        return ; /* Segment fault: may occur */
    }

    if (unlikely(size != tmp.size))
        pr_err("After switching bucket, errors were discovered [ %zd | %zd ] and forcibly corrected!", tmp.size, size);

    _this->size = tmp.size;
}
#else
static __always_inline bool __bucket_empty(const bucket_t* _this, bucket_ds_t type)
{
    return BKT_DS_RBTREE == type ? RB_EMPTY_ROOT(&_this->ds.rb) : hlist_empty(&_this->ds.hl);
}

static inline void __bucket_switch(bucket_t* _this, const class_bucket_ops_t* ops, bucket_ds_t type_from, bucket_ds_t type_to)
{
    bucket_t tmp = BUCKET_INIT(&tmp, type_to);
    bucket_size_t size = __bucket_size(_this);
    bucket_node_t* bkt_node;

    while (!__bucket_empty(_this, type_from)) {
        bkt_node = bucket_begin(_this, type_from);
        bucket_pop(_this, type_from, bkt_node);
        bucket_insert_has_checked_same(&tmp, ops, type_to, bkt_node);
    }
    BUCKET_DEINIT(_this, ops, type_from);

    *_this = BUCKET_INIT(_this, type_to); /* memset(_this, 0, sizeof(bucket_t)); */

    switch (type_to)
    {
    case BKT_DS_HLIST:
        hlist_move_list(&tmp.ds.hl, &_this->ds.hl);
        break;
    case BKT_DS_RBTREE:
        _this->ds.rb.rb_node = tmp.ds.rb.rb_node;
        break;
    default:
        return ; /* Segment fault: may occur */
    }

    if (unlikely(size != tmp.size))
        pr_err("After switching bucket, errors were discovered [ %zd | %zd ] and forcibly corrected!", tmp.size, size);

    _this->size = tmp.size;
}
#endif

static __always_inline void __bucket_resume(bucket_t* _this, bucket_ds_t type)
{
    struct hlist_head* hh = &_this->ds.hl;
    if (BKT_DS_HLIST == type && hh->first) {
        hh->first->pprev = &hh->first;
    }
}

__always_inline void __bucket_init(bucket_t* bucket, bucket_ds_t type)
{
    switch (type)
    {
    case BKT_DS_HLIST:
        INIT_HLIST_HEAD(&bucket->ds.hl);
        break;
    case BKT_DS_RBTREE:
        bucket->ds.rb = RB_ROOT;
        break;
    default:
        break;
    }
}

__always_inline void __bucket_deinit(bucket_t* bucket, const class_bucket_ops_t* ops, bucket_ds_t type)
{
    bucket_clear(bucket, ops, type);

    switch (type)
    {
    case BKT_DS_HLIST:
        INIT_HLIST_HEAD(&bucket->ds.hl);
        break;
    case BKT_DS_RBTREE:
        bucket->ds.rb = RB_ROOT;
        break;
    default:
        break;
    }

    bucket->size = 0;
}

#if 0
typedef struct class_bucket {
    bucket_size_t  (*size)(const bucket_t* _this);
    bucket_iterator_t* (*end)(const bucket_t* _this);
    bucket_iterator_t* (*begin)(const bucket_t* _this, bucket_ds_t type);
    bucket_iterator_t* (*next)(const bucket_t* _this, bucket_ds_t type, const bucket_iterator_t* iterator);
    bucket_iterator_t* (*prev)(const bucket_t* _this, bucket_ds_t type, const bucket_iterator_t* iterator);
    bucket_r_iterator_t* (*rend)(const bucket_t* _this);
    bucket_r_iterator_t* (*rbegin)(const bucket_t* _this, bucket_ds_t type);
    bucket_r_iterator_t* (*rnext)(const bucket_t* _this, bucket_ds_t type, const bucket_r_iterator_t* r_iterator);
    bucket_r_iterator_t* (*rprev)(const bucket_t* _this, bucket_ds_t type, const bucket_r_iterator_t* r_iterator);
    bucket_iterator_t* (*find)(const bucket_t* _this, const class_bucket_ops_t* ops, bucket_ds_t type, bucket_key_t key);
    bucket_iterator_t* (*insert)(bucket_t* _this, const class_bucket_ops_t* ops, bucket_ds_t type, bucket_hash_t hash, bucket_key_t key, bucket_value_t value);         /* if input key doesn't match -> insert | if input key match -> return NULL */
    bucket_iterator_t* (*insert_replace)(bucket_t* _this, const class_bucket_ops_t* ops, bucket_ds_t type, bucket_hash_t hash, bucket_key_t key, bucket_value_t value); /* if input key doesn't match -> insert | if input key match -> replace value (Refer to C++11 a[key] = value) */
    bucket_iterator_t* (*erase)(bucket_t* _this, const class_bucket_ops_t* ops, bucket_ds_t type, bucket_iterator_t* iterator);
    bucket_size_t  (*remove)(bucket_t* _this, const class_bucket_ops_t* ops, bucket_ds_t type, bucket_key_t key);
    bucket_size_t  (*clear)(bucket_t* _this, const class_bucket_ops_t* ops, bucket_ds_t type);
} class_bucket_t;

/* Private functions */
typedef struct class_bucket_priv {
    /* iterator */
    bucket_node_t* (*end)(const bucket_t* _this);
    bucket_node_t* (*begin)(const bucket_t* _this, bucket_ds_t type);
    bucket_node_t* (*next)(const bucket_t* _this, bucket_ds_t type, const bucket_node_t* node);
    bucket_node_t* (*prev)(const bucket_t* _this, bucket_ds_t type, const bucket_node_t* node);
    bucket_node_t* (*rend)(const bucket_t* _this);
    bucket_node_t* (*rbegin)(const bucket_t* _this, bucket_ds_t type);
    bucket_node_t* (*rnext)(const bucket_t* _this, bucket_ds_t type, const bucket_node_t* node);
    bucket_node_t* (*rprev)(const bucket_t* _this, bucket_ds_t type, const bucket_node_t* node);
    bucket_node_t* (*find)(const bucket_t* _this, const class_bucket_ops_t* ops, bucket_ds_t type, bucket_key_t key);
    bucket_node_t* (*insert)(bucket_t* _this, const class_bucket_ops_t* ops, bucket_ds_t type, bucket_hash_t hash, bucket_key_t key, bucket_value_t value);         /* if input key doesn't match -> insert | if input key match -> return NULL */
    bucket_node_t* (*insert_replace)(bucket_t* _this, const class_bucket_ops_t* ops, bucket_ds_t type, bucket_hash_t hash, bucket_key_t key, bucket_value_t value); /* if input key doesn't match -> insert | if input key match -> replace value (Refer to C++11 a[key] = value) */
    bucket_node_t* (*erase)(bucket_t* _this, const class_bucket_ops_t* ops, bucket_ds_t type, bucket_node_t* pos);
    /* hashmap */
    bucket_node_t* (*_first)(const bucket_t* _this, bucket_ds_t type);
    bucket_node_t* (*_last)(const bucket_t* _this, bucket_ds_t type);
    bucket_node_t* (*find_has_checked_valid)(const bucket_t* _this, const class_bucket_ops_t* ops, bucket_ds_t type, bucket_key_t key);
    bucket_node_t* (*insert_has_checked_same)(bucket_t* _this, const class_bucket_ops_t* ops, bucket_ds_t type, bucket_node_t* node);
    bucket_node_t* (*insert_has_checked_valid)(bucket_t* _this, const class_bucket_ops_t* ops, bucket_ds_t type, bucket_hash_t hash, bucket_key_t key, bucket_value_t value);         /* if input key doesn't match -> insert | if input key match -> return NULL */
    bucket_node_t* (*insert_replace_has_checked_valid)(bucket_t* _this, const class_bucket_ops_t* ops, bucket_ds_t type, bucket_hash_t hash, bucket_key_t key, bucket_value_t value); /* if input key doesn't match -> insert | if input key match -> replace value (Refer to C++11 a[key] = value) */
    bucket_size_t  (*remove_has_checked_valid)(bucket_t* _this, const class_bucket_ops_t* ops, bucket_ds_t type, bucket_key_t key);
    bucket_node_t* (*pop)(bucket_t* _this, bucket_ds_t type, bucket_node_t* pos);
    void           (*__switch)(bucket_t* _this, const class_bucket_ops_t* ops, bucket_ds_t type_from, bucket_ds_t type_to);
} class_bucket_priv_t;

typedef bucket_iterator_t* (*fp_end)(const bucket_t* _this);
typedef bucket_iterator_t* (*fp_begin)(const bucket_t* _this, bucket_ds_t type);
typedef bucket_iterator_t* (*fp_next)(const bucket_t* _this, bucket_ds_t type, const bucket_iterator_t* iterator);
typedef bucket_iterator_t* (*fp_prev)(const bucket_t* _this, bucket_ds_t type, const bucket_iterator_t* iterator);
typedef bucket_r_iterator_t* (*fp_rend)(const bucket_t* _this);
typedef bucket_r_iterator_t* (*fp_rbegin)(const bucket_t* _this, bucket_ds_t type);
typedef bucket_r_iterator_t* (*fp_rnext)(const bucket_t* _this, bucket_ds_t type, const bucket_r_iterator_t* r_iterator);
typedef bucket_r_iterator_t* (*fp_rprev)(const bucket_t* _this, bucket_ds_t type, const bucket_r_iterator_t* r_iterator);
typedef bucket_iterator_t* (*fp_find)(const bucket_t* _this, const class_bucket_ops_t* ops, bucket_ds_t type, bucket_key_t key);
typedef bucket_iterator_t* (*fp_insert)(bucket_t* _this, const class_bucket_ops_t* ops, bucket_ds_t type, bucket_hash_t hash, bucket_key_t key, bucket_value_t value);
typedef bucket_iterator_t* (*fp_insert_replace)(bucket_t* _this, const class_bucket_ops_t* ops, bucket_ds_t type, bucket_hash_t hash, bucket_key_t key, bucket_value_t value);
typedef bucket_iterator_t* (*fp_erase)(bucket_t* _this, const class_bucket_ops_t* ops, bucket_ds_t type, bucket_iterator_t* iterator);

/* __always_inline */ inline const class_bucket_t* class_bucket_ins(void)
{
    static const class_bucket_t ins = {
        .size           = _bucket_size,
        .end            = (fp_end)__bucket_end,
        .begin          = (fp_begin)bucket_begin,
        .next           = (fp_next)bucket_next,
        .prev           = (fp_prev)bucket_prev,
        .rend           = (fp_rend)__bucket_rend,
        .rbegin         = (fp_rbegin)bucket_rbegin,
        .rnext          = (fp_rnext)bucket_rnext,
        .rprev          = (fp_rprev)bucket_rprev,
        .find           = (fp_find)bucket_find,
        .insert         = (fp_insert)bucket_insert,
        .insert_replace = (fp_insert_replace)bucket_insert_replace,
        .erase          = (fp_erase)bucket_erase,
        .remove         = bucket_remove,
        .clear          = bucket_clear,
    };
    return &ins;
}

/* __always_inline */ inline const class_bucket_priv_t* class_bucket_priv_ins(void)
{
    static const class_bucket_priv_t ins = {
        /* iterator */
        .end            = __bucket_end,
        .begin          = bucket_begin,
        .next           = bucket_next,
        .prev           = bucket_prev,
        .rend           = __bucket_rend,
        .rbegin         = bucket_rbegin,
        .rnext          = bucket_rnext,
        .rprev          = bucket_rprev,
        .find           = bucket_find,
        .insert         = bucket_insert,
        .insert_replace = bucket_insert_replace,
        .erase          = bucket_erase,
        /* hashmap */
        ._first                           = _bucket_first,
        ._last                            = _bucket_last,
        .find_has_checked_valid           = bucket_find_has_checked_valid,
        .insert_has_checked_same          = bucket_insert_has_checked_same,
        .insert_has_checked_valid         = bucket_insert_has_checked_valid,
        .insert_replace_has_checked_valid = bucket_insert_replace_has_checked_valid,
        .remove_has_checked_valid         = bucket_remove_has_checked_valid,
        .pop                              = bucket_pop,
        .__switch                         = __bucket_switch,
    };
    return &ins;
}

const class_bucket_priv_t* class_bucket_priv_ins(void);
#define g_class_bucket_priv()            class_bucket_priv_ins()
#define cbucket_priv                     g_class_bucket_priv()
#else

/* Macros */
#define ___hmbucket_type(p) (p->ds.l & BKT_DS_VALID)

static __always_inline void ___hmbucket_set_type(bucket_shell_t* p, bucket_ds_t ntype)
{
    p->ds.l = (p->ds.l & ~BKT_DS_VALID) | ntype;
}

static __always_inline bucket_ds_t ___hmbucket_xchg_type(bucket_shell_t* p, bucket_ds_t ntype)
{
    bucket_ds_t ret = ___hmbucket_type(p);
    p->ds.l = (p->ds.l & ~BKT_DS_VALID) | ntype;
    return ret;
}

static __always_inline bool ___hmbucket_is_tree(const bucket_shell_t* p)
{
    return !!(p->ds.l & BKT_DS_RBTREE);
}

static __always_inline bool ___hmbucket_valid(const bucket_shell_t* p)
{
    return !!___hmbucket_type(p);
}

static __always_inline bool ___hmbucket_invalid(const bucket_shell_t* p)
{
    return !___hmbucket_type(p);
}

#define __hmbucket_is_tree(sh) ___hmbucket_is_tree(&(sh))
#define __hmbucket_valid(sh)   ___hmbucket_valid(&(sh))
#define __hmbucket_invalid(sh) ___hmbucket_invalid(&(sh))

#define BUCKET_SH_TYPE_GET_CLEAR(sh) bucket_ds_t type = ___hmbucket_xchg_type((bucket_shell_t*)(sh), 0)
#define BUCKET_SH_TYPE_RESUME(sh)    do { ___hmbucket_set_type((bucket_shell_t*)(sh), type); } while(0)

/* Interface: bucket shell */
static __always_inline bucket_size_t _shbucket_size(const bucket_shell_t* bucket_sh)
{
    return _bucket_size(bucket_sh);
}

static __always_inline bucket_node_t* __shbucket_end(const bucket_shell_t* bucket_sh)
{
    return __bucket_end(bucket_sh);
}

static __always_inline bucket_node_t* shbucket_begin(const bucket_shell_t* bucket_sh)
{
    BUCKET_SH_TYPE_GET_CLEAR(bucket_sh);
    bucket_node_t* ret = bucket_begin(bucket_sh, type);
    BUCKET_SH_TYPE_RESUME(bucket_sh);
    return ret;
}

static __always_inline bucket_node_t* shbucket_next(const bucket_shell_t* bucket_sh, const bucket_node_t* node)
{
    BUCKET_SH_TYPE_GET_CLEAR(bucket_sh);
    bucket_node_t* ret = bucket_next(bucket_sh, type, node);
    BUCKET_SH_TYPE_RESUME(bucket_sh);
    return ret;
}

static __always_inline bucket_node_t* shbucket_prev(const bucket_shell_t* bucket_sh, const bucket_node_t* node)
{
    BUCKET_SH_TYPE_GET_CLEAR(bucket_sh);
    bucket_node_t* ret = bucket_prev(bucket_sh, type, node);
    BUCKET_SH_TYPE_RESUME(bucket_sh);
    return ret;
}

static __always_inline bucket_node_t* __shbucket_rend(const bucket_shell_t* bucket_sh)
{
    return __bucket_rend(bucket_sh);
}

static __always_inline bucket_node_t* shbucket_rbegin(const bucket_shell_t* bucket_sh)
{
    BUCKET_SH_TYPE_GET_CLEAR(bucket_sh);
    bucket_node_t* ret = bucket_rbegin(bucket_sh, type);
    BUCKET_SH_TYPE_RESUME(bucket_sh);
    return ret;
}

static __always_inline bucket_node_t* shbucket_rnext(const bucket_shell_t* bucket_sh, const bucket_node_t* node)
{
    BUCKET_SH_TYPE_GET_CLEAR(bucket_sh);
    bucket_node_t* ret = bucket_rnext(bucket_sh, type, node);
    BUCKET_SH_TYPE_RESUME(bucket_sh);
    return ret;
}

static __always_inline bucket_node_t* shbucket_rprev(const bucket_shell_t* bucket_sh, const bucket_node_t* node)
{
    BUCKET_SH_TYPE_GET_CLEAR(bucket_sh);
    bucket_node_t* ret = bucket_rprev(bucket_sh, type, node);
    BUCKET_SH_TYPE_RESUME(bucket_sh);
    return ret;
}

static __always_inline bucket_node_t* shbucket_find(const bucket_shell_t* bucket_sh, const class_bucket_ops_t* ops, bucket_key_t key)
{
    BUCKET_SH_TYPE_GET_CLEAR(bucket_sh);
    bucket_node_t* ret = bucket_find(bucket_sh, ops, type, key);
    BUCKET_SH_TYPE_RESUME(bucket_sh);
    return ret;
}

static __always_inline bucket_node_t* shbucket_insert(bucket_shell_t* bucket_sh, const class_bucket_ops_t* ops, bucket_hash_t hash, bucket_key_t key, bucket_value_t value)
{
    BUCKET_SH_TYPE_GET_CLEAR(bucket_sh);
    bucket_node_t* ret = bucket_insert(bucket_sh, ops, type, hash, key, value);
    BUCKET_SH_TYPE_RESUME(bucket_sh);
    return ret;
}

static __always_inline bucket_node_t* shbucket_insert_replace(bucket_shell_t* bucket_sh, const class_bucket_ops_t* ops, bucket_hash_t hash, bucket_key_t key, bucket_value_t value)
{
    BUCKET_SH_TYPE_GET_CLEAR(bucket_sh);
    bucket_node_t* ret = bucket_insert_replace(bucket_sh, ops, type, hash, key, value);
    BUCKET_SH_TYPE_RESUME(bucket_sh);
    return ret;
}

static __always_inline bucket_node_t* shbucket_erase(bucket_shell_t* bucket_sh, const class_bucket_ops_t* ops, bucket_node_t* pos)
{
    BUCKET_SH_TYPE_GET_CLEAR(bucket_sh);
    bucket_node_t* ret = bucket_erase(bucket_sh, ops, type, pos);
    BUCKET_SH_TYPE_RESUME(bucket_sh);
    return ret;
}

static __always_inline bucket_size_t shbucket_remove(bucket_shell_t* bucket_sh, const class_bucket_ops_t* ops, bucket_key_t key)
{
    BUCKET_SH_TYPE_GET_CLEAR(bucket_sh);
    bucket_size_t ret = bucket_remove(bucket_sh, ops, type, key);
    BUCKET_SH_TYPE_RESUME(bucket_sh);
    return ret;
}

static __always_inline bucket_size_t shbucket_clear(bucket_shell_t* bucket_sh, const class_bucket_ops_t* ops)
{
    BUCKET_SH_TYPE_GET_CLEAR(bucket_sh);
    bucket_size_t ret = bucket_clear(bucket_sh, ops, type);
    BUCKET_SH_TYPE_RESUME(bucket_sh);
    return ret;
}

typedef bucket_iterator_t* (*shfp_end)(const bucket_shell_t* bucket_sh);
typedef bucket_iterator_t* (*shfp_begin)(const bucket_shell_t* bucket_sh);
typedef bucket_iterator_t* (*shfp_next)(const bucket_shell_t* bucket_sh, const bucket_iterator_t* iterator);
typedef bucket_iterator_t* (*shfp_prev)(const bucket_shell_t* bucket_sh, const bucket_iterator_t* iterator);
typedef bucket_r_iterator_t* (*shfp_rend)(const bucket_shell_t* bucket_sh);
typedef bucket_r_iterator_t* (*shfp_rbegin)(const bucket_shell_t* bucket_sh);
typedef bucket_r_iterator_t* (*shfp_rnext)(const bucket_shell_t* bucket_sh, const bucket_r_iterator_t* r_iterator);
typedef bucket_r_iterator_t* (*shfp_rprev)(const bucket_shell_t* bucket_sh, const bucket_r_iterator_t* r_iterator);
typedef bucket_iterator_t* (*shfp_find)(const bucket_shell_t* bucket_sh, const class_bucket_ops_t* ops, bucket_key_t key);
typedef bucket_iterator_t* (*shfp_insert)(bucket_shell_t* bucket_sh, const class_bucket_ops_t* ops, bucket_hash_t hash, bucket_key_t key, bucket_value_t value);
typedef bucket_iterator_t* (*shfp_insert_replace)(bucket_shell_t* bucket_sh, const class_bucket_ops_t* ops, bucket_hash_t hash, bucket_key_t key, bucket_value_t value);
typedef bucket_iterator_t* (*shfp_erase)(bucket_shell_t* bucket_sh, const class_bucket_ops_t* ops, bucket_iterator_t* iterator);

/* __always_inline */ inline const class_bucket_t* class_bucket_ins(void)
{
    static const class_bucket_t ins = {
        .size           = _shbucket_size,
        .end            = (shfp_end)__shbucket_end,
        .begin          = (shfp_begin)shbucket_begin,
        .next           = (shfp_next)shbucket_next,
        .prev           = (shfp_prev)shbucket_prev,
        .rend           = (shfp_rend)__shbucket_rend,
        .rbegin         = (shfp_rbegin)shbucket_rbegin,
        .rnext          = (shfp_rnext)shbucket_rnext,
        .rprev          = (shfp_rprev)shbucket_rprev,
        .find           = (shfp_find)shbucket_find,
        .insert         = (shfp_insert)shbucket_insert,
        .insert_replace = (shfp_insert_replace)shbucket_insert_replace,
        .erase          = (shfp_erase)shbucket_erase,
        .remove         = shbucket_remove,
        .clear          = shbucket_clear,
    };
    return &ins;
}



/* Interface: hashmap */
static __always_inline bool __hmbucket_empty(const bucket_shell_t* bucket_sh)
{
    return is_null(bucket_sh->ds.l & ~BKT_DS_VALID); /* TODO: Current approach is for performance, only works in this version */
}

static __always_inline bucket_size_t __hmbucket_size(const bucket_shell_t* bucket_sh)
{
    return __bucket_size(bucket_sh);
}

static __always_inline bucket_node_t* __hmbucket_end(const bucket_shell_t* bucket_sh)
{
    return __bucket_end(bucket_sh);
}

static __always_inline bucket_node_t* __hmbucket_rend(const bucket_shell_t* bucket_sh)
{
    return __bucket_rend(bucket_sh);
}

static __always_inline bucket_node_t* _hmbucket_first(const bucket_shell_t* bucket_sh)
{
    BUCKET_SH_TYPE_GET_CLEAR(bucket_sh);
    bucket_node_t* ret = _bucket_first(bucket_sh, type);
    BUCKET_SH_TYPE_RESUME(bucket_sh);
    return ret;
}

static __always_inline bucket_node_t* _hmbucket_last(const bucket_shell_t* bucket_sh)
{
    BUCKET_SH_TYPE_GET_CLEAR(bucket_sh);
    bucket_node_t* ret = _bucket_last(bucket_sh, type);
    BUCKET_SH_TYPE_RESUME(bucket_sh);
    return ret;
}

static __always_inline bucket_node_t* hmbucket_begin(const bucket_shell_t* bucket_sh)
{
    BUCKET_SH_TYPE_GET_CLEAR(bucket_sh);
    bucket_node_t* ret = bucket_begin(bucket_sh, type);
    BUCKET_SH_TYPE_RESUME(bucket_sh);
    return ret;
}

static __always_inline bucket_node_t* hmbucket_next(const bucket_shell_t* bucket_sh, const bucket_node_t* node)
{
    BUCKET_SH_TYPE_GET_CLEAR(bucket_sh);
    bucket_node_t* ret = bucket_next(bucket_sh, type, node);
    BUCKET_SH_TYPE_RESUME(bucket_sh);
    return ret;
}

static __always_inline bucket_node_t* hmbucket_prev(const bucket_shell_t* bucket_sh, const bucket_node_t* node)
{
    BUCKET_SH_TYPE_GET_CLEAR(bucket_sh);
    bucket_node_t* ret = bucket_prev(bucket_sh, type, node);
    BUCKET_SH_TYPE_RESUME(bucket_sh);
    return ret;
}

static __always_inline bucket_node_t* hmbucket_rnext(const bucket_shell_t* bucket_sh, const bucket_node_t* node)
{
    BUCKET_SH_TYPE_GET_CLEAR(bucket_sh);
    bucket_node_t* ret = bucket_rnext(bucket_sh, type, node);
    BUCKET_SH_TYPE_RESUME(bucket_sh);
    return ret;
}

static __always_inline bucket_node_t* hmbucket_rprev(const bucket_shell_t* bucket_sh, const bucket_node_t* node)
{
    BUCKET_SH_TYPE_GET_CLEAR(bucket_sh);
    bucket_node_t* ret = bucket_rprev(bucket_sh, type, node);
    BUCKET_SH_TYPE_RESUME(bucket_sh);
    return ret;
}

static __always_inline bucket_node_t* hmbucket_find_hc_valid(const bucket_shell_t* bucket_sh, const class_bucket_ops_t* ops, bucket_key_t key)
{
    BUCKET_SH_TYPE_GET_CLEAR(bucket_sh);
    bucket_node_t* ret = bucket_find_has_checked_valid(bucket_sh, ops, type, key);
    BUCKET_SH_TYPE_RESUME(bucket_sh);
    return ret;
}

static __always_inline bucket_node_t* hmbucket_insert_hc_same(bucket_shell_t* bucket_sh, const class_bucket_ops_t* ops, bucket_node_t* node)
{
    BUCKET_SH_TYPE_GET_CLEAR(bucket_sh);
    bucket_node_t* ret = bucket_insert_has_checked_same(bucket_sh, ops, type, node);
    BUCKET_SH_TYPE_RESUME(bucket_sh);
    return ret;
}

static __always_inline bucket_node_t* hmbucket_insert_hc_valid(bucket_shell_t* bucket_sh, const class_bucket_ops_t* ops, bucket_hash_t hash, bucket_key_t key, bucket_value_t value)
{
    BUCKET_SH_TYPE_GET_CLEAR(bucket_sh);
    bucket_node_t* ret = bucket_insert_has_checked_valid(bucket_sh, ops, type, hash, key, value);
    BUCKET_SH_TYPE_RESUME(bucket_sh);
    return ret;
}

static __always_inline bucket_node_t* hmbucket_insert_replace_hc_valid(bucket_shell_t* bucket_sh, const class_bucket_ops_t* ops, bucket_hash_t hash, bucket_key_t key, bucket_value_t value)
{
    BUCKET_SH_TYPE_GET_CLEAR(bucket_sh);
    bucket_node_t* ret = bucket_insert_replace_has_checked_valid(bucket_sh, ops, type, hash, key, value);
    BUCKET_SH_TYPE_RESUME(bucket_sh);
    return ret;
}

static __always_inline bucket_node_t* hmbucket_erase(bucket_shell_t* bucket_sh, const class_bucket_ops_t* ops, bucket_node_t* pos)
{
    BUCKET_SH_TYPE_GET_CLEAR(bucket_sh);
    bucket_node_t* ret = bucket_erase(bucket_sh, ops, type, pos);
    BUCKET_SH_TYPE_RESUME(bucket_sh);
    return ret;
}

static __always_inline bucket_size_t hmbucket_remove_hc_valid(bucket_shell_t* bucket_sh, const class_bucket_ops_t* ops, bucket_key_t key)
{
    BUCKET_SH_TYPE_GET_CLEAR(bucket_sh);
    bucket_size_t ret = bucket_remove_has_checked_valid(bucket_sh, ops, type, key);
    BUCKET_SH_TYPE_RESUME(bucket_sh);
    return ret;
}

static __always_inline bucket_node_t* hmbucket_pop(bucket_shell_t* bucket_sh, bucket_node_t* pos)
{
    BUCKET_SH_TYPE_GET_CLEAR(bucket_sh);
    bucket_node_t* ret = bucket_pop(bucket_sh, type, pos);
    BUCKET_SH_TYPE_RESUME(bucket_sh);
    return ret;
}

static __always_inline bucket_size_t hmbucket_clear(bucket_shell_t* bucket_sh, const class_bucket_ops_t* ops)
{
    BUCKET_SH_TYPE_GET_CLEAR(bucket_sh);
    bucket_size_t ret = bucket_clear(bucket_sh, ops, type);
    BUCKET_SH_TYPE_RESUME(bucket_sh);
    return ret;
}

static __always_inline void __hmbucket_resume(bucket_shell_t* bucket_sh)
{
    BUCKET_SH_TYPE_GET_CLEAR(bucket_sh);
    __bucket_resume(bucket_sh, type);
    BUCKET_SH_TYPE_RESUME(bucket_sh);
}
#endif
