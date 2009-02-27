PDFS=cfgconstruction.pdf pig-doc.pdf pag-interface.pdf manual.pdf
nobase_dist_data=$(PDFS)
nodist_EXTRA_data=manual/manual_vars.tex.in

dist-local: docs

.PHONY: docs
docs:
    for f in $(PDFS); do \
        dir=`basename $$f .pdf` ; \
        cp -r $(top_srcdir)/docs/$$dir $$dir.build ; \
        chmod -R u+w $$dir.build ; \
        cp -f manual/manual_vars.tex $$dir.build; \
        cd $$dir.build ; \
        $(MAKE) -f ../Makefile $$f; mv $$f .. ; \
        cd .. ; \
    done

clean-local:
    for f in $(PDFS); do \
       rm -rf `basename $$f .pdf`.build ; \
    done
    rm -f $(PDFS)


BIBTEX = :

# very poor man's pseudo-autoconf stuff
#.PHONY: manual_vars.tex
#manual_vars.tex: manual_vars.tex.in
#   sed -e 's/[@]satire_version[@]/$(SATIRE_VERSION)/g' < $< > $@

%.pdf: %.tex
    pdflatex $< < /dev/null || (rm -f $@ && false)
    $(BIBTEX) $*
    pdflatex $< < /dev/null || (rm -f $@ && false)
    while grep -q "Rerun to get cross-references right." $*.log; do \
        pdflatex $< < /dev/null || (rm -f $@ && exit 1) ; \
    done

