#ifndef __LINUX_DEFS_H
#define __LINUX_DEFS_H

#undef container_of
#include <stddef.h>
#define container_of(ptr, type, member) ({                      \
        const typeof( ((type *)0)->member ) *__mptr = (ptr);    \
        (type *)( (char *)__mptr - offsetof(type,member) );})

#endif /* __LINUX_DEFS_H */
