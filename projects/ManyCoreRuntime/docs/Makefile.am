include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

# soft links to required files (.sty, .tex, .png. .c etc)
docs: ManycoreRuntime.tex
	test -f fixme.sty || ln -s $(top_srcdir)/docs/Rose/fixme.sty fixme.sty
	test -f html.sty || ln -s $(top_srcdir)/docs/Rose/html.sty html.sty
	sh -c '\
	for file in `ls $(srcdir)` ;\
	do \
	        test -f $${file} || ln -s $(srcdir)/$${file} $${file} ;\
	done ;\
	'
	@rm -f Makefile.*
	pdflatex --interaction=batchmode ManycoreRuntime.tex 
	bibtex ManycoreRuntime
	pdflatex --interaction=batchmode ManycoreRuntime.tex
	pdflatex --interaction=batchmode ManycoreRuntime.tex

EXTRA_DIST = ManycoreRuntime.tex \
	introduction.tex 

clean-local:
	find . -type l -maxdepth 1 -exec rm {} \;
	rm -f *.log *.out *.aux *.toc *.lox *.blg *.bbl

CLEANFILES = ManycoreRuntime.pdf 
