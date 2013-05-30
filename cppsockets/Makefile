# -*- makefile -*-
all: libcppsockets.so
	@echo [FINISHED cppsockets]

HEADERS = socketaddress.h	\
		  abstractsocket.h	\
		  datasocket.h		\
		  tcplistener.h		\
		  tcpsocket.h		\
		  udpsocket.h

SOURCES = socketaddress.cpp		\
		  abstractsocket.cpp	\
		  datasocket.cpp		\
		  tcplistener.cpp		\
		  tcpsocket.cpp			\
		  udpsocket.cpp			\

include ../config.mk

.SUFFIXES: .cpp .o

.cpp.o:
	$(CC) $(CFLAGS) $(INCLUDE_PATHS) $(DEFINES) -fPIC -c -o $@ $<

libcppsockets.so: $(OBJECTS)
	mkdir -p $(BUILD)/lib
	$(CC) $(CFLAGS) $(INCLUDE_PATHS) $(DEFINES) -shared -o $(BUILD)/lib/libcppsockets.so $(OBJECTS)

clean:
	rm -rf *.o $(BUILD)/lib/libcppsockets.so *.d *.gcov *.gcda *.gcno
