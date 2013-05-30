# -*- makefile -*-
all: libdatastoragetest datastoragetest
	@echo [FINISHED data-storage-test]

SOURCES=datastoragetest.cpp main.cpp
HEADERS=datastoragetest.h

include ../../config.mk

LDFLAGS+=-lcppunit -lmysqlclient -lmysqlpp -L$(BUILD)/lib -ldata_storage

.SUFFIXES: .cpp .o

.cpp.o:
	$(CC) $(CFLAGS) $(INCLUDE_PATHS) $(DEFINES) -fPIC -c -o $@ $<

libdatastoragetest: $(OBJECTS) libdata_storage
	mkdir -p $(BUILD)/tests/lib
	$(CC) $(CFLAGS) $(INCLUDE_PATHS) $(DEFINES) -shared -o $(BUILD)/tests/lib/libdatastoragetest.so datastoragetest.o $(LDFLAGS)

datastoragetest: libdatastoragetest
	mkdir -p $(BUILD)/tests/bin
	$(CC) $(CFLAGS) $(INCLUDE_PATHS) $(DEFINES) -o $(BUILD)/tests/bin/datastoragetest $(OBJECTS) $(LDFLAGS)

libdata_storage: $(SRC_BASE)/data-storage/*.h $(SRC_BASE)/data-storage/*.cpp
	cd $(SRC_BASE) && make libdata_storage

clean:
	rm -rf $(BUILD)/tests/lib/libdatastoragetest.so $(BUILD)/tests/bin/datastoragetest *.o *.d *.gcov *.gcda *.gcno
