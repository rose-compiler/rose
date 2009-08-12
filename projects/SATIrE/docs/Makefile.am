PDFS=cfgconstruction.pdf pig-doc.pdf pag-interface.pdf manual.pdf termite.pdf
nobase_dist_data=$(PDFS)
nodist_EXTRA_data=manual/manual_vars.tex.in

dist-local: docs docs-termite

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

clean-local: clean-termite
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

# ---------------------------------------------------------------------
# TERMITE documentation
# ---------------------------------------------------------------------

SRCFILES= \
  $(top_srcdir)/src/termite/termite/ast_transform.pl \
  $(top_srcdir)/src/termite/termite/ast_properties.pl \
  $(top_srcdir)/src/termite/termite/ast_walk.pl \
  $(top_srcdir)/src/termite/termite/callgraph.pl \
  $(top_srcdir)/src/termite/termite/utils.pl \
  $(top_srcdir)/src/termite/termite/loops.pl \
  $(top_srcdir)/tools/loopbounds/loopbounds.pl

# underscores confuse the machina!
INPUTFILES=$(notdir $(subst _,,$(SRCFILES)))
$(INPUTFILES): $(SRCFILES)
	for f in $(SRCFILES); do \
		cp $$f `basename $$f |sed s/_//g` ; \
	done
TEXFILES=$(patsubst %.pl, %.tex,$(INPUTFILES))
GARBAGE=$(patsubst %.tex, %.aux,$(TEXFILES))
MYTEXINPUT=$(SWI_PROLOG_HOME_DIR)/library/pldoc:$(SWI_PROLOG_HOME_DIR)/library/ltx2htm:/opt/gupu/src/pl-57x/packages/ltx2htm:/opt/gupu/src/pl-57x/man
LTX2HTM=latex2html # this is part of SWI-Prolog (not to be confused!)


docs-termite: HTML termite.pdf mostlyclean

clean-termite: 
	rm -f termite-pdf.dvi termite-pdf.idx termite-pdf.log termite-pdf.aux \
	      termite-pdf.out termite-pdf.toc $(TEXFILES) $(INPUTFILES) \
	      termite-html.tex termite-pdf.tex
	rm -rf termite.pdf termite-html

%.tex: %.pl
	export TERMITE_LIB=$(datarootdir)/termite && \
	echo "use_module(library(doc_latex)), \
	      consult('$*'), \
	      open('$@', write, _, [alias(wstrm)]),\
              latex_for_file('$<', wstrm, \
	                     [stand_alone(false),public_only(true)]), \
	      close(wstrm)." | pl

termite-pdf.tex: termite.tex
	sed s/%%INPUTS%%/$(patsubst %.tex,\\\\input{%}\,$(TEXFILES))/ $< >$@

termite-html.tex: termite-pdf.tex
	sed s/xPDF/xHTML/ $< >$@

HTML: termite-html.tex $(TEXFILES)
	TEXINPUTS=$(MYTEXINPUT):$$TEXINPUTSl; $(LTX2HTM) $<

termite.pdf: termite-pdf.tex $(TEXFILES)
	env TEXINPUTS=$(MYTEXINPUT):$$TEXINPUTS pdflatex $<
	env TEXINPUTS=$(MYTEXINPUT):$$TEXINPUTS pdflatex $<
	env TEXINPUTS=$(MYTEXINPUT):$$TEXINPUTS pdflatex $<
	mv termite-pdf.pdf $@

