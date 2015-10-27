include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

AM_CPPFLAGS = $(ROSE_INCLUDES) 

# noinst_LTLIBRARIES lists all the libraries that should be built, but not installed
noinst_LTLIBRARIES =  libSSA_UnfilteredCfg.la

libSSA_UnfilteredCfg_la_DEPENDENCIES =
libSSA_UnfilteredCfg_la_SOURCES = ssaUnfilteredCfgCalculation.C ssaUnfilteredCfgQueries.C  defsAndUsesUnfilteredCfg.C \
		reachingDefUnfilteredCfg.C
pkginclude_HEADERS = ssaUnfilteredCfg.h defsAndUsesUnfilteredCfg.h reachingDefUnfilteredCfg.h 

