objects = $(patsubst %.c,%.o,$(wildcard *.c))

CFLAGS = -g
LIBS = -levent

evmain: $(objects)
	$(CC) $(CFLAGS) -o evmain $(objects) $(LIBS)

.PHONY: clean
clean:
	rm -f evmain $(objects)