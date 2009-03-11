include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

if ROSE_USE_SQLITE_DATABASE

bin_PROGRAMS = macroRewrapper #matchTokenStreamToAST
#bin_PROGRAMS =
macroRewrapper_SOURCES = \
	waveHelpers.C access-macros.cpp macroRewrapper.C argumentFilterer.C main.C helpFunctions.C

# DQ (3/8/2009): Fixed error: If only SQLite is required, do not include $(QT_LDFLAGS) $(QROSE_LDFLAGS)
# macroRewrapper_LDADD = $(ROSE_LIBS) $(QT_LDFLAGS) $(QROSE_LDFLAGS) -lrt
macroRewrapper_LDADD = $(ROSE_LIBS)

# DQ (3/8/2009): Fixed error: -I. and $(BOOST_CPPFLAGS) is included in $(ROSE_INCLUDES).
# INCLUDES = $(ROSE_INCLUDES) $(BOOST_CPPFLAGS) -I.
INCLUDES = $(ROSE_INCLUDES) 


check-local:
	@echo "************************************************************************************"
	@echo "*** ROSE/projects/AstEquivalence: make check rule complete (terminated normally) ***"
	@echo "************************************************************************************"

CLEANFILES = 


endif

EXTRA_DIST = argumentFilterer.h  helpFunctions.h  macro.h  macroRewrapper.h  waveHelpers.h

