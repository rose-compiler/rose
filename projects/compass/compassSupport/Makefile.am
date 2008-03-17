include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

INCLUDES = $(ROSE_INCLUDES)

if DEV_LIBS
lib_LTLIBRARIES = libCompass.la
else
lib_LTLIBRARIES = libCompass.la
endif

bin_PROGRAMS = compassEmptyMain

include_HEADERS = compass.h

libCompass_la_SOURCES = compass.C

LDADD = $(LIBS_WITH_RPATH) $(ROSE_LIBS) libCompass.la

compassEmptyMain_SOURCES = compassEmptyMain.C
compassEmptyMain_DEPENDENCIES = $(ROSE_LIBS_WITH_PATH) libCompass.la

test: compassEmptyMain 
	env COMPASS_PARAMETERS=$(srcdir)/compass_parameters ./compassEmptyMain $(srcdir)/../tests/exampleTest_1.C

check-local:
	@$(MAKE) test
	@echo "************************************************************************************"
	@echo "*** ROSE/projects/compassSupport: make check rule complete (terminated normally) ***"
	@echo "************************************************************************************"

EXTRA_DIST = compass.h  compass_parameters compassDocs.tex compassTestMain.C

clean-local:
	rm -f compass.* *.sty *.ti a.out *.tex

