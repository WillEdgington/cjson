CC=gcc
CFLAGS=-Isrc -Wall -Wextra -pedantic -fsanitize=address,undefined -MMD -MP
LDFLAGS=-fsanitize=address,undefined

SRCS = $(wildcard src/*.c)
OBJS = $(SRCS:.c=.o)
DEPS = $(SRCS:.c=.d)

cjson: $(OBJS)
	$(CC) -o $@ $^ $(LDFLAGS)

src/%.o: src/%.c
	$(CC) -c -o $@ $< $(CFLAGS)

-include $(DEPS)

.PHONY: clean
clean:
	rm -f src/*.o src/*.d cjson
