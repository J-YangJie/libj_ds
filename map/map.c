/*
  Map Implementations
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

#include <map/map.h>

#include <_memory.h>
#include <linux/rbtree.h>
#include <linux/_compiler.h>
#include <iterator/iterator.h>

#define map_entry(ptr) rb_entry((ptr), struct map_node, node)

static __always_inline map_node_t* map_find(const map_t* _this, map_key_t key);
static __always_inline map_node_t* __map_end(const map_t* _this);

static __always_inline map_size_t __map_size(const map_t* _this)
{
    return _this->size;
}

static __always_inline map_size_t _map_size(const map_t* _this)
{
    if (unlikely(is_null(_this)))
        return -1;
    return __map_size(_this);
}

static __always_inline map_count_t map_count(const map_t* _this, map_key_t key)
{
    map_node_t* t = map_find(_this, key);
    return is_null(t) ? -1 : (__map_end(_this) == t ? 0 : 1);
}

static __always_inline map_node_t* __map_first(const map_t* _this)
{
    struct rb_node* t = rb_first(&_this->root);
    return is_null(t) ? NULL : map_entry(t);
}

static __always_inline map_node_t* __map_last(const map_t* _this)
{
    struct rb_node* t = rb_last(&_this->root);
    return is_null(t) ? NULL : map_entry(t);
}

static __always_inline map_node_t* __map_end(const map_t* _this)
{
    return (map_node_t*)iterator_end();
}

static __always_inline map_node_t* __map_begin(const map_t* _this)
{
    map_node_t* t = __map_first(_this);
    return is_null(t) ? __map_end(_this) : t;
}

static __always_inline map_node_t* _map_begin(const map_t* _this)
{
    if (unlikely(is_null(_this)))
        return NULL;
    return __map_begin(_this);
}

static __always_inline map_node_t* __map_next(const map_t* _this, const map_node_t* node)
{
    struct rb_node* t = NULL;

    if (RB_EMPTY_ROOT(&_this->root) || __map_end(_this) == node)
        return __map_end(_this);

    /* The input parameter is `iterator`, and there's no need 
       to check whether it equals `rend` */

    /* This check should come after `end` or `rend`.
       This is a pre-judgment condition for `rb_next` or `rb_prev` */
    if (unlikely(RB_EMPTY_NODE(&node->node)))
        return NULL;

    t = rb_next(&node->node);
    return is_null(t) ? __map_end(_this) : map_entry(t);
}

static __always_inline map_node_t* _map_next(const map_t* _this, const map_node_t* node)
{
    if (unlikely(is_null(_this) || is_null(node)))
        return NULL;
    return __map_next(_this, node);
}

static __always_inline map_node_t* __map_prev(const map_t* _this, const map_node_t* node)
{
    struct rb_node* t = NULL;

    if (RB_EMPTY_ROOT(&_this->root))
        return __map_end(_this);

    if (__map_end(_this) == node)
        return __map_last(_this); /* Err: since the `ds` is non-empty, 
                                          the return value includes the error case of `NULL` */

    /* This check should come after `end` or `rend`.
       This is a pre-judgment condition for `rb_next` or `rb_prev` */
    if (unlikely(RB_EMPTY_NODE(&node->node)))
        return NULL;

    t = rb_prev(&node->node);
    return is_null(t) ? __map_end(_this) : map_entry(t);
}

static __always_inline map_node_t* _map_prev(const map_t* _this, const map_node_t* node)
{
    if (unlikely(is_null(_this) || is_null(node)))
        return NULL;
    return __map_prev(_this, node);
}

static __always_inline map_node_t* __map_rend(const map_t* _this)
{
    return (map_node_t*)iterator_rend();
}

static __always_inline map_node_t* __map_rbegin(const map_t* _this)
{
    map_node_t* t = __map_last(_this);
    return is_null(t) ? __map_rend(_this) : t;
}

static __always_inline map_node_t* _map_rbegin(const map_t* _this)
{
    if (unlikely(is_null(_this)))
        return NULL;
    return __map_rbegin(_this);
}

static __always_inline map_node_t* __map_rnext(const map_t* _this, const map_node_t* node)
{
    struct rb_node* t = NULL;

    if (RB_EMPTY_ROOT(&_this->root) || __map_rend(_this) == node)
        return __map_rend(_this);

    /* The input parameter is `reverse_iterator`, and there's no need 
       to check whether it equals `end` */

    /* This check should come after `end` or `rend`.
       This is a pre-judgment condition for `rb_next` or `rb_prev` */
    if (unlikely(RB_EMPTY_NODE(&node->node)))
        return NULL;

    t = rb_prev(&node->node);
    return is_null(t) ? __map_rend(_this) : map_entry(t);
}

static __always_inline map_node_t* _map_rnext(const map_t* _this, const map_node_t* node)
{
    if (unlikely(is_null(_this) || is_null(node)))
        return NULL;
    return __map_rnext(_this, node);
}

static __always_inline map_node_t* __map_rprev(const map_t* _this, const map_node_t* node)
{
    struct rb_node* t = NULL;

    if (RB_EMPTY_ROOT(&_this->root))
        return __map_rend(_this);

    if (__map_rend(_this) == node)
        return __map_first(_this); /* Err: since the `ds` is non-empty, 
                                           the return value includes the error case of `NULL` */

    /* This check should come after `end` or `rend`.
       This is a pre-judgment condition for `rb_next` or `rb_prev` */
    if (unlikely(RB_EMPTY_NODE(&node->node)))
        return NULL;

    t = rb_next(&node->node);
    return is_null(t) ? __map_rend(_this) : map_entry(t);
}

static __always_inline map_node_t* _map_rprev(const map_t* _this, const map_node_t* node)
{
    if (unlikely(is_null(_this) || is_null(node)))
        return NULL;
    return __map_rprev(_this, node);
}

static inline map_node_t* __map_find(const map_t* _this, map_key_t key)
{
    struct rb_node* n = _this->root.rb_node;
    map_node_t* t = NULL;

    if (is_null(_this->ops) || is_null(_this->ops->__lt)) {
        while (!is_null(n)) {
            t = map_entry(n);

            if (key < t->key)
                n = n->rb_left;
            else if (key > t->key)
                n = n->rb_right;
            else
                return t;
        }
    } else {
        while (!is_null(n)) {
            t = map_entry(n);

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

static __always_inline map_node_t* map_find(const map_t* _this, map_key_t key)
{
    map_node_t* t = NULL;

    if (unlikely(is_null(_this)))
        return NULL;

    if (!is_null(_this->ops) && !is_null(_this->ops->valid_key) && !_this->ops->valid_key(key))
        return NULL;

    t = __map_find(_this, key);
    return is_null(t) ? __map_end(_this) : t;
}

static inline map_node_t* __map_lower_bound(const map_t* _this, map_key_t key)
{
    struct rb_node* n = _this->root.rb_node;
    map_node_t* t = NULL;
    map_node_t* ret = NULL;

    if (is_null(_this->ops) || is_null(_this->ops->__lt)) {
        while (!is_null(n)) {
            t = map_entry(n);

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
            t = map_entry(n);

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

static __always_inline map_node_t* map_lower_bound(const map_t* _this, map_key_t key)
{
    map_node_t* t = NULL;

    if (unlikely(is_null(_this)))
        return NULL;

    if (!is_null(_this->ops) && !is_null(_this->ops->valid_key) && !_this->ops->valid_key(key))
        return NULL;

    t = __map_lower_bound(_this, key);
    return is_null(t) ? __map_end(_this) : t;
}

static inline map_node_t* __map_upper_bound(const map_t* _this, map_key_t key)
{
    struct rb_node* n = _this->root.rb_node;
    map_node_t* t = NULL;
    map_node_t* ret = NULL;

    if (is_null(_this->ops) || is_null(_this->ops->__lt)) {
        while (!is_null(n)) {
            t = map_entry(n);

            if (key < t->key) {
                n = n->rb_left;
                ret = t;
            } else if (key > t->key) {
                n = n->rb_right;
            } else {
                n = rb_next(n);
                return is_null(n) ? NULL : map_entry(n);
            }
        }
    } else {
        while (!is_null(n)) {
            t = map_entry(n);

            if (_this->ops->__lt(key, t->key)) {
                n = n->rb_left;
                ret = t;
            } else if (_this->ops->__lt(t->key, key)) {
                n = n->rb_right;
            } else {
                n = rb_next(n);
                return is_null(n) ? NULL : map_entry(n);
            }
        }
    }

    return ret;
}

static __always_inline map_node_t* map_upper_bound(const map_t* _this, map_key_t key)
{
    map_node_t* t = NULL;

    if (unlikely(is_null(_this)))
        return NULL;

    if (!is_null(_this->ops) && !is_null(_this->ops->valid_key) && !_this->ops->valid_key(key))
        return NULL;

    t = __map_upper_bound(_this, key);
    return is_null(t) ? __map_end(_this) : t;
}

static inline map_node_t* __map_insert(map_t* _this, map_node_t* node)
{
    struct rb_node** n = &_this->root.rb_node;
    struct rb_node* parent = NULL;
    map_node_t* t = NULL;

    if (is_null(_this->ops) || is_null(_this->ops->__lt)) {
        while (!is_null(*n)) {
            parent = *n;
            t = map_entry(parent);

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
            t = map_entry(parent);

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

static inline map_node_t* map_insert(map_t* _this, map_key_t key, map_value_t value)
{
    map_node_t* t = NULL;
    map_node_t* ret = NULL;

    if (unlikely(is_null(_this)))
        return NULL;

    if (!is_null(_this->ops) && !is_null(_this->ops->valid_value) && !_this->ops->valid_value(value))
        return NULL;

    if (!is_null(_this->ops) && !is_null(_this->ops->valid_key) && !_this->ops->valid_key(key))
        return NULL;

    t = (map_node_t*)p_calloc(1, sizeof(map_node_t));
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

    ret = __map_insert(_this, t);
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

static inline map_node_t* map_insert_replace(map_t* _this, map_key_t key, map_value_t value)
{
    map_node_t* t = NULL;
    map_node_t* ret = NULL;

    if (unlikely(is_null(_this)))
        return NULL;

    if (!is_null(_this->ops) && !is_null(_this->ops->valid_value) && !_this->ops->valid_value(value))
        return NULL;

    t = map_find(_this, key);
    if (is_null(t))
        return NULL;

    if (__map_end(_this) != t) {
        map_value_t tvalue = t->value;

        if (is_null(_this->ops) || is_null(_this->ops->copy_value)) {
            t->value = value;
        } else {
            if (!_this->ops->copy_value(value, &t->value)) {
                t->value = tvalue;
                return NULL;
            }

            if (!is_null(_this->ops) && !is_null(_this->ops->free_value))
                _this->ops->free_value(&tvalue);
        }

        return t;
    }

    t = (map_node_t*)p_calloc(1, sizeof(map_node_t));
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

    ret = __map_insert(_this, t);
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

static __always_inline map_node_t* __map_erase(map_t* _this, map_node_t* pos)
{
    rb_erase(&pos->node, &_this->root);
    _this->size--;
    return pos;
}

static inline map_node_t* map_erase(map_t* _this, map_node_t* pos)
{
    map_node_t* t = NULL;

    if (unlikely(is_null(_this) || is_null(pos)))
        return NULL;

    /* The input parameter is `iterator`, and there's no need 
       to check whether it equals `rend` */
    if (__map_size(_this) <= 0 || __map_end(_this) == pos/* || __map_rend(_this) == pos*/)
        return NULL;

    t = __map_next(_this, pos);
    if (is_null(t))
        return NULL;

    __map_erase(_this, pos);

    if (!is_null(_this->ops) && !is_null(_this->ops->free_key))
        _this->ops->free_key(&pos->key);

    if (!is_null(_this->ops) && !is_null(_this->ops->free_value))
        _this->ops->free_value(&pos->value);

    p_free(pos);

    return t;
}

static inline map_size_t map_remove(map_t* _this, map_key_t key)
{
    map_node_t* t = NULL;

    if (unlikely(is_null(_this)))
        return -1;

    t = map_find(_this, key);
    if (is_null(t))
        return -1;

    if (__map_end(_this) == t)
        return 0;

    __map_erase(_this, t);

    if (!is_null(_this->ops) && !is_null(_this->ops->free_key))
        _this->ops->free_key(&t->key);

    if (!is_null(_this->ops) && !is_null(_this->ops->free_value))
        _this->ops->free_value(&t->value);

    p_free(t);

    return 1;
}

static inline map_size_t map_remove_if(map_t* _this, remove_if_condition_kv cond)
{
    map_size_t ret = 0;
    map_node_t* t = NULL;

    if (unlikely(is_null(_this) || is_null(cond)))
        return -1;

    for (t = __map_begin(_this); __map_end(_this) != t; ) {
        if (!cond(t->key, t->value)) {
            t = __map_next(_this, t);
            continue;
        }

        t = map_erase(_this, t);
        ret++;
    }

    return ret;
}

static inline map_size_t map_clear(map_t* _this)
{
    map_size_t ret = 0;
    map_node_t* t = NULL;

    if (unlikely(is_null(_this)))
        return -1;

    for (t = __map_begin(_this); __map_end(_this) != t; ) {
        t = map_erase(_this, t); /* When `clear`, the performance of `erase` without rotation doesn't improve */
        ret++;
    }

    return ret;
}

__always_inline void __map_init(map_t* map)
{
    map->root = RB_ROOT;
}

__always_inline void __map_deinit(map_t* map)
{
    map_clear(map);

    map->ops = NULL;
    map->root = RB_ROOT;
    map->size = 0;
}

typedef map_iterator_t* (*fp_end)(const map_t* _this);
typedef map_iterator_t* (*fp_begin)(const map_t* _this);
typedef map_iterator_t* (*fp_next)(const map_t* _this, const map_iterator_t* iterator);
typedef map_iterator_t* (*fp_prev)(const map_t* _this, const map_iterator_t* iterator);
typedef map_r_iterator_t* (*fp_rend)(const map_t* _this);
typedef map_r_iterator_t* (*fp_rbegin)(const map_t* _this);
typedef map_r_iterator_t* (*fp_rnext)(const map_t* _this, const map_r_iterator_t* r_iterator);
typedef map_r_iterator_t* (*fp_rprev)(const map_t* _this, const map_r_iterator_t* r_iterator);
typedef map_iterator_t* (*fp_find)(const map_t* _this, map_key_t key);
typedef map_iterator_t* (*fp_lower_bound)(const map_t* _this, map_key_t key);
typedef map_iterator_t* (*fp_upper_bound)(const map_t* _this, map_key_t key);
typedef map_iterator_t* (*fp_insert)(map_t* _this, map_key_t key, map_value_t value);
typedef map_iterator_t* (*fp_insert_replace)(map_t* _this, map_key_t key, map_value_t value);
typedef map_iterator_t* (*fp_erase)(map_t* _this, map_iterator_t* iterator);

const class_map_t* class_map_ins(void)
{
    static const class_map_t ins = {
        .size           = _map_size,
        .count          = map_count,
        .end            = (fp_end)__map_end,
        .begin          = (fp_begin)_map_begin,
        .next           = (fp_next)_map_next,
        .prev           = (fp_prev)_map_prev,
        .rend           = (fp_rend)__map_rend,
        .rbegin         = (fp_rbegin)_map_rbegin,
        .rnext          = (fp_rnext)_map_rnext,
        .rprev          = (fp_rprev)_map_rprev,
        .find           = (fp_find)map_find,
        .lower_bound    = (fp_lower_bound)map_lower_bound,
        .upper_bound    = (fp_upper_bound)map_upper_bound,
        .insert         = (fp_insert)map_insert,
        .insert_replace = (fp_insert_replace)map_insert_replace,
        .erase          = (fp_erase)map_erase,
        .remove         = map_remove,
        .remove_if      = map_remove_if,
        .clear          = map_clear,
    };
    return &ins;
}
