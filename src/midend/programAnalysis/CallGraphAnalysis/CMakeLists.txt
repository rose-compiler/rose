include_directories(${KDE4_INCLUDES} ${KDE4_INCLUDE_DIR} ${QT_INCLUDES} )

########### header files to be installed ###############
set(CALLGRAPHANALYSIS_HEADERS newCallGraph.h 
                             CallGraph.h 
                             ClassHierarchyGraph.h)
# newCallGraph.h fails on windows, so we remove it in that case 
if(WIN32)
  list(REMOVE_ITEM CALLGRAPHANALYSIS_HEADERS newCallGraph.h)
endif() 
  
install(FILES ${CALLGRAPHANALYSIS_HEADERS} DESTINATION ${INCLUDE_INSTALL_DIR})

#original Makefile.am contents follow:

#include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs
#
#
#
#INCLUDES = $(ROSE_INCLUDES) $(BOOST_CPPFLAGS) 
#libCallGraphSources =  CallGraph.C CallGraphDB.C ClassHierarchyGraph.C
#
#noinst_LTLIBRARIES = libCallGraph.la
#libCallGraph_la_SOURCES = $(libCallGraphSources)
#
#clean-local:
#	rm -rf Templates.DB ii_files ti_files cxx_templates
#
#distclean-local:
#	rm -rf Templates.DB
#
#include_HEADERS = CallGraph.h ClassHierarchyGraph.h
#
#
