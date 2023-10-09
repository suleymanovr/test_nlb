INC_DIR   := ../../include

CLIENT_TARGET_EXEC := tclient

CLIENT_SRCS :=  ./test_client.cpp ../../src/inet_interface.cpp

INC_FLAGS := $(addprefix -I, $(INC_DIR))

CPPFLAGS ?= $(INC_FLAGS) -MMD -MP

$(CLIENT_TARGET_EXEC):
	g++ $(CPPFLAGS) $(CLIENT_SRCS) -o $(CLIENT_TARGET_EXEC)


.PHONY: clean

clean:
	$(RM) tclient* 

