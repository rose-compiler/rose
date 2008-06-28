include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

INCLUDES = $(ROSE_INCLUDES)

bin_PROGRAMS = bugSeeding

bugSeeding_SOURCES = bugSeeding.C bugSeedingSupport.C

LDADD = $(LIBS_WITH_RPATH) $(ROSE_LIBS)

clean-local:
	rm -f *.o bugSeeding rose_*.C introduction.tex design.tex appendix.tex

EXTRA_DIST = bugSeedingSupport.h introduction.tex design.tex appendix.tex

link_LaTeX_files:
#	test -f bugSeeding.tex || ln -s $(top_srcdir)/projects/bugSeeding/bugSeeding.tex bugSeeding.tex
	test -f introduction.tex || ln -s $(top_srcdir)/projects/bugSeeding/introduction.tex introduction.tex
	test -f design.tex || ln -s $(top_srcdir)/projects/bugSeeding/design.tex design.tex
	test -f appendix.tex || ln -s $(top_srcdir)/projects/bugSeeding/appendix.tex appendix.tex
	test -f fixme.sty || ln -s $(top_srcdir)/docs/Rose/fixme.sty fixme.sty

docs: bugSeeding.tex link_LaTeX_files
	pdflatex bugSeeding.tex


