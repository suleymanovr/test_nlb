INC_DIR   := ../include

CLIENT_TARGET_EXEC := tclient
SERVER_TARGET_EXEC := tserver

CLIENT_SRCS :=  ./test_client.cpp ../src/inet_interface.cpp
SERVER_SRCS :=  ./test_server.cpp ../src/inet_interface.cpp

INC_FLAGS := $(addprefix -I, $(INC_DIR))

CPPFLAGS ?= $(INC_FLAGS) -MMD -MP

$(CLIENT_TARGET_EXEC):
	g++ $(CPPFLAGS) $(CLIENT_SRCS) -o $(CLIENT_TARGET_EXEC)

$(SERVER_TARGET_EXEC):
	g++ $(CPPFLAGS) $(SERVER_SRCS) -o $(SERVER_TARGET_EXEC)

.PHONY: clean

clean:
	$(RM) tclient* tserver*

