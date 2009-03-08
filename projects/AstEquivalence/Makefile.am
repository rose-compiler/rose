include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

SUBDIRS =  gui


if ROSE_USE_QT
if ROSE_USE_SQLITE_DATABASE

bin_PROGRAMS = astEquivalence createTextFiles evaluateAllBinaries runIDA

astEquivalence_SOURCES = astEquivalenceGui.C astEquivalenceMainGui.C
createTextFiles_SOURCES = createTextFiles.C 
evaluateAllBinaries_SOURCES = evaluateAllBinaries.C 

# DQ (3/8/2009): These are included in $(ROSE_LIBS) as I understand it, plus -lrt will cause problems on OSX.
# astEquivalence_LDADD = $(ROSE_LIBS) $(QT_LDFLAGS) $(QROSE_LDFLAGS) -lrt
LDADD = $(ROSE_LIBS)


# DQ (3/8/2009): Removed in favor of a single LDADD rule.
# createTextFiles_LDADD = $(ROSE_LIBS) $(QT_LDFLAGS) $(QROSE_LDFLAGS) -lrt
# evaluateAllBinaries_LDADD = $(ROSE_LIBS) $(QT_LDFLAGS) $(QROSE_LDFLAGS) -lrt

copyFiles:
	cp $(srcdir)/split-into-tables.tcl $(top_builddir)/projects/AstEquivalence/.

runIDA_DEPENDENCIES = copyFiles
runIDA_SOURCES = runIDA.C 

# DQ (3/8/2009): These are included in $(ROSE_LIBS) as I understand it.
# runIDA_LDADD = $(ROSE_LIBS) $(QT_LDFLAGS) $(QROSE_LDFLAGS) -lrt \
#                     $(BOOST_LDFLAGS) $(BOOST_DATE_TIME_LIB) \
#		    $(BOOST_THREAD_LIB) $(BOOST_FILESYSTEM_LIB) \
#		    $(BOOST_PROGRAM_OPTIONS_LIB) $(BOOST_REGEX_LIB) \
#		    $(BOOST_SYSTEM_LIB) $(BOOST_SERIALIZATION_LIB) 

# DQ (3/8/2009): These are included in $(ROSE_INCLUDES) as I understand it.
# INCLUDES = $(ROSE_INCLUDES) $(BOOST_CPPFLAGS) -I.
INCLUDES = $(ROSE_INCLUDES)


check-local:
	@echo "************************************************************************************"
	@echo "*** ROSE/projects/AstEquivalence: make check rule complete (terminated normally) ***"
	@echo "************************************************************************************"

CLEANFILES = 


endif
endif

EXTRA_DIST = disks.xpm folder.xpm icons.h split-into-tables.tcl  astEquivalenceGui.h

