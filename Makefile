
CFLAGS   :=
CXXFLAGS := 
CC := gcc
CCX := g++

SRC_DIR   := ./src
INC_DIR   := ./include
BUILD_DIR := ./build

TARGET_EXEC := test_nlb

SRCS := $(shell find $(SRC_DIR) -name '*.cpp' -or -name '*.c' -or -name '*.cc')
OBJS := $(SRCS:%=$(BUILD_DIR)/%.o)

INC_FLAGS := $(addprefix -I, $(INC_DIR))

CPPFLAGS ?= $(INC_FLAGS) -g -MMD -MP

$(BUILD_DIR)/$(TARGET_EXEC): $(OBJS)
	g++ -o $@ $(OBJS)

$(BUILD_DIR)/%.cc.o: %.cc
	$(MKDIR_P) $(dir $@)
	g++ $(INC_FLAGS) -c $< -o $@

$(BUILD_DIR)/%.c.o: %.c
	$(MKDIR_P) $(dir $@)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/%.cpp.o: %.cpp
	$(MKDIR_P) $(dir $@)
	$(CCX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@


.PHONY: clean

clean:
	$(RM) -r $(BUILD_DIR)

MKDIR_P ?= mkdir -p
