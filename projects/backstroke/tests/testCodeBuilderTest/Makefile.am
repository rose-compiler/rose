include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

INCLUDES = $(ROSE_INCLUDES) -I$(top_srcdir)/projects/backstroke
LDADD    = $(LIBS_WITH_RPATH) $(ROSE_LIBS)

noinst_PROGRAMS = testCodeBuilderTest
testCodeBuilderTest_SOURCES = testCodeBuilderTestMain.C
testCodeBuilderTest_LDADD =	$(top_builddir)/projects/backstroke/utilities/libBackstrokeTestCodeBuilder.a \
			$(LIBS_WITH_RPATH) $(ROSE_LIBS)

EXTRA_DIST = 

CLEANFILES = 

check-local:
#@$(MAKE) $(TEST_Objects)
	@echo "*********************************************************************************************************"
	@echo "* ROSE/projects/backstroke/tests/testCodeBuilderTest: make check rule complete (terminated normally) *"
	@echo "*********************************************************************************************************"

