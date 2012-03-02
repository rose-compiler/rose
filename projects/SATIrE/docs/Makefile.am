if WITH_SWI_PROLOG

TERMITE_PDF=termite.pdf
TERMITE_HTML = \
	termite-html/Contents.html \
	termite-html/DocIndex.html \
	termite-html/index.html \
	termite-html/info.gif \
	termite-html/sec-1.html \
	termite-html/sec-1.1.html \
	termite-html/sec-2.html \
	termite-html/sec-2.1.html \
	termite-html/sec-3.html \
	termite-html/folder-saved-search.png \
	termite-html/go-next.png \
	termite-html/go-up.png \
	termite-html/go-home.png \
	termite-html/go-previous.png \
	termite-html/internet-news-reader.png \
	$(patsubst %.pl, termite-html/%.html,$(INPUTFILES))
endif

PDFS=manual.pdf pig-doc.pdf $(TERMITE_PDF)
nobase_dist_doc_DATA = $(TERMITE_HTML)
dist_doc_DATA=$(PDFS)
nodist_EXTRA_DATA=manual/manual_vars.tex.in

EXTRA_DIST= \
	satire_attributes_auxiliary_functions.txt \
    termite.tex \
	fancychap.sty \
	fancyheadings.sty \
	logo.sty \
    plonline.sty \
    plpage.sty \
	pldoc.sty \
	html.sty url.sty \
	icons/folder-saved-search.png \
	icons/go-next.png \
	icons/go-up.png \
	icons/go-home.png \
	icons/go-previous.png \
	icons/internet-news-reader.png

.PHONY: docs HTML clean-termite
docs: $(PDFS) SATIrE-@PACKAGE_VERSION@-manual.pdf docs-termite

SATIrE-@PACKAGE_VERSION@-manual.pdf: manual.pdf
	cp manual.pdf SATIrE-@PACKAGE_VERSION@-manual.pdf

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
	rm -f SATIrE-@PACKAGE_VERSION@-manual.pdf

# pig-doc is not using bibtex, thus we make these to steps explicit here
# without invoking bibtex (you cannot invoke bibtex without bibliography)
pig-doc.dvi: pig-doc.tex # We can't use pdlatex to include *.ps graphics
	latex pig-doc < /dev/null || (rm -f $@ && false)
	latex pig-doc < /dev/null || (rm -f $@ && false)

BIBTEX = bibtex

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
  $(TERMITE_DIR)/markers.pl \
  $(top_srcdir)/tools/loopbounds/loopbounds.pl \
  $(TERMITE_DIR)/termlint.pl

# underscores confuse the machina!
INPUTFILES=$(notdir $(subst _,,$(SRCFILES)))
$(INPUTFILES): $(SRCFILES)
	for f in $(SRCFILES); do \
	  cat $$f | sed -e 's|\@PACKAGE_VERSION\@|@PACKAGE_VERSION@|' \
              >`basename $$f |sed s/_//g` ; \
	done
TEXFILES=$(patsubst %.pl, %.tex,$(INPUTFILES))
GARBAGE=$(patsubst %.tex, %.aux,$(TEXFILES))
MYTEXINPUT=$(srcdir)/ltx2htm:$(srcdir)

if WITH_SWI_PROLOG  # --------------------------------------------------

docs-termite: termite.pdf mostlyclean # MS: deactivated HTML here (target is still available)

$(TERMITE_HTML): HTML

clean-termite: 
	rm -f termite-pdf.dvi termite-pdf.idx termite-pdf.log termite-pdf.aux \
	      termite-pdf.out termite-pdf.toc $(TEXFILES) \
              termite-pdf.ind termite-pdf.ilg $(INPUTFILES) \
	      termite-html.tex termite-pdf.tex \
	      termspec.tex c2term.txt term2c.txt \
	      termite_spec.pl types.pl while2for.pl comefrom.pl
	rm -rf termite.pdf termite-html ltx2htm-inst ltx2htm-build

termite_spec.pl: $(top_srcdir)/src/termite/termite/termite_spec.pl
	cp --preserve=timestamps $< $@
types.pl: $(top_srcdir)/tools/loopbounds/types.pl
	cp --preserve=timestamps $< $@
while2for.pl: $(top_srcdir)/tools/loopbounds/while2for.pl
	cp --preserve=timestamps $< $@
comefrom.pl: $(top_srcdir)/tools/loopbounds/comefrom.pl
	cp --preserve=timestamps $< $@

%.tex: %.pl while2for.pl types.pl comefrom.pl termite_spec.pl
	export TERMITE_LIB=$(top_srcdir)/src/termite/termite && \
	echo "getenv('PWD', CurDir), \
	      asserta(library_directory(CurDir)), \
	      asserta(library_directory('$(srcdir)/ltx2htm')), \
	      asserta(library_directory('$(builddir)/ltx2htm-inst/lib/latex2html')), \
	      asserta(library_directory('$(srcdir)')), \
	      asserta(library_directory('$(srcdir)/pldoc')), \
	      getenv('TERMITE_LIB', TermitePath), \
	      asserta(library_directory(TermitePath)), \
	      use_module(library(pldoc)), \
	      use_module(library(pldoc_latex)). \
	      consult('$*'), \
	      open('$@', write, _, [alias(wstrm)]),\
              latex_for_file('$<', wstrm, \
	                     [stand_alone(false),public_only(true)]). \
	      close(wstrm)." | $(SWI_PL) -q

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

c2term.txt: Makefile
	make -C $(top_builddir)/src/termite c2term
	echo '\begin{verbatim}' >$@
	$(top_builddir)/src/termite/c2term &>__tmp1 || true
	cat __tmp1 | sed -e 's!^U.*c2term!Usage: c2term!' >>$@
	rm -f __tmp1
	echo '\end{verbatim}' >>$@

term2c.txt: Makefile
	make -C $(top_builddir)/src/termite term2c
	echo '\begin{verbatim}' >$@
	$(top_builddir)/src/termite/term2c &>__tmp2 || true
	cat __tmp2 | sed -e 's!^U.*term2c!Usage: term2c!' >>$@
	rm -f __tmp2
	echo '\end{verbatim}' >>$@

termite-pdf.tex: termite.tex $(SRCFILES) termspec.tex c2term.txt term2c.txt
	sed -e s!%%INPUTS%%!$(patsubst %.tex,\\\\input{%}\\\\clearpage\,$(TEXFILES))! \
	    -e '/GRAMMAR/ r termspec.tex' \
	    -e '/C2TERM/ r c2term.txt' \
	    -e '/TERM2C/ r term2c.txt' \
	    $< >$@

termite-html.tex: termite-pdf.tex
	sed s/xPDF/xHTML/ $< >$@

LTX2HTM=ltx2htm-inst/bin/latex2html
HTML: termite-html.tex $(TEXFILES) $(LTX2HTM)
	$(LTX2HTM) $<
	sed -i -e 's/yellow_pages\.gif/folder-saved-search.png/' \
	       -e 's/home\.gif/go-home.png/' \
	       -e 's/up\.gif/go-up.png/' \
	       -e 's/next\.gif/go-next.png/' \
	       -e 's/prev\.gif/go-previous.png/' \
	       -e 's/index\.gif/internet-news-reader.png/' \
	  termite-html/*.html
	cp $(srcdir)/icons/* termite-html/
	chmod u+rw termite-html/*

termite.pdf: termite-pdf.tex $(TEXFILES)
	env TEXINPUTS=$(MYTEXINPUT):$$TEXINPUTS pdflatex $<
	makeindex $(patsubst %.tex,%,$<)
	env TEXINPUTS=$(MYTEXINPUT):$$TEXINPUTS pdflatex $<
	env TEXINPUTS=$(MYTEXINPUT):$$TEXINPUTS pdflatex $<
	mv termite-pdf.pdf $@

# LaTeX -> HTML from SWI-Prolog
EXTRA_DIST += \
  ltx2htm/COPYING \
  ltx2htm/configure.in \
  ltx2htm/latex.cmd \
  ltx2htm/pl.pl \
  ltx2htm/url.sty \
  ltx2htm/ChangeLog \
  ltx2htm/latex2html.css \
  ltx2htm/pl.sty \
  ltx2htm/xpce.cmd \
  ltx2htm/Makefile.in \
  ltx2htm/doc2tex \
  ltx2htm/latex2html.in \
  ltx2htm/ps2gif.pl \
  ltx2htm/xpce.pl \
  ltx2htm/html.sty \
  ltx2htm/latex2html.pl \
  ltx2htm/psfile.c \
  ltx2htm/config.h.in \
  ltx2htm/ltx2htm.doc \
  ltx2htm/run.pl \
  ltx2htm/configure \
  ltx2htm/install-sh \
  ltx2htm/pl.cmd \
  ltx2htm/tex.c \
  ltx2htm/icons/home.gif \
  ltx2htm/icons/index.gif \
  ltx2htm/icons/info.gif \
  ltx2htm/icons/next.gif \
  ltx2htm/icons/prev.gif \
  ltx2htm/icons/up.gif \
  ltx2htm/icons/yellow_pages.gif \
  ltx2htm/pldoc_latex.pl \
  pldoc/doc_access.pl   \
  pldoc/doc_colour.pl	\
  pldoc/doc_html.pl	\
  pldoc/doc_htmlsrc.pl	\
  pldoc/doc_http.pl	\
  pldoc/doc_index.pl	\
  pldoc/doc_library.pl	\
  pldoc/doc_man.pl	\
  pldoc/doc_modes.pl	\
  pldoc/doc_process.pl	\
  pldoc/doc_register.pl	\
  pldoc/doc_search.pl	\
  pldoc/doc_util.pl	\
  pldoc/doc_wiki.pl	\
  pldoc/edit.gif	\
  pldoc/favicon.ico	\
  pldoc/hooks.pl	\
  pldoc/pldoc.css	\
  pldoc/pldoc.js	\
  pldoc/pldoc.pl	\
  pldoc/pldoc.sty	\
  pldoc/pllisting.css	\
  pldoc/reload.gif	\
  pldoc/source.gif	\
  pldoc/up.gif		\
  pldoc/zoomin.gif	\
  pldoc/zoomout.gif

$(LTX2HTM):
	cp -r $(srcdir)/ltx2htm ltx2htm-build
	chmod u+rw -R ltx2htm-build
	mkdir -p ltx2htm-inst/bin ltx2htm-inst/lib
	cd ltx2htm-build && sh configure --prefix=$(realpath $(builddir))/ltx2htm-inst
	make -C ltx2htm-build all install
	perl -pi -e "s|library\(\'pldoc/pldoc.sty\'\)|\'$(realpath $(srcdir))/pldoc/pldoc.sty\'|" ltx2htm-inst/lib/latex2html/pldoc.pl
	rm -f $(realpath $(builddir))/ltx2htm-inst/lib/latex2html/pl.pl

else # -----------------------------------------------------------------

docs-termite:
	@echo "** WARNING: Termite docs can only be built when SWI-Prolog is used."

endif # ----------------------------------------------------------------
