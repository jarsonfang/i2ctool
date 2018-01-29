#CC		 := gcc
CC		:= arm-none-linux-gnueabi-gcc
CFLAGS	?= -std=c99 -pedantic -Wall
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

clean: $(OBJS)
	@echo "Clean object files"
	@rm -f $^
	@rm -f $(PROGNAME)

%.o: %.c
	$(CC) $(CFLAGS) -c $<
