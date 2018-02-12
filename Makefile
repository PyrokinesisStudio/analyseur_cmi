CC=clang++
CXXFLAGS=-W -Wall -std=c++11 -ggdb
LDFLAGS=
EXEC=analyze
DIST=build/
BIN=$(DIST)bin/

SRCS=$(wildcard *.cpp)
OBJS=${SRCS:.cpp=.o}
BIN_OBJS=$(addprefix $(DIST), $(OBJS))

all: $(EXEC)

analyze: $(OBJS)
	@mkdir -p $(BIN)
	$(CC) $(BIN_OBJS) -o $(BIN)$(EXEC) $(LDFLAGS)

%.o: %.cpp
	@mkdir -p $(DIST)
	$(CC) -o $(DIST)$@ -c $< $(CXXFLAGS)

clean:
	rm -rf $(DIST)*.o

mrproper: clean
	rm -rf $(BIN)$(EXEC)

