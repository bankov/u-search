# -*- makefile -*-
all: libdata_storage
	@echo [FINISHED data-storage]

SOURCES = entities.cpp
HEADERS = entities.h

include ../config.mk

LIBS+=-lmysqlclient -lmysqlpp

.SUFFIXES: .cpp .o

.cpp.o:
	$(CC) $(CFLAGS) $(INCLUDEPATH) $(DEFINES) -fPIC -c -o $@ $<

libdata_storage: $(OBJECTS)
	mkdir -p $(DESTDIR)/lib
	$(CC) $(CFLAGS) $(INCLUDEPATH) $(DEFINES) -shared -o $(DESTDIR)/lib/libdata_storage.so $(OBJECTS) $(LIBS)

clean:
	rm -rf *.o $(DESRDIR)/lib/libdata_storage.so *.d *.gcov *.gcda *.gcno
