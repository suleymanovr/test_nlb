INC_DIR   := ../../include

SERVER_TARGET_EXEC := tserver

SERVER_SRCS :=  ./test_server.cpp ../../src/inet_interface.cpp

INC_FLAGS := $(addprefix -I, $(INC_DIR))

CPPFLAGS ?= $(INC_FLAGS) -MMD -MP

target:
	g++ $(CPPFLAGS) $(SERVER_SRCS) -o $(SERVER_TARGET_EXEC)

.PHONY: clean

clean:
	$(RM) tserver*

