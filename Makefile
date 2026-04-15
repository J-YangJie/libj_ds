
DLIB_VERSION = 1.0.2
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
WITH_PERFORMANCE_STL=n
WITH_DEMO=y


CC  = $(CROSS_COMPILE)gcc
CXX = $(CROSS_COMPILE)g++
AR  = $(CROSS_COMPILE)ar

OBJS = \
	operations/ds_ops_string.o
#	iterator/iterator.o

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
OBJS += hashmap/hashmap.o
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
CFLAGS += -O2
# CFLAGS += -g3
CFLAGS += -Werror

CXXFLAGS += -std=c++11
CXXFLAGS += -Wall -fmessage-length=0 -fPIC -fno-common
CXXFLAGS += -O2
# CXXFLAGS += -g3
CXXFLAGS += -Werror

# Dynamic library name with version
DLIB_NAME_WITHVER := $(DLIB_NAME).$(DLIB_VERSION)


.PHONY: clean cleanall dlib slib performance_dlib performance performance_stl

ifeq ($(WITH_PERFORMANCE), y)
ifeq ($(WITH_VECTOR), y)
PERFORMANCE_BINS += performance_vector
endif
ifeq ($(WITH_LIST), y)
PERFORMANCE_BINS += performance_list
endif
ifeq ($(WITH_PRIORITY_QUEUE), y)
PERFORMANCE_BINS += performance_pqueue
endif
ifeq ($(WITH_HASHMAP), y)
PERFORMANCE_BINS += performance_hashmap
PERFORMANCE_BINS += performance_hashmap_reserve
endif
ifeq ($(WITH_MAP), y)
PERFORMANCE_BINS += performance_map
endif
ifeq ($(WITH_MULTIMAP), y)
PERFORMANCE_BINS += performance_multimap
endif
ifeq ($(WITH_SET), y)
PERFORMANCE_BINS += performance_set
endif
ifeq ($(WITH_MULTISET), y)
PERFORMANCE_BINS += performance_multiset
endif
endif # WITH_PERFORMANCE

ifeq ($(WITH_PERFORMANCE_STL), y)
ifeq ($(WITH_VECTOR), y)
PERFORMANCE_STL_BINS += performance_stl_vector
endif
ifeq ($(WITH_LIST), y)
PERFORMANCE_STL_BINS += performance_stl_list
endif
ifeq ($(WITH_PRIORITY_QUEUE), y)
PERFORMANCE_STL_BINS += performance_stl_pqueue
endif
ifeq ($(WITH_HASHMAP), y)
PERFORMANCE_STL_BINS += performance_stl_hashmap
PERFORMANCE_STL_BINS += performance_stl_hashmap_reserve
endif
ifeq ($(WITH_MAP), y)
PERFORMANCE_STL_BINS += performance_stl_map
endif
ifeq ($(WITH_MULTIMAP), y)
PERFORMANCE_STL_BINS += performance_stl_multimap
endif
ifeq ($(WITH_SET), y)
PERFORMANCE_STL_BINS += performance_stl_set
endif
ifeq ($(WITH_MULTISET), y)
PERFORMANCE_STL_BINS += performance_stl_multiset
endif
endif # WITH_PERFORMANCE_STL

ifeq ($(WITH_DEMO), y)
ifeq ($(WITH_VECTOR), y)
DEMO_BINS += demo/demo_vector_bin
endif
ifeq ($(WITH_LIST), y)
DEMO_BINS += demo/demo_list_bin
endif
ifeq ($(WITH_PRIORITY_QUEUE), y)
DEMO_BINS += demo/demo_pqueue_bin
endif
ifeq ($(WITH_HASHMAP), y)
DEMO_BINS += demo/demo_hashmap_bin
endif
ifeq ($(WITH_MAP), y)
DEMO_BINS += demo/demo_map_bin
endif
ifeq ($(WITH_MULTIMAP), y)
DEMO_BINS += demo/demo_multimap_bin
endif
ifeq ($(WITH_SET), y)
DEMO_BINS += demo/demo_set_bin
endif
ifeq ($(WITH_MULTISET), y)
DEMO_BINS += demo/demo_multiset_bin
endif
endif # WITH_DEMO

#all: dlib slib performance $(DEMO_BINS)
all: dlib performance performance_stl $(DEMO_BINS)
dlib : $(DLIB_NAME_WITHVER)
slib : $(SLIB_NAME)

PERFORMANCE_J_DS_TIMES_INSERT=10000000
PERFORMANCE_J_DS_TIMES_FIND=100000000
PERFORMANCE_J_DS_TIMES_FIND_V_L=100
PERFORMANCE_J_DS_TIMES_REMOVE=100000000
PERFORMANCE_J_DS_TIMES_REMOVE_V_L=100
PERFORMANCE_J_DS_HASHMAP_CAPACITY_INIT=30000000
PERFORMANCE_J_DS_DEFINES= \
		-DTIMES_INSERT=$(PERFORMANCE_J_DS_TIMES_INSERT) \
		-DTIMES_FIND=$(PERFORMANCE_J_DS_TIMES_FIND) -DTIMES_FIND_V_L=$(PERFORMANCE_J_DS_TIMES_FIND_V_L) \
		-DTIMES_REMOVE=$(PERFORMANCE_J_DS_TIMES_REMOVE) -DTIMES_REMOVE_V_L=$(PERFORMANCE_J_DS_TIMES_REMOVE_V_L)

performance_jds_vector.o : main.c
	@$(CC) $(CFLAGS) -c -o $@ $^ $(PERFORMANCE_J_DS_DEFINES) -DTEST_VECTOR

performance_jds_list.o : main.c
	@$(CC) $(CFLAGS) -c -o $@ $^ $(PERFORMANCE_J_DS_DEFINES) -DTEST_LIST

performance_jds_pqueue.o : main.c
	@$(CC) $(CFLAGS) -c -o $@ $^ $(PERFORMANCE_J_DS_DEFINES) -DTEST_PQUEUE

performance_jds_hashmap.o : main.c
	@$(CC) $(CFLAGS) -c -o $@ $^ $(PERFORMANCE_J_DS_DEFINES) -DTEST_HASHMAP -DHASHMAP_CAPACITY_INIT=0

performance_jds_hashmap_reserve.o : main.c
	@$(CC) $(CFLAGS) -c -o $@ $^ $(PERFORMANCE_J_DS_DEFINES) -DTEST_HASHMAP -DHASHMAP_CAPACITY_INIT=$(PERFORMANCE_J_DS_HASHMAP_CAPACITY_INIT)

performance_jds_map.o : main.c
	@$(CC) $(CFLAGS) -c -o $@ $^ $(PERFORMANCE_J_DS_DEFINES) -DTEST_MAP

performance_jds_multimap.o : main.c
	@$(CC) $(CFLAGS) -c -o $@ $^ $(PERFORMANCE_J_DS_DEFINES) -DTEST_MULTIMAP

performance_jds_set.o : main.c
	@$(CC) $(CFLAGS) -c -o $@ $^ $(PERFORMANCE_J_DS_DEFINES) -DTEST_SET

performance_jds_multiset.o : main.c
	@$(CC) $(CFLAGS) -c -o $@ $^ $(PERFORMANCE_J_DS_DEFINES) -DTEST_MULTISET

performance_% : performance_jds_%.o
	@$(CC) -o $@ $^ $(CFLAGS) -L. -lj_ds
	@echo "make $@"

performance_dlib :
	$(shell rm -f $(DLIB_NAME); ln -s $(DLIB_NAME_WITHVER) $(DLIB_NAME) > /dev/null 2>&1 || cp $(DLIB_NAME_WITHVER) $(DLIB_NAME))

performance : performance_dlib $(PERFORMANCE_BINS)

PERFORMANCE_STL_TIMES_INSERT=10000000
PERFORMANCE_STL_TIMES_FIND=100000000
PERFORMANCE_STL_TIMES_FIND_V_L=100
PERFORMANCE_STL_TIMES_REMOVE=100000000
PERFORMANCE_STL_TIMES_REMOVE_V_L=100
PERFORMANCE_STL_HASHMAP_CAPACITY_INIT=30000000
PERFORMANCE_STL_DEFINES= \
		-DTIMES_INSERT=$(PERFORMANCE_STL_TIMES_INSERT) \
		-DTIMES_FIND=$(PERFORMANCE_STL_TIMES_FIND) -DTIMES_FIND_V_L=$(PERFORMANCE_STL_TIMES_FIND_V_L) \
		-DTIMES_REMOVE=$(PERFORMANCE_STL_TIMES_REMOVE) -DTIMES_REMOVE_V_L=$(PERFORMANCE_STL_TIMES_REMOVE_V_L)

performance_stl_vector.o : main_stl.cpp
	@$(CXX) $(CXXFLAGS) -c -o $@ $^ $(PERFORMANCE_STL_DEFINES) -DTEST_VECTOR

performance_stl_list.o : main_stl.cpp
	@$(CXX) $(CXXFLAGS) -c -o $@ $^ $(PERFORMANCE_STL_DEFINES) -DTEST_LIST

performance_stl_pqueue.o : main_stl.cpp
	@$(CXX) $(CXXFLAGS) -c -o $@ $^ $(PERFORMANCE_STL_DEFINES) -DTEST_PQUEUE

performance_stl_hashmap.o : main_stl.cpp
	@$(CXX) $(CXXFLAGS) -c -o $@ $^ $(PERFORMANCE_STL_DEFINES) -DTEST_HASHMAP -DHASHMAP_CAPACITY_INIT=0

performance_stl_hashmap_reserve.o : main_stl.cpp
	@$(CXX) $(CXXFLAGS) -c -o $@ $^ $(PERFORMANCE_STL_DEFINES) -DTEST_HASHMAP -DHASHMAP_CAPACITY_INIT=$(PERFORMANCE_STL_HASHMAP_CAPACITY_INIT)

performance_stl_map.o : main_stl.cpp
	@$(CXX) $(CXXFLAGS) -c -o $@ $^ $(PERFORMANCE_STL_DEFINES) -DTEST_MAP

performance_stl_multimap.o : main_stl.cpp
	@$(CXX) $(CXXFLAGS) -c -o $@ $^ $(PERFORMANCE_STL_DEFINES) -DTEST_MULTIMAP

performance_stl_set.o : main_stl.cpp
	@$(CXX) $(CXXFLAGS) -c -o $@ $^ $(PERFORMANCE_STL_DEFINES) -DTEST_SET

performance_stl_multiset.o : main_stl.cpp
	@$(CXX) $(CXXFLAGS) -c -o $@ $^ $(PERFORMANCE_STL_DEFINES) -DTEST_MULTISET

performance_stl_% : performance_stl_%.o
	@$(CXX) -o $@ $^ $(CXXFLAGS)
	@echo "make $@"

performance_stl : $(PERFORMANCE_STL_BINS)

demo/%_bin : demo/%.c
	@$(CC) -o $@ $^ $(CFLAGS) -L. -lj_ds
	@echo "make $@"

clean :
	@rm -f $(OBJS) performance_jds_*.o performance_stl_*.o demo/demo_*.o
	@echo "$@"

cleanall : clean
	@rm -f $(DLIB_NAME_WITHVER) $(DLIB_NAME) $(SLIB_NAME) $(PERFORMANCE_BINS) $(PERFORMANCE_STL_BINS) $(DEMO_BINS) stress/*.o stress_*
	@echo "$@"

DCFLAGS += -shared -Wl,-soname,$(DLIB_NAME_WITHVER)

$(DLIB_NAME_WITHVER) : $(OBJS)
	@$(CC) $(DCFLAGS) -o $@ $^
	@echo "make $@"

SCFLAGS += rcs

$(SLIB_NAME) : $(OBJS)
	@$(AR) $(SCFLAGS) $@ $^
	@echo "make $@"

