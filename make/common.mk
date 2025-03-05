CC ?= gcc

INCLUDE_DIR ?= ./include
SRC_DIR ?= ./src
BUILD_DIR ?= ./build
DIST_DIR ?= $(BUILD_DIR)/dist
LIB_DIR ?= ./lib

CFLAGS += -I$(INCLUDE_DIR)
LDFLAGS += -L$(LIB_DIR)

ifeq ($(DEBUG),true)
CFLAGS += -ggdb3
endif
