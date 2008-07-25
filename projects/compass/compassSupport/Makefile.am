include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

if ROSE_MPI
INCLUDES = $(ROSE_INCLUDES) -DROSE_MPI
else
INCLUDES = $(ROSE_INCLUDES) 
endif

if DEV_LIBS
lib_LTLIBRARIES = libCompass.la
else
lib_LTLIBRARIES = libCompass.la
endif

if ROSE_MPI
LTCXXCOMPILE = \
	$(LIBTOOL) --tag=CXX --mode=compile $(MPICXX) $(DEFS) \
        $(DEFAULT_INCLUDES) $(INCLUDES) $(AM_CPPFLAGS) $(CPPFLAGS) \
        $(AM_CXXFLAGS) $(CXXFLAGS)
CXXLINK = $(LIBTOOL) --tag=CXX --mode=link $(MPICXX) $(AM_CXXFLAGS) \
        $(CXXFLAGS) $(AM_LDFLAGS) $(LDFLAGS) -o $@
endif

bin_PROGRAMS = compassEmptyMain 

include_HEADERS = compass.h 

libCompass_la_SOURCES = compass.C

LDADD = $(LIBS_WITH_RPATH) $(ROSE_LIBS) libCompass.la

if ROSE_USE_GCC_OMP
 LDADD += -lgomp
 CPPFLAGS += -fopenmp
endif


compassEmptyMain_SOURCES = compassEmptyMain.C
compassEmptyMain_LDADD = $(ROSE_LIBS_WITH_PATH) libCompass.la

test: compassEmptyMain 
	env COMPASS_PARAMETERS=$(srcdir)/compass_parameters ./compassEmptyMain $(srcdir)/../tests/exampleTest_1.C

check-local:
	@$(MAKE) test
	@echo "************************************************************************************"
	@echo "*** ROSE/projects/compassSupport: make check rule complete (terminated normally) ***"
	@echo "************************************************************************************"

EXTRA_DIST = compass.h  compass_parameters compassDocs.tex compassTestMain.C

clean-local:
	rm -f *.sty *.ti a.out

