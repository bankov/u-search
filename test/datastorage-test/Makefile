# -*- makefile -*-
all: datastoragetest
	@echo [FINISHED data-storage-test]

SOURCES=datastoragetest.cpp main.cpp
HEADERS=datastoragetest.h

include ../../config.mk

LDFLAGS+=-lcppunit -lmysqlclient -lmysqlpp -L$(DESTDIR)/lib -ldata_storage

.SUFFIXES: .cpp .o

.cpp.o:
	$(CC) $(CFLAGS) $(INCLUDE_PATHS) $(DEFINES) -fPIC -c -o $@ $<

datastoragetest: $(OBJECTS)
	mkdir -p $(DESTDIR)/test
	$(CC) $(CFLAGS) $(INCLUDE_PATHS) $(DEFINES) -o $(DESTDIR)/test/datastoragetest $(OBJECTS) $(LDFLAGS)

clean:
	rm -rf $(DESTDIR)/test/datastoragetest *.o *.d *.gcov *.gcda *.gcno
