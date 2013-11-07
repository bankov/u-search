# -*- makefile -*-
TARGET=smbtaskd
SOURCES=main.cpp serverqueue.cpp

include ../config.mk

.SUFFIXES: .cpp .o

.cpp.o:
	$(CC) $(CFLAGS) $(INCLUDEPATH) $(DEFINES) -fPIC -c -o $@ $<

$(TARGET): $(OBJECTS)
	mkdir -p $(BUILD)/bin
	$(CC) $(CFLAGS) $(INCLUDEPATH) $(DEFINES) -o $(DESTDIR)/bin/$(TARGET) $(OBJECTS) $(LDFLAGS)

clean:
	rm -rf $(DESTDIR)/bin/$(TARGET) *.o *.d *.gcov *.gcda *.gcno
