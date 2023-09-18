##################
 # MinSH Makefile #
  ##################

# shell choice
SHELL:=bash

# directories
PWD=$(shell pwd)
SRC_DIR=./src
BUILD_DIR=./build
OBJ_DIR=$(BUILD_DIR)/obj

# MinSH exe name
EXENAME=minsh

# g++ configs
CC=g++
CFLAGS=-Wall -Werror -Isrc -g 
INC_PATH?=
SRCS=$(shell find $(abspath ./src) -name "*.cpp")
OBJS=$(addprefix $(OBJ_DIR)/, $(notdir $(SRCS:%.cpp=%.o))) $(OBJ_DIR)/prs.tab.o

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

$(OBJ_DIR)/parser.o: $(SRC_DIR)/parser/parser.cpp
	$(CC) -c $(CFLAGS) $^ -o $@

$(OBJ_DIR)/lex.o: $(SRC_DIR)/parser/lex.cpp
	$(CC) -c $(CFLAGS) $^ -o $@

$(OBJ_DIR)/prs.tab.o: $(SRC_DIR)/parser/prs.tab.cc
	$(CC) -c $(CFLAGS) $^ -o $@

$(SRC_DIR)/parser/prs.tab.cc: $(SRC_DIR)/parser/prs.y
	cd $(SRC_DIR)/parser/ && bison -dv -Wcounterexamples prs.y

$(OBJ_DIR)/minsh.o: $(SRC_DIR)/minsh/minsh.cpp
	$(CC) -c $(CFLAGS) $< -o $@

$(OBJ_DIR)/cmdexec.o: $(SRC_DIR)/cmdexec/cmdexec.cpp
	$(CC) -c $(CFLAGS) $< -o $@

$(OBJ_DIR)/builtin.o: $(SRC_DIR)/builtin/builtin.cpp
	$(CC) -c $(CFLAGS) $< -o $@

# run
run: $(BIN)
	$(BIN)

clean:
	rm -r $(BUILD_DIR)

.PHONY: default all clean run