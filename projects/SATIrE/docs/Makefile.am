PDFS=cfgconstruction.pdf pig-doc.pdf pag-interface.pdf manual.pdf
dist_doc_DATA=$(PDFS)
nodist_EXTRA_DATA=manual/manual_vars.tex.in

EXTRA_DIST= \
        satire_cfgconstruction.pdf \
	satire_overview.pdf \
	satire_attributes_auxiliary_functions \
        termite.tex

.PHONY: docs HTML clean-termite
docs: $(PDFS) docs-termite

$(PDFS): 
	dir=`basename $@ .pdf` && \
	  cp -r $(top_srcdir)/docs/$$dir $$dir.build && \
	  chmod -R u+w $$dir.build && \
	  cp -f $(builddir)/manual/manual_vars.tex $$dir.build && \
	  cd $$dir.build && $(MAKE) -f ../Makefile $$dir.dvi && \
	  dvipdf $$dir.dvi ../$@

clean-local: clean-termite
	for f in $(PDFS); do \
		rm -rf `basename $$f .pdf`.build ; \
	done
	rm -f $(PDFS)


BIBTEX = :

%.dvi: %.tex # We can't use pdlatex to include *.ps graphics
	latex $< < /dev/null || (rm -f $@ && false)
	$(BIBTEX) $*
	latex $< < /dev/null || (rm -f $@ && false)
	while grep -q "Rerun to get cross-references right." $*.log; do \
		latex $< < /dev/null || (rm -f $@ && exit 1) ; \
	done

# ---------------------------------------------------------------------
# TERMITE documentation
# ---------------------------------------------------------------------

TERMITE_DIR=$(top_srcdir)/src/termite/termite
SRCFILES= \
  $(TERMITE_DIR)/asttransform.pl \
  $(TERMITE_DIR)/astproperties.pl \
  $(TERMITE_DIR)/astwalk.pl \
  $(TERMITE_DIR)/callgraph.pl \
  $(TERMITE_DIR)/utils.pl \
  $(TERMITE_DIR)/loops.pl \
  $(top_srcdir)/tools/loopbounds/loopbounds.pl \
  $(TERMITE_DIR)/termlint.pl

# underscores confuse the machina!
INPUTFILES=$(notdir $(subst _,,$(SRCFILES)))
$(INPUTFILES): $(SRCFILES)
	for f in $(SRCFILES); do \
		cp --no-preserve=mode -f $$f `basename $$f |sed s/_//g` ; \
	done
TEXFILES=$(patsubst %.pl, %.tex,$(INPUTFILES))
GARBAGE=$(patsubst %.tex, %.aux,$(TEXFILES))
MYTEXINPUT=$(SWI_PROLOG_HOME_DIR)/library/pldoc:$(SWI_PROLOG_HOME_DIR)/library/ltx2htm:/opt/gupu/src/pl-57x/packages/ltx2htm:/opt/gupu/src/pl-57x/man
LTX2HTM=/usr/local/mstools/bin/latex2html # this is part of SWI-Prolog (not to be confused!)

if WITH_SWI_PROLOG  # --------------------------------------------------

docs-termite: HTML termite.pdf mostlyclean
dist_doc_DATA += \
	termite.pdf \
	$(TERMITE_HTML)

$(TERMITE_HTML): HTML

TERMITE_HTML = \
	termite-html/Contents.html \
	termite-html/DocIndex.html \
	termite-html/home.gif \
	termite-html/index.gif \
	termite-html/index.html \
	termite-html/info.gif \
	termite-html/next.gif \
	termite-html/prev.gif \
	termite-html/sec-1.html \
	termite-html/sec-1.1.html \
	termite-html/sec-2.html \
	termite-html/sec-2.1.html \
	termite-html/sec-3.html \
	termite-html/up.gif \
	termite-html/yellow_pages.gif \
	$(patsubst %.pl, termite-html/%.html,$(INPUTFILES))

clean-termite: 
	rm -f termite-pdf.dvi termite-pdf.idx termite-pdf.log termite-pdf.aux \
	      termite-pdf.out termite-pdf.toc $(TEXFILES) \
              termite-pdf.ind termite-pdf.ilg $(INPUTFILES) \
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
	      close(wstrm)." | pl -q

# Sadly, ltx2html does not understand the listings package...
termspec.tex: $(top_srcdir)/src/termite/termite/termite_spec.pl
	sed -e '1,/Grammar rules./ c \\\\begin{verbatim}' \
	    -e 's/^.*::=/\\end{verbatim}\n\\begin{verbatim}\n\0/g' \
	    -e 's/^%\(.*\)/\\end{verbatim}\n\1\\\\\n\\begin{verbatim}\n/g' \
	    -e 's/---\(.*\)--- *\\\\/\\subsection{\1}/g' \
             $< | \
	awk 'BEGIN {FS="\n"} {                                                                 if ($$1 ~ /\\begin{verbatim}/) {                                                    l1=$$1; do getline; while ($$1 ~ /^$$/);                                           if ($$1 !~ /\\end{verbatim}/) { print l1; print }                             } else print }' \
	    | grep . >$@ 
	echo '\end{verbatim}' >>$@

c2term.txt:
	make -C $(top_builddir)/src/termite c2term
	echo '\begin{verbatim}' >$@
	$(top_builddir)/src/termite/c2term &>__tmp || true
	cat __tmp | sed -e 's!$(bindir)/!!' >>$@
	rm -f __tmp
	echo '\end{verbatim}' >>$@

term2c.txt: 
	make -C $(top_builddir)/src/termite term2c
	echo '\begin{verbatim}' >$@
	$(top_builddir)/src/termite/term2c &>__tmp || true
	cat __tmp | sed -e 's!$(bindir)/!!' >>$@
	rm -f __tmp
	echo '\end{verbatim}' >>$@

termite-pdf.tex: termite.tex $(SRCFILES) termspec.tex c2term.txt term2c.txt
	sed -e s!%%INPUTS%%!$(patsubst %.tex,\\\\input{%}\\\\clearpage\,$(TEXFILES))! \
	    -e '/GRAMMAR/ r termspec.tex' \
	    -e '/C2TERM/ r c2term.txt' \
	    -e '/TERM2C/ r term2c.txt' \
	    $< >$@

termite-html.tex: termite-pdf.tex
	sed s/xPDF/xHTML/ $< >$@

HTML: termite-html.tex $(TEXFILES)
	TEXINPUTS=$(MYTEXINPUT):$$TEXINPUTS
	$(LTX2HTM) $<

termite.pdf: termite-pdf.tex $(TEXFILES)
	env TEXINPUTS=$(MYTEXINPUT):$$TEXINPUTS pdflatex $<
	makeindex $(patsubst %.tex,%,$<)
	env TEXINPUTS=$(MYTEXINPUT):$$TEXINPUTS pdflatex $<
	env TEXINPUTS=$(MYTEXINPUT):$$TEXINPUTS pdflatex $<
	mv termite-pdf.pdf $@

else # -----------------------------------------------------------------

docs-termite:
	@echo "** WARNING: Termite docs can only be built when SWI-Prolog is used."

endif # ----------------------------------------------------------------
