# add the base name of your latex document here. you're done!
DOCUMENTS = pig-doc

DVIS = $(addsuffix .dvi, $(DOCUMENTS))
PDFS = $(addsuffix .pdf, $(DOCUMENTS))

all: pdfs

dvis: $(DVIS)

pdfs: $(PDFS)

%.dvi: %.tex
	latex $<

%.pdf: %.dvi
	dvipdfm $<

.PHONY: clean
clean:
	rm -f *.aux *.dvi *.log *.pdf *.bbl *.blg
