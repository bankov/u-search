# -*- makefile -*-
TARGET=test
include ../config.mk

cppsocketstest:
	cd $(SRCDIR)/test/cppsockets-test && make

datastoragetest:
	cd $(SRCDIR)/test/datastorage-test && make

spidertest:
	cd $(SRCDIR)/test/spider-test && make

fulltest:
	cd $(SRCDIR)/test/full-test && make

serverqueuetest:
	cd $(SRCDIR)/test/serverqueue-test && make

test: cppsocketstest datastoragetest serverqueuetest fulltest # spidertest

clean:
	rm -rf $(DESTDIR)/test
	cd cppsockets-test && make clean
	cd datastorage-test && make clean
	cd spider-test && make clean
	cd serverqueue-test && make clean
	cd full-test && make clean

.PHONY: cppsocketstest datastoragetest spidertest serverqueuetest fulltest
