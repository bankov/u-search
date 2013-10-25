# -*- makefile -*-
TARGET=smbtaskd
SOURCES=main.cpp

include ../config.mk

.SUFFIXES: .cpp .o

.cpp.o:
	$(CC) $(CFLAGS) $(INCLUDEPATH) $(DEFINES) -fPIC -c -o $@ $<

$(PROG): $(OBJECTS)
	mkdir -p $(BUILD)/bin
	$(CC) $(CFLAGS) $(INCLUDEPATH) $(DEFINES) -o $(BUILD)/bin/$(PROG) $(OBJECTS) $(LDFLAGS)

clean:
	rm -rf $(BUILD)/bin/$(PROG) *.o *.d *.gcov *.gcda *.gcno
