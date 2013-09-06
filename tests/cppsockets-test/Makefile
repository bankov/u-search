# -*- makefile -*-
all: libcppsocketstest cppsocketstest
	@echo [FINISHED cppsockets-test]

SOURCES=cppsocketstest.cpp main.cpp
HEADERS=cppsocketstest.h

include ../../config.mk

LDFLAGS+=-lcppunit -L$(BUILD)/lib -lcppsockets

.SUFFIXES: .cpp .o

.cpp.o:
	$(CC) $(CFLAGS) $(INCLUDE_PATHS) $(DEFINES) -fPIC -c -o $@ $<

libcppsocketstest: $(OBJECTS) libcppsockets
	mkdir -p $(BUILD)/tests/lib
	$(CC) $(CFLAGS) $(INCLUDE_PATHS) $(DEFINES) -shared -o $(BUILD)/tests/lib/libcppsocketstest.so cppsocketstest.o $(LDFLAGS)

cppsocketstest: libcppsocketstest
	mkdir -p $(BUILD)/tests/bin
	$(CC) $(CFLAGS) $(INCLUDE_PATHS) $(DEFINES) -o $(BUILD)/tests/bin/cppsocketstest $(OBJECTS) $(LDFLAGS)

libcppsockets: $(SRC_BASE)/cppsockets/*.h $(SRC_BASE)/cppsockets/*.cpp
	cd $(SRC_BASE) && make libcppsockets

clean:
	rm -rf *.o $(BUILD)/tests/bin/cppsocketstest $(BUILD)/tests/lib/libcppsocketstest *.d *.gcov *.gcda *.gcno
