# -*- makefile -*-
SRC_BASE:= $(shell dirname $(realpath $(lastword $(MAKEFILE_LIST))))

CC:=g++-4.7

ifneq ($(VERBOSE),yes)
MAKEFLAGS += -s
endif

ifeq ($(DEBUG),yes)
CFLAGS:=-Wall --std=c++11 -g -O0
BUILD:=$(SRC_BASE)/build/debug
DEFINES:=-DMSS_DEBUG
else
CFLAGS:=-Wall --std=c++11 -O2
BUILD:=$(SRC_BASE)/build/release
endif

ifeq ($(TEST_COVERAGE),yes)
CFLAGS+=--coverage -O0 -ftest-coverage
LDFLAGS:=-lgcov
endif

INCLUDE_PATHS:=-I$(SRC_BASE)

OBJECTS:=$(SOURCES:.cpp=.o)

install: all
