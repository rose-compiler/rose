include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

AM_CPPFLAGS = $(ROSE_INCLUDES) -I$(top_srcdir)/projects/backstroke
LDADD    = $(LIBS_WITH_RPATH) $(ROSE_LIBS)

# Build the function extraction translator
noinst_PROGRAMS = reverserTest

reverserTest_SOURCES =  CFGReverserMain.C
reverserTest_LDADD =	$(top_builddir)/projects/backstroke/libBackstroke.la \
		 	$(top_builddir)/projects/backstroke/restrictedLanguage/libBackstrokeRestrictedLanguage.la \
			$(top_builddir)/projects/backstroke/pluggableReverser/libBackstrokePluggableReverser.la \
			$(top_builddir)/projects/backstroke/normalizations/libBackstrokeNormalizations.la \
			$(top_builddir)/projects/backstroke/utilities/libBackstrokeUtilities.la \
			$(LIBS_WITH_RPATH) $(ROSE_LIBS)

ROSE_FLAGS = --edg:no_warnings -w -rose:verbose 0 -rose:detect_dangling_pointers 1

EXTRA_DIST = test_Basic.C test_Basic2.C test_Basic3.C

TESTCODE_AM_CPPFLAGS = -I$(top_srcdir)/projects/backstroke/pluggableReverser

TESTCODES_REQUIRED_TO_PASS = \
test_Basic1.C \
test_Basic2.C \
test_Basic3.C

CLEANFILES = 

TEST_Objects = ${TESTCODES_REQUIRED_TO_PASS:.C=.o}

$(TEST_Objects):reverserTest
	@cp $(top_srcdir)/projects/backstroke/tests/cfgReverseCodeGenerator/testcases/$(@:.o=.C) $(@:.o=.C)
	./reverserTest $(ROSE_FLAGS) $(TESTCODE_AM_CPPFLAGS) -c $(@:.o=.C)

check-local:
	@$(MAKE) $(TEST_Objects)
	@echo "********************************************************"
	@echo "*** ROSE/projects/backstroke/cfg: make check : PASS  ***"
	@echo "********************************************************"

clean-local:
	rm -f *.o *.C

