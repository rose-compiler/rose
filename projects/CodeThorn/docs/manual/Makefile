all: codethorn.pdf

clean:
	rm -f *.aux *.bbl *.blg *.dvi *.log

cleaner: clean
	rm -f codethorn.pdf

codethorn.pdf: codethorn.tex codethorn.bib

%.pdf: %.tex %.bib
	if test -n "`which rubber`"; then \
		rubber -Wall --pdf $<; \
	else \
		pdflatex -halt-on-error $< || exit 1; \
		bibtex $(patsubst %.tex, %, $<)  || exit 1; \
		pdflatex -halt-on-error $< || exit 1; \
		pdflatex -halt-on-error $< || exit 1; \
	fi
