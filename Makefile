
DLIB_VERSION = 1.0.0
DLIB_NAME := libj_ds.so
SLIB_NAME := libj_ds.a


WITH_LIST=y
WITH_VECTOR=y
WITH_PRIORITY_QUEUE=y
WITH_HASHMAP=y
WITH_MAP=y
WITH_MULTIMAP=y
WITH_SET=y
WITH_MULTISET=y
WITH_PERFORMANCE=y
WITH_DEMO=y


CC = $(CROSS_COMPILE)gcc
AR = $(CROSS_COMPILE)ar

OBJS = \
	iterator/iterator.o \
	operations/ds_ops_string.o

ifeq ($(WITH_LIST), y)
OBJS += list/list.o list/list_ops.o
endif

ifeq ($(WITH_VECTOR), y)
OBJS += vector/vector.o vector/vector_ops.o
endif

ifeq ($(WITH_PRIORITY_QUEUE), y)
OBJS += priority_queue/priority_queue.o priority_queue/priority_queue_ops.o
endif

ifeq ($(WITH_HASHMAP), y)
OBJS += bucket/bucket.o hashmap/hashmap.o
endif

ifeq ($(WITH_MAP), y)
OBJS += map/map.o
endif

ifeq ($(WITH_MULTIMAP), y)
OBJS += multimap/multimap.o
endif

ifeq ($(WITH_SET), y)
OBJS += set/set.o
endif

ifeq ($(WITH_MULTISET), y)
OBJS += multiset/multiset.o
endif

ifneq ($(findstring y, $(WITH_HASHMAP)$(WITH_MAP)$(WITH_MULTIMAP)$(WITH_SET)$(WITH_MULTISET)),)
OBJS += linux/rbtree.o
endif

ifneq ($(findstring y, $(WITH_VECTOR)$(WITH_LIST)),)
OBJS += sort/sort.o
endif

CFLAGS += -Iinclude
CFLAGS += -std=gnu99 -Wall -fmessage-length=0 -fPIC -fno-common
CFLAGS += -O3
# CFLAGS += -g3
CFLAGS += -Werror

# Dynamic library name with version
DLIB_NAME_WITHVER := $(DLIB_NAME).$(DLIB_VERSION)


.PHONY: clean cleanall dlib slib

ifeq ($(WITH_PERFORMANCE), y)
PERFORMANCE_BINS += performance
endif

ifeq ($(WITH_DEMO), y)
DEMO_BINS += demo/demo_vector_bin
DEMO_BINS += demo/demo_list_bin
DEMO_BINS += demo/demo_pqueue_bin
DEMO_BINS += demo/demo_hashmap_bin
DEMO_BINS += demo/demo_map_bin
DEMO_BINS += demo/demo_multimap_bin
DEMO_BINS += demo/demo_set_bin
DEMO_BINS += demo/demo_multiset_bin
endif

all: dlib slib $(PERFORMANCE_BINS) $(DEMO_BINS)
dlib : $(DLIB_NAME_WITHVER)
slib : $(SLIB_NAME)

performance : main.o
	@$(shell ln -s $(DLISB_NAME_WITHVER) $(DLIB_NAME) || cp $(DLIB_NAME_WITHVER) $(DLIB_NAME))
	@$(CC) -o $@ $^ $(CFLAGS) -L. -lj_ds
	@echo "make $@"

demo/%_bin : demo/%.c
	@$(CC) -o $@ $^ $(CFLAGS) -L. -lj_ds
	@echo "make $@"

clean :
	rm -f $(OBJS) main.o demo/demo_*.o

cleanall : clean
	rm -f $(DLIB_NAME_WITHVER) $(DLIB_NAME) $(SLIB_NAME) performance demo/demo_*_bin stress/*.o stress_*

DCFLAGS += -shared -Wl,-soname,$(DLIB_NAME_WITHVER)

$(DLIB_NAME_WITHVER) : $(OBJS)
	@$(CC) $(DCFLAGS) -o $@ $^
	@echo "make $@"

SCFLAGS += rcs

$(SLIB_NAME) : $(OBJS)
	@$(AR) $(SCFLAGS) $@ $^
	@echo "make $@"

