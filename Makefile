CC      = gcc
CFLAGS  = -Wall -Wextra -O2 -std=c99
LDFLAGS = -lm

TARGET  = bloom_filter
SRCS    = main.c bloom.c gerador.c medicao.c
OBJS    = $(SRCS:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f $(OBJS) $(TARGET)

run: all
	./$(TARGET)

.PHONY: all clean run
