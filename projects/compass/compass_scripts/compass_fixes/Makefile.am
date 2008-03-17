include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

include $(srcdir)/compass_makefile.inc

SUBDIRS = \
	compassSupport \
	util 

INCLUDES = $(ROSE_INCLUDES) -I$(srcdir)/compassSupport \
	-I$(srcdir)/util \
   $(COMPASS_INCLUDES)

bin_PROGRAMS = compassMain

LDADD = $(LIBS_WITH_RPATH) $(ROSE_LIBS) compassSupport/libCompass.la $(compass_detector_libs)

compassMain_SOURCES = compassMain.C buildCheckers.C $(COMPASS_CHECKER_SRCS)
compassMain_DEPENDENCIES = $(ROSE_LIBS_WITH_PATH) compassSupport/libCompass.la $(compass_detector_libs)


test: compassMain
	env COMPASS_PARAMETERS=$(srcdir)/compass_parameters ./compassMain $(srcdir)/tests/exampleTest_1.C

testAllInputsTogether:
	env COMPASS_PARAMETERS=$(srcdir)/compass_parameters ./compassMain $(COMPASS_CHECKER_TESTS)

check-local:
	@$(MAKE) test
	@echo "*****************************************************************************"
	@echo "*** ROSE/projects/compass: make check rule complete (terminated normally) ***"
	@echo "*****************************************************************************"

EXTRA_DIST = compass.h  compass_parameters tests

clean-local:
	rm -f compass.* *.sty *.ti a.out *.tex *.lo *.la
	rm -f CHECKER_LIST $(COMPASS_CHECKER_TEX_FILES)

