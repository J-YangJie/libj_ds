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
#include <_memory.h>
#include <linux/list.h>
#include <linux/rbtree.h>
#include <linux/_compiler.h>
#include <iterator/iterator.h>

#define bucket_rb_entry(ptr)    rb_entry((ptr), struct bucket_node, ds_node.rb_node)
#ifdef BUCKET_USE_DL
#define bucket_dl_entry(ptr)  list_entry((ptr), struct bucket_node, ds_node.dl_node)
#endif /* BUCKET_USE_DL */
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
static __always_inline bucket_node_t* __bucket_rb_first(const bucket_t* _this)
{
    struct rb_node* t = rb_first(&_this->ds.rb);
    return is_null(t) ? NULL : bucket_rb_entry(t);
}

static __always_inline bucket_node_t* __bucket_rb_last(const bucket_t* _this)
{
    struct rb_node* t = rb_last(&_this->ds.rb);
    return is_null(t) ? NULL : bucket_rb_entry(t);
}

static __always_inline bucket_node_t* __bucket_rb_begin(const bucket_t* _this)
{
    bucket_node_t* t = __bucket_rb_first(_this);
    return is_null(t) ? __bucket_end(_this) : t;
}

static __always_inline bucket_node_t* _bucket_rb_begin(const bucket_t* _this)
{
    if (unlikely(is_null(_this)))
        return NULL;
    return __bucket_rb_begin(_this);
}

static __always_inline bucket_node_t* __bucket_rb_next(const bucket_t* _this, const bucket_node_t* node)
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

static __always_inline bucket_node_t* _bucket_rb_next(const bucket_t* _this, const bucket_node_t* node)
{
    if (unlikely(is_null(_this) || is_null(node)))
        return NULL;
    return __bucket_rb_next(_this, node);
}

static __always_inline bucket_node_t* __bucket_rb_prev(const bucket_t* _this, const bucket_node_t* node)
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

static __always_inline bucket_node_t* _bucket_rb_prev(const bucket_t* _this, const bucket_node_t* node)
{
    if (unlikely(is_null(_this) || is_null(node)))
        return NULL;
    return __bucket_rb_prev(_this, node);
}

static __always_inline bucket_node_t* __bucket_rb_rbegin(const bucket_t* _this)
{
    bucket_node_t* t = __bucket_rb_last(_this);
    return is_null(t) ? __bucket_rend(_this) : t;
}

static __always_inline bucket_node_t* _bucket_rb_rbegin(const bucket_t* _this)
{
    if (unlikely(is_null(_this)))
        return NULL;
    return __bucket_rb_rbegin(_this);
}

static __always_inline bucket_node_t* __bucket_rb_rnext(const bucket_t* _this, const bucket_node_t* node)
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

static __always_inline bucket_node_t* _bucket_rb_rnext(const bucket_t* _this, const bucket_node_t* node)
{
    if (unlikely(is_null(_this) || is_null(node)))
        return NULL;
    return __bucket_rb_rnext(_this, node);
}

static __always_inline bucket_node_t* __bucket_rb_rprev(const bucket_t* _this, const bucket_node_t* node)
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

static __always_inline bucket_node_t* _bucket_rb_rprev(const bucket_t* _this, const bucket_node_t* node)
{
    if (unlikely(is_null(_this) || is_null(node)))
        return NULL;
    return __bucket_rb_rprev(_this, node);
}


#ifdef BUCKET_USE_DL
static __always_inline bucket_node_t* __bucket_dl_first(const bucket_t* _this)
{
    if (is_null(_this->ds.dl.next) || list_empty(&_this->ds.dl))
        return NULL; /* Err: the return value includes the error case where `next` is equal to `NULL` */
    return bucket_dl_entry(_this->ds.dl.next);
}

static __always_inline bucket_node_t* __bucket_dl_last(const bucket_t* _this)
{
    if (is_null(_this->ds.dl.prev) || list_empty(&_this->ds.dl))
        return NULL; /* Err: the return value includes the error case where `prev` is equal to `NULL` */
    return bucket_dl_entry(_this->ds.dl.prev);
}

static __always_inline bucket_node_t* __bucket_dl_begin(const bucket_t* _this)
{
    struct list_head* t = _this->ds.dl.next;

    if (unlikely(is_null(t)))
        return NULL; /* Err: memory has been modified illegally */

    if (list_empty(&_this->ds.dl))
        return __bucket_end(_this);
    return bucket_dl_entry(t); /* Be careful about the situation when `t` is `NULL` */
}

static __always_inline bucket_node_t* _bucket_dl_begin(const bucket_t* _this)
{
    if (unlikely(is_null(_this)))
        return NULL;
    return __bucket_dl_begin(_this);
}

static __always_inline bucket_node_t* __bucket_dl_next(const bucket_t* _this, const bucket_node_t* node)
{
    struct list_head* t;

    if (list_empty(&_this->ds.dl) || __bucket_end(_this) == node)
        return __bucket_end(_this);

    /* The input parameter is `iterator`, and there's no need 
       to check whether it equals `rend` */

    t = node->ds_node.dl_node.next;
    if (unlikely(is_null(t)))
        return NULL; /* Err: `node` doesn't belong to current list or memory has been modified illegally */

    if (&_this->ds.dl == t)
        return __bucket_end(_this);
    return bucket_dl_entry(t);
}

static __always_inline bucket_node_t* _bucket_dl_next(const bucket_t* _this, const bucket_node_t* node)
{
    if (unlikely(is_null(_this) || is_null(node)))
        return NULL;
    return __bucket_dl_next(_this, node);
}

static __always_inline bucket_node_t* __bucket_dl_prev(const bucket_t* _this, const bucket_node_t* node)
{
    struct list_head* t;

    if (list_empty(&_this->ds.dl))
        return __bucket_end(_this);

    if (__bucket_end(_this) == node)
        return __bucket_dl_last(_this); /* Err: since the `ds` is non-empty, the return value 
                                                includes the error case of `NULL` */

    t = node->ds_node.dl_node.prev;
    if (unlikely(is_null(t)))
        return NULL; /* Err: `node` doesn't belong to current list or memory has been modified illegally */

    if (&_this->ds.dl == t)
        return __bucket_end(_this);
    return bucket_dl_entry(t);
}

static __always_inline bucket_node_t* _bucket_dl_prev(const bucket_t* _this, const bucket_node_t* node)
{
    if (unlikely(is_null(_this) || is_null(node)))
        return NULL;
    return __bucket_dl_prev(_this, node);
}

static __always_inline bucket_node_t* __bucket_dl_rbegin(const bucket_t* _this)
{
    struct list_head* t = _this->ds.dl.prev;

    if (unlikely(is_null(t)))
        return NULL; /* Err: memory has been modified illegally */

    if (list_empty(&_this->ds.dl))
        return __bucket_rend(_this);
    return bucket_dl_entry(t); /* Be careful about the situation when `t` is `NULL` */
}

static __always_inline bucket_node_t* _bucket_dl_rbegin(const bucket_t* _this)
{
    if (unlikely(is_null(_this)))
        return NULL;
    return __bucket_dl_rbegin(_this);
}

static __always_inline bucket_node_t* __bucket_dl_rnext(const bucket_t* _this, const bucket_node_t* node)
{
    struct list_head* t;

    if (list_empty(&_this->ds.dl) || __bucket_rend(_this) == node)
        return __bucket_rend(_this);

    /* The input parameter is `reverse_iterator`, and there's no need 
       to check whether it equals `end` */

    t = node->ds_node.dl_node.prev;
    if (unlikely(is_null(t)))
        return NULL; /* Err: `node` doesn't belong to current list or memory has been modified illegally */

    if (&_this->ds.dl == t)
        return __bucket_rend(_this);
    return bucket_dl_entry(t);
}

static __always_inline bucket_node_t* _bucket_dl_rnext(const bucket_t* _this, const bucket_node_t* node)
{
    if (unlikely(is_null(_this) || is_null(node)))
        return NULL;
    return __bucket_dl_rnext(_this, node);
}

static __always_inline bucket_node_t* __bucket_dl_rprev(const bucket_t* _this, const bucket_node_t* node)
{
    struct list_head* t;

    if (list_empty(&_this->ds.dl))
        return __bucket_rend(_this);

    if (__bucket_rend(_this) == node)
        return __bucket_dl_first(_this); /* Err: since the `ds` is non-empty, the return value 
                                                 includes the error case of `NULL` */

    t = node->ds_node.dl_node.next;
    if (unlikely(is_null(t)))
        return NULL; /* Err: `node` doesn't belong to current list or memory has been modified illegally */

    if (&_this->ds.dl == t)
        return __bucket_rend(_this);
    return bucket_dl_entry(t);
}

static __always_inline bucket_node_t* _bucket_dl_rprev(const bucket_t* _this, const bucket_node_t* node)
{
    if (unlikely(is_null(_this) || is_null(node)))
        return NULL;
    return __bucket_dl_rprev(_this, node);
}
#endif /* BUCKET_USE_DL */


static __always_inline bucket_node_t* __bucket_hl_first(const bucket_t* _this)
{
    struct hlist_node* t = _this->ds.hl.first;

    if (hlist_empty(&_this->ds.hl))
        return NULL;
    return bucket_hl_entry(t);
}

static inline bucket_node_t* __bucket_hl_last(const bucket_t* _this)
{
    struct hlist_node* t = _this->ds.hl.first;

    if (hlist_empty(&_this->ds.hl))
        return NULL;

    /* TODO: it is not recommended to use this data structure to get rbegin when there are many nodes */
    while (!is_null(t->next)) t = t->next;
    return bucket_hl_entry(t);
}

static __always_inline bucket_node_t* __bucket_hl_begin(const bucket_t* _this)
{
    struct hlist_node* t = _this->ds.hl.first;

    if (hlist_empty(&_this->ds.hl))
        return __bucket_end(_this);
    return bucket_hl_entry(t);
}

static __always_inline bucket_node_t* _bucket_hl_begin(const bucket_t* _this)
{
    if (unlikely(is_null(_this)))
        return NULL;
    return __bucket_hl_begin(_this);
}

static __always_inline bucket_node_t* __bucket_hl_next(const bucket_t* _this, const bucket_node_t* node)
{
    struct hlist_node* t;

    if (hlist_empty(&_this->ds.hl) || __bucket_end(_this) == node)
        return __bucket_end(_this);

    /* The input parameter is `iterator`, and there's no need 
       to check whether it equals `rend` */

    t = node->ds_node.hl_node.next;
    return is_null(t) ? __bucket_end(_this) : bucket_hl_entry(t);
}

static __always_inline bucket_node_t* _bucket_hl_next(const bucket_t* _this, const bucket_node_t* node)
{
    if (unlikely(is_null(_this) || is_null(node)))
        return NULL;
    return __bucket_hl_next(_this, node);
}

static __always_inline bucket_node_t* __bucket_hl_prev(const bucket_t* _this, const bucket_node_t* node)
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

static __always_inline bucket_node_t* _bucket_hl_prev(const bucket_t* _this, const bucket_node_t* node)
{
    if (unlikely(is_null(_this) || is_null(node)))
        return NULL;
    return __bucket_hl_prev(_this, node);
}

static inline bucket_node_t* __bucket_hl_rbegin(const bucket_t* _this)
{
    struct hlist_node* t = _this->ds.hl.first;

    if (hlist_empty(&_this->ds.hl))
        return __bucket_rend(_this);

    /* TODO: it is not recommended to use this data structure to get rbegin when there are many nodes */
    while (!is_null(t->next)) t = t->next;
    return bucket_hl_entry(t);
}

static __always_inline bucket_node_t* _bucket_hl_rbegin(const bucket_t* _this)
{
    if (unlikely(is_null(_this)))
        return NULL;
    return __bucket_hl_rbegin(_this);
}

static __always_inline bucket_node_t* __bucket_hl_rnext(const bucket_t* _this, const bucket_node_t* node)
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

static __always_inline bucket_node_t* _bucket_hl_rnext(const bucket_t* _this, const bucket_node_t* node)
{
    if (unlikely(is_null(_this) || is_null(node)))
        return NULL;
    return __bucket_hl_rnext(_this, node);
}

static __always_inline bucket_node_t* __bucket_hl_rprev(const bucket_t* _this, const bucket_node_t* node)
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

static __always_inline bucket_node_t* _bucket_hl_rprev(const bucket_t* _this, const bucket_node_t* node)
{
    if (unlikely(is_null(_this) || is_null(node)))
        return NULL;
    return __bucket_hl_rprev(_this, node);
}


static __always_inline bucket_node_t* _bucket_first(const bucket_t* _this, bucket_ds_t type)
{
    if (unlikely(is_null(_this)))
        return NULL;

    switch (type)
    {
    case BUCKET_DS_HLIST:
        return __bucket_hl_first(_this);
#ifdef BUCKET_USE_DL
    case BUCKET_DS_DLIST:
        return __bucket_dl_first(_this);
#endif /* BUCKET_USE_DL */
    case BUCKET_DS_RBTREE:
        return __bucket_rb_first(_this);
    default:
        return NULL;
    }
}

static __always_inline bucket_node_t* _bucket_last(const bucket_t* _this, bucket_ds_t type)
{
    if (unlikely(is_null(_this)))
        return NULL;

    switch (type)
    {
    case BUCKET_DS_HLIST:
        return __bucket_hl_last(_this);
#ifdef BUCKET_USE_DL
    case BUCKET_DS_DLIST:
        return __bucket_dl_last(_this);
#endif /* BUCKET_USE_DL */
    case BUCKET_DS_RBTREE:
        return __bucket_rb_last(_this);
    default:
        return NULL;
    }
}

static __always_inline bucket_node_t* bucket_begin(const bucket_t* _this, bucket_ds_t type)
{
    switch (type)
    {
    case BUCKET_DS_HLIST:
        return _bucket_hl_begin(_this);
#ifdef BUCKET_USE_DL
    case BUCKET_DS_DLIST:
        return _bucket_dl_begin(_this);
#endif /* BUCKET_USE_DL */
    case BUCKET_DS_RBTREE:
        return _bucket_rb_begin(_this);
    default:
        return NULL;
    }
}

static __always_inline bucket_node_t* bucket_next(const bucket_t* _this, bucket_ds_t type, const bucket_node_t* node)
{
    switch (type)
    {
    case BUCKET_DS_HLIST:
        return _bucket_hl_next(_this, node);
#ifdef BUCKET_USE_DL
    case BUCKET_DS_DLIST:
        return _bucket_dl_next(_this, node);
#endif /* BUCKET_USE_DL */
    case BUCKET_DS_RBTREE:
        return _bucket_rb_next(_this, node);
    default:
        return NULL;
    }
}

static __always_inline bucket_node_t* bucket_prev(const bucket_t* _this, bucket_ds_t type, const bucket_node_t* node)
{
    switch (type)
    {
    case BUCKET_DS_HLIST:
        return _bucket_hl_prev(_this, node);
#ifdef BUCKET_USE_DL
    case BUCKET_DS_DLIST:
        return _bucket_dl_prev(_this, node);
#endif /* BUCKET_USE_DL */
    case BUCKET_DS_RBTREE:
        return _bucket_rb_prev(_this, node);
    default:
        return NULL;
    }
}

static __always_inline bucket_node_t* bucket_rbegin(const bucket_t* _this, bucket_ds_t type)
{
    switch (type)
    {
    case BUCKET_DS_HLIST:
        return _bucket_hl_rbegin(_this);
#ifdef BUCKET_USE_DL
    case BUCKET_DS_DLIST:
        return _bucket_dl_rbegin(_this);
#endif /* BUCKET_USE_DL */
    case BUCKET_DS_RBTREE:
        return _bucket_rb_rbegin(_this);
    default:
        return NULL;
    }
}

static __always_inline bucket_node_t* bucket_rnext(const bucket_t* _this, bucket_ds_t type, const bucket_node_t* node)
{
    switch (type)
    {
    case BUCKET_DS_HLIST:
        return _bucket_hl_rnext(_this, node);
#ifdef BUCKET_USE_DL
    case BUCKET_DS_DLIST:
        return _bucket_dl_rnext(_this, node);
#endif /* BUCKET_USE_DL */
    case BUCKET_DS_RBTREE:
        return _bucket_rb_rnext(_this, node);
    default:
        return NULL;
    }
}

static __always_inline bucket_node_t* bucket_rprev(const bucket_t* _this, bucket_ds_t type, const bucket_node_t* node)
{
    switch (type)
    {
    case BUCKET_DS_HLIST:
        return _bucket_hl_rprev(_this, node);
#ifdef BUCKET_USE_DL
    case BUCKET_DS_DLIST:
        return _bucket_dl_rprev(_this, node);
#endif /* BUCKET_USE_DL */
    case BUCKET_DS_RBTREE:
        return _bucket_rb_rprev(_this, node);
    default:
        return NULL;
    }
}



/* Find */
static inline bucket_node_t* __bucket_rb_find(const bucket_t* _this, const class_bucket_ops_t* ops, bucket_key_t key)
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

#ifdef BUCKET_USE_DL
static inline bucket_node_t* __bucket_dl_find(const bucket_t* _this, const class_bucket_ops_t* ops, bucket_key_t key)
{
    bucket_node_t* t = NULL;

    if (is_null(ops) || is_null(ops->__lt)) {
        list_for_each_entry(t, &_this->ds.dl, ds_node.dl_node) {
            if (key == t->key)
                return t;
        }
    } else {
        list_for_each_entry(t, &_this->ds.dl, ds_node.dl_node) {
            if (!ops->__lt(key, t->key) && !ops->__lt(t->key, key))
                return t;
        }
    }
    return NULL;
}
#endif /* BUCKET_USE_DL */

static inline bucket_node_t* __bucket_hl_find(const bucket_t* _this, const class_bucket_ops_t* ops, bucket_key_t key)
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

static __always_inline bucket_node_t* bucket_find(const bucket_t* _this, const class_bucket_ops_t* ops, bucket_ds_t type, bucket_key_t key)
{
    bucket_node_t* t = NULL;

    if (unlikely(is_null(_this)))
        return NULL;

    if (!is_null(ops) && !is_null(ops->valid_key) && !ops->valid_key(key))
        return NULL;

    switch (type)
    {
    case BUCKET_DS_HLIST:
        t = __bucket_hl_find(_this, ops, key);
        break;
#ifdef BUCKET_USE_DL
    case BUCKET_DS_DLIST:
        t = __bucket_dl_find(_this, ops, key);
        break;
#endif /* BUCKET_USE_DL */
    case BUCKET_DS_RBTREE:
        t = __bucket_rb_find(_this, ops, key);
        break;
    default:
        return NULL;
    }
    return is_null(t) ? __bucket_end(_this) : t;
}

static __always_inline bucket_node_t* bucket_find_has_checked_valid(const bucket_t* _this, const class_bucket_ops_t* ops, bucket_ds_t type, bucket_key_t key)
{
    bucket_node_t* t = NULL;

    if (unlikely(is_null(_this)))
        return NULL;

    switch (type)
    {
    case BUCKET_DS_HLIST:
        t = __bucket_hl_find(_this, ops, key);
        break;
#ifdef BUCKET_USE_DL
    case BUCKET_DS_DLIST:
        t = __bucket_dl_find(_this, ops, key);
        break;
#endif /* BUCKET_USE_DL */
    case BUCKET_DS_RBTREE:
        t = __bucket_rb_find(_this, ops, key);
        break;
    default:
        return NULL;
    }
    return is_null(t) ? __bucket_end(_this) : t;
}



/* Add */
static inline bucket_node_t* __bucket_rb_insert(bucket_t* _this, const class_bucket_ops_t* ops, bucket_node_t* node)
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

#ifdef BUCKET_USE_DL
static __always_inline bucket_node_t* __bucket_dl_push_front(bucket_t* _this, bucket_node_t* node)
{
    list_add(&node->ds_node.dl_node, &_this->ds.dl);
    _this->size++;
    return node;
}
#endif /* BUCKET_USE_DL */

static __always_inline bucket_node_t* __bucket_hl_push_front(bucket_t* _this, bucket_node_t* node)
{
    hlist_add_head(&node->ds_node.hl_node, &_this->ds.hl);
    _this->size++;
    return node;
}

static __always_inline bucket_node_t* bucket_insert_has_checked_same(bucket_t* _this, const class_bucket_ops_t* ops, bucket_ds_t type, bucket_node_t* node)
{
    if (unlikely(is_null(_this) || is_null(node)))
        return NULL;

    switch (type)
    {
    case BUCKET_DS_HLIST:
        return __bucket_hl_push_front(_this, node);
#ifdef BUCKET_USE_DL
    case BUCKET_DS_DLIST:
        return __bucket_dl_push_front(_this, node);
#endif /* BUCKET_USE_DL */
    case BUCKET_DS_RBTREE:
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

    if (BUCKET_DS_RBTREE != type) {
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
    case BUCKET_DS_HLIST:
        ret = __bucket_hl_push_front(_this, t);
        break;
#ifdef BUCKET_USE_DL
    case BUCKET_DS_DLIST:
        ret = __bucket_dl_push_front(_this, t);
        break;
#endif /* BUCKET_USE_DL */
    case BUCKET_DS_RBTREE:
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

    if (BUCKET_DS_RBTREE != type) {
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
    case BUCKET_DS_HLIST:
        ret = __bucket_hl_push_front(_this, t);
        break;
#ifdef BUCKET_USE_DL
    case BUCKET_DS_DLIST:
        ret = __bucket_dl_push_front(_this, t);
        break;
#endif /* BUCKET_USE_DL */
    case BUCKET_DS_RBTREE:
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
    case BUCKET_DS_HLIST:
        ret = __bucket_hl_push_front(_this, t);
        break;
#ifdef BUCKET_USE_DL
    case BUCKET_DS_DLIST:
        ret = __bucket_dl_push_front(_this, t);
        break;
#endif /* BUCKET_USE_DL */
    case BUCKET_DS_RBTREE:
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
    case BUCKET_DS_HLIST:
        ret = __bucket_hl_push_front(_this, t);
        break;
#ifdef BUCKET_USE_DL
    case BUCKET_DS_DLIST:
        ret = __bucket_dl_push_front(_this, t);
        break;
#endif /* BUCKET_USE_DL */
    case BUCKET_DS_RBTREE:
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
static __always_inline bucket_node_t* __bucket_rb_erase(bucket_t* _this, bucket_node_t* pos)
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
    if (__bucket_size(_this) <= 0 || __bucket_end(_this) == pos/* || __bucket_rend(_this) == pos*/)
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

#ifdef BUCKET_USE_DL
static __always_inline void __bucket_dl_erase(bucket_t* _this, bucket_node_t* pos)
{
    list_del_init(&pos->ds_node.dl_node);
    _this->size--;
}

static inline bucket_node_t* bucket_dl_erase(bucket_t* _this, const class_bucket_ops_t* ops, bucket_node_t* pos)
{
    bucket_node_t* t;

    if (unlikely(is_null(_this) || is_null(pos)))
        return NULL;

    /* The input parameter is `iterator`, and there's no need 
       to check whether it equals `rend` */
    if (list_empty(&_this->ds.dl) || __bucket_end(_this) == pos/* || __bucket_rend(_this) == pos*/)
        return NULL;

    t = __bucket_dl_next(_this, pos);
    if (is_null(t))
        return NULL;

    __bucket_dl_erase(_this, pos);

    if (!is_null(ops) && !is_null(ops->free_key))
        ops->free_key(&pos->key);

    if (!is_null(ops) && !is_null(ops->free_value))
        ops->free_value(&pos->value);

    p_free(pos);

    return t;
}
#endif /* BUCKET_USE_DL */

static __always_inline void __bucket_hl_erase(bucket_t* _this, bucket_node_t* pos)
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

static __always_inline bucket_node_t* bucket_erase(bucket_t* _this, const class_bucket_ops_t* ops, bucket_ds_t type, bucket_node_t* pos)
{
    switch (type)
    {
    case BUCKET_DS_HLIST:
        return bucket_hl_erase(_this, ops, pos);
#ifdef BUCKET_USE_DL
    case BUCKET_DS_DLIST:
        return bucket_dl_erase(_this, ops, pos);
#endif /* BUCKET_USE_DL */
    case BUCKET_DS_RBTREE:
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
    case BUCKET_DS_HLIST:
        __bucket_hl_erase(_this, t); /* Always right */
        break;
#ifdef BUCKET_USE_DL
    case BUCKET_DS_DLIST:
        __bucket_dl_erase(_this, t); /* Always right */
        break;
#endif /* BUCKET_USE_DL */
    case BUCKET_DS_RBTREE:
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
    case BUCKET_DS_HLIST:
        __bucket_hl_erase(_this, t); /* Always right */
        break;
#ifdef BUCKET_USE_DL
    case BUCKET_DS_DLIST:
        __bucket_dl_erase(_this, t); /* Always right */
        break;
#endif /* BUCKET_USE_DL */
    case BUCKET_DS_RBTREE:
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
static __always_inline bucket_node_t* bucket_rb_pop(bucket_t* _this, bucket_node_t* pos)
{
    bucket_node_t* t;

    if (unlikely(is_null(_this) || is_null(pos)))
        return NULL;

    /* The input parameter is `iterator`, and there's no need 
       to check whether it equals `rend` */
    if (__bucket_size(_this) <= 0 || __bucket_end(_this) == pos/* || __bucket_rend(_this) == pos*/)
        return NULL;

    t = __bucket_rb_next(_this, pos);
    if (is_null(t))
        return NULL;

    __bucket_rb_erase(_this, pos);

    return t;
}

#ifdef BUCKET_USE_DL
static __always_inline bucket_node_t* bucket_dl_pop(bucket_t* _this, bucket_node_t* pos)
{
    bucket_node_t* t;

    if (unlikely(is_null(_this) || is_null(pos)))
        return NULL;

    /* The input parameter is `iterator`, and there's no need 
       to check whether it equals `rend` */
    if (list_empty(&_this->ds.dl) || __bucket_end(_this) == pos/* || __bucket_rend(_this) == pos*/)
        return NULL;

    t = __bucket_dl_next(_this, pos);
    if (is_null(t))
        return NULL;

    __bucket_dl_erase(_this, pos);

    return t;
}
#endif /* BUCKET_USE_DL */

static __always_inline bucket_node_t* bucket_hl_pop(bucket_t* _this, bucket_node_t* pos)
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

static __always_inline bucket_node_t* bucket_pop(bucket_t* _this, bucket_ds_t type, bucket_node_t* pos)
{
    switch (type)
    {
    case BUCKET_DS_HLIST:
        return bucket_hl_pop(_this, pos);
#ifdef BUCKET_USE_DL
    case BUCKET_DS_DLIST:
        return bucket_dl_pop(_this, pos);
#endif /* BUCKET_USE_DL */
    case BUCKET_DS_RBTREE:
        return bucket_rb_pop(_this, pos);
    default:
        return NULL;
    }
}



/* Clear */
static inline bucket_size_t bucket_rb_clear(bucket_t* _this, const class_bucket_ops_t* ops)
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

#ifdef BUCKET_USE_DL
static inline bucket_size_t bucket_dl_clear(bucket_t* _this, const class_bucket_ops_t* ops)
{
    bucket_size_t ret = 0;
    bucket_node_t* t = NULL;
    bucket_node_t* n = NULL;

    if (unlikely(is_null(_this)))
        return -1;

    list_for_each_entry_safe(t, n, &_this->ds.dl, ds_node.dl_node) {
        __bucket_dl_erase(_this, t);

        if (!is_null(ops) && !is_null(ops->free_key))
            ops->free_key(&t->key);

        if (!is_null(ops) && !is_null(ops->free_value))
            ops->free_value(&t->value);

        p_free(t);
        ret++;
    }

    return ret;
}
#endif /* BUCKET_USE_DL */

static inline bucket_size_t bucket_hl_clear(bucket_t* _this, const class_bucket_ops_t* ops)
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

static __always_inline bucket_size_t bucket_clear(bucket_t* _this, const class_bucket_ops_t* ops, bucket_ds_t type)
{
    switch (type)
    {
    case BUCKET_DS_HLIST:
        return bucket_hl_clear(_this, ops);
#ifdef BUCKET_USE_DL
    case BUCKET_DS_DLIST:
        return bucket_dl_clear(_this, ops);
#endif /* BUCKET_USE_DL */
    case BUCKET_DS_RBTREE:
        return bucket_rb_clear(_this, ops);
    default:
        return -1;
    }
}

static inline void __bucket_switch(bucket_t* _this, const class_bucket_ops_t* ops, bucket_ds_t type_from, bucket_ds_t type_to)
{
    bucket_t tmp = BUCKET_INIT(&tmp, type_to);
    bucket_node_t* tb_node = NULL;

    /* Segment fault: the bucket size doesn't match the actual number of nodes */
    while (__bucket_size(_this) > 0) {
        tb_node = bucket_begin(_this, type_from);
        bucket_pop(_this, type_from, tb_node);
        bucket_insert_has_checked_same(&tmp, ops, type_to, tb_node);
    }

    memset(_this, 0, sizeof(bucket_t));

    switch (type_to)
    {
    case BUCKET_DS_HLIST:
        hlist_move_list(&tmp.ds.hl, &_this->ds.hl);
        break;
#ifdef BUCKET_USE_DL
    case BUCKET_DS_DLIST:
        list_replace(&tmp.ds.dl, &_this->ds.dl);
        break;
#endif /* BUCKET_USE_DL */
    case BUCKET_DS_RBTREE:
        _this->ds.rb.rb_node = tmp.ds.rb.rb_node;
        break;
    default:
        return ; /* Segment fault: may occur */
    }

    _this->size = tmp.size;
}

__always_inline void __bucket_init(bucket_t* bucket, bucket_ds_t type)
{
    switch (type)
    {
    case BUCKET_DS_HLIST:
        INIT_HLIST_HEAD(&bucket->ds.hl);
        break;
#ifdef BUCKET_USE_DL
    case BUCKET_DS_DLIST:
        INIT_LIST_HEAD(&bucket->ds.dl);
        break;
#endif /* BUCKET_USE_DL */
    case BUCKET_DS_RBTREE:
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
    case BUCKET_DS_HLIST:
        INIT_HLIST_HEAD(&bucket->ds.hl);
        break;
#ifdef BUCKET_USE_DL
    case BUCKET_DS_DLIST:
        INIT_LIST_HEAD(&bucket->ds.dl);
        break;
#endif /* BUCKET_USE_DL */
    case BUCKET_DS_RBTREE:
        bucket->ds.rb = RB_ROOT;
        break;
    default:
        break;
    }

    bucket->size = 0;
}

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

__always_inline const class_bucket_t* class_bucket_ins(void)
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

__always_inline const class_bucket_priv_t* class_bucket_priv_ins(void)
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
