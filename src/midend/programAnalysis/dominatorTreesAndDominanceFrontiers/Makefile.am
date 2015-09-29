include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

AM_CPPFLAGS = $(ROSE_INCLUDES)

## The grammar generator (ROSETTA) should use its own template repository
CXX_TEMPLATE_REPOSITORY_PATH = .

libdominance_la_SOURCES =  ControlFlowGraph.C  DominanceFrontier.C  DominatorTree.C

noinst_LTLIBRARIES = libdominance.la

clean-local:
	rm -rf Templates.DB ii_files ti_files cxx_templates

distclean-local:
	rm -rf Templates.DB

pkginclude_HEADERS = ControlFlowGraph.h  SimpleDirectedGraph.h  DominanceFrontier.h  DominatorTree.h

EXTRA_DIST = CMakeLists.txt
