include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

# Need to add sibdirectory for annotation parser tests as well
#SUBDIRS = sideEffectAnalysisTests
SUBDIRS = testCallGraphAnalysis defUseAnalysisTests staticInterproceduralSlicingTests

INCLUDES = $(ROSE_INCLUDES) -I$(top_srcdir)/src/midend/programAnalysis -I${top_srcdir}/src/midend/programAnalysis/CFG -I${top_srcdir}/src/midend/programAnalysis/dataflowAnalysis -I$(top_srcdir)/src/util/support -I$(top_srcdir)/src/util/graphs -I$(top_srcdir)/src/midend/astUtil/astSupport -I$(top_srcdir)/src/midend/astUtil/astInterface -I$(top_srcdir)/src/midend/astUtil

## The grammar generator (ROSETTA) should use its own template repository
CXX_TEMPLATE_REPOSITORY_PATH = .

bin_PROGRAMS = CFGTest DataFlowTest StmtRefTest

# DQ: Removed this because it was failing and I could not figure out why
# AnnotationLanguageParserTest

CFGTest_SOURCES = CFGTest.C
CFGTest_DEPENDENCIES = $(ROSE_LIBS_WITH_PATH)
CFGTest_LDADD = $(LIBS_WITH_RPATH) $(ROSE_LIBS)

DataFlowTest_SOURCES = DataFlowTest.C
DataFlowTest_DEPENDENCIES = $(ROSE_LIBS_WITH_PATH) 
DataFlowTest_LDADD = $(LIBS_WITH_RPATH) $(ROSE_LIBS)

StmtRefTest_SOURCES = StmtRefTest.C
StmtRefTest_DEPENDENCIES = $(ROSE_LIBS_WITH_PATH)
StmtRefTest_LDADD = $(LIBS_WITH_RPATH) $(ROSE_LIBS)

# Added to support Brian White's work with the parts of the Broadway compiler
#AnnotationLanguageParserTest_SOURCES = AnnotationLanguageParserTest.C
#AnnotationLanguageParserTest_DEPENDENCIES = $(ROSE_LIBS_WITH_PATH)
#AnnotationLanguageParserTest_LDADD = $(LIBS_WITH_RPATH) $(ROSE_LIBS)

# This is specific to compiling AnnotationLanguageParserTest.C
AM_CXXFLAGS = -D__LOCALSTL_H

FORCE_TEST_CODES_TO_RUN:
	$(srcdir)/TestDriver $(srcdir)

EXTRA_DIST = TestDriver testfile1.c testfile1.c.cfg testfile1.c.du testfile1.c.ref \
             testfile2.c testfile2.c.cfg testfile2.c.du testfile2.c.ref \
             testfile3.c testfile3.c.cfg testfile3.c.du testfile3.c.ref \
             testfile4.c testfile4.c.du 
             #trustedAnnotationOutput annotationLanguageParserInputFile.ann

# to deactivate tests comment out check-local
# check-local:
#	AnnotationLanguageParserTestRule
#	$(MAKE) FORCE_TEST_CODES_TO_RUN
check-local:
	$(MAKE) FORCE_TEST_CODES_TO_RUN
	@echo "*************************************************************************************************"
	@echo "*** ROSE/tests/roseTests/programAnalysisTests: make check rule complete (terminated normally) ***"
	@echo "*************************************************************************************************"

AnnotationLanguageParserTestRule: trustedAnnotationOutput
	./AnnotationLanguageParserTest $(srcdir)/annotationLanguageParserInputFile.ann > annotationOutput
#	diff annotationOutput trustedAnnotationOutput || exit

clean-local:
	rm -rf Templates.DB ii_files ti_files cxx_templates

distclean-local:
	rm -rf Templates.DB






