# -*- makefile -*-
TARGET:=doc
include ../config.mk

doxygen: footer.tex header.tex u-search
	doxygen u-search

copyfiles: images
	cp -r images latex

parse: doxygen tex-parser.py
	python tex-parser.py latex/refman.tex

$(TARGET): parse copyfiles
	cd latex && pdflatex refman && pdflatex refman
	cp latex/refman.pdf u-search.pdf

clean:
	rm -rf latex html

.PHONY: parse copyfiles doxygen
