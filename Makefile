C_SOURCES = $(wildcard src/*.c)
HEADERS = $(wildcard include/*.h)
OBJ = $(C_SOURCES:src/%.c=%.o)

CC = /usr/bin/gcc
CFLAGS = -Wall -Wextra -Werror

defuse: $(OBJ)
	$(CC) $(CFLAGS) -o bin/$@ $^

%.o: src/%.c $(HEADERS)
	$(CC) $(CFLAGS) -o $@ -c $<

.PHONY = clean

clean:
	rm -f bin/* *.o
