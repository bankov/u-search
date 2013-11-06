# -*- makefile -*-
include config.mk

all: test
	@echo [FINISHED u-search]

libcppsockets:
	cd $(SRCDIR)/cppsockets && make

spider: copyfiles libdata_storage
	cd $(SRCDIR)/spider && make

libdata_storage:
	cd $(SRCDIR)/data-storage && make

test: libcppsockets libdata_storage spider
	cd $(SRCDIR)/test && make

copyfiles: database.dat.example servers.dat.example
	mkdir -p $(DESTDIR)/etc/u-search
	cp database.dat.example $(DESTDIR)/etc/u-search
	cp servers.dat.example $(DESTDIR)/etc/u-search

doc:
	cd $(SRCDIR)/doc && make

help:
	@echo Available modules: libcppsockets libspider spiderd libdata_storage test copygiles docs
	@echo Debug mode: DEBUG=yes
	@echo Test coverage: TEST_COVERAGE=yes
	@echo Show build commands: VERBOSE=yes

clean:
	rm -rf build
	cd $(SRCDIR)/spider && make clean
	cd $(SRCDIR)/cppsockets && make clean
	cd $(SRCDIR)/data-storage && make clean
	cd $(SRCDIR)/test && make clean
	cd $(SRCDIR)/doc && make clean

.PHONY: help doc spider copyfiles test libdata_storage clean
