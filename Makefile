BINARY=bin/minestorm.exe
CFLAGS=-Wall -Werror -O0 -g -MMD -Iinclude
LDLIBS=-lSDL2 -lSDL2_ttf -lSDL2_image
SRCS=$(wildcard src/*.c)
OBJS=$(SRCS:.c=.o)

.PHONY: all clean run

all: $(BINARY)

-include $(OBJS:.o=.d)

%.o: %.c
	gcc -c $(CFLAGS) $< -o $@ -lm

bin:
	mkdir bin

$(BINARY): $(OBJS) | bin
	gcc $(LDFLAGS) $^ $(LDLIBS) -o $@ -lm

run: $(BINARY)
	./$(BINARY)

clean:
	rm -rf $(OBJS) $(OBJS:.o=.d) $(BINARY)
