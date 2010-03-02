#SET(PARENT_SCOPE)
include_directories(${KDE4_INCLUDES} ${KDE4_INCLUDE_DIR} ${QT_INCLUDES} )


########### next target ###############
# Liao, 11/10/2009
# Using static library here as a "convenience" library to be merged into bigger shared librose.so
#add_library( libastMerge STATIC ${libastMerge_SRC} )
#add_dependencies( libastMerge ROSETTA )

# tps (12/17/2009) : Experiments to create (small) dynamic libraries.
# Currently the libastMerge.lib file under windows is 250 MB (static)
#add_library( libastMerge_shared SHARED ${libastMerge_SRC}  )
#add_library( libastMerge_module MODULE ${libastMerge_SRC}  )
#add_dependencies( libastMerge_module ROSETTA )
#add_dependencies( libastMerge_shared ROSETTA )

# tps (12/17/2009) : Experiments to create (small) dynamic libraries.
# I read that the symbols should be exported by default and that the
# error message I get is a bug. But I cannot figure out how to create
# dynamic libraries otherwise.
#set_property(TARGET ROSETTA PROPERTY ENABLE_EXPORTS 1)
#target_link_libraries(libastMerge_shared ROSETTA)

########### install files ###############

install(FILES  buildMangledNameMap.h  buildReplacementMap.h  collectAssociateNodes.h  deleteOrphanNodes.h       fixupTraversal.h  merge.h  merge_support.h  nullifyAST.h  test_support.h requiredNodes.h astMergeAPI.h AstFixParentTraversal.h DESTINATION ${INCLUDE_INSTALL_DIR})



#original Makefile.am contents follow:

#include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs
#
#INCLUDES = $(ROSE_INCLUDES)
#
## Some source code is generated (call make in ROSE/src/ROSETTA/src to generate these files)
#GENERATED_SOURCE  = ../Cxx_Grammar.C ../Cxx_GrammarTreeTraversalSuccessorContainer.C ../Cxx_GrammarRTI.C
#GENERATED_OBJECTS = ../Cxx_Grammar.o ../Cxx_GrammarTreeTraversalSuccessorContainer.o ../Cxx_GrammarRTI.o
#
#$(GENERATED_SOURCE):
#	cd $(top_builddir)/src/ROSETTA/src; $(MAKE)
#
#noinst_LTLIBRARIES = libastMerge.la
#libastMerge_la_SOURCES      = \
#     merge_support.C test_support.C buildMangledNameMap.C buildSetOfFrontendSpecificNodes.C \
#     deleteNodes.C fixupTraversal.C nullifyAST.C buildReplacementMap.C collectAssociateNodes.C \
#     deleteOrphanNodes.C normalizeTypes.C requiredNodes.C merge.C AstFixParentTraversal.C
#
#libastMerge_la_LIBADD       = 
#libastMerge_la_DEPENDENCIES = $(GENERATED_SOURCE)
#
#include_HEADERS = \
#     buildMangledNameMap.h  buildReplacementMap.h  collectAssociateNodes.h  deleteOrphanNodes.h \
#     fixupTraversal.h  merge.h  merge_support.h  nullifyAST.h  test_support.h requiredNodes.h astMergeAPI.h AstFixParentTraversal.h
#
