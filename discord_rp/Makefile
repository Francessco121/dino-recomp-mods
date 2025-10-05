BUILD_DIR := build
MOD_TOML := ./mod.toml
EXTLIB_PREFIX := lib

ifeq ($(OS),Windows_NT)
PYTHON_EXEC ?= python
else
PYTHON_EXEC ?= python3
endif
PYTHON_FUNC_MODULE := make_python_functions

define get_python_func_no_build_info
$(shell $(PYTHON_EXEC) -c "import $(PYTHON_FUNC_MODULE); $(PYTHON_FUNC_MODULE).ModInfo(\"$(MOD_TOML)\", \"$(BUILD_DIR)\").$(1)($(2))")
endef

EXTLIB_NAME := $(call get_python_func_no_build_info,get_extlib_name,)

# Extlib Building Info:
EXTLIB_CMAKE_PRESET_GROUP := Debug
ZIG_WINDOWS_CONFIGURE_PRESET := $(call get_python_func_no_build_info,get_extlib_windows_configure_preset,\"$(EXTLIB_CMAKE_PRESET_GROUP)\",)
ZIG_MACOS_CONFIGURE_PRESET := $(call get_python_func_no_build_info,get_extlib_macos_configure_preset,\"$(EXTLIB_CMAKE_PRESET_GROUP)\",)
ZIG_LINUX_CONFIGURE_PRESET := $(call get_python_func_no_build_info,get_extlib_linux_configure_preset,\"$(EXTLIB_CMAKE_PRESET_GROUP)\",)
NATIVE_CMAKE_CONFIGURE_PRESET ?= $(call get_python_func_no_build_info,get_extlib_native_configure_preset,\"$(EXTLIB_CMAKE_PRESET_GROUP)\",)

ZIG_WINDOWS_BUILD_PRESET := $(call get_python_func_no_build_info,get_extlib_windows_build_preset,\"$(EXTLIB_CMAKE_PRESET_GROUP)\",)
ZIG_MACOS_BUILD_PRESET := $(call get_python_func_no_build_info,get_extlib_macos_build_preset,\"$(EXTLIB_CMAKE_PRESET_GROUP)\",)
ZIG_LINUX_BUILD_PRESET := $(call get_python_func_no_build_info,get_extlib_linux_build_preset,\"$(EXTLIB_CMAKE_PRESET_GROUP)\",)
NATIVE_CMAKE_BUILD_PRESET ?= $(call get_python_func_no_build_info,get_extlib_native_build_preset,\"$(EXTLIB_CMAKE_PRESET_GROUP)\",)

ifeq ($(OS),Windows_NT)
MOD_TOOL_ZIG_TRIPLET ?= x86_64-windows
NATIVE_SUBDIR := bin
NATIVE_EXTENSION := dll
else ifneq ($(shell uname),Darwin)
MOD_TOOL_ZIG_TRIPLET ?= x86_64-linux
NATIVE_SUBDIR := lib
NATIVE_EXTENSION := so
else
MOD_TOOL_ZIG_TRIPLET ?= aarch64-macos
NATIVE_SUBDIR := lib
NATIVE_EXTENSION := dylib
endif

# MOD_TOOL_ZIG_TRIPLET ?= $(MOD_TOOL_ZIG_TRIPLET)

define extlib_build_file
$(BUILD_DIR)/$(1)/$(2)/$(EXTLIB_PREFIX)$(EXTLIB_NAME).$(3)
endef

define native_extlib_build_file
$(BUILD_DIR)/$(1)/$(2)/$(EXTLIB_NAME).$(3)
endef

EXTLIB_BUILD_WIN := $(call extlib_build_file,$(ZIG_WINDOWS_CONFIGURE_PRESET),bin,dll)
EXTLIB_BUILD_MACOS := $(call extlib_build_file,$(ZIG_MACOS_CONFIGURE_PRESET),lib,dylib)
EXTLIB_BUILD_LINUX := $(call extlib_build_file,$(ZIG_LINUX_CONFIGURE_PRESET),lib,so)
EXTLIB_BUILD_NATIVE := $(call native_extlib_build_file,$(NATIVE_CMAKE_CONFIGURE_PRESET),$(NATIVE_SUBDIR),$(NATIVE_EXTENSION))

# Python Build Info:
define call_python_func
	$(PYTHON_EXEC) -c "import $(PYTHON_FUNC_MODULE); $(PYTHON_FUNC_MODULE).ModInfo(\"$(MOD_TOML)\", \"$(BUILD_DIR)\").set_extlib_info(\"$(EXTLIB_BUILD_WIN)\", \"$(EXTLIB_BUILD_MACOS)\", \"$(EXTLIB_BUILD_LINUX)\", \"$(EXTLIB_BUILD_NATIVE)\").$(1)($(2))"
endef

define get_python_func
$(shell $(PYTHON_EXEC) -c "import $(PYTHON_FUNC_MODULE); $(PYTHON_FUNC_MODULE).ModInfo(\"$(MOD_TOML)\", \"$(BUILD_DIR)\").set_extlib_info(\"$(EXTLIB_BUILD_WIN)\", \"$(EXTLIB_BUILD_MACOS)\", \"$(EXTLIB_BUILD_LINUX)\", \"$(EXTLIB_BUILD_NATIVE)\").$(1)($(2))")
endef

define get_python_val
$(shell $(PYTHON_EXEC) -c "import $(PYTHON_FUNC_MODULE); print($(PYTHON_FUNC_MODULE).ModInfo(\"$(MOD_TOML)\", \"$(BUILD_DIR)\").set_extlib_info(\"$(EXTLIB_BUILD_WIN)\", \"$(EXTLIB_BUILD_MACOS)\", \"$(EXTLIB_BUILD_LINUX)\", \"$(EXTLIB_BUILD_NATIVE)\").$(1))")
endef

# Get the mod code compilers from a config.
CC      := $(call get_python_func,get_mod_compiler,)
LD      := $(call get_python_func,get_mod_linker,)

RECOMP_MOD_TOOL := ../RecompModTool

# Mod Building Info:

MOD_FILE := $(call get_python_func,get_mod_file,)
$(info MOD_FILE = $(MOD_FILE))
MOD_ELF  := $(call get_python_func,get_mod_elf,)
$(info MOD_ELF = $(MOD_ELF))

LDSCRIPT := ../mod.ld
CFLAGS   := -target mips -mips2 -mabi=32 -O2 -G0 -mno-abicalls -mno-odd-spreg -mno-check-zero-division \
			-fomit-frame-pointer -ffast-math -fno-unsafe-math-optimizations -fno-builtin \
			-Wall -Wextra -Wno-incompatible-library-redeclaration -Wno-unused-parameter -Wno-unknown-pragmas -Wno-unused-variable \
			-Wno-missing-braces -Wno-unsupported-floating-point-opt -Werror=section
CPPFLAGS := -nostdinc -D_LANGUAGE_C -DMIPS -D_MIPS_SZLONG=32 -DF3DEX_GBI_2 \
			-I ../lib/dino-recomp-mod-api/include -I ../lib/dino-recomp-decomp-bridge/include -I ../lib/dino-recomp-decomp-bridge/dinosaur-planet/include \
			-I include
LDFLAGS  := -nostdlib -T $(LDSCRIPT) -Map $(BUILD_DIR)/mod.map --unresolved-symbols=ignore-all --emit-relocs -e 0 --no-nmagic

rwildcard = $(foreach d,$(wildcard $(1:=/*)),$(call rwildcard,$d,$2) $(filter $(subst *,%,$2),$d))
getdirs = $(sort $(dir $(1)))

C_SRCS := $(call rwildcard,src/mod,*.c)
C_OBJS := $(addprefix $(BUILD_DIR)/, $(C_SRCS:.c=.o))
C_DEPS := $(addprefix $(BUILD_DIR)/, $(C_SRCS:.c=.d))

ALL_OBJS := $(C_OBJS)
ALL_DEPS := $(C_DEPS)
BUILD_DIRS := $(call getdirs,$(ALL_OBJS))

# General Recipes:
# If no extlib is to be built, then don't include it in recipe 'all'
ifeq ($(EXTLIB_NAME),None)
all: nrm runtime
else
all: nrm extlib-all runtime
endif

create_user_build_config:
	$(call call_python_func,create_user_build_config,)

native: nrm extlib-native runtime_native

windows: nrm extlib-win runtime

macos: nrm extlib-macos runtime

linux: nrm extlib-linux runtime

runtime:
	$(call call_python_func,copy_to_runtime_dir,)

runtime_native:
	$(call call_python_func,copy_to_runtime_dir_native,)

# Mod Recipes:
nrm: $(MOD_FILE)

$(MOD_FILE): $(RECOMP_MOD_TOOL) $(MOD_ELF) $(MOD_TOML)
	$(RECOMP_MOD_TOOL) $(MOD_TOML) $(BUILD_DIR)

elf: $(MOD_ELF) 

$(MOD_ELF): $(C_OBJS) $(LDSCRIPT) | $(BUILD_DIR)
	$(LD) $(C_OBJS) $(LDFLAGS) -o $@

$(BUILD_DIR) $(BUILD_DIR)/src $(BUILD_DIR)/src/mod:
ifeq ($(OS),Windows_NT)
	if not exist "$(subst /,\,$@)" mkdir "$(subst /,\,$@)"
else
	mkdir -p $@
endif

$(C_OBJS): $(BUILD_DIR)/%.o : %.c | $(BUILD_DIR) $(BUILD_DIR)/src $(BUILD_DIR)/src/mod
	$(CC) $(CFLAGS) $(CPPFLAGS) $< -MMD -MF $(@:.o=.d) -c -o $@

# Extlib Recipes:
extlib-all: extlib-win extlib-macos extlib-linux

extlib-win:
	cmake --preset=$(ZIG_WINDOWS_CONFIGURE_PRESET) -DLIB_NAME=$(EXTLIB_NAME) .
	cmake --build --preset=$(ZIG_WINDOWS_BUILD_PRESET)

extlib-macos:
	cmake --preset=$(ZIG_MACOS_CONFIGURE_PRESET) -DLIB_NAME=$(EXTLIB_NAME) .
	cmake --build --preset=$(ZIG_MACOS_BUILD_PRESET)

extlib-linux:
	cmake --preset=$(ZIG_LINUX_CONFIGURE_PRESET) -DLIB_NAME=$(EXTLIB_NAME) .
	cmake --build --preset=$(ZIG_LINUX_BUILD_PRESET)

extlib-native:
	cmake --preset=$(NATIVE_CMAKE_CONFIGURE_PRESET) -DLIB_NAME=$(EXTLIB_NAME) .
	cmake --build --preset=$(NATIVE_CMAKE_BUILD_PRESET)

clean:
ifeq ($(OS),Windows_NT)
	- rmdir "$(BUILD_DIR)" /s /q
else
	- rm -rf $(BUILD_DIR)
endif

-include $(ALL_DEPS)

.PHONY: all native windows macos linux runtime nrm extlib-all extlib-win extlib-macos extlib-linux extlib-native clean

# Print target for debugging
print-% : ; $(info $* is a $(flavor $*) variable set to [$($*)]) @true
