TARGET = phantom_env_test
SRC_CC = main.cc
INC_DIR += $(call select_from_ports,phantom)/src/app/phantom/include
LIBS   = base
