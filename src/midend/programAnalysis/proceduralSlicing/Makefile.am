include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

AM_CPPFLAGS = $(ROSE_INCLUDES)

## The grammar generator (ROSETTA) should use its own template repository
CXX_TEMPLATE_REPOSITORY_PATH = .

# DQ (9/10/2005): Commented out until Millind can get this code compiled (Qing changed the Graph interface)
 libalternativeProgramSlicing_la_SOURCES =  FindStatements.cpp  MarkingNodes.cpp  RemovalOfNodes.cpp  Slicing.cpp  SlicingCriterion.cpp  SlicingInfo.C
noinst_LTLIBRARIES = libalternativeProgramSlicing.la

clean-local:
	rm -rf Templates.DB ii_files ti_files cxx_templates

distclean-local:
	rm -rf Templates.DB

pkginclude_HEADERS = CheckWhichAreMarked.h  FindStatements.h  KeepAttribute.h  MarkingNodes.h  RemovalOfNodes.h  SlicingCriterion.h  Slicing.h

EXTRA_DIST = CMakeLists.txt
