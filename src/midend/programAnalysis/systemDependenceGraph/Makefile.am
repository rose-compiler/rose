include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

AM_CPPFLAGS = $(ROSE_INCLUDES)

if ROSE_USING_GCC_VERSION_LATER_4_6
noinst_LTLIBRARIES =  
else
# DQ (10/7/2015): I think we don't want to support this code any more (was not supported 
# on GNU compilers greater than 4.6 and so should not be supported on Intel compilers.
noinst_LTLIBRARIES =  
# noinst_LTLIBRARIES =  libSDG.la
endif

# DQ (10/7/2015): I think we don't want to support this code any more (was not supported 
# on GNU compilers greater than 4.6 and so should not be supported on Intel compilers.
# DQ (2/9/2014): This code does not compile using GNU g++ version 4.7 and later compilers.
# This requires more investigation to decide on a fix, but we might eliminate 
# this code in favor of more recent analysis work so it might not be work fixing).
# Not clear if this is redundant with modification in src/midend/programAnalysis/Makefile.am

# libSDG_la_SOURCES = staticCFG.C PDG.C SDG.C util.C defUseChains.C newDDG.C newCDG.C
if ROSE_USING_GCC_VERSION_LATER_4_6
# not supported anymore
else
# not supported anymore
# pkginclude_HEADERS = cong_staticCFG.h PDG.h util.h SDG.h defUseChains.h  newCDG.h newDDG.h
# libSDG_la_DEPENDENCIES =
# libSDG_la_SOURCES = staticCFG.C PDG.C SDG.C util.C defUseChains.C newDDG.C newCDG.C
endif

EXTRA_DIST =
