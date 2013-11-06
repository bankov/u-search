# -*- makefile -*-
all: fulltest
	@echo [FINISHED full-test]

SOURCES=fulltest.cpp
SOURCES+=$(SRC_BASE)/test/cppsockets-test/cppsocketstest.cpp
SOURCES+=$(SRC_BASE)/test/spider-test/spidertest.cpp
SOURCES+=$(SRC_BASE)/test/datastorage-test/datastoragetest.cpp
SOURCES+=$(SRC_BASE)/spider/spider.cpp

include ../../config.mk

LDFLAGS+=-lcppunit -lmysqlpp -lsmbclient -lmysqlclient -L$(DESTDIR)/lib -lcppsockets -ldata_storage -lmagic

.cpp.o:
	$(CC) $(CFLAGS) $(INCLUDE_PATHS) $(DEFINES) -fPIC -c -o $@ $<

fulltest: $(OBJECTS)
	$(CC) $(CFLAGS) $(INCLUDE_PATHS) $(DEFINES) -o $(DESTDIR)/test/fulltest $(OBJECTS) $(LDFLAGS)

clean:
	rm -rf $(DESTDIR)/test/fulltest *.o *.d *.gcov *.gcda *.gcno
