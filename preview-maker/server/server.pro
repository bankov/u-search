include(../../common.pro)

TEMPLATE = app

CONFIG -= qt

TARGET = server

SOURCES +=                  \
    main.cpp                \
    task-listener.cpp       \
    threadobject.cpp        \
    broadcastlistener.cpp

HEADERS +=              \
    data_structure.h    \
    task-listener.h     \
    threadobject.h      \
    broadcastlistener.h

unix: LIBS += -lpthread

DESTDIR = $$DESTDIR/bin

