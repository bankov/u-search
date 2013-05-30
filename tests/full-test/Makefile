# -*- makefile -*-
all: fulltest
	@echo [FINISHED full-test]

SOURCES=fulltest.cpp

include ../../config.mk

LDFLAGS+=-lcppunit -lmysqlpp -lmysqlclient -L$(BUILD)/lib -lcppsockets -lspider -ldata_storage -L$(BUILD)/tests/lib -lcppsocketstest -lspidertest -ldatastoragetest -lmagic

fulltest: $(SOURCES) libcppsocketstest libdatastoragetest libspidertest
	$(CC) $(CFLAGS) $(INCLUDE_PATHS) $(DEFINES) -o $(BUILD)/tests/bin/fulltest $(SOURCES) $(LDFLAGS)

libcppsocketstest: $(SRC_BASE)/tests/cppsockets-test/*.h $(SRC_BASE)/tests/cppsockets-test/*.cpp
	cd $(SRC_BASE)/tests make libcppsocketstest

libspidertest: $(SRC_BASE)/tests/spider-test/*.h $(SRC_BASE)/tests/spider-test/*.cpp
	cd $(SRC_BASE)/tests make libspidertest

libdatastoragetest: $(SRC_BASE)/tests/datastorage-test/*.h $(SRC_BASE)/tests/datastorage-test/*.cpp
	cd $(SRC_BASE)/tests make libdatastoragetest

clean:
	rm -rf $(BUILD)/tests/bin/fulltest *.o *.d *.gcov *.gcda *.gcno
