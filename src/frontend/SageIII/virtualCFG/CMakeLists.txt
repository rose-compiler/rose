
include_directories(${KDE4_INCLUDES} ${KDE4_INCLUDE_DIR} ${QT_INCLUDES} )


########### next target ###############


#add_library( virtualCFG ${virtualCFG_SRC}  )
#add_dependencies( virtualCFG ROSETTA )
########### install files ###############

install(FILES  virtualCFG.h       virtualBinCFG.h       staticCFG.h       cfgToDot.h       filteredCFG.h       filteredCFGImpl.h DESTINATION ${INCLUDE_INSTALL_DIR})



#original Makefile.am contents follow:

#include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs
#
#INCLUDES = $(ROSE_INCLUDES)
#
## Some source code is generated (call make in ROSE/src/ROSETTA/src to generate these files)
#GENERATED_SOURCE  = ../Cxx_Grammar.C ../Cxx_GrammarTreeTraversalSuccessorContainer.C ../Cxx_GrammarRTI.C
#GENERATED_OBJECTS = ../Cxx_Grammar.o ../Cxx_GrammarTreeTraversalSuccessorContainer.o ../Cxx_GrammarRTI.o
#$(GENERATED_SOURCE):
#	cd $(top_builddir)/src/ROSETTA/src; $(MAKE)
#
#noinst_LTLIBRARIES = libvirtualCFG.la
#libvirtualCFG_la_SOURCES      = \
#     virtualCFG.C \
#     virtualBinCFG.C \
#     cfgToDot.C \
#     memberFunctions.C
#
#libvirtualCFG_la_LIBADD       = 
#libvirtualCFG_la_DEPENDENCIES = $(GENERATED_SOURCE)
#
#include_HEADERS = \
#     virtualCFG.h \
#     virtualBinCFG.h \
#     cfgToDot.h \
#     filteredCFG.h \
#     filteredCFGImpl.h
