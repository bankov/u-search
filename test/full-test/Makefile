# -*- makefile -*-
all: fulltest
	@echo [FINISHED full-test]

SOURCES=fulltest.cpp
SOURCES+=$(SRCDIR)/test/cppsockets-test/cppsocketstest.cpp
SOURCES+=$(SRCDIR)/test/spider-test/spidertest.cpp
SOURCES+=$(SRCDIR)/test/datastorage-test/datastoragetest.cpp
SOURCES+=$(SRCDIR)/spider/spider.cpp

include ../../config.mk

LIBS+=-lcppunit -lmysqlpp -lsmbclient -lmysqlclient -lcppsockets -ldata_storage -lmagic

.cpp.o:
	$(CC) $(CFLAGS) $(INCLUDEPATH) $(DEFINES) -fPIC -c -o $@ $<

fulltest: $(OBJECTS)
	$(CC) $(CFLAGS) $(INCLUDEPATH) $(DEFINES) -o $(DESTDIR)/test/fulltest $(OBJECTS) $(LIBS)

clean:
	rm -rf $(DESTDIR)/test/fulltest *.o *.d *.gcov *.gcda *.gcno
