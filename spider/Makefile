# -*- makefile -*-
all: spider
	@echo [FINISHED spider]

HEADERS=spider.h
SOURCES=spider.cpp main.cpp

include ../config.mk

LIBS+=-lsmbclient -lmysqlpp -lmysqlclient -ldata_storage -lmagic

.SUFFIXES: .cpp .o

.cpp.o:
	$(CC) $(CFLAGS) $(INCLUDEPATH) $(DEFINES) -fPIC -c -o $@ $<

spider: $(OBJECTS)
	mkdir -p $(DESTDIR)/bin
	$(CC) $(CFLAGS) $(INCLUDEPATH) $(DEFINES) -o $(DESTDIR)/bin/spider $(OBJECTS) $(LIBS)

clean:
	rm -rf $(DESTDIR)/bin/spider *.o *.d *.gcov *.gcda *.gcno
