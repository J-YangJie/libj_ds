/*
  Red Black Trees
  (C) 1999  Andrea Arcangeli <andrea@suse.de>
  
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

#ifndef __LINUX_RBTREE_H
#define __LINUX_RBTREE_H

#include <linux/_defs.h>
#include <linux/_types.h>

#define rb_parent(r)    ((struct rb_node *)((r)->rb_parent_color & ~3))
#define rb_color(r)     ((r)->rb_parent_color & 1)
#define rb_is_red(r)    (!rb_color(r))
#define rb_is_black(r)  rb_color(r)
#define rb_set_red(r)   do { (r)->rb_parent_color &= ~1; } while (0)
#define rb_set_black(r) do { (r)->rb_parent_color |= 1; } while (0)

static inline void rb_set_parent(struct rb_node *rb, struct rb_node *p)
{
    rb->rb_parent_color = (rb->rb_parent_color & 3) | (unsigned long)p;
}
static inline void rb_set_color(struct rb_node *rb, int color)
{
    rb->rb_parent_color = (rb->rb_parent_color & ~1) | color;
}

#define RB_ROOT	(struct rb_root) { NULL, }
#define	rb_entry(ptr, type, member) container_of(ptr, type, member)

#define RB_EMPTY_ROOT(root)	((root)->rb_node == NULL)
#define RB_EMPTY_NODE(node)	(rb_parent(node) == node)
#define RB_CLEAR_NODE(node)	(rb_set_parent(node, node))

/*
 * 初始化一棵树，可选地挂一个哨兵节点（header）。
 *
 * header == NULL 时与 stock Linux rbtree 行为逐字节一致（根节点的 parent 为 NULL）。
 *
 * header != NULL（本仓库里 map 用）时的不变量：
 *   1) 根节点的 parent 指向 header（不再是 NULL）—— 因此“谁算根”的判据
 *      在 rbtree.c 里统一写成 (parent != root->header)，header 为 NULL 时自动退化为旧语义；
 *   2) header 自身 parent 自指，于是 RB_EMPTY_NODE(header) 成立，充当“我是哨兵”的标记；
 *   3) header->rb_left 保存当前根节点（rb_insert_color / rb_erase 末尾同步），
 *      这样从“哨兵”能反查到树，`prev(end)` 不必再带容器指针。
 *
 * header 必须位于容器内部（地址与容器同寿），end()/rend() 才稳定。
 */
static inline void rb_root_init(struct rb_root* root, struct rb_node* header)
{
    root->rb_node = NULL;
    root->header  = header;

    if (header) {
        header->rb_left  = NULL;
        header->rb_right = NULL;
        rb_set_parent(header, header);
        rb_set_black(header);
    }
}

static inline void rb_init_node(struct rb_node *rb)
{
    rb->rb_parent_color = 0;
    rb->rb_right = NULL;
    rb->rb_left = NULL;
    RB_CLEAR_NODE(rb);
}

void rb_insert_color(struct rb_node *, struct rb_root *);
void rb_erase(struct rb_node *, struct rb_root *);

struct rb_node *rb_next(const struct rb_node *node);
struct rb_node *rb_prev(const struct rb_node *node);
struct rb_node *rb_first(const struct rb_root *root);
struct rb_node *rb_last(const struct rb_root *root);

void rb_replace_node(struct rb_node *victim, struct rb_node *new, struct rb_root *root);

static inline void rb_link_node(struct rb_node * node, struct rb_node * parent, struct rb_node ** rb_link)
{
    node->rb_parent_color = (unsigned long )parent;
    node->rb_left = node->rb_right = NULL;

    *rb_link = node;
}

#endif /* __LINUX_RBTREE_H */
