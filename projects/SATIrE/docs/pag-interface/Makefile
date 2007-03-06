# add the base name of your latex document here. you're done!
DOCUMENTS = pag-interface

DVIS = $(addsuffix .dvi, $(DOCUMENTS))
PDFS = $(addsuffix .pdf, $(DOCUMENTS))

all: pdfs

dvis: $(DVIS)

pdfs: $(PDFS)

%.dvi: %.tex
	( \
	latex $<; \
	while \grep -q "Rerun to get cross-references right." $(<:.tex=.log); \
	do \
		latex $<; \
	done \
	)

%.pdf: %.tex
	( \
	pdflatex $<; \
	while \grep -q "Rerun to get cross-references right." $(<:.tex=.log); \
	do \
		pdflatex $<; \
	done \
	)

.PHONY: clean
clean:
	rm -f *.aux *.dvi *.log *.pdf *.bbl *.blg
