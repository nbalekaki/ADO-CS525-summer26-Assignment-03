# Advanced Database Organization - Assignment 03
# Student Makefile

CC     = gcc
CFLAGS = -Wall -g

# Source files
OPS_SRC = operators.c dberror.c

# Test files
TEST_BASIC_SRC = test_operators_basic.c

.PHONY: all clean test test-operators

# Default target
all: test_operators_basic

# Build basic operator tests
test_operators_basic: $(TEST_BASIC_SRC) $(OPS_SRC)
	$(CC) $(CFLAGS) -o $@ $^

# Run all tests
test: test-operators

test-operators: test_operators_basic
	@echo "========================================"
	@echo "Running Operator Engine Basic Tests"
	@echo "========================================"
	./test_operators_basic

# Memory check with Valgrind
memcheck: test_operators_basic
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes ./test_operators_basic

# Clean build artifacts and test files
clean:
	rm -f test_operators_basic
	rm -f *.o
	rm -f *.bin
