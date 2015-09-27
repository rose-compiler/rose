include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

AM_CPPFLAGS = -I$(top_srcdir)/src/frontend/SageIII

## The grammar generator (ROSETTA) should use its own template repository
CXX_TEMPLATE_REPOSITORY_PATH = .

libsupportSources = VectorCommandOptions.C CommandOptions.C DAG.C DirectedGraph.C

# Compile with BOOST_CPPFLAGS because that's where the "-pthread" switch is if user wants mult-thread support.
noinst_LTLIBRARIES = libsupport.la
libsupport_la_SOURCES = $(libsupportSources)
libsupport_la_CPPFLAGS = $(BOOST_CPPFLAGS)
libsupport_la_LDFLAGS = $(BOOST_THREAD_LIB)

clean-local:
	rm -rf Templates.DB ii_files ti_files cxx_templates

distclean-local:
	rm -rf Templates.DB

pkginclude_HEADERS = \
   DAG.h               IteratorCompound.h  TreeImpl.h \
   BitVectorRepr.h   DirectedGraph.h     IteratorTmpl.h      PtrMap.h       union_find.h \
   VectorCommandOptions.h CommandOptions.h  DoublyLinkedList.h  LatticeElemList.h   PtrSet.h \
   const.h           FunctionObject.h    Matrix.h            SinglyLinkedList.h \
   CountRefHandle.h  general.h           ObserveObject.h     StreamIO.h \
   FileHelper.h CollectionHelper.h  


EXTRA_DIST = CMakeLists.txt
