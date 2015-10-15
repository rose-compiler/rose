include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

PDFLATEX = env TEXINPUTS=$(srcdir):$(top_srcdir)/docs/Rose:.: pdflatex --interaction=batchmode

AM_CPPFLAGS = $(ROSE_INCLUDES)


bin_PROGRAMS = bugSeeding

bugSeeding_SOURCES = bugSeeding.C bugSeedingSupport.C bugSeedingAttributes.C bufferOverFlowSecurityFlaw.C

# bugSeeding_HEADERS = bugSeeding.h bugSeedingSupport.h bugSeedingAttributes.h bufferOverFlowSecurityFlaw.h

LDADD = $(LIBS_WITH_RPATH) $(ROSE_LIBS)

clean-local:
	rm -f *.o *~ bugSeeding bugSeeding.tex rose_*.C *.log *.aux *.out *.dot *.pdf
	rm -rf examples

EXTRA_DIST = CMakeLists.txt introduction.tex design.tex appendix.tex \
   bugSeeding.h bugSeedingSupport.h bugSeedingAttributes.h \
   cloneGrainularity.h  vulnerabilityDetection.h \
   bufferOverFlowSecurityFlaw.h inputCode_bufferOverflow_arrayIndexing.C


docFiles = \
   inputCode_bufferOverflow_arrayIndexing_C_before.pdf \
   inputCode_bufferOverflow_arrayIndexing_C_afterCloneGeneration.pdf \
   inputCode_bufferOverflow_arrayIndexing_C_afterIdentificationOfVulnerabilities.pdf \
   inputCode_bufferOverflow_arrayIndexing_C_afterSeedingOfSecurityFlaws.pdf

docs: bugSeeding.tex testBufferOverflow_arrayIndexing $(docFiles)
	$(PDFLATEX) bugSeeding.tex

testBufferOverflow_arrayIndexing: bugSeeding
	./bugSeeding -rose:verbose 0 -c $(srcdir)/inputCode_bufferOverflow_arrayIndexing.C

inputCode_bufferOverflow_arrayIndexing_C_before.pdf: inputCode_bufferOverflow_arrayIndexing.C_before.dot
	@echo "Generate postscript file (default name: test.pdf) from dot file ..."
	$(top_srcdir)/scripts/dotgenLetterPDF inputCode_bufferOverflow_arrayIndexing.C_before.dot inputCode_bufferOverflow_arrayIndexing_C_before.pdf

inputCode_bufferOverflow_arrayIndexing_C_afterCloneGeneration.pdf: inputCode_bufferOverflow_arrayIndexing.C_afterCloneGeneration.dot
	@echo "Generate postscript file (default name: test.pdf) from dot file ..."
	$(top_srcdir)/scripts/dotgenLetterPDF inputCode_bufferOverflow_arrayIndexing.C_afterCloneGeneration.dot inputCode_bufferOverflow_arrayIndexing_C_afterCloneGeneration.pdf

inputCode_bufferOverflow_arrayIndexing_C_afterIdentificationOfVulnerabilities.pdf: inputCode_bufferOverflow_arrayIndexing.C_afterIdentificationOfVulnerabilities.dot
	@echo "Generate postscript file (default name: test.pdf) from dot file ..."
	$(top_srcdir)/scripts/dotgenLetterPDF inputCode_bufferOverflow_arrayIndexing.C_afterIdentificationOfVulnerabilities.dot inputCode_bufferOverflow_arrayIndexing_C_afterIdentificationOfVulnerabilities.pdf

inputCode_bufferOverflow_arrayIndexing_C_afterSeedingOfSecurityFlaws.pdf: inputCode_bufferOverflow_arrayIndexing.C_afterSeedingOfSecurityFlaws.dot
	@echo "Generate postscript file (default name: test.pdf) from dot file ..."
#	$(top_srcdir)/scripts/dotgenLetterPDF inputCode_bufferOverflow_arrayIndexing.C_afterSeedingOfSecurityFlaws.dot inputCode_bufferOverflow_arrayIndexing_C_afterSeedingOfSecurityFlaws.pdf
	$(top_srcdir)/scripts/dotgenLetterPDF inputCode_bufferOverflow_arrayIndexing.C_afterSeedingOfSecurityFlaws.dot inputCode_bufferOverflow_arrayIndexing_C_afterSeedingOfSecurityFlaws.pdf


dotFiles = \
   inputCode_bufferOverflow_arrayIndexing.C_before.dot \
   inputCode_bufferOverflow_arrayIndexing.C_afterCloneGeneration.dot \
   inputCode_bufferOverflow_arrayIndexing.C_afterIdentificationOfVulnerabilities.dot \
   inputCode_bufferOverflow_arrayIndexing.C_afterSeedingOfSecurityFlaws.dot

$(dotFiles) : testBufferOverflow_arrayIndexing

check-local: testBufferOverflow_arrayIndexing

copy:
	rm -rf examples
	mkdir examples
	cp $(srcdir)/*.[hC] examples
	cp *.dot examples
	cp *.pdf examples
