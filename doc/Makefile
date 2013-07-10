# -*- makefile -*-
include ../config.mk
all: pdf

doxygen: footer.tex header.tex u-search images
	doxygen u-search
	cp -r images latex

parse: doxygen tex-parser.py
	python tex-parser.py latex/refman.tex

pdf: parse
	cd latex && make
	cp latex/refman.pdf u-search.pdf

clean:
	rm -rf latex html
