include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

INCLUDES = $(ROSE_INCLUDES) -I$(top_srcdir)/projects/backstroke
LINK	 = $(CXXLINK)
LDADD    = $(LIBS_WITH_RPATH) $(ROSE_LIBS)


# noinst_LIBRARIES lists all the librarlies that should be built, but not installed
#noinst_LIBRARIES =  libBackstrokeSlicing.a
noinst_PROGRAMS = slicing

slicing_SOURCES = backstrokeSlicer.C dominatorAndPostDominatorTree.C
slicing_LDADD = $(top_builddir)/projects/backstroke/utilities/libBackstrokeUtilities.a \
		$(LIBS_WITH_RPATH) $(ROSE_LIBS)
slicing_include_HEADERS = backstrokeCFG.h backstrokeCDG.h backstrokeDDG.h backstrokePDG.h backstrokeSlicer.h
slicing_includedir = $(includedir)/backstroke


# EXTRA_DIST are files that are not compiled or installed. These include readme's, internal header files, etc.
EXTRA_DIST = 

CLEANFILES = 


check-local:
	@echo "*********************************************************************************************************"
	@echo "****** ROSE/projects/backstroke/slicing: make check rule complete (terminated normally) ******"
	@echo "*********************************************************************************************************"

test:
	./slicing -c temp.C
