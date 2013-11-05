# -*- makefile -*-
all: libdatastoragetest datastoragetest
	@echo [FINISHED data-storage-test]

SOURCES=datastoragetest.cpp main.cpp
HEADERS=datastoragetest.h

include ../../config.mk

LDFLAGS+=-lcppunit -lmysqlclient -lmysqlpp -L$(DESTDIR)/lib -ldata_storage

.SUFFIXES: .cpp .o

.cpp.o:
	$(CC) $(CFLAGS) $(INCLUDE_PATHS) $(DEFINES) -fPIC -c -o $@ $<

libdatastoragetest: $(OBJECTS)
	mkdir -p $(DESTDIR)/tests/lib
	$(CC) $(CFLAGS) $(INCLUDE_PATHS) $(DEFINES) -shared -o $(DESTDIR)/tests/lib/libdatastoragetest.so datastoragetest.o $(LDFLAGS)

datastoragetest: libdatastoragetest
	mkdir -p $(DESTDIR)/tests/bin
	$(CC) $(CFLAGS) $(INCLUDE_PATHS) $(DEFINES) -o $(DESTDIR)/tests/bin/datastoragetest $(OBJECTS) $(LDFLAGS)

clean:
	rm -rf $(DESTDIR)/tests/lib/libdatastoragetest.so $(DESTDIR)/tests/bin/datastoragetest *.o *.d *.gcov *.gcda *.gcno
