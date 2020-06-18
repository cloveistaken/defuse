# System compiler
CC = /usr/bin/gcc
CFLAGS = -Wall -Wextra -Werror

# Source files
C_SOURCES = $(wildcard src/*.c)
HEADERS = $(wildcard include/*.h)
TARGET = bin/defuse

# Release settings
RL_BIN = bin/defuse-release
RL_OBJ = $(C_SOURCES:src/%.c=build/%-release.o)
RL_FLAGS = -O3

# Debug settings
DB_BIN = bin/defuse-debug
DB_OBJ = $(C_SOURCES:src/%.c=build/%-debug.o)
DB_FLAGS = -g -O0 -DVERBOSE


# Start here
.PHONY = all clean debug release remake
.DEFAULT_GOAL = all

all: release debug

release: $(RL_BIN)
	strip -S --strip-unneeded $(RL_BIN)
	cp $(RL_BIN) $(TARGET)

debug: $(DB_BIN)

# Release
$(RL_BIN): $(RL_OBJ)
	$(CC) $(CFLAGS) $(RL_FLAGS) -o $@ $^

build/%-release.o: src/%.c $(HEADERS)
	$(CC) $(CFLAGS) $(RL_FLAGS) -o $@ -c $<

# Debug
$(DB_BIN): $(DB_OBJ)
	$(CC) $(CFLAGS) $(DB_FLAGS) -o $@ $^

build/%-debug.o: src/%.c $(HEADERS)
	$(CC) $(CFLAGS) $(DB_FLAGS) -o $@ -c $<

remake: clean all

clean:
	rm -f bin/* build/* tmp/* answer.txt
