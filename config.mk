# -*- makefile -*-
SRC_BASE:= $(shell dirname $(realpath $(lastword $(MAKEFILE_LIST))))

CC:=g++

ifneq ($(VERBOSE),yes)
MAKEFLAGS += -s
endif  # VERBOSE

CFLAGS:=-Wall --std=c++11

ifeq ($(DEBUG),yes)
CFLAGS+=-g -O0 -DMSS_DEBUG
DESTDIR:=$(SRC_BASE)/build/debug
DEFINES+=-DMSS_DEBUG
else  # DEBUG
CFLAGS+=-O2
DESTDIR:=$(SRC_BASE)/build/release
endif  # DEBUG

ifeq ($(TEST_COVERAGE),yes)
CFLAGS+=--coverage -O0 -ftest-coverage
LDFLAGS:=-lgcov
endif  # TEST_COVERAGE

INCLUDE_PATHS:=-I$(SRC_BASE)

OBJECTS:=$(SOURCES:.cpp=.o)
