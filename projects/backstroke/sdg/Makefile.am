include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

INCLUDES = $(ROSE_INCLUDES) -I$(top_srcdir)/projects/backstroke
LINK	 = $(CXXLINK)
LDADD    = $(LIBS_WITH_RPATH) $(ROSE_LIBS)


# noinst_LIBRARIES lists all the librarlies that should be built, but not installed
noinst_PROGRAMS = testSDG

testSDG_SOURCES = newCFG.C newCDG.C newDDG.C newPDG.C SDG.C testSDG.C util.C defUseChains.C
testSDG_LDADD = $(top_builddir)/projects/backstroke/utilities/libBackstrokeUtilities.la \
		$(top_builddir)/projects/backstroke/ssa/libSSA.la \
		$(LIBS_WITH_RPATH) $(ROSE_LIBS)
testSDG_include_HEADERS = newCFG.h newCDG.h newDDG.h newPDG.h util.h SDG.h defUseChains.h
testSDG_includedir = $(includedir)/backstroke


# EXTRA_DIST are files that are not compiled or installed. These include readme's, internal header files, etc.
EXTRA_DIST = 

CLEANFILES = 


check-local:
	@echo "*********************************************************************************************************"
	@echo "************* ROSE/projects/backstroke/sdg: make check rule complete (terminated normally) **************"
	@echo "*********************************************************************************************************"

test:
	./testSDG -c test.C
