# -*- makefile -*-
TARGET=scheduler
SOURCES=main.cpp schedulerserver.cpp serverqueue.cpp

include ../config.mk

.SUFFIXES: .cpp .o

.cpp.o:
	$(CC) $(CFLAGS) $(INCLUDEPATH) $(DEFINES) -fPIC -c -o $@ $<

$(TARGET): $(OBJECTS)
	mkdir -p $(DESTDIR)/bin
	$(CC) $(CFLAGS) $(INCLUDEPATH) $(DEFINES) -o $(DESTDIR)/bin/$(TARGET) $(OBJECTS) $(LDFLAGS)

clean:
	rm -rf $(BUILD)/bin/$(TARGET) *.o *.d *.gcov *.gcda *.gcno
