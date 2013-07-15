# -*- makefile -*-
all: fulltest
	@echo [FINISHED full-test]

SOURCES=fulltest.cpp

include ../../config.mk

LDFLAGS+=-lcppunit -lmysqlpp -lmysqlclient -L$(BUILD)/lib -lcppsockets -lspider -ldata_storage -L$(BUILD)/tests/lib -lcppsocketstest -lspidertest -ldatastoragetest -lmagic

fulltest: $(SOURCES) libcppsocketstest libdatastoragetest libspidertest
	$(CC) $(CFLAGS) $(INCLUDE_PATHS) $(DEFINES) -o $(BUILD)/tests/bin/fulltest $(SOURCES) $(LDFLAGS)

libcppsocketstest:
	cd $(SRC_BASE)/tests make libcppsocketstest

libspidertest:
	cd $(SRC_BASE)/tests make libspidertest

libdatastoragetest:
	cd $(SRC_BASE)/tests make libdatastoragetest

clean:
	rm -rf $(BUILD)/tests/bin/fulltest *.o *.d *.gcov *.gcda *.gcno
