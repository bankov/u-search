# -*- makefile -*-
all: fulltest
	@echo [FINISHED full-test]

SOURCES=fulltest.cpp

include ../../config.mk

LDFLAGS+=-lcppunit -lmysqlpp -lsmbclient -lmysqlclient -L$(DESTDIR)/lib -lcppsockets -ldata_storage -lmagic

.cpp.o:
	$(CC) $(CFLAGS) $(INCLUDE_PATHS) $(DEFINES) -fPIC -c -o $@ $<

OBJECTS+=$(SRC_BASE)/test/cppsockets-test/cppsocketstest.o
OBJECTS+=$(SRC_BASE)/test/spider-test/spidertest.o
OBJECTS+=$(SRC_BASE)/test/datastorage-test/datastoragetest.o
OBJECTS+=$(SRC_BASE)/spider/spider.o

fulltest: $(OBJECTS)
	$(CC) $(CFLAGS) $(INCLUDE_PATHS) $(DEFINES) -o $(DESTDIR)/test/bin/fulltest $(OBJECTS) $(LDFLAGS)

clean:
	rm -rf $(DESTDIR)/test/bin/fulltest *.o *.d *.gcov *.gcda *.gcno
