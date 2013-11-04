# -*- makefile -*-
TARGET=serverqueuetest
SOURCES=serverqueuetest.cpp main.cpp
HEADERS=serverqueuetest.h

include ../../config.mk

.SUFFIXES: .cpp .o

LDFLAGS+=-lcppunit

.cpp.o:
	$(CC) $(CFLAGS) $(INCLUDEPATH) $(DEFINES) -fPIC -c -o $@ $<

serverqueuetest: $(OBJECTS)
	mkdir -p $(DESTDIR)/test/bin
	$(CC) $(CFLAGS) $(INCLUDEPATH) $(DEFINES) -o $(DESTDIR)/test/bin/serverqueuetest $(OBJECTS) $(LDFLAGS) $(SRCDIR)/smbtaskd/serverqueue.o

clean:
	rm -rf $(DESTDIR)/test/bin/serverqueuetest *.o *.d *.gcov *.gcda *.gcno
