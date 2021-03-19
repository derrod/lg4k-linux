
ARCH ?= $(shell uname -m)
ifeq ($(ARCH),x86_64)
DRV_ARCH=i64
else ifeq ($(ARCH),i686)
DRV_ARCH=i32
else 
DRV_ARCH=$(ARCH)
endif

ifdef KDIR
KERNEL_VER:=$(shell cat $(KDIR)/include/config/kernel.release 2> /dev/null)
else
KERNEL_VER:=$(shell uname -r)
endif

BOARD ?= cx511h

ifeq ($(MAKELEVEL),0)
export BASE_DIR:=$(realpath $(dir $(MAKEFILE_LIST)))/
endif
include $(BASE_DIR)helper.mk

MODULE_NAME ?= $(BOARD)
UTILS_DIR := $(BASE_DIR)utils/
LIB_DIR := $(BASE_DIR)lib/

ifeq ($(call check_dir,$(BOARD_BASE_DIR)/$(BOARD)),$(NULL))
$(error no board $(BOARD) exists)
endif

basic-objs := entry.o cxt_mgr.o

utils := $(call dirs, $(UTILS_DIR))
utils_src := $(call rpath, $(foreach util, $(utils), $(call list_files_from_file, $(UTILS_DIR)$(util)SOURCE)))
utils_objs := $(patsubst %.c, %.o, $(utils_src))
utils_inc := $(foreach util, $(utils), $(addprefix -I, $(UTILS_DIR)$(util)include))

board_src:=$(call rpath,$(call list_files_from_file, $(BOARD_BASE_DIR)/$(BOARD)/SOURCE))
board_objs:=$(patsubst %.c,%.o,$(board_src))

lib_src:=$(call rpath,$(wildcard $(LIB_DIR)*.c) $(wildcard $(LIB_DIR)*.o) $(subst _shipped,,$(wildcard $(LIB_DIR)*.o_shipped)))
lib_objs := $(patsubst %.c, %.o, $(lib_src))
lib_cmd := $(addprefix $(call rpath, $(LIB_DIR))., $(addsuffix .cmd, $(subst _shipped,, $(notdir $(wildcard $(LIB_DIR)*.o_shipped)))))
lib_inc := $(addprefix -I, $(LIB_DIR)include)

common-include-dir := $(BASE_DIR)include
common-include := $(addprefix -I, $(common-include-dir))
common-include += $(utils_inc) $(lib_inc)

$(MODULE_NAME)-objs += $(basic-objs)
$(MODULE_NAME)-objs += $(utils_objs)
$(MODULE_NAME)-objs += $(board_objs)
$(MODULE_NAME)-objs += $(lib_objs)

#$(call add_cflags, $(MODULE_NAME)-objs, common-include)  -fno-common

EXTRA_CFLAGS += $(common-include)  -Wno-maybe-uninitialized
EXTRA_LDFLAGS += --whole-archive
obj-m += $(MODULE_NAME).o

build:
	$(info test1 $(lib_cmd))
	$(info test $(call rpath, $(LIB_DIR)))
	$(shell touch $(lib_cmd))
	make -C $(KDIR) M=$(BASE_DIR) modules

all: buildlib build
	make -C $(KDIR) M=$(BASE_DIR) modules

clean:
	$(shell rm $(lib_cmd))
	$(MAKE) -C $(KDIR) M=$(BASE_DIR) clean

.PHONY: all clean buildlib build

