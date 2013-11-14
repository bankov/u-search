# -*- makefile -*-
TARGET:=serverqueuetest
SOURCES=serverqueuetest.cpp main.cpp $(SRCDIR)/scheduler/serverqueue.cpp
HEADERS=serverqueuetest.h

include ../../config.mk

.SUFFIXES: .cpp .o

LIBS+=-lcppunit

.cpp.o:
	$(CC) $(CFLAGS) $(INCLUDEPATH) $(DEFINES) -fPIC -c -o $@ $<

serverqueuetest: $(OBJECTS)
	mkdir -p $(DESTDIR)/test
	$(CC) $(CFLAGS) $(INCLUDEPATH) $(DEFINES) -o $(DESTDIR)/test/serverqueuetest $(OBJECTS) $(LIBS)

clean:
	rm -rf $(DESTDIR)/test/serverqueuetest *.o *.d *.gcov *.gcda *.gcno
