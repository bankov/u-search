# -*- makefile -*-
all: libspidertest spidertest
	@echo [FINISHED spider-test]

SOURCES=spidertest.cpp main.cpp
HEADERS=spidertest.h

include ../../config.mk

.SUFFIXES: .cpp .o

LDFLAGS+=-lcppunit -lsmbclient -L$(BUILD)/lib -lspider -ldata_storage -lmagic

.cpp.o:
	$(CC) $(CFLAGS) $(INCLUDE_PATHS) $(DEFINES) -fPIC -c -o $@ $<

libspidertest: $(OBJECTS) libspider
	mkdir -p $(BUILD)/tests/lib
	$(CC) $(CFLAGS) $(INCLUDE_PATHS) $(DEFINES) -shared -o $(BUILD)/tests/lib/libspidertest.so spidertest.o $(LDFLAGS)

spidertest: libspidertest $(OBJECTS)
	mkdir -p $(BUILD)/tests/bin
	$(CC) $(CFLAGS) $(INCLUDE_PATHS) $(DEFINES) -o $(BUILD)/tests/bin/spidertest $(OBJECTS) $(LDFLAGS)

libspider: $(SRC_BASE)/spider/*.h $(SRC_BASE)/spider/*.cpp
	cd $(SRC_BASE) && make libspider

clean:
	rm -rf $(BUILD)/tests/lib/libspidertest.so $(BUILD)/tests/bin/spidertest *.o *.d *.gcov *.gcda *.gcno
