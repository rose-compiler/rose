PDFLATEX = env TEXINPUTS=$(srcdir):$(top_srcdir)/docs/Rose:.: pdflatex --interaction=batchmode

include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

INCLUDES = $(ROSE_INCLUDES)

bin_PROGRAMS = bugSeeding

bugSeeding_SOURCES = bugSeeding.C bugSeedingSupport.C bufferOverFlowSecurityFlaw.C

# bugSeeding_HEADERS = bugSeeding.h bugSeedingSupport.h bufferOverFlowSecurityFlaw.h

LDADD = $(LIBS_WITH_RPATH) $(ROSE_LIBS)

clean-local:
	rm -f *.o bugSeeding rose_*.C bugSeeding.pdf *.log *.aux *.out *.dot

EXTRA_DIST = introduction.tex design.tex appendix.tex \
   bugSeeding.h bugSeedingSupport.h \
   bufferOverFlowSecurityFlaw.h

docs: bugSeeding.tex
	$(PDFLATEX) bugSeeding.tex

testBufferOverflow_arrayIndexing: bugSeeding
	./bugSeeding -rose:verbose 0 -c $(srcdir)/inputCode_bufferOverflow_arrayIndexing.C


