# -*- makefile -*-
all: spidertest
	@echo [FINISHED spider-test]

HEADERS=spidertest.h
SOURCES=spidertest.cpp main.cpp
SOURCES+=$(SRC_BASE)/spider/spider.cpp

include ../../config.mk

LDFLAGS+=-lcppunit -lsmbclient -L$(DESTDIR)/lib -ldata_storage -lmagic

.SUFFIXES: .cpp .o

.cpp.o:
	$(CC) $(CFLAGS) $(INCLUDE_PATHS) $(DEFINES) -fPIC -c -o $@ $<

spidertest: $(OBJECTS)
	mkdir -p $(DESTDIR)/test
	$(CC) $(CFLAGS) $(INCLUDE_PATHS) $(DEFINES) -o $(DESTDIR)/test/spidertest $(OBJECTS) $(LDFLAGS)

clean:
	rm -rf $(DESTDIR)/test/spidertest *.o *.d *.gcov *.gcda *.gcno
