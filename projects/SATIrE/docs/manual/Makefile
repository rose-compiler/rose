BIBTEX = :

all: pdf

pdf: satire_manual.pdf

# very poor man's pseudo-autoconf stuff
.PHONY: manual_vars.tex
manual_vars.tex: manual_vars.tex.in
	sed -e 's/[@]satire_version[@]/$(SATIRE_VERSION)/g' < $< > $@

satire_manual.tex: manual_vars.tex

%.pdf: %.tex
	pdflatex $< < /dev/null || (rm -f $@ && false)
	$(BIBTEX) $*
	pdflatex $< < /dev/null || (rm -f $@ && false)
	while grep -q "Rerun to get cross-references right." $*.log; do \
		pdflatex $< < /dev/null || (rm -f $@ && exit 1) ; \
	done

.PHONY: clean
clean:
	rm -f *.pdf *.toc *.aux *.log
