TEMPLATE = subdirs
SUBDIRS +=          \
    cppsockets      \
    data-storage    \
    spider          \
    tests
HEADERS += common.h
OTHER_FILES +=      \
    servers.dat     \
    database.dat    \
    README          \
    config.mk       \
    Makefile        \
    AUTHORS
