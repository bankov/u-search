# -*- makefile -*-
all: libcppsocketstest cppsocketstest
	@echo [FINISHED cppsockets-test]

SOURCES=cppsocketstest.cpp main.cpp
HEADERS=cppsocketstest.h

include ../../config.mk

LDFLAGS+=-lcppunit -L$(DESTDIR)/lib -lcppsockets

.SUFFIXES: .cpp .o

.cpp.o:
	$(CC) $(CFLAGS) $(INCLUDE_PATHS) $(DEFINES) -fPIC -c -o $@ $<

libcppsocketstest: $(OBJECTS)
	mkdir -p $(DESTDIR)/tests/lib
	$(CC) $(CFLAGS) $(INCLUDE_PATHS) $(DEFINES) -shared -o $(DESTDIR)/tests/lib/libcppsocketstest.so cppsocketstest.o $(LDFLAGS)

cppsocketstest: libcppsocketstest
	mkdir -p $(DESTDIR)/tests/bin
	$(CC) $(CFLAGS) $(INCLUDE_PATHS) $(DEFINES) -o $(DESTDIR)/tests/bin/cppsocketstest $(OBJECTS) $(LDFLAGS)

clean:
	rm -rf *.o $(DESTDIR)/tests/bin/cppsocketstest $(DESTDIR)/tests/lib/libcppsocketstest *.d *.gcov *.gcda *.gcno
