include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs
include $(top_srcdir)/projects/compass/src/compassSupport/compass_dirs.inc

if ROSE_MPI
INCLUDES = $(ROSE_INCLUDES) -DROSE_MPI -I$(compass_prereqs_dir)
else
INCLUDES = $(ROSE_INCLUDES) -I$(compass_prereqs_dir)
endif

lib_LTLIBRARIES = libCompass.la

if ROSE_MPI
LTCXXCOMPILE = \
	$(LIBTOOL) --tag=CXX --mode=compile $(MPICXX) $(DEFS) \
        $(DEFAULT_INCLUDES) $(INCLUDES) $(AM_CPPFLAGS) $(CPPFLAGS) \
        $(AM_CXXFLAGS) $(CXXFLAGS)
CXXCOMPILE = \
	$(MPICXX) $(DEFS) \
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

BUILT_SOURCES = prerequisites.h instantiate_prerequisites.h 

prerequisites.h: $(compass_prereqs_dir)
	find $(compass_prereqs_dir) -name "*.h" | awk -F/ '{print "#include \"" $$NF "\""}' > $@

instantiate_prerequisites.h: $(compass_prereqs_dir)
	tail -1 $(compass_prereqs_dir)/*.h | grep "extern" | sed -e 's@extern[\t\ ]*@Compass::@g' | awk '{print $$1 " Compass::" $$2}' > $@

test: compassEmptyMain 
	env COMPASS_PARAMETERS=$(srcdir)/compass_parameters ./compassEmptyMain $(compass_test_dir)/exampleTest_1.C

check-local:
	@$(MAKE) test
	@echo "************************************************************************************"
	@echo "*** ROSE/projects/compassSupport: make check rule complete (terminated normally) ***"
	@echo "************************************************************************************"

EXTRA_DIST = \
	compass.C \
        compass.C.gergo \
        compass_dirs.inc \
        compassDocs.tex \
        compassEmptyMain.C \
        compass.h \
        compass.h.gergo \
        compass.inc \
        compassMain.C \
        compass_parameters \
        compassQRoseMain.C \
        compassTestMain.C \
        README 

clean-local:
	rm -f *.sty *.ti a.out prerequisites.h instantiate_prerequisites.h 
