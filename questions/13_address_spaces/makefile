# Compiler and flags — change here if using gcc or different flags
CC = gcc
CFLAGS = -Wall -g
EXT = c

# Output binary names
TARGETS = main main2 main3 test test1 test2 test3 test4 test5

.PHONY: all clean tests $(TARGETS)

# Build everything
all: $(TARGETS)

# Build all test programs with `make tests`
tests: test test1 test2 test3 test4 test5

# Individual test targets
test: test.$(EXT)
	$(CC) $(CFLAGS) $< -o $@

test1: test1.$(EXT)
	$(CC) $(CFLAGS) $< -o $@

test2: test2.$(EXT)
	$(CC) $(CFLAGS) $< -o $@

test3: test3.$(EXT)
	$(CC) $(CFLAGS) $< -o $@

test4: test4.$(EXT)
	$(CC) $(CFLAGS) $< -o $@

test5: test5.$(EXT)
	$(CC) $(CFLAGS) $< -o $@

# Main program targets
main: main.$(EXT)
	$(CC) $(CFLAGS) $< -o $@

main2: main2.$(EXT)
	$(CC) $(CFLAGS) $< -o $@

main3: main3.$(EXT)
	$(CC) $(CFLAGS) $< -o $@

# Clean rule
clean:
	rm -f *.o $(TARGETS)
