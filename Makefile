##################
 # MinSH Makefile #
  ##################

# MinSH exe name
EXENAME=minsh

# g++ configs
CC=g++
CFLAGS=-Wall -Werror -Isrc -g 
INC_PATH?=
SRCS=$(shell find $(abspath ./src) -name "*.c" -or -name "*.cpp")

# target
BUILD_DIR=./build
OBJ_DIR=$(BUILD_DIR)/obj
BIN=$(BUILD_DIR)/$(EXENAME)

# make build dir
$(shell mkdir -p $(BUILD_DIR))

# default goal
default: $(BIN)

$(BIN): $(SRCS)
	$(CC) $(CFLAGS) $(SRCS) -o $@

run: $(BIN)
	$(BIN)

clean:
	rm -r $(BUILD_DIR)

.PHONY: default all clean run