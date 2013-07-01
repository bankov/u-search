# -*- makefile -*-
include ../config.mk
all: pdf

doxygen: footer.tex header.tex u-search images
	doxygen u-search
	cp -r images latex

pdf: doxygen
	cd latex && make
	cp latex/refman.pdf u-search.pdf

clean:
	rm -rf latex html
