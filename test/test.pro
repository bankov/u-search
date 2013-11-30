TEMPLATE = subdirs

SUBDIRS +=                  \
    cppsockets-test         \
    datastorage-test        \
    spider-test             \
    serverqueue-test        \
    full-test

OTHER_FILES += testing.sh   \
               Makefile
