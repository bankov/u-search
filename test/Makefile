# -*- makefile -*-
include ../config.mk

all: cppsocketstest datastoragetest spidertest fulltest
	@echo [FINISHED test]

cppsocketstest:
	cd $(SRC_BASE)/test/cppsockets-test && make

datastoragetest:
	cd $(SRC_BASE)/test/datastorage-test && make

spidertest:
	cd $(SRC_BASE)/test/spider-test && make

fulltest:
	cd $(SRC_BASE)/test/full-test && make

clean:
	rm -rf $(DESTDIR)/test
	cd cppsockets-test && make clean
	cd datastorage-test && make clean
	cd spider-test && make clean
	cd full-test && make clean
