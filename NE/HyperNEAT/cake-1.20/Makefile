# cake - a checkers engine
# Copyright (C) 2000,2005 by Martin Fierz

# Makefile for use with gcc
# and install target
# init: Peter Chiocchetti Sat Jun 24 13:47:15 MEST 2000
# book: -"- Mit Feb 23 14:42:43 CET 2005

VERSION=1.20

### system configuration
# this defaults to little endian (intel) hardware if you are
# on a big endian machine (eg. ppc, sparc), uncomment XDEFS

### install directories for binary and database

PREFIX = /usr/local
BINDIR = $(PREFIX)/bin
DBDIR  = $(PREFIX)/share/cake
MANDIR = $(PREFIX)/man/man6

### compiler, debug, optimization, byteorder

CC = gcc
DEFS = -DDBINI=\"$(DBDIR)/db.ini\" -DXBOOK=\"$(DBDIR)/xbook.bin\" -DSYS_UNIX
#XDEFS = -DBIGEND

#CFLAGS = -g -Wall -O3 $(DEFS) $(XDEFS)
CFLAGS = -O3 $(DEFS) $(XDEFS)

### install program

MAKEDIR = /usr/bin/install -d
INSTALL = /usr/bin/install -c -s
INSTALLDATA = /usr/bin/install -c -m 644

#### end of configuration


OBJECTS = cakepp.o db.o movegen.o ansicake.o book.o
TEST = cakepp.o db.o interface.o movegen.o testcake.o

all: cake

cake: $(OBJECTS)
	$(CC) $(OBJECTS) -o cake

testcake: $(TEST)
	$(CC) $(TEST) -lm -o testcake

clean:
	rm -f *.o *~ *core *.BAK *.txt sys.db.ini tags cake

tags: *.c
	vim-ctags *

install: all
	@sed -e "s:db:$(DBDIR)/db:g" db.ini >sys.db.ini
	$(INSTALL) cake $(BINDIR)/cake
	$(MAKEDIR) $(DBDIR)
	$(INSTALLDATA) sys.db.ini $(DBDIR)/db.ini
	$(INSTALLDATA) db4 $(DBDIR)/db4
	$(INSTALLDATA) db4.idx $(DBDIR)/db4.idx
	$(INSTALLDATA) xbook.bin $(DBDIR)/xbook.bin
	$(MAKEDIR) $(MANDIR)
	$(INSTALLDATA) cake.man $(MANDIR)/cake.6

### dist package

DISTFILES = Makefile README COPYING cake.man db.ini db4 db4.idx \
	ansicake.c cakepp.c cakepp.h consts.h db.c db.h interface.c \
	movegen.c movegen.h structs.h switches.h testcake.c \
	book.c book.h xbook.bin cake \
	cake.dev cake.exe

dist: clean cake
	strip cake
	@sh -c 'cd ..; \
	tar cvfz cake-$(VERSION).tar.gz \
	`for i in $(DISTFILES); do echo cake-$(VERSION)/$$i; done`'
