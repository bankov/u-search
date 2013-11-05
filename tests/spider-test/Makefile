# -*- makefile -*-
all: libspidertest spidertest
	@echo [FINISHED spider-test]

SOURCES=spidertest.cpp main.cpp
HEADERS=spidertest.h

include ../../config.mk

.SUFFIXES: .cpp .o

LDFLAGS+=-lcppunit -lsmbclient -L$(DESTDIR)/lib -ldata_storage -lmagic

.cpp.o:
	$(CC) $(CFLAGS) $(INCLUDE_PATHS) $(DEFINES) -fPIC -c -o $@ $<

OBJECTS+=$(SRC_BASE)/spider/spider.o

libspidertest: $(OBJECTS)
	mkdir -p $(DESTDIR)/tests/lib
	$(CC) $(CFLAGS) $(INCLUDE_PATHS) $(DEFINES) -shared -o $(DESTDIR)/tests/lib/libspidertest.so spidertest.o $(LDFLAGS)

spidertest: libspidertest $(OBJECTS)
	mkdir -p $(DESTDIR)/tests/bin
	$(CC) $(CFLAGS) $(INCLUDE_PATHS) $(DEFINES) -o $(DESTDIR)/tests/bin/spidertest $(OBJECTS) $(LDFLAGS)

clean:
	rm -rf $(DESTDIR)/tests/lib/libspidertest.so $(DESTDIR)/tests/bin/spidertest *.o *.d *.gcov *.gcda *.gcno
