include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

INCLUDES = $(ROSE_INCLUDES) -I$(top_srcdir)/projects/backstroke
LDADD    = $(LIBS_WITH_RPATH) $(ROSE_LIBS)

noinst_PROGRAMS = testCodeBuilderTest
testCodeBuilderTest_SOURCES = testCodeBuilderTestMain.C
testCodeBuilderTest_LDADD = $(top_builddir)/projects/backstroke/libBackstroke.a \
			$(top_builddir)/projects/backstroke/testCodeGeneration/libBackstrokeTestCodeBuilder.a \
			$(top_builddir)/projects/backstroke/restrictedLanguage/libBackstrokeRestrictedLanguage.a \
			$(top_builddir)/projects/backstroke/normalizations/libBackstrokeNormalizations.a \
			$(top_builddir)/projects/backstroke/pluggableReverser/libBackstrokePluggableReverser.a \
			$(top_builddir)/projects/backstroke/utilities/libBackstrokeUtilities.a \
			$(LIBS_WITH_RPATH) $(ROSE_LIBS)

EXTRA_DIST = 

CLEANFILES =

TESTCODE_INCLUDES = -I$(top_srcdir)/projects/backstroke/tests/pluggableReverserTest/test -I/opt/local/include \
	-I$(top_srcdir)/projects/backstroke/testCodeGeneration/include

check-local:
#@$(MAKE) $(TEST_Objects)
	@echo "*********************************************************************************************************"
	@echo "* ROSE/projects/backstroke/tests/testCodeBuilderTest: make check rule complete (terminated normally) *"
	@echo "*********************************************************************************************************"

test: FORCE
	./testCodeBuilderTest $(TESTCODE_INCLUDES)
	g++ test.C -o test $(TESTCODE_INCLUDES)
	./test

FORCE:
