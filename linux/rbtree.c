/*
  Red Black Trees
  (C) 1999  Andrea Arcangeli <andrea@suse.de>
  (C) 2002  David Woodhouse <dwmw2@infradead.org>
  
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

#include <linux/rbtree.h>

#define	RB_RED   0
#define	RB_BLACK 1

static void __rb_rotate_left(struct rb_node *node, struct rb_root *root)
{
    struct rb_node *right = node->rb_right;
    struct rb_node *parent = rb_parent(node);

    if ((node->rb_right = right->rb_left))
        rb_set_parent(right->rb_left, node);
    right->rb_left = node;

    rb_set_parent(right, parent);

    if (parent)
    {
        if (node == parent->rb_left)
            parent->rb_left = right;
        else
            parent->rb_right = right;
    }
    else
        root->rb_node = right;
    rb_set_parent(node, right);
}

static void __rb_rotate_right(struct rb_node *node, struct rb_root *root)
{
    struct rb_node *left = node->rb_left;
    struct rb_node *parent = rb_parent(node);

    if ((node->rb_left = left->rb_right))
        rb_set_parent(left->rb_right, node);
    left->rb_right = node;

    rb_set_parent(left, parent);

    if (parent)
    {
        if (node == parent->rb_right)
            parent->rb_right = left;
        else
            parent->rb_left = left;
    }
    else
        root->rb_node = left;
    rb_set_parent(node, left);
}

void rb_insert_color(struct rb_node *node, struct rb_root *root)
{
    struct rb_node *parent, *gparent;

    while ((parent = rb_parent(node)) && rb_is_red(parent))
    {
        gparent = rb_parent(parent);

        if (parent == gparent->rb_left)
        {
            {
                register struct rb_node *uncle = gparent->rb_right;
                if (uncle && rb_is_red(uncle))
                {
                    rb_set_black(uncle);
                    rb_set_black(parent);
                    rb_set_red(gparent);
                    node = gparent;
                    continue;
                }
            }

            if (parent->rb_right == node)
            {
                register struct rb_node *tmp;
                __rb_rotate_left(parent, root);
                tmp = parent;
                parent = node;
                node = tmp;
            }

            rb_set_black(parent);
            rb_set_red(gparent);
            __rb_rotate_right(gparent, root);
        } else {
            {
                register struct rb_node *uncle = gparent->rb_left;
                if (uncle && rb_is_red(uncle))
                {
                    rb_set_black(uncle);
                    rb_set_black(parent);
                    rb_set_red(gparent);
                    node = gparent;
                    continue;
                }
            }

            if (parent->rb_left == node)
            {
                register struct rb_node *tmp;
                __rb_rotate_right(parent, root);
                tmp = parent;
                parent = node;
                node = tmp;
            }

            rb_set_black(parent);
            rb_set_red(gparent);
            __rb_rotate_left(gparent, root);
        }
    }

    rb_set_black(root->rb_node);
}

static void __rb_erase_color(struct rb_node *node, struct rb_node *parent, struct rb_root *root)
{
    struct rb_node *other;

    while ((!node || rb_is_black(node)) && node != root->rb_node)
    {
        if (parent->rb_left == node)
        {
            other = parent->rb_right;
            if (rb_is_red(other))
            {
                rb_set_black(other);
                rb_set_red(parent);
                __rb_rotate_left(parent, root);
                other = parent->rb_right;
            }
            if ((!other->rb_left || rb_is_black(other->rb_left)) &&
                (!other->rb_right || rb_is_black(other->rb_right)))
            {
                rb_set_red(other);
                node = parent;
                parent = rb_parent(node);
            }
            else
            {
                if (!other->rb_right || rb_is_black(other->rb_right))
                {
                    rb_set_black(other->rb_left);
                    rb_set_red(other);
                    __rb_rotate_right(other, root);
                    other = parent->rb_right;
                }
                rb_set_color(other, rb_color(parent));
                rb_set_black(parent);
                rb_set_black(other->rb_right);
                __rb_rotate_left(parent, root);
                node = root->rb_node;
                break;
            }
        }
        else
        {
            other = parent->rb_left;
            if (rb_is_red(other))
            {
                rb_set_black(other);
                rb_set_red(parent);
                __rb_rotate_right(parent, root);
                other = parent->rb_left;
            }
            if ((!other->rb_left || rb_is_black(other->rb_left)) &&
                (!other->rb_right || rb_is_black(other->rb_right)))
            {
                rb_set_red(other);
                node = parent;
                parent = rb_parent(node);
            }
            else
            {
                if (!other->rb_left || rb_is_black(other->rb_left))
                {
                    rb_set_black(other->rb_right);
                    rb_set_red(other);
                    __rb_rotate_left(other, root);
                    other = parent->rb_left;
                }
                rb_set_color(other, rb_color(parent));
                rb_set_black(parent);
                rb_set_black(other->rb_left);
                __rb_rotate_right(parent, root);
                node = root->rb_node;
                break;
            }
        }
    }
    if (node)
        rb_set_black(node);
}

void rb_erase(struct rb_node *node, struct rb_root *root)
{
    struct rb_node *child, *parent;
    int color;

    if (!node->rb_left)
        child = node->rb_right;
    else if (!node->rb_right)
        child = node->rb_left;
    else
    {
        struct rb_node *old = node, *left;

        node = node->rb_right;
        while ((left = node->rb_left) != NULL)
            node = left;

        if (rb_parent(old)) {
            if (rb_parent(old)->rb_left == old)
                rb_parent(old)->rb_left = node;
            else
                rb_parent(old)->rb_right = node;
        } else
            root->rb_node = node;

        child = node->rb_right;
        parent = rb_parent(node);
        color = rb_color(node);

        if (parent == old) {
            parent = node;
        } else {
            if (child)
                rb_set_parent(child, parent);
            parent->rb_left = child;

            node->rb_right = old->rb_right;
            rb_set_parent(old->rb_right, node);
        }

        node->rb_parent_color = old->rb_parent_color;
        node->rb_left = old->rb_left;
        rb_set_parent(old->rb_left, node);

        goto color;
    }

    parent = rb_parent(node);
    color = rb_color(node);

    if (child)
        rb_set_parent(child, parent);
    if (parent)
    {
        if (parent->rb_left == node)
            parent->rb_left = child;
        else
            parent->rb_right = child;
    }
    else
        root->rb_node = child;

color:
    if (color == RB_BLACK)
        __rb_erase_color(child, parent, root);
}

struct rb_node *rb_first(const struct rb_root *root)
{
    struct rb_node *n;

    n = root->rb_node;
    if (!n)
        return NULL;
    while (n->rb_left)
        n = n->rb_left;
    return n;
}

struct rb_node *rb_last(const struct rb_root *root)
{
    struct rb_node *n;

    n = root->rb_node;
    if (!n)
        return NULL;
    while (n->rb_right)
        n = n->rb_right;
    return n;
}

struct rb_node *rb_next(const struct rb_node *node)
{
    struct rb_node *parent;

    if (rb_parent(node) == node)
        return NULL;

    if (node->rb_right) {
        node = node->rb_right; 
        while (node->rb_left)
            node = node->rb_left;
        return (struct rb_node *)node;
    }

    while ((parent = rb_parent(node)) && node == parent->rb_right)
        node = parent;

    return parent;
}

struct rb_node *rb_prev(const struct rb_node *node)
{
    struct rb_node *parent;

    if (rb_parent(node) == node)
        return NULL;

    if (node->rb_left) {
        node = node->rb_left; 
        while (node->rb_right)
            node = node->rb_right;
        return (struct rb_node *)node;
    }

    while ((parent = rb_parent(node)) && node == parent->rb_left)
        node = parent;

    return parent;
}

void rb_replace_node(struct rb_node *victim, struct rb_node *new, struct rb_root *root)
{
	struct rb_node *parent = rb_parent(victim);

    if (parent) {
        if (victim == parent->rb_left)
            parent->rb_left = new;
        else
            parent->rb_right = new;
    } else {
        root->rb_node = new;
    }
    if (victim->rb_left)
        rb_set_parent(victim->rb_left, new);
    if (victim->rb_right)
        rb_set_parent(victim->rb_right, new);

    *new = *victim;
}
