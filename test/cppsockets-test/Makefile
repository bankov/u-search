# -*- makefile -*-
all: cppsocketstest
	@echo [FINISHED cppsockets-test]

SOURCES=cppsocketstest.cpp main.cpp
HEADERS=cppsocketstest.h

include ../../config.mk

LIBS+=-lcppunit -lcppsockets

.SUFFIXES: .cpp .o

.cpp.o:
	$(CC) $(CFLAGS) $(INCLUDEPATH) $(DEFINES) -fPIC -c -o $@ $<

cppsocketstest: $(OBJECTS)
	mkdir -p $(DESTDIR)/test
	$(CC) $(CFLAGS) $(INCLUDEPATH) $(DEFINES) -o $(DESTDIR)/test/cppsocketstest $(OBJECTS) $(LIBS)

clean:
	rm -rf *.o $(DESTDIR)/test/cppsocketstest *.d *.gcov *.gcda *.gcno
