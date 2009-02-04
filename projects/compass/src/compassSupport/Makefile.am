include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs
include $(top_srcdir)/projects/compass/src/compassSupport/compass_dirs.inc

if ROSE_MPI
INCLUDES = $(ROSE_INCLUDES) -DROSE_MPI -I$(compass_prereqs_dir) 
else
INCLUDES = $(ROSE_INCLUDES) -I$(compass_prereqs_dir)
endif

lib_LTLIBRARIES = libCompass.la


bin_PROGRAMS = compassEmptyMain 

include_HEADERS = compass.h 

libCompass_la_SOURCES = compass.C

LDADD = libCompass.la $(ROSE_LIBS_WITH_PATH)

if ROSE_USE_GCC_OMP
 LDADD += -lgomp
 CPPFLAGS += -fopenmp
endif


compassEmptyMain_SOURCES = compassEmptyMain.C
compassEmptyMain_LDADD = libCompass.la $(ROSE_LIBS_WITH_PATH)

BUILT_SOURCES = prerequisites.h instantiate_prerequisites.h 

prerequisites.h: $(compass_prereqs_dir)
	find $(compass_prereqs_dir) -name "*.h" | awk -F/ '{print "#include \"" $$NF "\""}' > $@

instantiate_prerequisites.h: $(compass_prereqs_dir)
	cat $(compass_prereqs_dir)/*.h | grep "^extern" | sed -e 's@extern[\t\ ]*@Compass::@g' | awk '{print $$1 " Compass::" $$2}' > $@

test: compassEmptyMain 
	env COMPASS_PARAMETERS=$(srcdir)/compass_parameters ./compassEmptyMain $(compass_test_dir)/exampleTest_1.C

check-local:
	@$(MAKE) test
	@echo "************************************************************************************"
	@echo "*** ROSE/projects/compassSupport: make check rule complete (terminated normally) ***"
	@echo "************************************************************************************"

nodist_include_HEADERS = prerequisites.h instantiate_prerequisites.h

install-exec-hook: 
	cp $(compass_prereqs_dir)/*.h $(DESTDIR)$(includedir)

uninstall-local:
	@sh -c 'for file in `ls $(compass_prereqs_dir)/*.h`; do rm -f $(DESTDIR)$(includedir)/`basename $$file`; done'

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
