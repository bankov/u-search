# -*- makefile -*-
all: libdata_storage
	@echo [FINISHED data-storage]

SOURCES = entities.cpp
HEADERS = entities.h

include ../config.mk

LDFLAGS+=-lmysqlclient -lmysqlpp

.SUFFIXES: .cpp .o

.cpp.o:
	$(CC) $(CFLAGS) $(INCLUDE_PATHS) $(DEFINES) -fPIC -c -o $@ $<

libdata_storage: $(OBJECTS)
	mkdir -p $(BUILD)/lib
	$(CC) $(CFLAGS) $(INCLUDEPATHS) $(DEFINES) -shared -o $(BUILD)/lib/libdata_storage.so $(OBJECTS) $(LDFLAGS)

clean:
	rm -rf *.o $(BUILD)/lib/libdata_storage.so *.d *.gcov *.gcda *.gcno
