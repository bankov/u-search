# -*- makefile -*-
all: spidertest
	@echo [FINISHED spider-test]

HEADERS=spidertest.h
SOURCES=spidertest.cpp main.cpp
SOURCES+=$(SRCDIR)/spider/spider.cpp

include ../../config.mk

LIBS+=-lcppunit -lsmbclient -ldata_storage -lmagic

.SUFFIXES: .cpp .o

.cpp.o:
	$(CC) $(CFLAGS) $(INCLUDEPATH) $(DEFINES) -fPIC -c -o $@ $<

spidertest: $(OBJECTS)
	mkdir -p $(DESTDIR)/test
	$(CC) $(CFLAGS) $(INCLUDEPATH) $(DEFINES) -o $(DESTDIR)/test/spidertest $(OBJECTS) $(LIBS)

clean:
	rm -rf $(DESTDIR)/test/spidertest *.o *.d *.gcov *.gcda *.gcno
