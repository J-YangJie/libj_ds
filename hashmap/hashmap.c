/*
  Hashmap Implementations
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

#include <hashmap/hashmap.h>

#include <string.h>
#include <stdarg.h>
#include <_log.h>
#include <_memory.h>
#include <linux/_compiler.h>
#include <iterator/iterator.h>

#define TAG "[hashmap]"

#define DEFAULT_INITIAL_CAPACITY (16)
#define MAXIMUM_CAPACITY         (0x40000000) /* 1 << 30 */
#define DEFAULT_LOAD_FACTOR      (0.75f)
#define TREEIFY_THRESHOLD        (8)
#define UNTREEIFY_THRESHOLD      (6)
#define MIN_TREEIFY_CAPACITY     (64)

#define BUCKET_SET_TYPE(_ptr, _type) do { (_ptr) = (bucket_t*)(((intptr_t)(_ptr)) | (_type)); } while(0)
#define BUCKET_CLEAR_TYPE(_ptr)      do { (_ptr) = (bucket_t*)(((intptr_t)(_ptr)) & ~3); } while(0)
#define BUCKET_GET_TYPE(_ptr)        ((bucket_ds_t)(((intptr_t)(_ptr)) & 3))
#define BUCKET_GET_PTR(_ptr)         ((bucket_t*)(((intptr_t)(_ptr)) & ~3))
#define BUCKET_GET_OPS(_this)        (is_null(_this->ops) ? NULL : ((const class_bucket_ops_t*)(&_this->ops->valid_key)))

static inline hashmap_bnode_t* hashmap_find(const hashmap_t* _this, hashmap_key_t key);
static __always_inline hashmap_bnode_t* __hashmap_end(const hashmap_t* _this);
static __always_inline hashmap_bnode_t* __hashmap_rend(const hashmap_t* _this);

static __always_inline bool __hashmap_bkt_only_l(const hashmap_t* _this)
{
    return _this->config.c.b_bkt_only_l;
}

static __always_inline bool __hashmap_bkt_only_r(const hashmap_t* _this)
{
    return _this->config.c.b_bkt_only_r;
}

static __always_inline bool __hashmap_bkt_l_to_r(const hashmap_t* _this)
{
    return _this->config.c.b_bkt_l_to_r;
}

static __always_inline hashmap_size_t __hashmap_size(const hashmap_t* _this)
{
    return _this->size;
}

static __always_inline hashmap_size_t _hashmap_size(const hashmap_t* _this)
{
    if (unlikely(is_null(_this)))
        return -1;
    return __hashmap_size(_this);
}

static __always_inline hashmap_size_t __hashmap_capacity(const hashmap_t* _this)
{
    return _this->capacity;
}

static __always_inline hashmap_size_t _hashmap_capacity(const hashmap_t* _this)
{
    if (unlikely(is_null(_this)))
        return -1;
    return __hashmap_capacity(_this);
}

static __always_inline hashmap_bcount_t __hashmap_bucket_count(const hashmap_t* _this) /* TODO: return `hashmap_bcount_t` or others? */
{
    return _this->bucket_count;
}

static __always_inline hashmap_bcount_t _hashmap_bucket_count(const hashmap_t* _this)
{
    if (unlikely(is_null(_this)))
        return -1;
    return __hashmap_bucket_count(_this);
}

static __always_inline hashmap_count_t hashmap_count(const hashmap_t* _this, hashmap_key_t key)
{
    hashmap_bnode_t* tb_node = hashmap_find(_this, key);
    return is_null(tb_node) ? -1 : __hashmap_end(_this) != tb_node ? 1 : 0;
}

static inline hashmap_bnode_t* __hashmap_first(const hashmap_t* _this)
{
    hashmap_size_t i;
    hashmap_size_t te;
    hashmap_t* tthis = (hashmap_t*)_this;

    /* Segment fault: size > 0 && _this->head == NULL */
    if (__hashmap_size(_this) <= 0)
        return NULL;

    /* TODO: optimize to O(1) */
    i  = _this->pi_s < 0 ? 0 : _this->pi_s;
    te = _this->pi_e < 0 ? __hashmap_capacity(_this) - 1 : _this->pi_e;
    for (; i <= te; ++i) {
        if (!is_null(_this->head[i].bucket)) {
            tthis->pi_s = i;
            return cbucket_priv->_first(BUCKET_GET_PTR(_this->head[i].bucket), BUCKET_GET_TYPE(_this->head[i].bucket)); /* Err: by bucket */
        }
    }
    return NULL; /* Err: it's impossible to get here when `size` is gt 0 */
}

static inline hashmap_bnode_t* __hashmap_last(const hashmap_t* _this)
{
    hashmap_size_t i;
    hashmap_size_t te;
    hashmap_t* tthis = (hashmap_t*)_this;

    /* Segment fault: size > 0 && _this->head == NULL */
    if (__hashmap_size(_this) <= 0)
        return NULL;

    /* TODO: optimize to O(1) */
    i  = _this->pi_e < 0 ? __hashmap_capacity(_this) - 1 : _this->pi_e;
    te = _this->pi_s < 0 ? 0 : _this->pi_s;
    for (; i >= te; --i) {
        if (!is_null(_this->head[i].bucket)) {
            tthis->pi_e = i;
            return cbucket_priv->_last(BUCKET_GET_PTR(_this->head[i].bucket), BUCKET_GET_TYPE(_this->head[i].bucket)); /* Err: by bucket */
        }
    }
    return NULL; /* Err: it's impossible to get here when `size` is gt 0 */
}

static __always_inline hashmap_bnode_t* __hashmap_end(const hashmap_t* _this)
{
    return (hashmap_bnode_t*)iterator_end();
}

static inline hashmap_bnode_t* __hashmap_begin(const hashmap_t* _this)
{
    hashmap_size_t i;
    hashmap_size_t te;
    hashmap_t* tthis = (hashmap_t*)_this;

    /* Segment fault: size > 0 && _this->head == NULL */
    if (__hashmap_size(_this) <= 0)
        return __hashmap_end(_this);

    /* TODO: optimize to O(1) */
    i  = _this->pi_s < 0 ? 0 : _this->pi_s;
    te = _this->pi_e < 0 ? __hashmap_capacity(_this) - 1 : _this->pi_e;
    for (; i <= te; ++i) {
        if (!is_null(_this->head[i].bucket)) {
            tthis->pi_s = i;
            return cbucket_priv->_first(BUCKET_GET_PTR(_this->head[i].bucket), BUCKET_GET_TYPE(_this->head[i].bucket)); /* Err: by bucket */
        }
    }
    return NULL; /* Err: it's impossible to get here when `size` is gt 0 */
}

static __always_inline hashmap_bnode_t* _hashmap_begin(const hashmap_t* _this)
{
    if (unlikely(is_null(_this)))
        return NULL;
    return __hashmap_begin(_this);
}

static inline hashmap_bnode_t* __hashmap_next(const hashmap_t* _this, const hashmap_bnode_t* node)
{
    hashmap_size_t i;
    hashmap_size_t te;
    bucket_t* tb;
    bucket_node_t* tb_node;

    if (__hashmap_size(_this) <= 0 || __hashmap_end(_this) == node)
        return __hashmap_end(_this);

    /* The input parameter is `iterator`, and there's no need to check whether it equals `rend` */

    if (__hashmap_last(_this) == node)
        return __hashmap_end(_this);

    i = node->hash & (__hashmap_capacity(_this) - 1);
    tb = _this->head[i].bucket;
    if (is_null(tb))
        return NULL; /* Err: node doesn't belong to current hashmap or memory node->hash has been modified illegally */

    tb_node = cbucket_priv->next(BUCKET_GET_PTR(tb), BUCKET_GET_TYPE(tb), node);
    if (is_null(tb_node) || cbucket_priv->end(BUCKET_GET_PTR(tb)) != tb_node)
        return tb_node; /* Err: by bucket */

    ++i;
    te = _this->pi_e < 0 ? __hashmap_capacity(_this) - 1 : _this->pi_e;
    for (; i <= te; ++i) {
        tb = _this->head[i].bucket;
        if (!is_null(tb))
            return cbucket_priv->_first(BUCKET_GET_PTR(tb), BUCKET_GET_TYPE(tb)); /* Err: by bucket */
    }
    return NULL; /* Err: it's impossible to get here when `size` is gt 0 and `node` is ne to `last` */
}

static __always_inline hashmap_bnode_t* _hashmap_next(const hashmap_t* _this, const hashmap_bnode_t* node)
{
    if (unlikely(is_null(_this) || is_null(node)))
        return NULL;
    return __hashmap_next(_this, node);
}

static inline hashmap_bnode_t* __hashmap_prev(const hashmap_t* _this, const hashmap_bnode_t* node)
{
    hashmap_size_t i;
    hashmap_size_t te;
    bucket_t* tb;
    bucket_node_t* tb_node;

    if (__hashmap_size(_this) <= 0)
        return __hashmap_end(_this);

    if (__hashmap_end(_this) == node)
        return __hashmap_last(_this); /* Err: since the `ds` is non-empty, the return value includes the error case of `NULL` */

    if (__hashmap_first(_this) == node)
        return __hashmap_end(_this);

    i = node->hash & (__hashmap_capacity(_this) - 1);
    tb = _this->head[i].bucket;
    if (is_null(tb))
        return NULL; /* Err: node doesn't belong to current hashmap or memory node->hash has been modified illegally */

    tb_node = cbucket_priv->prev(BUCKET_GET_PTR(tb), BUCKET_GET_TYPE(tb), node);
    if (is_null(tb_node) || cbucket_priv->end(BUCKET_GET_PTR(tb)) != tb_node)
        return tb_node; /* Err: by bucket */

    --i;
    te = _this->pi_s < 0 ? 0 : _this->pi_s;
    for (; i >= te; --i) {
        tb = _this->head[i].bucket;
        if (!is_null(tb))
            return cbucket_priv->_last(BUCKET_GET_PTR(tb), BUCKET_GET_TYPE(tb)); /* Err: by bucket */
    }
    return NULL; /* Err: it's impossible to get here when `size` is gt 0 and `node` is ne to `first` */
}

static __always_inline hashmap_bnode_t* _hashmap_prev(const hashmap_t* _this, const hashmap_bnode_t* node)
{
    if (unlikely(is_null(_this) || is_null(node)))
        return NULL;
    return __hashmap_prev(_this, node);
}

static __always_inline hashmap_bnode_t* __hashmap_rend(const hashmap_t* _this)
{
    return (hashmap_bnode_t*)iterator_rend();
}

static inline hashmap_bnode_t* __hashmap_rbegin(const hashmap_t* _this)
{
    hashmap_size_t i;
    hashmap_size_t te;
    hashmap_t* tthis = (hashmap_t*)_this;

    /* Segment fault: size > 0 && _this->head == NULL */
    if (__hashmap_size(_this) <= 0)
        return __hashmap_rend(_this);

    /* TODO: optimize to O(1) */
    i  = _this->pi_e < 0 ? __hashmap_capacity(_this) - 1 : _this->pi_e;
    te = _this->pi_s < 0 ? 0 : _this->pi_s;
    for (; i >= te; --i) {
        if (!is_null(_this->head[i].bucket)) {
            tthis->pi_e = i;
            return cbucket_priv->_last(BUCKET_GET_PTR(_this->head[i].bucket), BUCKET_GET_TYPE(_this->head[i].bucket)); /* Err: by bucket */
        }
    }
    return NULL; /* Err: it's impossible to get here when `size` is gt 0 */
}

static __always_inline hashmap_bnode_t* _hashmap_rbegin(const hashmap_t* _this)
{
    if (unlikely(is_null(_this)))
        return NULL;
    return __hashmap_rbegin(_this);
}

static inline hashmap_bnode_t* __hashmap_rnext(const hashmap_t* _this, const hashmap_bnode_t* node)
{
    hashmap_size_t i;
    hashmap_size_t te;
    bucket_t* tb;
    bucket_node_t* tb_node;

    if (__hashmap_size(_this) <= 0 || __hashmap_rend(_this) == node)
        return __hashmap_rend(_this);

    /* The input parameter is `reverse_iterator`, and there's no need to check whether it equals `end` */

    if (__hashmap_first(_this) == node)
        return __hashmap_rend(_this);

    i = node->hash & (__hashmap_capacity(_this) - 1);
    tb = _this->head[i].bucket;
    if (is_null(tb))
        return NULL; /* Err: node doesn't belong to current hashmap or memory node->hash has been modified illegally */

    tb_node = cbucket_priv->rnext(BUCKET_GET_PTR(tb), BUCKET_GET_TYPE(tb), node);
    if (is_null(tb_node) || cbucket_priv->rend(BUCKET_GET_PTR(tb)) != tb_node)
        return tb_node; /* Err: by bucket */

    --i;
    te = _this->pi_s < 0 ? 0 : _this->pi_s;
    for (; i >= te; --i) {
        tb = _this->head[i].bucket;
        if (!is_null(tb))
            return cbucket_priv->_last(BUCKET_GET_PTR(tb), BUCKET_GET_TYPE(tb)); /* Err: by bucket */
    }
    return NULL; /* Err: it's impossible to get here when `size` is gt 0 and `node` is ne to `first` */
}

static __always_inline hashmap_bnode_t* _hashmap_rnext(const hashmap_t* _this, const hashmap_bnode_t* node)
{
    if (unlikely(is_null(_this) || is_null(node)))
        return NULL;
    return __hashmap_rnext(_this, node);
}

static inline hashmap_bnode_t* __hashmap_rprev(const hashmap_t* _this, const hashmap_bnode_t* node)
{
    hashmap_size_t i;
    hashmap_size_t te;
    bucket_t* tb;
    bucket_node_t* tb_node;

    if (__hashmap_size(_this) <= 0)
        return __hashmap_rend(_this);

    if (__hashmap_rend(_this) == node)
        return __hashmap_first(_this); /* Err: since the `ds` is non-empty, the return value includes the error case of `NULL` */

    if (__hashmap_last(_this) == node)
        return __hashmap_rend(_this);

    i = node->hash & (__hashmap_capacity(_this) - 1);
    tb = _this->head[i].bucket;
    if (is_null(tb))
        return NULL; /* Err: node doesn't belong to current hashmap or memory node->hash has been modified illegally */

    tb_node = cbucket_priv->rprev(BUCKET_GET_PTR(tb), BUCKET_GET_TYPE(tb), node);
    if (is_null(tb_node) || cbucket_priv->rend(BUCKET_GET_PTR(tb)) != tb_node)
        return tb_node; /* Err: by bucket */

    ++i;
    te = _this->pi_e < 0 ? __hashmap_capacity(_this) - 1 : _this->pi_e;
    for (; i <= te; ++i) {
        tb = _this->head[i].bucket;
        if (!is_null(tb))
            return cbucket_priv->_first(BUCKET_GET_PTR(tb), BUCKET_GET_TYPE(tb)); /* Err: by bucket */
    }
    return NULL; /* Err: it's impossible to get here when `size` is gt 0 and `node` is ne to `last` */
}

static __always_inline hashmap_bnode_t* _hashmap_rprev(const hashmap_t* _this, const hashmap_bnode_t* node)
{
    if (unlikely(is_null(_this) || is_null(node)))
        return NULL;
    return __hashmap_rprev(_this, node);
}

static inline hashmap_bnode_t* hashmap_find(const hashmap_t* _this, hashmap_key_t key)
{
    hashmap_size_t ti;
    hashmap_hash_t thash;
    bucket_t* tb;

    if (unlikely(is_null(_this)))
        return NULL;

    /* TODO: in order to improve performance, the size check comes before parameter check.
             However, the return value needs to be considered carefully */
    if (__hashmap_size(_this) <= 0)
        return __hashmap_end(_this);

    if (!is_null(_this->ops) && !is_null(_this->ops->valid_key) && !_this->ops->valid_key(key))
        return NULL;

    if (is_null(_this->ops) || is_null(_this->ops->__hash))
        thash = key;
    else
        thash = _this->ops->__hash(key);

    ti = thash & (__hashmap_capacity(_this) - 1);

    tb = _this->head[ti].bucket;
    if (is_null(tb))
        return __hashmap_end(_this);
    return cbucket_priv->find_has_checked_valid(BUCKET_GET_PTR(tb), BUCKET_GET_OPS(_this), BUCKET_GET_TYPE(tb), key); /* Err: by bucket */
}

static __always_inline bucket_t* hashmap_alloc_new_bucket(const hashmap_t* _this)
{
    bucket_t* ret = (bucket_t*)p_malloc(sizeof(bucket_t));

    if (unlikely(is_null(ret)))
        return NULL;

#ifdef BUCKET_USE_DL
    *ret = BUCKET_INIT(ret, __hashmap_bkt_only_r(_this) ? BUCKET_DS_RBTREE : BUCKET_DS_DLIST);
    BUCKET_SET_TYPE(ret, __hashmap_bkt_only_r(_this) ? BUCKET_DS_RBTREE : BUCKET_DS_DLIST);
#else
    *ret = BUCKET_INIT(ret, __hashmap_bkt_only_r(_this) ? BUCKET_DS_RBTREE : BUCKET_DS_HLIST);
    BUCKET_SET_TYPE(ret, __hashmap_bkt_only_r(_this) ? BUCKET_DS_RBTREE : BUCKET_DS_HLIST);
#endif /* BUCKET_USE_DL */

    return ret;
}

static __always_inline bucket_t* __hashmap_bucket_switch(hashmap_t* _this, bucket_t* bucket)
{
    bucket_t* t_ptr = BUCKET_GET_PTR(bucket);

    if (!__hashmap_bkt_l_to_r(_this))
        return bucket;

#ifdef BUCKET_USE_DL
    bucket_ds_t t_type = BUCKET_DS_RBTREE == BUCKET_GET_TYPE(bucket) ? BUCKET_DS_DLIST : BUCKET_DS_RBTREE;
#else
    bucket_ds_t t_type = BUCKET_DS_RBTREE == BUCKET_GET_TYPE(bucket) ? BUCKET_DS_HLIST : BUCKET_DS_RBTREE;
#endif /* BUCKET_USE_DL */

    cbucket_priv->__switch(t_ptr, BUCKET_GET_OPS(_this), BUCKET_GET_TYPE(bucket), t_type);
    BUCKET_SET_TYPE(t_ptr, t_type);
    return t_ptr;
}

static bool __hashmap_rehash(hashmap_t* _this, hashmap_node_t* n, hashmap_size_t n_capacity)
{
    hashmap_size_t i_o = 0;
    hashmap_size_t i_n = 0;
    hashmap_size_t trange_e = 0;
    bucket_t*   tb_o = NULL;
    bucket_t*   tb_o_ptr = NULL;
    bucket_ds_t tb_o_type;
    bucket_t*   tb_n = NULL;
    bucket_t*   tb_n_ptr = NULL;
    bucket_ds_t tb_n_type;
    bucket_node_t* tb_node = NULL;
    bucket_node_t* tb_it = NULL;
    hashmap_bcount_t ocount = __hashmap_bucket_count(_this);
    hashmap_bcount_t ncount = 0;
    hashmap_size_t tpi_s = -1;
    hashmap_size_t tpi_e = -1;
    hashmap_size_t times_rehash = 0;

    if (__hashmap_capacity(_this) << 1 != n_capacity)
        return false;

    i_o      = _this->pi_s < 0 ? 0 : _this->pi_s;
    trange_e = _this->pi_e < 0 ? __hashmap_capacity(_this) - 1 : _this->pi_e;
    for (; i_o <= trange_e; ++i_o) {
        tb_o = _this->head[i_o].bucket;
        if (is_null(tb_o))
            continue;

        tb_o_ptr  = BUCKET_GET_PTR(tb_o);
        tb_o_type = BUCKET_GET_TYPE(tb_o);
        for (tb_it = cbucket_priv->begin(tb_o_ptr, tb_o_type); cbucket_priv->end(tb_o_ptr) != tb_it; ) {
            if (0 == (tb_it->hash & __hashmap_capacity(_this))) {
                tb_it = cbucket_priv->next(tb_o_ptr, tb_o_type, tb_it);
                continue;
            }

            i_n = i_o + __hashmap_capacity(_this);
            tb_n = n[i_n].bucket;
            if (is_null(tb_n)) {
                tb_n = hashmap_alloc_new_bucket(_this);
                if (unlikely(is_null(tb_n)))
                    goto err;

                n[i_n].bucket = tb_n;
                ncount++;
            }

            tb_node = tb_it;
            tb_it = cbucket_priv->pop(tb_o_ptr, tb_o_type, tb_node); /* No need to check */

            tb_n_ptr  = BUCKET_GET_PTR(tb_n);
            tb_n_type = BUCKET_GET_TYPE(tb_n);
            if (cbucket->size(tb_n_ptr) + 1 > TREEIFY_THRESHOLD 
                && n_capacity >= MIN_TREEIFY_CAPACITY 
                && (BUCKET_DS_HLIST == tb_n_type || BUCKET_DS_DLIST == tb_n_type)) {
                n[i_n].bucket = __hashmap_bucket_switch(_this, tb_n);
                tb_n = n[i_n].bucket;
                tb_n_ptr  = BUCKET_GET_PTR(tb_n);
                tb_n_type = BUCKET_GET_TYPE(tb_n);
            }

            tpi_s = tpi_s < 0 ? i_n : i_n < tpi_s ? i_n : tpi_s;
            tpi_e = tpi_e < 0 ? i_n : i_n > tpi_e ? i_n : tpi_e;
            cbucket_priv->insert_has_checked_same(tb_n_ptr, BUCKET_GET_OPS(_this), tb_n_type, tb_node); /* No need to check */

            times_rehash++;
        }

        if (cbucket->size(tb_o_ptr) <= 0) {
            p_free(tb_o_ptr);
            _this->head[i_o].bucket = NULL;
            _this->bucket_count--;
            continue;
        }

        if (cbucket->size(tb_o_ptr) < UNTREEIFY_THRESHOLD && BUCKET_DS_RBTREE == tb_o_type)
            _this->head[i_o].bucket = __hashmap_bucket_switch(_this, tb_o);

        n[i_o].bucket = _this->head[i_o].bucket;
        _this->head[i_o].bucket = NULL;
        ncount++;
        _this->bucket_count--;
        tpi_s = tpi_s < 0 ? i_o : i_o < tpi_s ? i_o : tpi_s;
        tpi_e = tpi_e < 0 ? i_o : i_o > tpi_e ? i_o : tpi_e;
    }

    /* TODO: Report the error to user, besides that, assert or others? */
    if (0 != _this->bucket_count)
        pr_err("After rehash, 0 != bucket_count, [ %zd ]", _this->bucket_count);

    _this->bucket_count = ncount;
    _this->pi_s = tpi_s < 0 ? _this->pi_s : tpi_s;
    _this->pi_e = tpi_e < 0 ? _this->pi_e : tpi_e;
    pr_notice("Rehash successfully, capacity [ %zd -> %zd ], bucket_count [ %zd -> %zd ], range (%zd, %zd)!", 
                __hashmap_capacity(_this), n_capacity, ocount, __hashmap_bucket_count(_this), _this->pi_s, _this->pi_e);

    if (times_rehash > 50)
        pr_attn("Rehash times [ %zd ]", times_rehash);
    else
        pr_info("Rehash times [ %zd ]", times_rehash);

    return true;

err:
    pr_warn("Failed to rehash [ %zd -> %zd ], recovering...", __hashmap_capacity(_this), n_capacity);

    for (i_o = 0; i_o < __hashmap_capacity(_this); ++i_o) {
        tb_o = n[i_o].bucket;
        if (is_null(tb_o))
            continue;

        _this->head[i_o].bucket = n[i_o].bucket;
        n[i_o].bucket = NULL;
        _this->bucket_count++;
    }

    for (i_o = __hashmap_capacity(_this); i_o < n_capacity; ++i_o) {
        tb_o = n[i_o].bucket;
        if (is_null(tb_o))
            continue;

        if (is_null(_this->head[i_o - __hashmap_capacity(_this)].bucket)) {
            _this->head[i_o - __hashmap_capacity(_this)].bucket = n[i_o].bucket;
            n[i_o].bucket = NULL;
            _this->bucket_count++;
            continue;
        }

        tb_o_ptr  = BUCKET_GET_PTR(tb_o);
        tb_o_type = BUCKET_GET_TYPE(tb_o);
        while (cbucket->size(tb_o_ptr) > 0) {
            tb_node = cbucket_priv->begin(tb_o_ptr, tb_o_type);
            cbucket_priv->pop(tb_o_ptr, tb_o_type, tb_node); /* No need to check */

            i_n = i_o - __hashmap_capacity(_this);
            tb_n = _this->head[i_n].bucket;
            tb_n_ptr  = BUCKET_GET_PTR(tb_n);
            tb_n_type = BUCKET_GET_TYPE(tb_n);
            if (cbucket->size(tb_n_ptr) + 1 > TREEIFY_THRESHOLD 
                && __hashmap_capacity(_this) >= MIN_TREEIFY_CAPACITY 
                && (BUCKET_DS_HLIST == tb_n_type || BUCKET_DS_DLIST == tb_n_type)) {
                _this->head[i_n].bucket = __hashmap_bucket_switch(_this, tb_n);
                tb_n = _this->head[i_n].bucket;
                tb_n_ptr  = BUCKET_GET_PTR(tb_n);
                tb_n_type = BUCKET_GET_TYPE(tb_n);
            }

            cbucket_priv->insert_has_checked_same(tb_n_ptr, BUCKET_GET_OPS(_this), tb_n_type, tb_node); /* No need to check */
        }

        p_free(tb_o_ptr);
        n[i_o].bucket = NULL;
    }

    return false;
}

static __always_inline bool __hashmap_init_alloc(hashmap_t* _this)
{
    if (!is_null(_this->head))
        return true;

    pr_info("Init alloc size [ %zd ]", _this->capacity_init);
    _this->head = (hashmap_node_t*)p_calloc(_this->capacity_init, sizeof(hashmap_node_t));
    if (is_null(_this->head))
        return false;

    _this->capacity = _this->capacity_init;
    return true;
}

static bool __hashmap_expansion(hashmap_t* _this)
{
    hashmap_node_t* t = NULL;

    if (is_null(_this->head))
        return __hashmap_init_alloc(_this);

    if (__hashmap_size(_this) <= __hashmap_capacity(_this) * _this->capacity_load_factor)
        return true;

    /* The upper limit has already been reached, keep it */
    if (__hashmap_capacity(_this) >= _this->capacity_max)
        return true;

    if (is_null(_this->head_e)) {
        t = (hashmap_node_t*)p_calloc(__hashmap_capacity(_this) << 1, sizeof(hashmap_node_t));
        if (is_null(t))
            return false;
    } else {
        t = _this->head_e;
        _this->head_e = NULL;
        pr_notice("Used head_e!");
    }

    if (!__hashmap_rehash(_this, t, __hashmap_capacity(_this) << 1))
        goto err;

    p_free(_this->head);
    _this->head = t;
    _this->capacity = __hashmap_capacity(_this) << 1;
    return true;

err:
    /* Don't release the memory for now, save it instead */
    _this->head_e = t;
    return false;
}

static hashmap_bnode_t* hashmap_insert(hashmap_t* _this, hashmap_key_t key, hashmap_value_t value)
{
    hashmap_hash_t thash;
    hashmap_size_t ti;
    bucket_node_t* tb_node;
    bool flag_alloc_head = false;
    bool flag_alloc_bucket = false;

    if (unlikely(is_null(_this)))
        return NULL;

    if (!is_null(_this->ops) && !is_null(_this->ops->valid_key) && !_this->ops->valid_key(key))
        return NULL;

    if (!is_null(_this->ops) && !is_null(_this->ops->valid_value) && !_this->ops->valid_value(value))
        return NULL;

    if (is_null(_this->head)) {
        if (!__hashmap_init_alloc(_this))
            return NULL;

        flag_alloc_head = true;
    }

    if (is_null(_this->ops) || is_null(_this->ops->__hash))
        thash = key;
    else
        thash = _this->ops->__hash(key);

    ti = thash & (__hashmap_capacity(_this) - 1);

    if (is_null(_this->head[ti].bucket)) {
        _this->head[ti].bucket = hashmap_alloc_new_bucket(_this);
        if (is_null(_this->head[ti].bucket))
            goto err;

        _this->bucket_count++;
        flag_alloc_bucket = true;
    }

    tb_node = cbucket_priv->insert_has_checked_valid(BUCKET_GET_PTR(_this->head[ti].bucket), BUCKET_GET_OPS(_this), 
                                                        BUCKET_GET_TYPE(_this->head[ti].bucket), thash, key, value);
    if (is_null(tb_node))
        goto err;

    if (cbucket->size(BUCKET_GET_PTR(_this->head[ti].bucket)) > TREEIFY_THRESHOLD 
        && __hashmap_capacity(_this) >= MIN_TREEIFY_CAPACITY 
        && (BUCKET_DS_HLIST == BUCKET_GET_TYPE(_this->head[ti].bucket) 
            || BUCKET_DS_DLIST == BUCKET_GET_TYPE(_this->head[ti].bucket))) {
        _this->head[ti].bucket = __hashmap_bucket_switch(_this, _this->head[ti].bucket);
    }

    _this->size++;
    _this->pi_s = _this->pi_s < 0 ? ti : ti < _this->pi_s ? ti : _this->pi_s;
    _this->pi_e = _this->pi_e < 0 ? ti : ti > _this->pi_e ? ti : _this->pi_e;

    __hashmap_expansion(_this);
    return tb_node;

err:
    if (flag_alloc_bucket) {
        BUCKET_CLEAR_TYPE(_this->head[ti].bucket);
        p_free(_this->head[ti].bucket);
        _this->bucket_count--;
    }

    if (flag_alloc_head) {
        p_free(_this->head);
        _this->capacity = 0;
    }
    return NULL;
}

static hashmap_bnode_t* hashmap_insert_replace(hashmap_t* _this, hashmap_key_t key, hashmap_value_t value)
{
    hashmap_hash_t thash;
    hashmap_size_t ti;
    bucket_node_t* tb_node;
    bucket_size_t tb_size;
    bool flag_alloc_head = false;
    bool flag_alloc_bucket = false;

    if (unlikely(is_null(_this)))
        return NULL;

    if (!is_null(_this->ops) && !is_null(_this->ops->valid_key) && !_this->ops->valid_key(key))
        return NULL;

    if (!is_null(_this->ops) && !is_null(_this->ops->valid_value) && !_this->ops->valid_value(value))
        return NULL;

    if (is_null(_this->head)) {
        if (!__hashmap_init_alloc(_this))
            return NULL;

        flag_alloc_head = true;
    }

    if (is_null(_this->ops) || is_null(_this->ops->__hash))
        thash = key;
    else
        thash = _this->ops->__hash(key);

    ti = thash & (__hashmap_capacity(_this) - 1);

    if (is_null(_this->head[ti].bucket)) {
        _this->head[ti].bucket = hashmap_alloc_new_bucket(_this);
        if (is_null(_this->head[ti].bucket))
            goto err;

        _this->bucket_count++;
        flag_alloc_bucket = true;
    }

    tb_size = cbucket->size(BUCKET_GET_PTR(_this->head[ti].bucket));
    tb_node = cbucket_priv->insert_replace_has_checked_valid(BUCKET_GET_PTR(_this->head[ti].bucket), BUCKET_GET_OPS(_this), 
                                                                BUCKET_GET_TYPE(_this->head[ti].bucket), thash, key, value);
    if (is_null(tb_node))
        goto err;

    if (tb_size == cbucket->size(BUCKET_GET_PTR(_this->head[ti].bucket)))
        return tb_node;

    if (cbucket->size(BUCKET_GET_PTR(_this->head[ti].bucket)) > TREEIFY_THRESHOLD 
        && __hashmap_capacity(_this) >= MIN_TREEIFY_CAPACITY 
        && (BUCKET_DS_HLIST == BUCKET_GET_TYPE(_this->head[ti].bucket) 
            || BUCKET_DS_DLIST == BUCKET_GET_TYPE(_this->head[ti].bucket))) {
        _this->head[ti].bucket = __hashmap_bucket_switch(_this, _this->head[ti].bucket);
    }

    _this->size++;
    _this->pi_s = _this->pi_s < 0 ? ti : ti < _this->pi_s ? ti : _this->pi_s;
    _this->pi_e = _this->pi_e < 0 ? ti : ti > _this->pi_e ? ti : _this->pi_e;

    __hashmap_expansion(_this);
    return tb_node;

err:
    if (flag_alloc_bucket) {
        BUCKET_CLEAR_TYPE(_this->head[ti].bucket);
        p_free(_this->head[ti].bucket);
        _this->bucket_count--;
    }

    if (flag_alloc_head) {
        p_free(_this->head);
        _this->capacity = 0;
    }
    return NULL;
}

static hashmap_bnode_t* hashmap_erase(hashmap_t* _this, hashmap_bnode_t* pos)
{
    hashmap_size_t i;
    hashmap_size_t ti;
    hashmap_size_t te;
    bucket_t* tb;
    bucket_t* tb_for;
    bucket_node_t* tb_node;
    bucket_node_t* ret;

    if (unlikely(is_null(_this) || is_null(pos)))
        return NULL;

    /* The input parameter is `iterator`, and there's no need to check whether it equals `rend` */
    if (__hashmap_size(_this) <= 0 || __hashmap_end(_this) == pos/* || __hashmap_rend(_this) == pos*/)
        return NULL;

    ti = pos->hash & (__hashmap_capacity(_this) - 1);

    tb = _this->head[ti].bucket;
    if (is_null(tb))
        return NULL; /* Err: `pos` doesn't belong to current hashmap or memory `pos->hash` has been modified illegally */

    ret = cbucket_priv->next(BUCKET_GET_PTR(tb), BUCKET_GET_TYPE(tb), pos);
    if (is_null(ret))
        return NULL; /* Err: by bucket */

    if (cbucket_priv->end(BUCKET_GET_PTR(tb)) == ret) {
        i  = ti + 1;
        te = _this->pi_e < 0 ? __hashmap_capacity(_this) - 1 : _this->pi_e;
        for (; i <= te; ++i) {
            tb_for = _this->head[i].bucket;
            if (is_null(tb_for))
                continue;

            ret = cbucket_priv->begin(BUCKET_GET_PTR(tb_for), BUCKET_GET_TYPE(tb_for));
            if (is_null(ret) || cbucket_priv->end(BUCKET_GET_PTR(tb_for)) == ret)
                return NULL; /* Err: by bucket */

            break;
        }
    }

    /* 1. The `pos` belongs to this bucket, erase normally.
       2. The `pos` belongs to this bucket, memory issues are detected.
       3. The `pos` doesn't belong to this bucket, erase normally.
       4. The `pos` doesn't belong to this bucket, memory issues are detected. */
    tb_node = cbucket_priv->erase(BUCKET_GET_PTR(tb), BUCKET_GET_OPS(_this), BUCKET_GET_TYPE(tb), pos);
    if (is_null(tb_node))
        return NULL; /* Err: by bucket, but the erasing operation was not carried out */

    _this->size--;

    if (cbucket->size(BUCKET_GET_PTR(tb)) < UNTREEIFY_THRESHOLD && BUCKET_DS_RBTREE == BUCKET_GET_TYPE(tb))
        _this->head[ti].bucket = __hashmap_bucket_switch(_this, _this->head[ti].bucket);

    if (cbucket->size(BUCKET_GET_PTR(tb)) <= 0) {
        /* For performance considerations, the `bucket->erase` is unable to check whether the `pos` belongs to itself.
           In such a situation, the `bucket->size` and `hashmap->size` have already been affected, we can only accept
           the mistake and continue, `bucket->clear` and release bucket. */
        cbucket->clear(BUCKET_GET_PTR(tb), BUCKET_GET_OPS(_this), BUCKET_GET_TYPE(tb));
        BUCKET_CLEAR_TYPE(_this->head[ti].bucket);
        p_free(_this->head[ti].bucket);
        _this->bucket_count--;
    }

    return ret;
}

static inline hashmap_size_t hashmap_remove(hashmap_t* _this, hashmap_key_t key)
{
    hashmap_size_t ret;
    hashmap_size_t ti;
    hashmap_hash_t thash;
    bucket_t* tb;

    if (unlikely(is_null(_this)))
        return -1;

    if (__hashmap_size(_this) <= 0)
        return 0;

    if (!is_null(_this->ops) && !is_null(_this->ops->valid_key) && !_this->ops->valid_key(key))
        return -1;

    if (is_null(_this->ops) || is_null(_this->ops->__hash))
        thash = key;
    else
        thash = _this->ops->__hash(key);

    ti = thash & (__hashmap_capacity(_this) - 1);

    tb = _this->head[ti].bucket;
    if (is_null(tb))
        return 0;

    ret = cbucket_priv->remove_has_checked_valid(BUCKET_GET_PTR(tb), BUCKET_GET_OPS(_this), BUCKET_GET_TYPE(tb), key);
    if (ret > 0)
        _this->size--; /* ret is at most `1` */

    if (cbucket->size(BUCKET_GET_PTR(tb)) < UNTREEIFY_THRESHOLD && BUCKET_DS_RBTREE == BUCKET_GET_TYPE(tb))
        _this->head[ti].bucket = __hashmap_bucket_switch(_this, _this->head[ti].bucket);

    if (cbucket->size(BUCKET_GET_PTR(tb)) <= 0) {
        /* Here, `bucket->clear` is used to prevent the problems caused by `erase`(`pos` doesn't belong to bucket) */
        cbucket->clear(BUCKET_GET_PTR(tb), BUCKET_GET_OPS(_this), BUCKET_GET_TYPE(tb));
        BUCKET_CLEAR_TYPE(_this->head[ti].bucket);
        p_free(_this->head[ti].bucket);
        _this->bucket_count--;
    }

    return ret;
}

static hashmap_size_t hashmap_clear(hashmap_t* _this)
{
    hashmap_size_t ret = _hashmap_size(_this);
    hashmap_size_t i;
    hashmap_size_t te;
    bucket_t* tb_ptr = NULL;
    bucket_ds_t tb_type;

    if (unlikely(is_null(_this)))
        return -1;

    if (__hashmap_size(_this) <= 0)
        return 0;

    i  = _this->pi_s < 0 ? 0 : _this->pi_s;
    te = _this->pi_e < 0 ? __hashmap_capacity(_this) - 1 : _this->pi_e;
    for (; i <= te; ++i) {
        if (is_null(_this->head[i].bucket))
            continue;

        tb_ptr  = BUCKET_GET_PTR(_this->head[i].bucket);
        tb_type = BUCKET_GET_TYPE(_this->head[i].bucket);
        _this->size -= cbucket->clear(tb_ptr, BUCKET_GET_OPS(_this), tb_type);

        p_free(tb_ptr);
        _this->head[i].bucket = NULL;
        _this->bucket_count--;

        if (0 == _this->size && 0 == _this->bucket_count)
            break;
    }

    _this->pi_s = -1;
    _this->pi_e = -1;
    if (0 != _this->size || 0 != _this->bucket_count) {
        pr_err("After clearing `ds`, errors were discovered [ %zd | %zd | %zd ] and forcibly corrected!", 
                _this->size, _this->capacity, _this->bucket_count);
        _this->size = 0;
        _this->bucket_count = 0;
    } else {
        pr_attn("After clearing `ds`, [ %zd | %zd | %zd ]", _this->size, _this->capacity, _this->bucket_count);
    }
    return ret;
}

static inline hashmap_size_t capacity_correct(hashmap_size_t capacity)
{
    uint8_t t = 0;
    hashmap_size_t ret;

    if (capacity <= 0)
        return 0;

    ret = ((hashmap_size_t)1) << (sizeof(hashmap_size_t) * 8 - 2);
    if (capacity & ret)
        return ret;

    while (capacity) {
        ret = capacity;
        capacity &= capacity - 1;
        t++;
    }

    return t > 1 ? ret << 1 : ret;
}

__always_inline void __hashmap_init(hashmap_t* hashmap)
{
    hashmap->head = NULL;
    hashmap->head_e = NULL;
    hashmap->capacity = 0;
    hashmap->capacity_init = DEFAULT_INITIAL_CAPACITY;
    hashmap->capacity_max  = MAXIMUM_CAPACITY;
    hashmap->capacity_load_factor = DEFAULT_LOAD_FACTOR;
    hashmap->config.c.b_bkt_only_l = 0;
    hashmap->config.c.b_bkt_only_r = 0;
    hashmap->config.c.b_bkt_l_to_r = 1;
    hashmap->bucket_count = 0;
    hashmap->pi_s = -1;
    hashmap->pi_e = -1;
}

inline void __hashmap_init_arg(hashmap_t* hashmap, int num_arg, ...)
{
    hashmap_size_t capacity_init;
    hashmap_size_t capacity_max;
    float          load_factor;
    hashmap_config_t* config = NULL;
    va_list alist;

    hashmap->head = NULL;
    hashmap->head_e = NULL;
    hashmap->capacity = 0;
    hashmap->bucket_count = 0;
    hashmap->pi_s = -1;
    hashmap->pi_e = -1;

    va_start(alist, num_arg);
    capacity_init = num_arg > 0 ? va_arg(alist, hashmap_size_t) : 0;
    capacity_max  = num_arg > 1 ? va_arg(alist, hashmap_size_t) : 0;
    load_factor   = num_arg > 2 ? va_arg(alist, double) : 0.0f;
    config        = num_arg > 3 ? va_arg(alist, hashmap_config_t*) : NULL;
    va_end(alist);

    hashmap->capacity_max = capacity_max <= 0 ? MAXIMUM_CAPACITY : capacity_correct(capacity_max);
    hashmap->capacity_init = capacity_init <= 0 
                                ? DEFAULT_INITIAL_CAPACITY 
                                : capacity_init >= hashmap->capacity_max 
                                    ? hashmap->capacity_max 
                                    : capacity_correct(capacity_init);
    hashmap->capacity_load_factor = load_factor < 0.001f || load_factor > 1.0f ? DEFAULT_LOAD_FACTOR : load_factor;

    hashmap->config.d = 0;

    if (is_null(config)) {
        hashmap->config.c.b_bkt_l_to_r = 1;
        goto end;
    }

    if (config->c.b_bkt_only_l && !config->c.b_bkt_only_r && !config->c.b_bkt_l_to_r)
        hashmap->config.c.b_bkt_only_l = 1;
    else if (!config->c.b_bkt_only_l && config->c.b_bkt_only_r && !config->c.b_bkt_l_to_r)
        hashmap->config.c.b_bkt_only_r = 1;
    else if (!config->c.b_bkt_only_l && !config->c.b_bkt_only_r && config->c.b_bkt_l_to_r)
        hashmap->config.c.b_bkt_l_to_r = 1;
    else
        hashmap->config.c.b_bkt_l_to_r = 1;

end:
    pr_attn("In [ %s ], [ %zd | 0x%zx | %f | 0x%x ] -> [ %zd | 0x%zx | %f | 0x%x ]", __func__, 
            capacity_init, capacity_max, load_factor, is_null(config) ? 0 : config->d,
            hashmap->capacity_init, hashmap->capacity_max, hashmap->capacity_load_factor, hashmap->config.d);
}

__always_inline void __hashmap_deinit(hashmap_t* hashmap)
{
    hashmap_clear(hashmap);
    p_free(hashmap->head_e);
    p_free(hashmap->head);

    hashmap->ops = NULL;
    hashmap->head = NULL;
    hashmap->head_e = NULL;
    hashmap->size = 0;
    hashmap->capacity = 0;
    hashmap->capacity_init = 0;
    hashmap->capacity_max = 0;
    hashmap->capacity_load_factor = 0.0f;
    hashmap->config.d = 0;
    hashmap->bucket_count = 0;
    hashmap->pi_s = -1;
    hashmap->pi_e = -1;
}

typedef hashmap_iterator_t* (*fp_end)(const hashmap_t* _this);
typedef hashmap_iterator_t* (*fp_begin)(const hashmap_t* _this);
typedef hashmap_iterator_t* (*fp_next)(const hashmap_t* _this, const hashmap_iterator_t* iterator);
typedef hashmap_iterator_t* (*fp_prev)(const hashmap_t* _this, const hashmap_iterator_t* iterator);
typedef hashmap_r_iterator_t* (*fp_rend)(const hashmap_t* _this);
typedef hashmap_r_iterator_t* (*fp_rbegin)(const hashmap_t* _this);
typedef hashmap_r_iterator_t* (*fp_rnext)(const hashmap_t* _this, const hashmap_r_iterator_t* r_iterator);
typedef hashmap_r_iterator_t* (*fp_rprev)(const hashmap_t* _this, const hashmap_r_iterator_t* r_iterator);
typedef hashmap_iterator_t* (*fp_find)(const hashmap_t* _this, hashmap_key_t key);
typedef hashmap_iterator_t* (*fp_insert)(hashmap_t* _this, hashmap_key_t key, hashmap_value_t value);
typedef hashmap_iterator_t* (*fp_insert_replace)(hashmap_t* _this, hashmap_key_t key, hashmap_value_t value);
typedef hashmap_iterator_t* (*fp_erase)(hashmap_t* _this, hashmap_iterator_t* iterator);

__always_inline const class_hashmap_t* class_hashmap_ins(void)
{
    static const class_hashmap_t ins = {
        .size           = _hashmap_size,
        .capacity       = _hashmap_capacity,
        .bucket_count   = _hashmap_bucket_count,
        .count          = hashmap_count,
        .end            = (fp_end)__hashmap_end,
        .begin          = (fp_begin)_hashmap_begin,
        .next           = (fp_next)_hashmap_next,
        .prev           = (fp_prev)_hashmap_prev,
        .rend           = (fp_rend)__hashmap_rend,
        .rbegin         = (fp_rbegin)_hashmap_rbegin,
        .rnext          = (fp_rnext)_hashmap_rnext,
        .rprev          = (fp_rprev)_hashmap_rprev,
        .find           = (fp_find)hashmap_find,
        .insert         = (fp_insert)hashmap_insert,
        .insert_replace = (fp_insert_replace)hashmap_insert_replace,
        .erase          = (fp_erase)hashmap_erase,
        .remove         = hashmap_remove,
        .clear          = hashmap_clear,
    };
    return &ins;
}
