include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

## The grammar generator (ROSETTA) should use its own template repository
CXX_TEMPLATE_REPOSITORY_PATH = .

libsupportSources = CommandOptions.C DAG.C DirectedGraph.C

# lib_LIBRARIES = libsupport.a
# libsupport_a_SOURCES  = $(libsupportSources)
# libsupport_a_DEPENDENCIES  = CommandOptions.o

if DEV_LIBS
lib_LTLIBRARIES = libsupport.la
else
noinst_LTLIBRARIES = libsupport.la
endif
libsupport_la_SOURCES = $(libsupportSources)

clean-local:
	rm -rf Templates.DB ii_files ti_files cxx_templates

distclean-local:
	rm -rf Templates.DB

include_HEADERS = \
   DAG.h               IteratorCompound.h  TreeImpl.h \
   BitVectorRepr.h   DirectedGraph.h     IteratorTmpl.h      PtrMap.h       union_find.h \
   CommandOptions.h  DoublyLinkedList.h  LatticeElemList.h   PtrSet.h \
   const.h           FunctionObject.h    Matrix.h            SinglyLinkedList.h \
   CountRefHandle.h  general.h           ObserveObject.h     StreamIO.h  








