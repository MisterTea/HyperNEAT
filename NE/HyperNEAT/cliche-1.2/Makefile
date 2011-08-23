#
# cliche - command line interface checkers
# Makefile for gcc
# Mit Jan 15 20:21:12 CET 2003
# Fre Jul  8 13:41:43 CEST 2005
#
VERSION=1.2

# install paths
DESTDIR=/usr/local
BIN_DIR=$(DESTDIR)/games/
LIB_DIR=$(DESTDIR)/share/games/cliche/

# installed files
BIN=cliche
LIB=dama.so simplech.so

# compiler flags
CFLAGS=-g -O3
LDFLAGS=-g
APP_CFLAGS=-rdynamic -DLIB_DIR=\"$(LIB_DIR)\"
APP_LDFLAGS=-ldl
LIB_CFLAGS=-fpic
LIB_LDFLAGS=-shared -Wl,-soname,$@

# TARGETS
all: $(LIB) $(BIN)

dummy.o: dummy.c
	$(CC) $(CFLAGS) $(LIB_CFLAGS) -o $@ -c $<
dama.o: dama.c
	$(CC) $(CFLAGS) $(LIB_CFLAGS) -o $@ -c $<
simplech.o: simplech.c
	$(CC) $(CFLAGS) $(LIB_CFLAGS) -o $@ -c $<
pdnplay.o: pdnplay.c
	$(CC) $(CFLAGS) $(LIB_CFLAGS) -o $@ -c $<
cliche.o: cliche.c
	$(CC) -Wall $(CFLAGS) $(APP_CFLAGS) -o $@ -c $<

dummy.so: dummy.o
	$(CC) $(LDFLAGS) $(LIB_LDFLAGS) -o $@ $<
dama.so: dama.o
	$(CC) $(LDFLAGS) $(LIB_LDFLAGS) -o $@ $<
simplech.so: simplech.o
	$(CC) $(LDFLAGS) $(LIB_LDFLAGS) -o $@ $<
cliche: cliche.o
	$(CC) $(LDFLAGS) $(APP_LDFLAGS) -o $@ $<

install: all
	install -d $(BIN_DIR) $(LIB_DIR)
	install -s $(BIN) $(BIN_DIR)
	install -s $(LIB) $(LIB_DIR)
clean:
	rm -f *.o *.out *~ tags *.pdn *-stamp
	rm -rf tmp
	which dh_clean && dh_clean
mrproper: clean
	rm -f *.so cliche
dist: clean
	strip cliche simplech.so dama.so
	(cd .. && cp -a cliche cliche-$(VERSION) \
	&& tar cvfz cliche-$(VERSION).tar.gz cliche-$(VERSION) \
	&& rm -rf cliche-$(VERSION) )

# DEBIAN PACKAGE
dpkg: mrproper
	fakeroot debian/rules binary
dpkg2: mrproper
	dpkg-buildpackage -rfakeroot -us -uc

# FUN TARGETS
profile: all
	@mkdir -p tmp
	LD_LIBRARY_PATH=/usr/lib/debug \
	LD_PROFILE_OUTPUT=tmp \
	LD_PROFILE=dama.so \
	./cliche dama
	@[ -f ./tmp/dama.so.profile ] \
	&& echo "Profile data in ./tmp." \
	|| echo "No profile data created (Old glibc?)."

memusage: all
	LD_PRELOAD=/lib/libmemusage.so ./cliche 

.PHONY: install clean dist dpkg-all dpkg-bin profile
