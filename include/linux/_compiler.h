#ifndef __LINUX_COMPILER_H

#define likely_notrace(x)   __builtin_expect(!!(x), 1)
#define unlikely_notrace(x) __builtin_expect(!!(x), 0)
#define likely(x)           likely_notrace(x)
#define unlikely(x)         unlikely_notrace(x)

#endif /* __LINUX_COMPILER_H */
