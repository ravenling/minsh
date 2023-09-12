##################
 # MinSH Makefile #
  ##################

# shell choice
SHELL:=bash

# directories
SRC_DIR=./src
BUILD_DIR=./build
OBJ_DIR=$(BUILD_DIR)/obj

# MinSH exe name
EXENAME=minsh

# g++ configs
CC=g++
CFLAGS=-Wall -Werror -Isrc -g 
INC_PATH?=
SRCS=$(shell find $(abspath ./src) -name "*.c" -or -name "*.cpp")
OBJS=$(addprefix $(OBJ_DIR)/, $(notdir $(SRCS:.cpp=.o)))

# target
BIN=$(BUILD_DIR)/$(EXENAME)

# make build dir
$(shell mkdir -p $(BUILD_DIR))
$(shell mkdir -p $(OBJ_DIR))

# default goal
default: $(BIN)

$(BIN): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $@

# components
$(OBJ_DIR)/main.o: $(SRC_DIR)/main.cpp
	$(CC) -c $(CFLAGS) $< -o $@

$(OBJ_DIR)/dbg.o: $(SRC_DIR)/debug/dbg.cpp
	$(CC) -c $(CFLAGS) $< -o $@

$(OBJ_DIR)/init.o: $(SRC_DIR)/init/init.cpp
	$(CC) -c $(CFLAGS) $< -o $@

$(OBJ_DIR)/minsh.o: $(SRC_DIR)/minsh/minsh.cpp
	$(CC) -c $(CFLAGS) $< -o $@

$(OBJ_DIR)/parser.o: $(SRC_DIR)/parser/parser.cpp
	$(CC) -c $(CFLAGS) $< -o $@

# run
run: $(BIN)
	$(BIN)

clean:
	rm -r $(BUILD_DIR)

.PHONY: default all clean run