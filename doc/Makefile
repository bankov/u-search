# -*- makefile -*-
all: doc

doc: footer.tex header.tex u-search images
	doxygen u-search
	cp -r images latex/
	cd latex && xelatex refman.tex && makeindex refman.idx && xelatex refman.tex
	cp latex/refman.pdf u-search.pdf

clean:
	rm -rf *.aux *.out *.log latex

