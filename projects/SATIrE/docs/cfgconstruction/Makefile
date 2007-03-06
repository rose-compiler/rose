# add the base name of your latex document here. you're done!
DOCUMENTS = cfgconstruction

DVIS = $(addsuffix .dvi, $(DOCUMENTS))
PDFS = $(addsuffix .pdf, $(DOCUMENTS))

all: pdfs

dvis: $(DVIS)

pdfs: $(PDFS)

%.dvi: %.tex
	latex $<

%.pdf: %.tex
	pdflatex $<

.PHONY: clean
clean:
	rm -f *.aux *.dvi *.log *.pdf *.bbl *.blg


