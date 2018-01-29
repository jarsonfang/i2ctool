#CC		 := gcc
CC		:= arm-none-linux-gnueabi-gcc
CFLAGS	?= -D _BSD_SOURCE -std=c99 -pedantic -Wall
LDFLAGS ?=

PROGNAME = i2ctool
OBJS = main.o i2c-util.o

exec_prefix ?= /usr
bindir ?= $(exec_prefix)/bin

all: $(OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $(PROGNAME) $(OBJS)

install: all
	install -d $(DESTDIR)$(bindir)
	install -m 0755 $(PROGNAME) $(DESTDIR)$(bindir)

%.o: %.c
	$(CC) $(CFLAGS) -c $<

clean:
	@echo "Clean object files"
	@rm -f *.o
	@rm -f $(PROGNAME)

