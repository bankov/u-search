# -*- makefile -*-
SRCDIR:= $(shell dirname $(realpath $(lastword $(MAKEFILE_LIST))))

CC:=g++

ifneq ($(VERBOSE),yes)
MAKEFLAGS += -s
endif  # VERBOSE

CFLAGS+=-Wall --std=c++11

ifeq ($(DEBUG),yes)
CFLAGS+=-g -O0 -DMSS_DEBUG
DESTDIR:=$(SRCDIR)/build/debug
DEFINES+=-DMSS_DEBUG
else  # DEBUG
CFLAGS+=-O2
DESTDIR:=$(SRCDIR)/build/release
endif  # DEBUG

INCLUDEPATH+=-I$(SRCDIR)
LIBS+=-L$(DESTDIR)/lib

ifeq ($(TEST_COVERAGE),yes)
CFLAGS+=--coverage -O0 -ftest-coverage
LIBS:=-lgcov
endif  # TEST_COVERAGE

OBJECTS:=$(SOURCES:.cpp=.o)

all: $(TARGET)
	@echo SUCCESS: $(TARGET)

.PHONY: all $(TARGET) clean
