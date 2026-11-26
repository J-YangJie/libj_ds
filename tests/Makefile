# libj_ds permanent test build front end.
#
# The container list is discovered from containers/*/container.mk. Adding a
# container directory with that file and its sources is enough; this Makefile
# does not name individual containers.
#
# Common commands:
#   make test            default configuration, all non-stress suites
#   make check           header smoke plus default/debug/align/32-bit matrix
#   make test-debug      debug build (JDSC_DEBUG, death tests)
#   make test-align      JDSC_ALIGNMEM=1 build
#   make test-32         32-bit build when the toolchain is available
#   make test-valgrind   default suites under Valgrind
#   make stress          stress suite, STRESS_ITERS iterations
#   make stress-long     stress suite with STRESS_LONG_ITERS iterations
#   make stress-valgrind stress suite under Valgrind
#   make bench           benchmark suite
#   make test-diff       C++ differential suite
#   make test-model      randomized model suite
#   make test-alloc      allocation failure suite
#   make headers         public header smoke for C and C++
#   make matrix          default, debug, align, and 32-bit runs
#   make clean           remove build/

TEST_ROOT := $(patsubst %/,%,$(dir $(abspath $(lastword $(MAKEFILE_LIST)))))
ROOT := $(abspath $(TEST_ROOT)/..)

CC ?= gcc
CXX ?= g++
AR ?= ar

CONFIG ?= default
BUILD := $(TEST_ROOT)/build
OBJDIR := $(BUILD)/$(CONFIG)/obj
LIB := $(BUILD)/$(CONFIG)/libprod.a
BIN := $(BUILD)/$(CONFIG)/libj_ds_tests
LOGDIR := $(BUILD)/logs/$(CONFIG)

STRESS_ITERS ?= 200000
STRESS_LONG_ITERS ?= 5000000
TEST_SEED ?=
SEED_ARG := $(if $(strip $(TEST_SEED)),--seed $(TEST_SEED),)
LOG_NAME ?= run
RUN_ARGS ?=
VALGRIND ?=
VALGRIND_CMD ?= valgrind --error-exitcode=99 --leak-check=full --errors-for-leak-kinds=all

M32 :=
ALIGNMEM := 0
CONFIG_CFLAGS := -O2 -g
ifeq ($(CONFIG),debug)
CONFIG_CFLAGS := -O1 -g -DJDSC_DEBUG
else ifeq ($(CONFIG),align)
ALIGNMEM := 1
else ifeq ($(CONFIG),debug-align)
CONFIG_CFLAGS := -O1 -g -DJDSC_DEBUG
ALIGNMEM := 1
else ifeq ($(CONFIG),32)
M32 := -m32
else ifneq ($(CONFIG),default)
$(error Unknown CONFIG `$(CONFIG)`; use default, debug, align, debug-align, or 32)
endif

COMMON_CFLAGS := -std=gnu99 -Wall -Werror -fno-common -fmessage-length=0 \
                 -MMD -MP $(CONFIG_CFLAGS) $(M32) \
                 -DJDSC_MEMPOOL -DJDSC_ALIGNMEM=$(ALIGNMEM) \
                 -DTEST_CONFIG_NAME=\"$(CONFIG)\"
COMMON_CXXFLAGS := -std=c++11 -Wall -Werror -fno-common -fmessage-length=0 \
                   -MMD -MP $(CONFIG_CFLAGS) $(M32) \
                   -DJDSC_MEMPOOL -DJDSC_ALIGNMEM=$(ALIGNMEM) \
                   -DTEST_CONFIG_NAME=\"$(CONFIG)\"

CONTAINER_MKS := $(sort $(wildcard $(TEST_ROOT)/containers/*/container.mk))
CONTAINER_DIRS := $(patsubst %/container.mk,%,$(CONTAINER_MKS))
CONTAINER_NAMES := $(notdir $(CONTAINER_DIRS))
include $(CONTAINER_MKS)

CPPFLAGS := -I$(ROOT)/include -I$(TEST_ROOT)/core -I$(TEST_ROOT)/contracts \
            $(foreach d,$(CONTAINER_DIRS),-I$(d))
PUBLIC_HEADERS := $(foreach c,$(CONTAINER_NAMES),$($(c)_PUBLIC_HEADERS))

# All production sources, independent of the root Makefile's WITH_* switches.
PROD_SRCS := \
    $(ROOT)/operations/ds_ops_string.c \
    $(ROOT)/sequence_container/sequence_container_ops.c \
    $(ROOT)/sequence_container/operations/ds_ops_sso.c \
    $(ROOT)/associative_container/associative_container_ops.c \
    $(ROOT)/deque/deque.c \
    $(ROOT)/vector/vector.c \
    $(ROOT)/list/list.c \
    $(ROOT)/stack/stack.c \
    $(ROOT)/queue/queue.c \
    $(ROOT)/priority_queue/priority_queue.c \
    $(ROOT)/priority_queue/priority_queue_ops.c \
    $(ROOT)/hashmap/hashmap.c \
    $(ROOT)/hashmap/hashmap_ops.c \
    $(ROOT)/map/map.c \
    $(ROOT)/multimap/multimap.c \
    $(ROOT)/set/set.c \
    $(ROOT)/multiset/multiset.c \
    $(ROOT)/linux/rbtree.c \
    $(ROOT)/sort/sort.c

CORE_SRCS := $(sort $(wildcard $(TEST_ROOT)/core/*.c))
CONTRACT_SRCS := $(sort $(wildcard $(TEST_ROOT)/contracts/*.c))
CONTAINER_C_SRCS := $(sort $(foreach d,$(CONTAINER_DIRS),$(wildcard $(d)/*.c)))
CONTAINER_CXX_SRCS := $(sort $(foreach d,$(CONTAINER_DIRS),$(wildcard $(d)/*.cpp)))

TEST_C_SRCS := $(CORE_SRCS) $(CONTRACT_SRCS) $(CONTAINER_C_SRCS)
TEST_CXX_SRCS := $(CONTAINER_CXX_SRCS)

obj_for = $(OBJDIR)/$(subst /,_,$(patsubst $(ROOT)/%,%,$(1))).o

PROD_OBJS := $(foreach s,$(PROD_SRCS),$(call obj_for,$(s)))
TEST_C_OBJS := $(foreach s,$(TEST_C_SRCS),$(call obj_for,$(s)))
TEST_CXX_OBJS := $(foreach s,$(TEST_CXX_SRCS),$(call obj_for,$(s)))
TEST_OBJS := $(TEST_C_OBJS) $(TEST_CXX_OBJS)
ALLOC_OBJ := $(call obj_for,$(TEST_ROOT)/core/test_alloc.c)
LINKER := $(if $(TEST_CXX_OBJS),$(CXX),$(CC))

DEPS := $(PROD_OBJS:.o=.d) $(TEST_OBJS:.o=.d)

.PHONY: all test check matrix headers \
        test-debug test-align test-32 test-valgrind \
        stress stress-long stress-valgrind bench \
        test-diff test-model test-alloc \
        run-config clean

all: $(BIN)

define compile_c
$(call obj_for,$(1)): $(1)
	@mkdir -p $$(@D)
	@echo "  CC  [$(CONFIG)] $$<"
	@$(CC) $(COMMON_CFLAGS) $(CPPFLAGS) -c $$< -o $$@
endef

define compile_cxx
$(call obj_for,$(1)): $(1)
	@mkdir -p $$(@D)
	@echo "  CXX [$(CONFIG)] $$<"
	@$(CXX) $(COMMON_CXXFLAGS) $(CPPFLAGS) -c $$< -o $$@
endef

$(foreach s,$(PROD_SRCS) $(TEST_C_SRCS),$(eval $(call compile_c,$(s))))
$(foreach s,$(TEST_CXX_SRCS),$(eval $(call compile_cxx,$(s))))

$(LIB): $(PROD_OBJS)
	@mkdir -p $(@D)
	@echo "  AR  [$(CONFIG)] $@"
	@$(AR) rcs $@ $^

$(BIN): $(TEST_OBJS) $(LIB)
	@mkdir -p $(@D)
	@echo "  LD  [$(CONFIG)] $@"
	@$(LINKER) $(M32) -o $@ $(ALLOC_OBJ) \
	    $(filter-out $(ALLOC_OBJ),$(TEST_OBJS)) $(LIB) -lm

# Test objects must be brought in explicitly so the strong test allocator
# overrides the weak allocator definitions inside the production archive.
run-config: $(BIN)
	@mkdir -p $(LOGDIR)
	@echo "== $(CONFIG): $(LOG_NAME) =="
	@bash -o pipefail -c '$(VALGRIND) "$(BIN)" $(RUN_ARGS) 2>&1 | tee "$(LOGDIR)/$(LOG_NAME).log"'

test:
	@$(MAKE) --no-print-directory run-config CONFIG=default \
	    LOG_NAME=test RUN_ARGS="$(SEED_ARG)"

test-debug:
	@$(MAKE) --no-print-directory run-config CONFIG=debug \
	    LOG_NAME=test RUN_ARGS="$(SEED_ARG)"

test-align:
	@$(MAKE) --no-print-directory run-config CONFIG=align \
	    LOG_NAME=test RUN_ARGS="$(SEED_ARG)"

test-32:
	@mkdir -p $(BUILD)
	@if printf 'int main(void){return 0;}\n' \
	        | $(CC) -m32 -x c - -o $(BUILD)/probe32 >/dev/null 2>&1 \
	    && printf 'int main(){return 0;}\n' \
	        | $(CXX) -m32 -x c++ - -o $(BUILD)/probe32 >/dev/null 2>&1; then \
	    $(MAKE) --no-print-directory run-config CONFIG=32 \
	        LOG_NAME=test RUN_ARGS="$(SEED_ARG)"; \
	else \
	    echo "SKIP: 32-bit test build unavailable (need gcc/g++ -m32 support)."; \
	fi

test-valgrind:
	@$(MAKE) --no-print-directory run-config CONFIG=default \
	    LOG_NAME=valgrind RUN_ARGS="$(SEED_ARG)" \
	    VALGRIND="$(VALGRIND_CMD)"

test-diff:
	@$(MAKE) --no-print-directory run-config CONFIG=default \
	    LOG_NAME=diff RUN_ARGS="--suite diff $(SEED_ARG)"

test-model:
	@$(MAKE) --no-print-directory run-config CONFIG=default \
	    LOG_NAME=model RUN_ARGS="--suite model $(SEED_ARG)"

test-alloc:
	@$(MAKE) --no-print-directory run-config CONFIG=default \
	    LOG_NAME=alloc RUN_ARGS="--suite alloc $(SEED_ARG)"

stress:
	@$(MAKE) --no-print-directory run-config CONFIG=default \
	    LOG_NAME=stress RUN_ARGS="--suite stress --iters $(STRESS_ITERS) $(SEED_ARG)"

stress-long:
	@$(MAKE) --no-print-directory run-config CONFIG=default \
	    LOG_NAME=stress-long \
	    RUN_ARGS="--suite stress --iters $(STRESS_LONG_ITERS) $(SEED_ARG)"

stress-valgrind:
	@$(MAKE) --no-print-directory run-config CONFIG=default \
	    LOG_NAME=stress-valgrind \
	    RUN_ARGS="--suite stress --iters $(STRESS_ITERS) $(SEED_ARG)" \
	    VALGRIND="$(VALGRIND_CMD)"

bench:
	@$(MAKE) --no-print-directory run-config CONFIG=default \
	    LOG_NAME=bench RUN_ARGS="--suite bench $(SEED_ARG)"

headers:
	@mkdir -p $(BUILD)/headers
	@set -e; \
	for h in $(PUBLIC_HEADERS); do \
	    inc=$${h#$(ROOT)/include/}; \
	    printf '#include <%s>\nint main(void) { return 0; }\n' "$$inc" \
	        > $(BUILD)/headers/one.c; \
	    $(CC) $(COMMON_CFLAGS) $(CPPFLAGS) -fsyntax-only $(BUILD)/headers/one.c; \
	    printf '#include <%s>\nint main() { return 0; }\n' "$$inc" \
	        > $(BUILD)/headers/one.cpp; \
	    $(CXX) -std=c++11 $(M32) -Wall -Wno-sign-compare \
	        $(CPPFLAGS) -fsyntax-only $(BUILD)/headers/one.cpp; \
	done; \
	echo "header smoke: OK"

matrix: test test-debug test-align test-32

check: headers matrix

clean:
	@rm -rf $(BUILD)

-include $(DEPS)
