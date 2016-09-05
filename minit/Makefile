CPPFLAGS =
CFLAGS   = -Os -Wall -pedantic
LDFLAGS  = -s

DATE   := $(shell date +%Y%m%d)
VERSION = $(DATE)

BIN = minit
SRC = $(BIN).c

DIST_BASE = $(BIN)-$(VERSION)
DIST      = $(DIST_BASE).tar.gz

prefix      =
exec_prefix = $(prefix)
sbindir     = $(exec_prefix)/sbin

DEB_BUILD = deb-build


all: $(BIN)
$(BIN): $(SRC)
	$(CC) $(CPPFLAGS) $(CFLAGS) -std=gnu99 $^ $(LDFLAGS) -o $@

clean:
	rm -f $(BIN)

install: $(BIN)
	install -D $(BIN) $(DESTDIR)$(sbindir)/$(BIN)
uninstall:
	rm -f $(DESTDIR)$(sbindir)/$(BIN)

dist: $(DIST)
$(DIST): $(SRC) COPYING Makefile README.md example/Dockerfile example/startup \
		image/Dockerfile
	mkdir $(DIST_BASE)
	cp -a --parents $^ $(DIST_BASE)
	tar czf $@ $(DIST_BASE)
	rm -rf $(DIST_BASE)

# Sets up a directory where we can build debian packages.
$(DEB_BUILD): $(DIST)
	mkdir $@
	tar xzf $(DIST) -C $@
	cp -a $(DIST) $@/$(BIN)_$(VERSION).orig.tar.gz
	cp -a debian $@/$(DIST_BASE)

deb-clean:
	rm -rf $(DEB_BUILD)

.PHONY: all clean install uninstall dist deb-clean
