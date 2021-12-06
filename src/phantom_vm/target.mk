TARGET = vmonly
LIBS   += libc posix cxx vfs

# LIBS += ld-sel4

export PHANTOM_BUILD_DIR = $(BUILD_BASE_DIR)/app/phantom

export PHANTOM_HOME=$(call select_from_ports,phantom)/src/app/phantom

# include $(call select_from_ports,phantom)/src/app/phantom/Makefile

# Prerequisites

# phantom/gl
include $(REP_DIR)/src/app/phantom/phantom_gl.inc
# phantom/libtuned
# include $(REP_DIR)/src/app/phantom/phantom_libtuned.inc
# phantom/libphantom
include $(REP_DIR)/src/app/phantom/phantom_libphantom.inc
# phantom/libwin
include $(REP_DIR)/src/app/phantom/phantom_libwin.inc
# phantom/libfreetype
include $(REP_DIR)/src/app/phantom/phantom_libfreetype.inc
# phantom/libc
# include $(REP_DIR)/src/app/phantom/phantom_libc.inc

# PVM

SRC_PVM = phantom/vm/pvm_main.c \
					phantom/libc/strnstrn.c \
					phantom/vm/exec.c \
					phantom/vm/syscall_io.c \
					phantom/vm/backtrace.c \
					phantom/vm/syscall_sync.c \
					phantom/vm/refdec.c \
					phantom/vm/gen_machindep.c \
					phantom/vm/syscall.c \
					phantom/vm/json_parse.c \
					phantom/vm/gdb.c \
					phantom/vm/vm_sleep.c \
					phantom/vm/e4c.c \
					phantom/vm/stacks.c \
					phantom/vm/wpool.c \
					phantom/vm/code.c \
					phantom/vm/root.c \
					phantom/vm/syscall_net_kernel.c \
					phantom/vm/vtest.c \
					phantom/vm/syscall_win.c \
					phantom/vm/create.c \
					phantom/vm/load_class.c \
					phantom/vm/ftype.c \
					phantom/vm/video_test.c \
					phantom/vm/syscall_tty.c \
					phantom/vm/object.c \
					phantom/vm/alloc.c \
					phantom/vm/syscall_net_windows.c \
					phantom/vm/find_class_file.c \
					phantom/vm/vm_threads.c \
					phantom/vm/jit.c \
					phantom/vm/internal.c \
					phantom/vm/bulk.c \
					phantom/vm/gc.c \
					phantom/vm/wpaint.c \
					phantom/vm/syscall_net.c \
					phantom/vm/directory.c \
					phantom/vm/sys/i_stat.c \
					phantom/vm/sys/i_udp.c \
					phantom/vm/sys/i_time.c \
					phantom/vm/sys/i_ui_control.c \
					phantom/vm/sys/i_io.c \
					phantom/vm/sys/i_ui_font.c \
					phantom/vm/sys/i_port.c \
					phantom/vm/sys/i_http.c \
					phantom/vm/sys/i_net.c \
					phantom/vm/win_bulk.c \
					phantom/vm/headless_screen.c \
					phantom/vm/unix_hal.c

SRC_C += $(SRC_PVM)

# Actual pvm_main files
# HLS_TEST_OBJFILES=pvm_main.o nonstandalone.o win_bulk.o headless_screen.o unix_hal.o unix_hal_unix.o 
# pvm_headless: pvm_main.o nonstandalone.o $(GLLIB) libphantom_vm.a  $(HLS_TEST_OBJFILES)

PVM_MAIN = phantom/vm/pvm_main.c \
				   phantom/vm/win_bulk.c \
				   phantom/vm/headless_screen.c \
				   phantom/vm/unix_hal.c \
				   phantom/vm/unix_hal_unix.c \
				   phantom/vm/nonstandalone.c


SRC_C += $(PVM_MAIN)

vpath %.c $(PHANTOM_HOME)

# From local-config.mk

export PHANTOM_HOME=$(call select_from_ports,phantom)/src/app/phantom
export ARCH = genode-amd64
export BOARD = amd64-pc
export PHANTOM_GENODE = 1

# libc includes. Have to be before Phantom includes
# XXX : Not really a good solution. Probably should fix

LIBC_DIR = $(call select_from_ports,libc)

INC_DIR += $(LIBC_DIR)/include/libc \
					 $(LIBC_DIR)/include/spec/x86_64/libc \
					 $(LIBC_DIR)/include/spec/x86/libc \
					 $(LIBC_DIR)/include/libc \
					 $(LIBC_DIR)/include/spec/x86_64/libc \
					 $(LIBC_DIR)/include/spec/x86/libc 


# Includes from Phantom build system


INC_DIR += $(PRG_DIR)
# TODO : check realpath
INC_DIR += $(realpath $(PHANTOM_HOME))/include $(realpath $(PHANTOM_HOME))/include/$(ARCH) \
	$(PHANTOM_HOME)/include/$(ARCH) $(PHANTOM_HOME)/include 

# Phantom arch includes
INC_DIR += $(realpath $(PHANTOM_HOME))/include $(realpath $(PHANTOM_HOME))/include/$(ARCH) \
	$(PHANTOM_HOME)/include/$(ARCH) $(PHANTOM_HOME)/include 

# GL
INC_DIR += $(PHANTOM_HOME)/phantom/gl

# Debug

CC_C_OPT += -g
# CC_C_OPT += -DDEBUG=1


# Flags and macros from Phantom build system

CC_C_OPT += -std=gnu89
CC_C_OPT += -DKERNEL
CC_C_OPT += -DPHANTOM_GENODE
CC_C_OPT += -DNO_NETWORK

# Workaround
# phantom/libfreetype/afangles.c
# CC_OPT += -D_JBLEN=32
# phantom/libphantom/unix/fname.c
# CC_OPT += -DARCH_N_TRAPS=32
# CC_OPT += -DARCH_amd64=
# phantom/libwin/ctl_text_field.c:210:36:
# CC_OPT += -DCONF_TRUETYPE=0
CC_C_OPT += -include $(PHANTOM_HOME)/include/kernel/config.h

# COMMON_FLAGS= $(WARN) $(ARCH_FLAGS) -O0 -g -MD $(DEFINES) $(addprefix -I,$(INCDIRS)) -nostdinc -std=gnu89 -DKERNEL -DARCH_$(ARCH)=1 -DBOARD_$(BOARD)=1 -DBOARD=$(BOARD) \
#   -include $(realpath $(PHANTOM_HOME))/include/kernel/config.h \
#   -include $(realpath $(PHANTOM_HOME))/include/$(ARCH)/arch/arch-config.h \
#   -include $(realpath $(PHANTOM_HOME))/include/$(ARCH)/arch/board-$(BOARD)-config.h 
