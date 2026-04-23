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

#include <../bucket/bucket.c>
#include <hashmap/hashmap.h>

#include <string.h>
#include <stdarg.h>
#include <_log.h>
#include <_memory.h>
#include <linux/_compiler.h>
#include <iterator/iterator.h>

#ifndef TAG
#define TAG "[hashmap]"
#endif /* TAG */

#define DEFAULT_INITIAL_CAPACITY (16)
#define MAXIMUM_CAPACITY         (0x40000000) /* 1 << 30 */
#define DEFAULT_LOAD_FACTOR      (0.75f)
#define TREEIFY_THRESHOLD        (8)
#define UNTREEIFY_THRESHOLD      (6)
#define MIN_TREEIFY_CAPACITY     (64)

#define phmbkt(sh)               (&(sh))
#define bucket_ops(_this)        (is_null(_this->ops) ? NULL : ((const class_bucket_ops_t*)(&_this->ops->valid_key)))

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

static __always_inline hashmap_bcount_t __hashmap_bucket_count(const hashmap_t* _this)
{
    return _this->bucket_count;
}

static __always_inline hashmap_bcount_t _hashmap_bucket_count(const hashmap_t* _this)
{
    if (unlikely(is_null(_this)))
        return -1;
    return __hashmap_bucket_count(_this);
}

static __always_inline hashmap_bcount_t __hashmap_bucket_valid_count(const hashmap_t* _this)
{
    return _this->bucket_valid_count;
}

static __always_inline hashmap_bcount_t _hashmap_bucket_valid_count(const hashmap_t* _this)
{
    if (unlikely(is_null(_this)))
        return -1;
    return __hashmap_bucket_valid_count(_this);
}

static /* __always_inline */ inline hashmap_count_t hashmap_count(const hashmap_t* _this, hashmap_key_t key)
{
    hashmap_bnode_t* n = hashmap_find(_this, key);
    return is_null(n) ? -1 : __hashmap_end(_this) != n ? 1 : 0;
}

static hashmap_bnode_t* __hashmap_first(const hashmap_t* _this)
{
    hashmap_bcount_t i, idx_e;
    hashmap_t* tthis = (hashmap_t*)_this;

    /* Segment fault: size > 0 && _this->head == NULL */
    if (__hashmap_size(_this) <= 0)
        return NULL;

    i     = _this->pi_s < 0 ? 0 : _this->pi_s;
    idx_e = _this->pi_e < 0 ? __hashmap_bucket_count(_this) - 1 : _this->pi_e;
    for (; i <= idx_e; ++i) {
        if (__hmbucket_valid(_this->head[i].sh)) {
            tthis->pi_s = i;
            return _hmbucket_first(phmbkt(_this->head[i].sh)); /* Err: by bucket */
        }
    }
    return NULL; /* Err: it's impossible to get here when `size` is gt 0 */
}

static hashmap_bnode_t* __hashmap_last(const hashmap_t* _this)
{
    hashmap_bcount_t i, idx_e;
    hashmap_t* tthis = (hashmap_t*)_this;

    /* Segment fault: size > 0 && _this->head == NULL */
    if (__hashmap_size(_this) <= 0)
        return NULL;

    i     = _this->pi_e < 0 ? __hashmap_bucket_count(_this) - 1 : _this->pi_e;
    idx_e = _this->pi_s < 0 ? 0 : _this->pi_s;
    for (; i >= idx_e; --i) { /* The type of `i` is a signed type */
        if (__hmbucket_valid(_this->head[i].sh)) {
            tthis->pi_e = i;
            return _hmbucket_last(phmbkt(_this->head[i].sh)); /* Err: by bucket */
        }
    }
    return NULL; /* Err: it's impossible to get here when `size` is gt 0 */
}

static __always_inline hashmap_bnode_t* __hashmap_end(const hashmap_t* _this)
{
    return (hashmap_bnode_t*)iterator_end();
}

static hashmap_bnode_t* __hashmap_begin(const hashmap_t* _this)
{
    hashmap_bcount_t i, idx_e;
    hashmap_t* tthis = (hashmap_t*)_this;

    /* Segment fault: size > 0 && _this->head == NULL */
    if (__hashmap_size(_this) <= 0)
        return __hashmap_end(_this);

    i     = _this->pi_s < 0 ? 0 : _this->pi_s;
    idx_e = _this->pi_e < 0 ? __hashmap_bucket_count(_this) - 1 : _this->pi_e;
    for (; i <= idx_e; ++i) {
        if (__hmbucket_valid(_this->head[i].sh)) {
            tthis->pi_s = i;
            return _hmbucket_first(phmbkt(_this->head[i].sh)); /* Err: by bucket */
        }
    }
    return NULL; /* Err: it's impossible to get here when `size` is gt 0 */
}

static /* __always_inline */ inline hashmap_bnode_t* _hashmap_begin(const hashmap_t* _this)
{
    if (unlikely(is_null(_this)))
        return NULL;
    return __hashmap_begin(_this);
}

static hashmap_bnode_t* __hashmap_next(const hashmap_t* _this, const hashmap_bnode_t* node)
{
    hashmap_bcount_t i, idx_e;
    bucket_shell_t* bkt_sh;
    bucket_node_t* bkt_node;

    if (__hashmap_size(_this) <= 0 || __hashmap_end(_this) == node)
        return __hashmap_end(_this);

    /* The input parameter is `iterator`, and there's no need to check whether it equals `rend` */

    if (__hashmap_last(_this) == node)
        return __hashmap_end(_this);

    i = node->hash & (__hashmap_bucket_count(_this) - 1);
    bkt_sh = phmbkt(_this->head[i].sh);
    if (___hmbucket_invalid(bkt_sh))
        return NULL; /* Err: node doesn't belong to current hashmap or memory node->hash has been modified illegally */

    bkt_node = hmbucket_next(bkt_sh, node);
    if (is_null(bkt_node) || __hmbucket_end(bkt_sh) != bkt_node)
        return bkt_node; /* Err: by bucket */

    ++i;
    idx_e = _this->pi_e < 0 ? __hashmap_bucket_count(_this) - 1 : _this->pi_e;
    for (; i <= idx_e; ++i) {
        if (__hmbucket_valid(_this->head[i].sh))
            return _hmbucket_first(phmbkt(_this->head[i].sh)); /* Err: by bucket */
    }
    return NULL; /* Err: it's impossible to get here when `size` is gt 0 and `node` is ne to `last` */
}

static /* __always_inline */ inline hashmap_bnode_t* _hashmap_next(const hashmap_t* _this, const hashmap_bnode_t* node)
{
    if (unlikely(is_null(_this) || is_null(node)))
        return NULL;
    return __hashmap_next(_this, node);
}

static hashmap_bnode_t* __hashmap_prev(const hashmap_t* _this, const hashmap_bnode_t* node)
{
    hashmap_bcount_t i, idx_e;
    bucket_shell_t* bkt_sh;
    bucket_node_t* bkt_node;

    if (__hashmap_size(_this) <= 0)
        return __hashmap_end(_this);

    if (__hashmap_end(_this) == node)
        return __hashmap_last(_this); /* Err: since the `ds` is non-empty, the return value includes the error case of `NULL` */

    if (__hashmap_first(_this) == node)
        return __hashmap_end(_this);

    i = node->hash & (__hashmap_bucket_count(_this) - 1);
    bkt_sh = phmbkt(_this->head[i].sh);
    if (___hmbucket_invalid(bkt_sh))
        return NULL; /* Err: node doesn't belong to current hashmap or memory node->hash has been modified illegally */

    bkt_node = hmbucket_prev(bkt_sh, node);
    if (is_null(bkt_node) || __hmbucket_end(bkt_sh) != bkt_node)
        return bkt_node; /* Err: by bucket */

    --i;
    idx_e = _this->pi_s < 0 ? 0 : _this->pi_s;
    for (; i >= idx_e; --i) { /* The type of `i` is a signed type */
        if (__hmbucket_valid(_this->head[i].sh))
            return _hmbucket_last(phmbkt(_this->head[i].sh)); /* Err: by bucket */
    }
    return NULL; /* Err: it's impossible to get here when `size` is gt 0 and `node` is ne to `first` */
}

static /* __always_inline */ inline hashmap_bnode_t* _hashmap_prev(const hashmap_t* _this, const hashmap_bnode_t* node)
{
    if (unlikely(is_null(_this) || is_null(node)))
        return NULL;
    return __hashmap_prev(_this, node);
}

static __always_inline hashmap_bnode_t* __hashmap_rend(const hashmap_t* _this)
{
    return (hashmap_bnode_t*)iterator_rend();
}

static hashmap_bnode_t* __hashmap_rbegin(const hashmap_t* _this)
{
    hashmap_bcount_t i, idx_e;
    hashmap_t* tthis = (hashmap_t*)_this;

    /* Segment fault: size > 0 && _this->head == NULL */
    if (__hashmap_size(_this) <= 0)
        return __hashmap_rend(_this);

    i     = _this->pi_e < 0 ? __hashmap_bucket_count(_this) - 1 : _this->pi_e;
    idx_e = _this->pi_s < 0 ? 0 : _this->pi_s;
    for (; i >= idx_e; --i) { /* The type of `i` is a signed type */
        if (__hmbucket_valid(_this->head[i].sh)) {
            tthis->pi_e = i;
            return _hmbucket_last(phmbkt(_this->head[i].sh)); /* Err: by bucket */
        }
    }
    return NULL; /* Err: it's impossible to get here when `size` is gt 0 */
}

static /* __always_inline */ inline hashmap_bnode_t* _hashmap_rbegin(const hashmap_t* _this)
{
    if (unlikely(is_null(_this)))
        return NULL;
    return __hashmap_rbegin(_this);
}

static hashmap_bnode_t* __hashmap_rnext(const hashmap_t* _this, const hashmap_bnode_t* node)
{
    hashmap_bcount_t i, idx_e;
    bucket_shell_t* bkt_sh;
    bucket_node_t* bkt_node;

    if (__hashmap_size(_this) <= 0 || __hashmap_rend(_this) == node)
        return __hashmap_rend(_this);

    /* The input parameter is `reverse_iterator`, and there's no need to check whether it equals `end` */

    if (__hashmap_first(_this) == node)
        return __hashmap_rend(_this);

    i = node->hash & (__hashmap_bucket_count(_this) - 1);
    bkt_sh = phmbkt(_this->head[i].sh);
    if (___hmbucket_invalid(bkt_sh))
        return NULL; /* Err: node doesn't belong to current hashmap or memory node->hash has been modified illegally */

    bkt_node = hmbucket_rnext(bkt_sh, node);
    if (is_null(bkt_node) || __hmbucket_rend(bkt_sh) != bkt_node)
        return bkt_node; /* Err: by bucket */

    --i;
    idx_e = _this->pi_s < 0 ? 0 : _this->pi_s;
    for (; i >= idx_e; --i) { /* The type of `i` is a signed type */
        if (__hmbucket_valid(_this->head[i].sh))
            return _hmbucket_last(phmbkt(_this->head[i].sh)); /* Err: by bucket */
    }
    return NULL; /* Err: it's impossible to get here when `size` is gt 0 and `node` is ne to `first` */
}

static /* __always_inline */ inline hashmap_bnode_t* _hashmap_rnext(const hashmap_t* _this, const hashmap_bnode_t* node)
{
    if (unlikely(is_null(_this) || is_null(node)))
        return NULL;
    return __hashmap_rnext(_this, node);
}

static hashmap_bnode_t* __hashmap_rprev(const hashmap_t* _this, const hashmap_bnode_t* node)
{
    hashmap_bcount_t i, idx_e;
    bucket_shell_t* bkt_sh;
    bucket_node_t* bkt_node;

    if (__hashmap_size(_this) <= 0)
        return __hashmap_rend(_this);

    if (__hashmap_rend(_this) == node)
        return __hashmap_first(_this); /* Err: since the `ds` is non-empty, the return value includes the error case of `NULL` */

    if (__hashmap_last(_this) == node)
        return __hashmap_rend(_this);

    i = node->hash & (__hashmap_bucket_count(_this) - 1);
    bkt_sh = phmbkt(_this->head[i].sh);
    if (___hmbucket_invalid(bkt_sh))
        return NULL; /* Err: node doesn't belong to current hashmap or memory node->hash has been modified illegally */

    bkt_node = hmbucket_rprev(bkt_sh, node);
    if (is_null(bkt_node) || __hmbucket_rend(bkt_sh) != bkt_node)
        return bkt_node; /* Err: by bucket */

    ++i;
    idx_e = _this->pi_e < 0 ? __hashmap_bucket_count(_this) - 1 : _this->pi_e;
    for (; i <= idx_e; ++i) {
        if (__hmbucket_valid(_this->head[i].sh))
            return _hmbucket_first(phmbkt(_this->head[i].sh)); /* Err: by bucket */
    }
    return NULL; /* Err: it's impossible to get here when `size` is gt 0 and `node` is ne to `last` */
}

static /* __always_inline */ inline hashmap_bnode_t* _hashmap_rprev(const hashmap_t* _this, const hashmap_bnode_t* node)
{
    if (unlikely(is_null(_this) || is_null(node)))
        return NULL;
    return __hashmap_rprev(_this, node);
}

static inline hashmap_bnode_t* hashmap_find(const hashmap_t* _this, hashmap_key_t key)
{
    hashmap_bcount_t idx;
    hashmap_hash_t hash;
    bucket_shell_t* bkt_sh;

    if (unlikely(is_null(_this)))
        return NULL;

    /* TODO: in order to improve performance, the size check comes before parameter check.
             However, the return value needs to be considered carefully */
    if (__hashmap_size(_this) <= 0)
        return __hashmap_end(_this);

    if (!is_null(_this->ops) && !is_null(_this->ops->valid_key) && !_this->ops->valid_key(key))
        return NULL;

    if (is_null(_this->ops) || is_null(_this->ops->__hash))
        hash = key;
    else
        hash = _this->ops->__hash(key);

    idx = hash & (__hashmap_bucket_count(_this) - 1);

    bkt_sh = phmbkt(_this->head[idx].sh);
    if (___hmbucket_invalid(bkt_sh))
        return __hashmap_end(_this);
    return hmbucket_find_hc_valid(bkt_sh, bucket_ops(_this), key); /* Err: by bucket */
}

/* TODO: If the bucket is implemented as a pointer type, pay attention to the `init` and `switch` functions. 
   Both functions must perform an assignment before returning */
static __always_inline void __hashmap_bucket_init(const hashmap_t* _this, bucket_shell_t* bucket_sh)
{
    *bucket_sh = BUCKET_INIT(bucket_sh, __hashmap_bkt_only_r(_this) ? BKT_DS_RBTREE : BKT_DS_HLIST);
    ___hmbucket_set_type(bucket_sh, __hashmap_bkt_only_r(_this) ? BKT_DS_RBTREE : BKT_DS_HLIST);
}

static __always_inline void __hashmap_bucket_deinit(const hashmap_t* _this, bucket_shell_t* bucket_sh)
{
    bucket_ds_t type = ___hmbucket_xchg_type(bucket_sh, 0);
    BUCKET_DEINIT(bucket_sh, bucket_ops(_this), type);
    ___hmbucket_set_type(bucket_sh, BKT_DS_INVALID);
}

static /* __always_inline */ inline void __hashmap_bucket_switch(hashmap_t* _this, bucket_shell_t* bucket_sh)
{
    bucket_ds_t otype, ntype;

    if (!__hashmap_bkt_l_to_r(_this))
        return ;

    ntype = ___hmbucket_is_tree(bucket_sh) ? BKT_DS_HLIST : BKT_DS_RBTREE;
    otype = ___hmbucket_xchg_type(bucket_sh, 0);
    __bucket_switch(bucket_sh, bucket_ops(_this), otype, ntype);
    ___hmbucket_set_type(bucket_sh, ntype);

    pr_debug("Switch [ %s ] -> [ %s ], size [ %zd ]", 
                BKT_DS_RBTREE == otype ? "tree" : "list", 
                BKT_DS_RBTREE == ntype ? "tree" : "list", 
                __hmbucket_size(bucket_sh));
}

static void __hashmap_rehash_resume(hashmap_t* _this, hashmap_node_t* n, hashmap_bcount_t n_size)
{
    hashmap_node_t* p = n;
    hashmap_bcount_t bcnt_o = __hashmap_bucket_count(_this), bcnt_n = n_size;
    hashmap_bcount_t idx_o, idx_n, idx_e;
    bucket_shell_t* bsh_o, * bsh_n;
    bucket_node_t* it, * bnode;
    hashmap_bcount_t vcnt_o = __hashmap_bucket_valid_count(_this), vcnt_n = 0;
    hashmap_bcount_t tpi_s = -1, tpi_e = -1;
    hashmap_size_t times_rehash = 0;

    idx_o = _this->pi_s < 0 ? 0 : _this->pi_s;
    idx_e = _this->pi_e < 0 ? bcnt_o - 1 : _this->pi_e;

    for (p += idx_o; idx_o <= idx_e; ++idx_o, ++p) {
        if (__hmbucket_invalid(p->sh))
            continue;

        bsh_o = phmbkt(p->sh);
        __hmbucket_resume(bsh_o);

        _this->bucket_valid_count--; /* This logic only checks if the valid bucket count is correct */
        vcnt_n++;

        for (it = hmbucket_begin(bsh_o); __hmbucket_end(bsh_o) != it; ) {
            if (!(it->hash & bcnt_o)) { /* 2x expansion logic */
                it = hmbucket_next(bsh_o, it);
                continue;
            }

            idx_n = idx_o + bcnt_o; /* 2x expansion logic */
            bsh_n = phmbkt((p + bcnt_o)->sh);
            if (___hmbucket_invalid(bsh_n)) {
                __hashmap_bucket_init(_this, bsh_n);
                vcnt_n++;
            }

            bnode = it;
            it = hmbucket_pop(bsh_o, bnode); /* No need to check */

            if (__hmbucket_size(bsh_n) + 1 >= TREEIFY_THRESHOLD 
                && bcnt_n >= MIN_TREEIFY_CAPACITY 
                && !___hmbucket_is_tree(bsh_n)) {
                __hashmap_bucket_switch(_this, bsh_n);
            }

            tpi_s = tpi_s < 0 ? idx_n : idx_n < tpi_s ? idx_n : tpi_s;
            tpi_e = tpi_e < 0 ? idx_n : idx_n > tpi_e ? idx_n : tpi_e;
            hmbucket_insert_hc_same(bsh_n, bucket_ops(_this), bnode); /* No need to check */

            times_rehash++;
        }

        if (__hmbucket_empty(bsh_o)) {
            ___hmbucket_set_type(bsh_o, BKT_DS_INVALID);
            vcnt_n--;
            continue;
        }

        if (__hmbucket_size(bsh_o) <= UNTREEIFY_THRESHOLD && ___hmbucket_is_tree(bsh_o))
            __hashmap_bucket_switch(_this, bsh_o);

        tpi_s = tpi_s < 0 ? idx_o : idx_o < tpi_s ? idx_o : tpi_s;
        tpi_e = tpi_e < 0 ? idx_o : idx_o > tpi_e ? idx_o : tpi_e;
    }

    /* TODO: Report the error to user, besides that, assert or others? */
    if (0 != _this->bucket_valid_count)
        pr_err("After rehash, 0 != bucket_valid_count, [ %zd ]", _this->bucket_valid_count);

    _this->bucket_valid_count = vcnt_n;
    _this->pi_s = tpi_s < 0 ? _this->pi_s : tpi_s;
    _this->pi_e = tpi_e < 0 ? _this->pi_e : tpi_e;
    pr_notice("Rehash successfully, bucket_count [ %zd -> %zd ], bucket_valid_count [ %zd -> %zd ], range (%zd, %zd)!", 
                bcnt_o, bcnt_n, vcnt_o, __hashmap_bucket_valid_count(_this), _this->pi_s, _this->pi_e);

    if (times_rehash > 50)
        pr_attn("Rehash times [ %zd ]", times_rehash);
    else
        pr_info("Rehash times [ %zd ]", times_rehash);
}

static /* __always_inline */ inline bool __hashmap_buckets_init_alloc(hashmap_t* _this)
{
    if (!is_null(_this->head))
        return true;

    _this->head = (hashmap_node_t*)p_calloc(_this->bucket_count_init, sizeof(hashmap_node_t)); /* TODO: malloc and memset? because of inline */
    if (is_null(_this->head))
        return false;

    _this->bucket_count = _this->bucket_count_init;
    pr_info("Buckets alloc by bucket_count_init, [ default | real ] = [ %d | %zd ]", 
            DEFAULT_INITIAL_CAPACITY, _this->bucket_count);
    return true;
}

static /* __always_inline */ inline bool __hashmap_buckets_free(hashmap_t* _this)
{
    p_free(_this->head);
    _this->bucket_count = 0;
    pr_info("Buckets free!");
    return true;
}

static bool __hashmap_rehash(hashmap_t* _this)
{
    hashmap_node_t* n = NULL;
    hashmap_bcount_t bcnt_n, bcnt_o = __hashmap_bucket_count(_this);

    if (is_null(_this->head))
        return __hashmap_buckets_init_alloc(_this);

    if (__hashmap_size(_this) <= bcnt_o * _this->load_factor)
        return true;

    /* The upper limit has already been reached, keep it */
    if (bcnt_o >= _this->bucket_count_max)
        return true;

    /* `rehash` follows the 2x bucket_count expansion logic by default. 
       If multi-fold expansion is used, the logic needs to be checked */
    bcnt_n = bcnt_o << 1;

    n = p_realloc(_this->head, bcnt_n * sizeof(hashmap_node_t));
    if (is_null(n))
        goto err;

    memset(n + bcnt_o, 0, bcnt_o * sizeof(hashmap_node_t));

    pr_debug("Preparing for rehash, current [ %zd > %lf(%zd * %f) ], max [ %zd ]", 
                __hashmap_size(_this), bcnt_o * _this->load_factor, 
                bcnt_o, _this->load_factor, _this->bucket_count_max);

    if (_this->head != n) {
        pr_info("Preparing for rehash, head addr [ %p -> %p ], bucket_count [ %zd -> %zd ] by [ %s ]", 
                _this->head, n, bcnt_o, bcnt_n, __func__);
    } else {
        pr_info("Preparing for rehash, head new addr   [ %p ], bucket_count [ %zd -> %zd ] by [ %s ]", 
                n, bcnt_o, bcnt_n, __func__);
    }

    __hashmap_rehash_resume(_this, n, bcnt_n);
    _this->head = n;
    _this->bucket_count = bcnt_n;
    return true;

err:
    return false;
}

static hashmap_bnode_t* hashmap_insert(hashmap_t* _this, hashmap_key_t key, hashmap_value_t value)
{
    hashmap_hash_t hash;
    hashmap_bcount_t idx;
    bucket_shell_t* bkt_sh;
    bucket_node_t* bkt_node;
    bool f_head = false, f_bkt = false;

    if (unlikely(is_null(_this)))
        return NULL;

    if (!is_null(_this->ops) && !is_null(_this->ops->valid_key) && !_this->ops->valid_key(key))
        return NULL;

    if (!is_null(_this->ops) && !is_null(_this->ops->valid_value) && !_this->ops->valid_value(value))
        return NULL;

    if (is_null(_this->head)) {
        if (!__hashmap_buckets_init_alloc(_this))
            return NULL;

        f_head = true;
    }

    if (is_null(_this->ops) || is_null(_this->ops->__hash))
        hash = key;
    else
        hash = _this->ops->__hash(key);

    idx = hash & (__hashmap_bucket_count(_this) - 1);
    bkt_sh = phmbkt(_this->head[idx].sh);

    if (___hmbucket_invalid(bkt_sh)) {
        __hashmap_bucket_init(_this, bkt_sh);
        if (unlikely(___hmbucket_invalid(bkt_sh)))
            goto err;

        _this->bucket_valid_count++;
        f_bkt = true;
    }

    bkt_node = hmbucket_insert_hc_valid(bkt_sh, bucket_ops(_this), hash, key, value);
    if (is_null(bkt_node))
        goto err;

    if (__hmbucket_size(bkt_sh) >= TREEIFY_THRESHOLD 
        && __hashmap_bucket_count(_this) >= MIN_TREEIFY_CAPACITY 
        && !___hmbucket_is_tree(bkt_sh)) {
        __hashmap_bucket_switch(_this, bkt_sh);
    }

    _this->size++;
    _this->pi_s = _this->pi_s < 0 ? idx : idx < _this->pi_s ? idx : _this->pi_s;
    _this->pi_e = _this->pi_e < 0 ? idx : idx > _this->pi_e ? idx : _this->pi_e;

    __hashmap_rehash(_this);
    return bkt_node;

err:
    if (f_bkt) {
        __hashmap_bucket_deinit(_this, bkt_sh);
        _this->bucket_valid_count--;
    }

    if (f_head)
        __hashmap_buckets_free(_this);
    return NULL;
}

static hashmap_bnode_t* hashmap_insert_replace(hashmap_t* _this, hashmap_key_t key, hashmap_value_t value)
{
    hashmap_hash_t hash;
    hashmap_bcount_t idx;
    bucket_shell_t* bkt_sh;
    bucket_node_t* bkt_node;
    bucket_size_t bkt_size;
    bool f_head = false, f_bkt = false;

    if (unlikely(is_null(_this)))
        return NULL;

    if (!is_null(_this->ops) && !is_null(_this->ops->valid_key) && !_this->ops->valid_key(key))
        return NULL;

    if (!is_null(_this->ops) && !is_null(_this->ops->valid_value) && !_this->ops->valid_value(value))
        return NULL;

    if (is_null(_this->head)) {
        if (!__hashmap_buckets_init_alloc(_this))
            return NULL;

        f_head = true;
    }

    if (is_null(_this->ops) || is_null(_this->ops->__hash))
        hash = key;
    else
        hash = _this->ops->__hash(key);

    idx = hash & (__hashmap_bucket_count(_this) - 1);
    bkt_sh = phmbkt(_this->head[idx].sh);

    if (___hmbucket_invalid(bkt_sh)) {
        __hashmap_bucket_init(_this, bkt_sh);
        if (unlikely(___hmbucket_invalid(bkt_sh)))
            goto err;

        _this->bucket_valid_count++;
        f_bkt = true;
    }

    bkt_size = __hmbucket_size(bkt_sh);
    bkt_node = hmbucket_insert_replace_hc_valid(bkt_sh, bucket_ops(_this), hash, key, value);
    if (is_null(bkt_node))
        goto err;

    if (bkt_size == __hmbucket_size(bkt_sh))
        return bkt_node;

    if (__hmbucket_size(bkt_sh) >= TREEIFY_THRESHOLD 
        && __hashmap_bucket_count(_this) >= MIN_TREEIFY_CAPACITY 
        && !___hmbucket_is_tree(bkt_sh)) {
        __hashmap_bucket_switch(_this, bkt_sh);
    }

    _this->size++;
    _this->pi_s = _this->pi_s < 0 ? idx : idx < _this->pi_s ? idx : _this->pi_s;
    _this->pi_e = _this->pi_e < 0 ? idx : idx > _this->pi_e ? idx : _this->pi_e;

    __hashmap_rehash(_this);
    return bkt_node;

err:
    if (f_bkt) {
        __hashmap_bucket_deinit(_this, bkt_sh);
        _this->bucket_valid_count--;
    }

    if (f_head)
        __hashmap_buckets_free(_this);
    return NULL;
}

static hashmap_bnode_t* hashmap_erase(hashmap_t* _this, hashmap_bnode_t* pos)
{
    hashmap_bcount_t i, idx, idx_e;
    bucket_shell_t* bkt_sh, * bkt_for;
    bucket_node_t* bkt_node, * ret;

    if (unlikely(is_null(_this) || is_null(pos)))
        return NULL;

    /* The input parameter is `iterator`, and there's no need to check whether it equals `rend` */
    if (__hashmap_size(_this) <= 0 || __hashmap_end(_this) == pos/* || __hashmap_rend(_this) == pos*/)
        return NULL;

    idx = pos->hash & (__hashmap_bucket_count(_this) - 1);

    bkt_sh = phmbkt(_this->head[idx].sh);
    if (___hmbucket_invalid(bkt_sh))
        return NULL; /* Err: `pos` doesn't belong to current hashmap or memory `pos->hash` has been modified illegally */

    ret = hmbucket_next(bkt_sh, pos);
    if (is_null(ret))
        return NULL; /* Err: by bucket */

    if (__hmbucket_end(bkt_sh) == ret) {
        i = idx + 1;
        idx_e = _this->pi_e < 0 ? __hashmap_bucket_count(_this) - 1 : _this->pi_e;
        for (; i <= idx_e; ++i) {
            if (__hmbucket_invalid(_this->head[i].sh))
                continue;

            bkt_for = phmbkt(_this->head[i].sh);
            ret = hmbucket_begin(bkt_for);
            if (is_null(ret) || __hmbucket_end(bkt_for) == ret)
                return NULL; /* Err: by bucket */

            break;
        }
    }

    /* 1. The `pos` belongs to this bucket, erase normally.
       2. The `pos` belongs to this bucket, memory issues are detected.
       3. The `pos` doesn't belong to this bucket, erase normally.
       4. The `pos` doesn't belong to this bucket, memory issues are detected. */
    bkt_node = hmbucket_erase(bkt_sh, bucket_ops(_this), pos);
    if (is_null(bkt_node))
        return NULL; /* Err: by bucket, but the erasing operation was not carried out */

    _this->size--;

    if (__hmbucket_size(bkt_sh) <= UNTREEIFY_THRESHOLD && ___hmbucket_is_tree(bkt_sh))
        __hashmap_bucket_switch(_this, bkt_sh);

    if (__hmbucket_empty(bkt_sh)) {
        ___hmbucket_set_type(bkt_sh, BKT_DS_INVALID);
        _this->bucket_valid_count--;
    }

    return ret;
}

static inline hashmap_size_t hashmap_remove(hashmap_t* _this, hashmap_key_t key)
{
    hashmap_size_t ret;
    hashmap_bcount_t idx;
    hashmap_hash_t hash;
    bucket_shell_t* bkt_sh;

    if (unlikely(is_null(_this)))
        return -1;

    if (__hashmap_size(_this) <= 0)
        return 0;

    if (!is_null(_this->ops) && !is_null(_this->ops->valid_key) && !_this->ops->valid_key(key))
        return -1;

    if (is_null(_this->ops) || is_null(_this->ops->__hash))
        hash = key;
    else
        hash = _this->ops->__hash(key);

    idx = hash & (__hashmap_bucket_count(_this) - 1);

    bkt_sh = phmbkt(_this->head[idx].sh);
    if (___hmbucket_invalid(bkt_sh))
        return 0;

    ret = hmbucket_remove_hc_valid(bkt_sh, bucket_ops(_this), key);
    if (ret > 0)
        _this->size--; /* ret is at most `1` */

    if (__hmbucket_size(bkt_sh) <= UNTREEIFY_THRESHOLD && ___hmbucket_is_tree(bkt_sh))
        __hashmap_bucket_switch(_this, bkt_sh);

    if (__hmbucket_empty(bkt_sh)) {
        ___hmbucket_set_type(bkt_sh, BKT_DS_INVALID);
        _this->bucket_valid_count--;
    }

    return ret;
}

static hashmap_size_t hashmap_clear(hashmap_t* _this)
{
    hashmap_size_t ret = _hashmap_size(_this);
    hashmap_bcount_t i, idx_e;
    bucket_shell_t* bkt_sh;

    if (unlikely(is_null(_this)))
        return -1;

    if (__hashmap_size(_this) <= 0)
        return 0;

    i     = _this->pi_s < 0 ? 0 : _this->pi_s;
    idx_e = _this->pi_e < 0 ? __hashmap_bucket_count(_this) - 1 : _this->pi_e;
    for (; i <= idx_e; ++i) {
        if (__hmbucket_invalid(_this->head[i].sh))
            continue;

        bkt_sh = phmbkt(_this->head[i].sh);
        _this->size -= hmbucket_clear(bkt_sh, bucket_ops(_this));
        ___hmbucket_set_type(bkt_sh, BKT_DS_INVALID);
        _this->bucket_valid_count--;

        if (0 == _this->size && 0 == _this->bucket_valid_count)
            break;
    }

    ret -= _this->size;
    _this->pi_s = -1;
    _this->pi_e = -1;

    if (0 != _this->size || 0 != _this->bucket_valid_count) {
        pr_err("After clearing `ds`, errors were discovered [ %zd | %zd | %zd ] and forcibly corrected!", 
                _this->size, _this->bucket_count, _this->bucket_valid_count);
        _this->size = 0;
        _this->bucket_valid_count = 0;
    } else {
        pr_attn("After clearing `ds`, [ %zd | %zd | %zd ]", 
                _this->size, _this->bucket_count, _this->bucket_valid_count);
    }
    return ret; /* Returns the actual operation count */
}

static hashmap_bcount_t bucket_count_correct(hashmap_bcount_t bucket_count)
{
    uint8_t t = 0;
    hashmap_bcount_t ret;

    if (bucket_count <= 0)
        return 0;

    ret = ((hashmap_bcount_t)1) << (sizeof(hashmap_bcount_t) * 8 - 2);
    if (bucket_count & ret)
        return ret;

    while (bucket_count) {
        ret = bucket_count;
        bucket_count &= bucket_count - 1;
        t++;
    }

    return t > 1 ? ret << 1 : ret;
}

/* __always_inline */ inline void __hashmap_init(hashmap_t* hashmap)
{
    hashmap->head = NULL;
    hashmap->bucket_count = 0;
    hashmap->bucket_count_init = DEFAULT_INITIAL_CAPACITY;
    hashmap->bucket_count_max  = MAXIMUM_CAPACITY;
    hashmap->load_factor = DEFAULT_LOAD_FACTOR;
    hashmap->config.c.b_bkt_only_l = 0;
    hashmap->config.c.b_bkt_only_r = 0;
    hashmap->config.c.b_bkt_l_to_r = 1;
    hashmap->bucket_valid_count = 0;
    hashmap->pi_s = -1;
    hashmap->pi_e = -1;
}

inline void __hashmap_init_arg(hashmap_t* hashmap, int num_arg, ...)
{
    hashmap_bcount_t  bucket_count_init;
    hashmap_bcount_t  bucket_count_max;
    float             load_factor;
    hashmap_config_t* config = NULL;
    va_list alist;

    hashmap->head = NULL;
    hashmap->bucket_count = 0;
    hashmap->bucket_valid_count = 0;
    hashmap->pi_s = -1;
    hashmap->pi_e = -1;

    va_start(alist, num_arg);
    bucket_count_init = num_arg > 0 ? va_arg(alist, hashmap_bcount_t) : 0;
    bucket_count_max  = num_arg > 1 ? va_arg(alist, hashmap_bcount_t) : 0;
    load_factor       = num_arg > 2 ? va_arg(alist, double) : 0.0f;
    config            = num_arg > 3 ? va_arg(alist, hashmap_config_t*) : NULL;
    va_end(alist);

    hashmap->bucket_count_max  = bucket_count_max <= 0 ? MAXIMUM_CAPACITY : bucket_count_correct(bucket_count_max);
    hashmap->bucket_count_init = bucket_count_init <= 0 
                                    ? DEFAULT_INITIAL_CAPACITY 
                                    : bucket_count_init >= hashmap->bucket_count_max 
                                        ? hashmap->bucket_count_max 
                                        : bucket_count_correct(bucket_count_init);
    hashmap->load_factor = load_factor < 0.001f || load_factor > 1.0f ? DEFAULT_LOAD_FACTOR : load_factor;

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
            bucket_count_init, bucket_count_max, load_factor, is_null(config) ? 0 : config->d, 
            hashmap->bucket_count_init, hashmap->bucket_count_max, hashmap->load_factor, hashmap->config.d);
}

/* __always_inline */ inline void __hashmap_deinit(hashmap_t* hashmap)
{
    hashmap_clear(hashmap);
    p_free(hashmap->head);

    hashmap->ops = NULL;
    hashmap->head = NULL;
    hashmap->size = 0;
    hashmap->bucket_count = 0;
    hashmap->bucket_count_init = 0;
    hashmap->bucket_count_max = 0;
    hashmap->load_factor = 0.0f;
    hashmap->config.d = 0;
    hashmap->bucket_valid_count = 0;
    hashmap->pi_s = -1;
    hashmap->pi_e = -1;
}

typedef hashmap_iterator_t* (*hm_fp_end)(const hashmap_t* _this);
typedef hashmap_iterator_t* (*hm_fp_begin)(const hashmap_t* _this);
typedef hashmap_iterator_t* (*hm_fp_next)(const hashmap_t* _this, const hashmap_iterator_t* iterator);
typedef hashmap_iterator_t* (*hm_fp_prev)(const hashmap_t* _this, const hashmap_iterator_t* iterator);
typedef hashmap_r_iterator_t* (*hm_fp_rend)(const hashmap_t* _this);
typedef hashmap_r_iterator_t* (*hm_fp_rbegin)(const hashmap_t* _this);
typedef hashmap_r_iterator_t* (*hm_fp_rnext)(const hashmap_t* _this, const hashmap_r_iterator_t* r_iterator);
typedef hashmap_r_iterator_t* (*hm_fp_rprev)(const hashmap_t* _this, const hashmap_r_iterator_t* r_iterator);
typedef hashmap_iterator_t* (*hm_fp_find)(const hashmap_t* _this, hashmap_key_t key);
typedef hashmap_iterator_t* (*hm_fp_insert)(hashmap_t* _this, hashmap_key_t key, hashmap_value_t value);
typedef hashmap_iterator_t* (*hm_fp_insert_replace)(hashmap_t* _this, hashmap_key_t key, hashmap_value_t value);
typedef hashmap_iterator_t* (*hm_fp_erase)(hashmap_t* _this, hashmap_iterator_t* iterator);

/* __always_inline */ inline const class_hashmap_t* class_hashmap_ins(void)
{
    static const class_hashmap_t ins = {
        .size               = _hashmap_size,
        .bucket_count       = _hashmap_bucket_count,
        .bucket_valid_count = _hashmap_bucket_valid_count,
        .count              = hashmap_count,
        .end                = (hm_fp_end)__hashmap_end,
        .begin              = (hm_fp_begin)_hashmap_begin,
        .next               = (hm_fp_next)_hashmap_next,
        .prev               = (hm_fp_prev)_hashmap_prev,
        .rend               = (hm_fp_rend)__hashmap_rend,
        .rbegin             = (hm_fp_rbegin)_hashmap_rbegin,
        .rnext              = (hm_fp_rnext)_hashmap_rnext,
        .rprev              = (hm_fp_rprev)_hashmap_rprev,
        .find               = (hm_fp_find)hashmap_find,
        .insert             = (hm_fp_insert)hashmap_insert,
        .insert_replace     = (hm_fp_insert_replace)hashmap_insert_replace,
        .erase              = (hm_fp_erase)hashmap_erase,
        .remove             = hashmap_remove,
        .clear              = hashmap_clear,
    };
    return &ins;
}

