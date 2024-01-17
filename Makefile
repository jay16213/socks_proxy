CXX=g++
CXXFLAGS=-std=c++11 -Wall
CXX_INCLUDE_DIRS=./include
CXX_INCLUDE_PARAMS=$(addprefix -I , $(CXX_INCLUDE_DIRS))
CXX_LIB_DIRS=/usr/local/lib
CXX_LIB_PARAMS=$(addprefix -L , $(CXX_LIB_DIRS))
SOCKS_SERVER_OBJS=main.o socks_server.o socks_handler.o util.o firewall.o
SOCKS_SERVER=socks_server

all: executable

debug: CXXFLAGS += -DDEBUG -g3 -fno-inline -O0
debug: executable

executable: $(SOCKS_SERVER)

$(SOCKS_SERVER): $(SOCKS_SERVER_OBJS)
	$(CXX) $^ -o $@ $(CXXFLAGS)

%.o: %.cpp
	$(CXX) -c $< -o $@ $(CXX_INCLUDE_PARAMS) $(CXX_LIB_PARAMS) $(CXXFLAGS)

.PHONY: clean
clean:
	rm -rf $(SOCKS_SERVER) $(SOCKS_SERVER_OBJS)
