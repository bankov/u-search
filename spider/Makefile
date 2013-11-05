# -*- makefile -*-
all: spider
	@echo [FINISHED spider]

HEADERS=spider.h
SOURCES=spider.cpp main.cpp

include ../config.mk

LDFLAGS+=-lsmbclient -lmysqlpp -lmysqlclient -L$(DESTDIR)/lib -ldata_storage -lmagic

.SUFFIXES: .cpp .o

.cpp.o:
	$(CC) $(CFLAGS) $(INCLUDE_PATHS) $(DEFINES) -fPIC -c -o $@ $<

spider: $(OBJECTS)
	mkdir -p $(DESTDIR)/bin
	$(CC) $(CFLAGS) $(INCLUDEPATHS) $(DEFINES) -o $(DESTDIR)/bin/spider $(OBJECTS) $(LDFLAGS)

clean:
	rm -rf $(DESTDIR)/bin/spider *.o *.d *.gcov *.gcda *.gcno
