include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

# soft links to required files (.sty, .tex, .png. .c etc)
docs: autoTuning.tex
	test -f fixme.sty || ln -s $(top_srcdir)/docs/Rose/fixme.sty fixme.sty
	test -f html.sty || ln -s $(top_srcdir)/docs/Rose/html.sty html.sty
	sh -c '\
	for file in `ls $(srcdir)` ;\
	do \
	        test -f $${file} || ln -s $(srcdir)/$${file} $${file} ;\
	done ;\
	'
	@rm -f Makefile.*
	pdflatex --interaction=batchmode autoTuning.tex 
	bibtex autoTuning
	pdflatex --interaction=batchmode autoTuning.tex
	pdflatex --interaction=batchmode autoTuning.tex

EXTRA_DIST = autoTuning.tex \
	introduction.tex \
	preparation.tex \
	codeTriageTransformation.tex \
	empiricalTuning.tex \
	activeHarmony.tex \
	summary.tex \
	phase12.pdf \
	phase3.pdf \
	activeHarmony2.png \
	activeHarmony_offline.png \
	hpctoolkit-smg2000.png

clean-local:
	find . -type l -maxdepth 1 -exec rm {} \;
	rm -f *.log *.out *.aux *.toc *.lox

CLEANFILES = autoTuning.pdf 
