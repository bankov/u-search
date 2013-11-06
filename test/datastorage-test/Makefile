# -*- makefile -*-
all: datastoragetest
	@echo [FINISHED data-storage-test]

SOURCES=datastoragetest.cpp main.cpp
HEADERS=datastoragetest.h

include ../../config.mk

LIBS+=-lcppunit -lmysqlclient -lmysqlpp -ldata_storage

.SUFFIXES: .cpp .o

.cpp.o:
	$(CC) $(CFLAGS) $(INCLUDEPATH) $(DEFINES) -fPIC -c -o $@ $<

datastoragetest: $(OBJECTS)
	mkdir -p $(DESTDIR)/test
	$(CC) $(CFLAGS) $(INCLUDEPATH) $(DEFINES) -o $(DESTDIR)/test/datastoragetest $(OBJECTS) $(LIBS)

clean:
	rm -rf $(DESTDIR)/test/datastoragetest *.o *.d *.gcov *.gcda *.gcno
