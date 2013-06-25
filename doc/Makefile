# -*- makefile -*-
all: pdf

doxygen: footer.tex header.tex u-search images
	doxygen u-search
	cp -r images latex

pdf: doxygen
	cd latex && xelatex refman.tex && makeindex refman.idx && xelatex refman.tex
	cp latex/refman.pdf u-search.pdf

clean:
	rm -rf *.aux *.out *.log latex html
