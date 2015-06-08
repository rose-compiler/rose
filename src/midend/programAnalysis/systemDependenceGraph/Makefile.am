include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

INCLUDES = $(ROSE_INCLUDES)

if ROSE_USING_GCC_VERSION_LATER_4_6
noinst_LTLIBRARIES =  
else
noinst_LTLIBRARIES =  libSDG.la
endif

libSDG_la_DEPENDENCIES =

# DQ (2/9/2014): This code does not compile using GNU g++ version 4.7 and later compilers.
# This requires more investigation to decide on a fix, but we might eliminate 
# this code in favor of more recent analysis work so it might not be work fixing).
# Not clear if this is redundant with modification in src/midend/programAnalysis/Makefile.am

# libSDG_la_SOURCES = staticCFG.C PDG.C SDG.C util.C defUseChains.C newDDG.C newCDG.C
if ROSE_USING_GCC_VERSION_LATER_4_6
  libSDG_la_SOURCES = 
# libSDG_la_SOURCES = staticCFG.C PDG.C SDG.C util.C defUseChains.C newDDG.C newCDG.C
else
  libSDG_la_SOURCES = staticCFG.C PDG.C SDG.C util.C defUseChains.C newDDG.C newCDG.C
endif

pkginclude_HEADERS = cong_staticCFG.h PDG.h util.h SDG.h defUseChains.h  newCDG.h newDDG.h

EXTRA_DIST =
