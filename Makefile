# -*- makefile -*-
include config.mk

all: test
	@echo [FINISHED u-search]

libcppsockets:
	cd $(SRC_BASE)/cppsockets && make

spider: copyfiles libdata_storage
	cd $(SRC_BASE)/spider && make

libdata_storage:
	cd $(SRC_BASE)/data-storage && make

test: libcppsockets libdata_storage spider
	cd $(SRC_BASE)/tests && make

copyfiles: database.dat.example servers.dat.example
	mkdir -p $(DESTDIR)/etc/u-search
	cp database.dat.example $(DESTDIR)/etc/u-search
	cp servers.dat.example $(DESTDIR)/etc/u-search

doc:
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

.PHONY: help doc spider copyfiles test libdata_storage clean
