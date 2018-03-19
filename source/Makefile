GOOGLETEST_INCLUDE:=$(ROOT)/googletest/googletest/include
GOOGLETEST_LIB:=$(ROOT)/googletest/build/googlemock/gtest
INCLUDE:=include

OBJ:=obj
SRC:=src

TEST_BIN:=test/bin
TEST_OBJ:=test/obj
TEST_SRC:=test/src

CFLAGS:=-std=c99
CXXFLAGS:=-std=c++11
CPPFLAGS:=-g -O2 -Wall -I$(INCLUDE)

OBJS:=$(patsubst $(SRC)/%.c, $(OBJ)/%.o, $(wildcard $(SRC)/*.c))
TESTS:=$(patsubst $(TEST_SRC)/%.cc, $(TEST_BIN)/%, $(wildcard $(TEST_SRC)/test_*.cc))


test: $(TESTS)

$(TEST_BIN)/%: $(TEST_OBJ)/%.o $(OBJS)
	g++ -o $@ $^ $(CXXFLAGS) $(CPPFLAGS) -L$(GOOGLETEST_LIB) -lgtest -lgtest_main -pthread

.PRECIOUS: $(TEST_OBJ)/%.o
$(TEST_OBJ)/%.o: $(TEST_SRC)/%.cc
	g++ -c -o $@ $< $(CXXFLAGS) $(CPPFLAGS) -I$(GOOGLETEST_INCLUDE)

.PRECIOUS: $(OBJ)/%.o
$(OBJ)/%.o: $(SRC)/%.c
	gcc -c -o $@ $< $(CFLAGS) $(CPPFLAGS)

.PHONY: clean
clean:
	rm -f $(OBJ)/*
	rm -f $(TEST_BIN)/*
	rm -f $(TEST_OBJ)/*
