
ifdef KDIR
KERNEL_VER:=$(shell cat $(KDIR)/include/config/kernel.release 2> /dev/null)
else
KERNEL_VER:= $(shell uname -r)
endif
KDIR ?= /lib/modules/$(shell uname -r)/build
BOARD_BASE_DIR := $(BASE_DIR)board

NULL:=
SPACE:=$(NULL) $(NULL)
COMMA:=,


#return dir if dir exits
define check_dir
$(shell test -d $(1) && echo $(1))
endef 


define basic_check
ifeq ($(call check_dir,$(BOARD_BASE_DIR)/$(BOARD)),$(NULL))
$(info no board $(BOARD) exists)
endif
endef

define list_files_from_file 
$(addprefix $(dir $(1)),$(shell cat $(1)))
endef


define dirs 
$(subst $(dir $(1)/),,$(sort $(dir $(wildcard $(1)/*/))))
endef

define rdirs 
$(subst $(dir $(1)/),,$(sort $(dir $(wildcard $(1)/*/  $(1)/*/*/))))
endef

define rpath
$(subst $(BASE_DIR),,$(1))
endef

define add_cflags
$(foreach obj,$($(1)),$(eval CFLAGS_$(notdir $(obj)) += $($(2))))
endef

define get_depend_modules
$(subst $(COMMA),$(SPACE),$(strip $(subst depends:,,$(shell /sbin/modinfo $1 | grep depends))))
endef

# gen_insmod_script 
# param 1:script_name 
# param 2:varible name of module 
# param 3:outputdir
define gen_insmod_script
always +=$(3)$(1)

$(1)_script_header:
	@echo "#!/bin/sh" > $(obj)/$(3)$(1)

$(1)_modprobe:
	@$(foreach module,$(filter-out  $(foreach mod,$($(2)),$(basename $(notdir $(mod)))),$(sort $(foreach module,$($(2)),$(call get_depend_modules,$(module))))),echo modprobe $(module) >> $(obj)/$(3)$(1);)

$(1)_insmod_%:
	@echo insmod $$(subst $(1)_insmod_,,$$@).ko >> $(obj)/$(3)$(1)

$(foreach mod,$($(2)),$(1)_insmod_$(basename $(notdir $(mod))):$(addprefix $(1)_insmod_,$(filter $(foreach mod,$($(2)),$(basename $(notdir $(mod)))),$(call get_depend_modules,$(mod))))${\n})


$(obj)/$(3)$(1): $(1)_script_header $(1)_modprobe $(foreach mod,$($(2)),$(1)_insmod_$(basename $(notdir $(mod))))
	@chmod +x $(obj)/$(3)$(1)
	@echo $(obj)/$(3)$(1) done
		
endef

# gen_install_script
# param 1:script_name 
# param 2:varible name of module 
# param 3:outputdir
# param 4:module install dir
define gen_install_script
always +=$(3)$(1)
$(obj)/$(3)$(1):
	$(shell echo "#!/bin/sh" > $(obj)/$(3)$(1))
	$(shell echo "#TARGET_KERNEL_VER=$(KERNEL_VER)" >>$(obj)/$(3)$(1))
	$(shell echo '#KERNEL_VER=`uname -r`'>>$(obj)/$(3)$(1))
	$(shell echo '#if [ $$KERNEL_VER!=$$TARGET_KERNEL_VER ]'>>$(obj)/$(3)$(1))
	$(shell echo "#then" >>$(obj)/$(3)$(1))
	$(shell echo '#echo "current kernel version is not match $$(TARGET_KERNEL_VER).' >>$(obj)/$(3)$(1))
	$(shell echo '#echo "Please boot to correct kernel version then install again. Thanks"' >>$(obj)/$(3)$(1))
	$(shell echo "#exit">>$(obj)/$(3)$(1))
	$(shell echo "#fi" >>$(obj)/$(3)$(1))
	$(shell echo MODULE_INSTALL_DIR=$(4) >>$(obj)/$(3)$(1))
	$(shell echo 'install -d  $$MODULE_INSTALL_DIR'>> $(obj)/$(3)$(1))\
	$(foreach mod, $($(2)), \
		$(shell echo 'install -m 644 $(notdir $(mod)) $$MODULE_INSTALL_DIR' >> $(obj)/$(3)$(1))\
		)
	$(shell echo "/sbin/depmod -a" >> $(obj)/$(3)$(1) )
	$(shell chmod +x $(obj)/$(3)$(1))

endef

# gen_build_script
# param 1:script_name 
# param 2:varible name of module 
# param 3:outputdir
define gen_build_script
always +=$(3)$(1)
$(obj)/$(3)$(1):
	$(shell echo "#!/bin/sh" > $(obj)/$(3)$(1))
	$(shell echo "make -C driver clean" >> $(obj)/$(3)$(1))
	$(shell echo "make -C driver" >> $(obj)/$(3)$(1))
	$(foreach mod, $($(2)), $(shell echo cp driver/$(notdir $(mod)) ./ >> $(obj)/$(3)$(1)))
	$(shell chmod +x $(obj)/$(3)$(1))

endef

