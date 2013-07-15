# -*- makefile -*-
include ../config.mk

all: cppsocketstest datastoragetest spidertest fulltest
	@echo [FINISHED test]

cppsocketstest:
	cd $(SRC_BASE)/tests/cppsockets-test && make

datastoragetest:
	cd $(SRC_BASE)/tests/datastorage-test && make

spidertest:
	cd $(SRC_BASE)/tests/spider-test && make

fulltest: spidertest datastoragetest cppsocketstest
	cd $(SRC_BASE)/tests/full-test && make

clean:
	rm -rf $(BUILD)/tests
	cd cppsockets-test && make clean
	cd datastorage-test && make clean
	cd spider-test && make clean
	cd full-test && make clean
