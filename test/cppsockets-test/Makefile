# -*- makefile -*-
all: cppsocketstest
	@echo [FINISHED cppsockets-test]

SOURCES=cppsocketstest.cpp main.cpp
HEADERS=cppsocketstest.h

include ../../config.mk

LDFLAGS+=-lcppunit -L$(DESTDIR)/lib -lcppsockets

.SUFFIXES: .cpp .o

.cpp.o:
	$(CC) $(CFLAGS) $(INCLUDE_PATHS) $(DEFINES) -fPIC -c -o $@ $<

cppsocketstest: $(OBJECTS)
	mkdir -p $(DESTDIR)/test
	$(CC) $(CFLAGS) $(INCLUDE_PATHS) $(DEFINES) -o $(DESTDIR)/test/cppsocketstest $(OBJECTS) $(LDFLAGS)

clean:
	rm -rf *.o $(DESTDIR)/test/cppsocketstest *.d *.gcov *.gcda *.gcno
