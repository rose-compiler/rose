include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs
include $(top_srcdir)/projects/compass/src/compassSupport/compass_dirs.inc

################################################################################


CHECKER_LIST_WITHOUT_COMMENTS: $(srcdir)/CHECKER_LIST
	grep -v '^#' < $(srcdir)/CHECKER_LIST | awk '{print $$0 " " toupper(substr($$0, 0, 1)) substr($$0, 2)}' > CHECKER_LIST_WITHOUT_COMMENTS



compass_makefile.inc: CHECKER_LIST_WITHOUT_COMMENTS $(compass_scripts_dir)/buildCompassMakefileInc.sh
	$(compass_scripts_dir)/buildCompassMakefileInc.sh $(compass_input_dir) > compass_makefile.inc



compass_parameters: CHECKER_LIST_WITHOUT_COMMENTS $(srcdir)/RULE_SELECTION
	echo "# this is an automatically generated file " > compass_parameters
	echo "Compass.RuleSelection=`cd $(srcdir); pwd`/RULE_SELECTION" >> compass_parameters
	echo '' >> compass_parameters
	cat CHECKER_LIST_WITHOUT_COMMENTS | while read ch chupper; do if [ -f $(compass_checker_dir)/$$ch/compass_parameters ]; then grep -v "^#" $(compass_checker_dir)/$$ch/compass_parameters >> compass_parameters || touch compass_parameters; fi; done



buildCheckers.C: CHECKER_LIST_WITHOUT_COMMENTS $(compass_input_dir)/buildCheckers.C.in
	sed -n '1,/@@@/p' < $(compass_input_dir)/buildCheckers.C.in > buildCheckers.C
	cut -f 1 -d \  < CHECKER_LIST_WITHOUT_COMMENTS | sed 's,^.*$$,BUILD_ONE_CHECKER(&),' >> buildCheckers.C
	sed -n '/@@@/,$$p' < $(compass_input_dir)/buildCheckers.C.in >> buildCheckers.C



checkers.h: CHECKER_LIST_WITHOUT_COMMENTS
	cut -f 1 -d \  < CHECKER_LIST_WITHOUT_COMMENTS | sed 's,^.*$$,extern const Compass::Checker* const &Checker;,' > checkers.h



#$(srcdir)/RULE_SELECTION: CHECKER_LIST_WITHOUT_COMMENTS
dothis:
	test -f $(srcdir)/RULE_SELECTION || touch $(srcdir)/RULE_SELECTION
	sh -c '\
	for checker in `cut -f 2 -d \ < CHECKER_LIST_WITHOUT_COMMENTS` ;\
	do \
		echo $${checker} ;\
		if [[ `grep "$${checker}" $(srcdir)/RULE_SELECTION` == "" ]]; then\
			echo "Updating $${checker} to $(srcdir)/RULE_SELECTION" ;\
			echo "+:$${checker}" >> $(srcdir)/RULE_SELECTION ;\
		fi ;\
	done ;\
	'


regenerate:
	 @echo "**************************************************************"
	 @echo "***       Regenerating automatically generated files       ***"
	 @echo "**************************************************************"
#       When using "make distcheck" rule we need to make the read only srcdir writable.
	 chmod -R u+w $(compass_checker_dir)
	 @echo "Builddir $(top_builddir)"
	 bash $(compass_scripts_dir)/compass_submission_setup.sh $(compass_checker_dir) $(srcdir) "regenerate"


# GMY Jul. 2008 Test for new checker integration of Compass
#        chmod -R u+w $(compass_checker_dir)
#        chmod -R u+w $(srcdir)
testNewChecker: $(compass_scripts_dir)/gen_checker.sh
	 cd $(compass_checker_dir) && $(compass_scripts_dir)/gen_checker.sh blank test checker 
	 cp $(srcdir)/CHECKER_LIST $(srcdir)/CHECKER_LIST.testNewChecker.bk
	 cp $(srcdir)/RULE_SELECTION $(srcdir)/RULE_SELECTION.testNewChecker.bk
	 echo "blankTestChecker" >> $(srcdir)/CHECKER_LIST
	 $(MAKE) regenerate
	 rm -f compass_makefile.inc; $(MAKE) compass_makefile.inc
	 $(MAKE); $(MAKE) test
	 mv $(srcdir)/CHECKER_LIST.testNewChecker.bk $(srcdir)/CHECKER_LIST
	 mv $(srcdir)/RULE_SELECTION.testNewChecker.bk $(srcdir)/RULE_SELECTION
	 rm -rf $(compass_checker_dir)/blankTestChecker blankTestChecker.lo
	 $(MAKE) regenerate; $(MAKE)



 include compass_makefile.inc

################################################################################

if ROSE_MPI
INCLUDES_MPI = -DROSE_MPI
endif

if ROSE_USE_GCC_OMP
INCLUDES_OMP = -DROSE_GCC_OMP
endif

if ROSE_MPI
INCLUDES = $(ROSE_INCLUDES) -DROSE_MPI -I$(compass_support_dir) $(COMPASS_INCLUDES) -I$(compass_srcdir) -I$(compass_checker_dir) -I.
else 
INCLUDES = $(ROSE_INCLUDES) -I$(compass_support_dir) $(COMPASS_INCLUDES) -I$(compass_srcdir) -I$(compass_checker_dir) -I.
endif

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

LDADD = $(LIBS_WITH_RPATH) $(ROSE_LIBS) $(compass_support_bdir)/libCompass.la

lib_LTLIBRARIES = libCompassCheckers.la
libCompassCheckers_la_SOURCES = buildCheckers.C checkers.h
libCompassCheckers_la_LIBADD = $(LIBS_WITH_RPATH) $(ROSE_LIBS) $(compass_support_bdir)/libCompass.la

BUILT_SOURCES = buildCheckers.C checkers.h

libCompassCheckers.la: $(libCompassCheckers_la_OBJECTS) $(libCompassCheckers_la_DEPENDENCIES) $(COMPASS_CHECKER_OBJS)
	$(CXXLINK) -rpath $(libdir) $(libCompassCheckers_la_LDFLAGS) $(libCompassCheckers_la_OBJECTS) $(COMPASS_CHECKER_OBJS) $(libCompassCheckers_la_LIBADD) $(LIBS)
