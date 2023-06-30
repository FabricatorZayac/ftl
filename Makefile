##
# Project Title
#
# @file
# @version 0.1

CC          := clang++
RMDIR       := rm -rf
RM          := rm -f
MKDIR       := mkdir

SRC         := ./src
OBJ         := ./obj
BIN         := ./bin
INCLUDE     := ./include

SRCS        := $(wildcard $(SRC)/*.cpp)
OBJS        := $(patsubst $(SRC)/%.cpp, $(OBJ)/%.o, $(SRCS))
TARGET      := $(BIN)/appname

LDFLAGS     :=
CFLAGS      := -I$(INCLUDE) -std=c++20 -Wall -Wextra
DEBUGFLAGS  := -O0 -ggdb

.PHONY: all clean run debug gdb

all: $(TARGET)

run: all
	./$(TARGET)

debug: CFLAGS := $(CFLAGS) $(DEBUGFLAGS)
debug: all

gdb: debug
	gdb $(TARGET)

$(TARGET): $(OBJS) | $(BIN)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

$(OBJ)/%.o: $(SRC)/%.cpp | $(OBJ)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ) $(BIN):
	$(MKDIR) $@

clean:
	$(RMDIR) $(BIN) $(OBJ)


# end