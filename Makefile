# -*- makefile -*-
include config.mk

all: test spiderd
	@echo [FINISHED u-search]

libcppsockets: cppsockets
	cd $(SRC_BASE)/cppsockets && make

libspider: libdata_storage spider
	cd $(SRC_BASE)/spider && make libspider

spiderd: libspider copyfiles
	cd $(SRC_BASE)/spider && make spider

libdata_storage: data-storage
	cd $(SRC_BASE)/data-storage && make

test: libspider libcppsockets libdata_storage copyfiles tests
	cd $(SRC_BASE)/tests && make

copyfiles: database.dat.example servers.dat.example
	mkdir -p $(BUILD)/etc/u-search
	cp database.dat.example $(BUILD)/etc/u-search
	cp servers.dat.example $(BUILD)/etc/u-search

docs: doc
	cd $(SRC_BASE)/doc && make

help:
	@echo Available modules: libcppsockets libspider spiderd libdata_storage test copygiles docs
	@echo Debug mode: DEBUG=yes
	@echo Test coverage: TEST_COVERAGE=yes
	@echo Show build commands: VERBOSE=yes

clean:
	rm -rf build
	cd $(SRC_BASE)/spider && make clean
	cd $(SRC_BASE)/cppsockets && make clean
	cd $(SRC_BASE)/data-storage && make clean
	cd $(SRC_BASE)/tests && make clean
	cd $(SRC_BASE)/doc && make clean
