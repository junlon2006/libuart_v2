.PHONY: all clean examples install

CC = gcc
AR = ar
CFLAGS = -Wall -Wextra -std=c99 -O2 -Iprotocol
LDFLAGS = -pthread

LIB_SRC = protocol/uni_communication.c
LIB_OBJ = $(LIB_SRC:.c=.o)
LIB_TARGET = libuart_comm.a

EXAMPLES = linux_demo

all: $(LIB_TARGET)

$(LIB_TARGET): $(LIB_OBJ)
	$(AR) rcs $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

examples: $(EXAMPLES)

linux_demo: example/linux_posix_demo.c $(LIB_TARGET)
	$(CC) $(CFLAGS) $< -L. -luart_comm $(LDFLAGS) -o $@

clean:
	rm -f $(LIB_OBJ) $(LIB_TARGET) $(EXAMPLES)

install: $(LIB_TARGET)
	install -d $(DESTDIR)/usr/local/lib
	install -d $(DESTDIR)/usr/local/include
	install -m 644 $(LIB_TARGET) $(DESTDIR)/usr/local/lib/
	install -m 644 protocol/uni_communication.h $(DESTDIR)/usr/local/include/
