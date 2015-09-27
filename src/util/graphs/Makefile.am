include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

AM_CPPFLAGS = -I$(top_srcdir)/src/util/support -I$(srcdir)/.. -I$(top_srcdir)/src/frontend/SageIII

CXX_TEMPLATE_REPOSITORY_PATH = .

libgraphsSources = \
   DGBaseGraphImpl.C SCCAnalysis.C  TransAnalysis.C  TypedFusion.C 

# lib_LIBRARIES = libgraphs.a
# libgraphs_a_SOURCES  = $(libgraphsSources)

# Use BOOST_CPPFLAGS because that's where the "-pthread" switch is if user wants multi-thread support.
noinst_LTLIBRARIES = libgraphs.la
libgraphs_la_SOURCES = $(libgraphsSources)
libgraphs_la_CPPFLAGS = $(BOOST_CPPFLAGS)
libgraphs_la_LDFLAGS = $(BOOST_THREAD_LIB)

clean-local:
	rm -rf Templates.DB ii_files ti_files cxx_templates

distclean-local:
	rm -rf Templates.DB

pkginclude_HEADERS = \
   BaseGraphCreate.h  GraphScope.h     VirtualGraphCreate.h \
   TransAnalysis.h    GraphGroup.h   MultiGraphCreate.h \
   TypedFusion.h DGBaseGraphImpl.h  GraphAccess.h GraphUtils.h\
   GraphUpdate.h    SCCAnalysis.h GraphDotOutput.h GraphIO.h \
   MultiGraphObserve.h

EXTRA_DIST = CMakeLists.txt
