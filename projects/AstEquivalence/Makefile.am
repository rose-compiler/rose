include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

if ROSE_USE_QROSE

bin_PROGRAMS = astEquivalence runIDA

astEquivalence_SOURCES = \
	astEquivalenceGui.C \
        astEquivalenceMainGui.C

      
astEquivalence_LDADD = $(ROSE_LIBS) $(QT_LDFLAGS) $(QROSE_LDFLAGS) -lrt

copyFiles:
	cp $(srcdir)/split-into-tables.tcl $(top_builddir)/projects/AstEquivalence/.

runIDA_DEPENDENCIES = copyFiles
runIDA_SOURCES = \
        runIDA.C 
runIDA_LDADD = $(ROSE_LIBS) $(QT_LDFLAGS) $(QROSE_LDFLAGS) -lrt \
                     $(BOOST_LDFLAGS) $(BOOST_DATE_TIME_LIB) \
		    $(BOOST_THREAD_LIB) $(BOOST_FILESYSTEM_LIB) \
		    $(BOOST_PROGRAM_OPTIONS_LIB) $(BOOST_REGEX_LIB) \
		    $(BOOST_SYSTEM_LIB) $(BOOST_SERIALIZATION_LIB) 

INCLUDES = $(ROSE_INCLUDES) $(BOOST_CPPFLAGS) -I.


check-local:
	@echo "************************************************************************************"
	@echo "*** ROSE/projects/AstEquivalence: make check rule complete (terminated normally) ***"
	@echo "************************************************************************************"

CLEANFILES = 

EXTRA_DIST = 


endif
