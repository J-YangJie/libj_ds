#ifndef __LINUX_TYPES_H
#define __LINUX_TYPES_H

struct list_head {
    struct list_head* next;
    struct list_head* prev;
};

struct hlist_head {
	struct hlist_node* first;
};

struct hlist_node {
	struct hlist_node* next;
    struct hlist_node** pprev;
};

struct rb_node {
    unsigned long rb_parent_color;
    struct rb_node* rb_right;
    struct rb_node* rb_left;
} __attribute__((aligned(sizeof(long))));

struct rb_root {
    struct rb_node* rb_node;
};

#endif /* __LINUX_TYPES_H */
