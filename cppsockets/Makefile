# -*- makefile -*-
TARGET:=libcppsockets

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
	$(CC) $(CFLAGS) $(INCLUDEPATH) $(DEFINES) -fPIC -c -o $@ $<

$(TARGET): $(OBJECTS)
	mkdir -p $(DESTDIR)/lib
	$(CC) $(CFLAGS) $(INCLUDEPATH) $(DEFINES) -shared -o $(DESTDIR)/lib/libcppsockets.so $(OBJECTS)

clean:
	rm -rf *.o $(DESTDIR)/lib/libcppsockets.so *.d *.gcov *.gcda *.gcno
