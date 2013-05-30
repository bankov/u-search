# -*- makefile -*-
all: libspider spider
	@echo [FINISHED spider]

HEADERS=spider.h
SOURCES=spider.cpp main.cpp

include ../config.mk

LDFLAGS+=-lsmbclient -lmysqlpp -lmysqlclient -L$(BUILD)/lib -ldata_storage -lmagic

.SUFFIXES: .cpp .o

.cpp.o:
	$(CC) $(CFLAGS) $(INCLUDE_PATHS) $(DEFINES) -fPIC -c -o $@ $<

libspider: spider.o libdata_storage
	$(CC) $(CFLAGS) $(INCLUDEPATHS) $(DEFINES) -shared -o $(BUILD)/lib/libspider.so spider.o $(LDFLAGS)

libdata_storage:
	cd $(SRC_BASE) && make libdata_storage

spider: libspider $(OBJECTS)
	mkdir -p $(BUILD)/bin
	$(CC) $(CFLAGS) $(INCLUDEPATHS) $(DEFINES) -o $(BUILD)/bin/spider $(OBJECTS) $(LDFLAGS)

clean:
	rm -rf $(BUILD)/bin/spider $(BUILD)/lib/libspider.so *.o *.d *.gcov *.gcda *.gcno
