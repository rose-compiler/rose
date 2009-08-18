PDFS=cfgconstruction.pdf pig-doc.pdf pag-interface.pdf manual.pdf
nobase_dist_data=$(PDFS)  termite.pdf
nodist_EXTRA_data=manual/manual_vars.tex.in

dist-local: docs

.PHONY: docs
docs: docs-termite
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

TERMITE_DIR=$(top_srcdir)/src/termite/termite
SRCFILES= \
  $(TERMITE_DIR)/ast_transform.pl \
  $(TERMITE_DIR)/ast_properties.pl \
  $(TERMITE_DIR)/ast_walk.pl \
  $(TERMITE_DIR)/callgraph.pl \
  $(TERMITE_DIR)/utils.pl \
  $(TERMITE_DIR)/loops.pl \
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
              termite-pdf.ind termite-pdf.ilg \
	      termite-html.tex termite-pdf.tex \
	      termspec.tex c2term.txt term2c.txt
	rm -rf termite.pdf termite-html

%.tex: %.pl
	export TERMITE_LIB=$(datarootdir)/termite && \
	echo " use_module(library(doc_latex)), \
	      consult('$*'), \
	      open('$@', write, _, [alias(wstrm)]),\
              latex_for_file('$<', wstrm, \
	                     [stand_alone(false),public_only(true)]), \
	      close(wstrm)." | pl

# Sadly, ltx2html does not understand the listings package...
termspec.tex: $(top_srcdir)/src/termite/termite_spec.pl
	sed -e '1,/Grammar rules./ c \\\\begin{verbatim}' \
	    -e 's/^.*::=/\\end{verbatim}\n\\begin{verbatim}\n\0/g' \
	    -e 's/^%\(.*\)/\\end{verbatim}\n\1\\\\\n\\begin{verbatim}\n/g' \
	    -e 's/---\(.*\)--- *\\\\/\\subsection{\1}/g' \
             $< | \
	awk 'BEGIN {FS="\n"} {                                                                 if ($$1 ~ /\\begin{verbatim}/) {                                                    l1=$$1; do getline; while ($$1 ~ /^$$/);                                           if ($$1 !~ /\\end{verbatim}/) { print l1; print }                             } else print }' \
	    | grep . >$@ 
	echo '\end{verbatim}' >>$@

c2term.txt: $(bindir)/c2term
	echo '\begin{verbatim}' >$@
	$< &>__tmp || true
	cat __tmp | sed -e 's!$(bindir)/!!' >>$@
	rm -f __tmp
	echo '\end{verbatim}' >>$@

term2c.txt: $(bindir)/term2c
	echo '\begin{verbatim}' >$@
	$< &>__tmp || true
	cat __tmp | sed -e 's!$(bindir)/!!' >>$@
	rm -f __tmp
	echo '\end{verbatim}' >>$@

termite-pdf.tex: termite.tex $(SRCFILES) termspec.tex c2term.txt term2c.txt
	sed -e s/%%INPUTS%%/$(patsubst %.tex,\\\\input{%}\\\\clearpage\,$(TEXFILES))/ \
	    -e '/GRAMMAR/ r termspec.tex' \
	    -e '/C2TERM/ r c2term.txt' \
	    -e '/TERM2C/ r term2c.txt' \
	    $< >$@

termite-html.tex: termite-pdf.tex
	sed s/xPDF/xHTML/ $< >$@

HTML: termite-html.tex $(TEXFILES)
	TEXINPUTS=$(MYTEXINPUT):$$TEXINPUTSl; $(LTX2HTM) $<

termite.pdf: termite-pdf.tex $(TEXFILES)
	env TEXINPUTS=$(MYTEXINPUT):$$TEXINPUTS pdflatex $<
	makeindex $(patsubst %.tex,%,$<)
	env TEXINPUTS=$(MYTEXINPUT):$$TEXINPUTS pdflatex $<
	env TEXINPUTS=$(MYTEXINPUT):$$TEXINPUTS pdflatex $<
	mv termite-pdf.pdf $@

