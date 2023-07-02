##
# FTL
#
# @file
# @version 0.1

CXX         := clang++
RM          := rm -f
RMDIR       := rm -rf
MKDIR       := mkdir
DEBUGGER    := lldb

INCLUDE     := include

TESTDIR     := test
TESTSRC     := $(TESTDIR)/src
TESTOBJ     := $(TESTDIR)/obj
TESTBIN     := $(TESTDIR)/bin

TESTSRCS    := $(shell find $(TESTSRC) -name "*.cpp")
TESTOBJS    := $(patsubst $(TESTSRC)/%.cpp, $(TESTOBJ)/%.o, $(TESTSRCS))
TESTS       := $(patsubst $(TESTOBJ)/%.o, $(TESTBIN)/%, $(TESTOBJS))

LDFLAGS     :=
CFLAGS      := -I$(INCLUDE) -std=c++17 -Wall -Wextra
DEBUGFLAGS  := -O0 -ggdb

define execute
$(1)

endef

.PHONY: all clean debug lldb test

all: $(TESTS)

# TODO: implement proper execution
test: all
	$(foreach x, $(TESTS), $(call execute, ./$(x)))

debug: CFLAGS := $(CFLAGS) $(DEBUGFLAGS)
debug: all

lldb: debug
	lldb $(TARGET)

$(TESTBIN)/%: $(TESTOBJ)/%.o | $(TESTBIN)
	$(CXX) $(CFLAGS) $^ -o $@ $(LDFLAGS)

$(TESTOBJ)/%.o: $(TESTSRC)/%.cpp $(TESTOBJ)
	$(CXX) $(CFLAGS) -c $< -o $@

$(TESTOBJ) $(TESTBIN):
	$(MKDIR) $@

clean:
	$(RMDIR) $(TESTDIR)/bin $(TESTDIR)/obj

# end
