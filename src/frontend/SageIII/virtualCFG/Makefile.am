include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

INCLUDES = $(ROSE_INCLUDES) $(BOOST_CPPFLAGS)

# Some source code is generated (call make in ROSE/src/ROSETTA/src to generate these files)
GENERATED_SOURCE  = ../Cxx_Grammar.C ../Cxx_GrammarTreeTraversalSuccessorContainer.C ../Cxx_GrammarRTI.C
GENERATED_OBJECTS = ../Cxx_Grammar.o ../Cxx_GrammarTreeTraversalSuccessorContainer.o ../Cxx_GrammarRTI.o
$(GENERATED_SOURCE):
	cd $(top_builddir)/src/ROSETTA/src; $(MAKE)

noinst_LTLIBRARIES = libvirtualCFG.la
libvirtualCFG_la_SOURCES      = \
     virtualCFG.C \
     cfgToDot.C \
     memberFunctions.C \
     staticCFG.C \
     interproceduralCFG.C

if ROSE_BUILD_BINARY_ANALYSIS_SUPPORT
     libvirtualCFG_la_SOURCES += virtualBinCFG.C
endif

libvirtualCFG_la_LIBADD       = 
libvirtualCFG_la_DEPENDENCIES = $(GENERATED_SOURCE)

include_HEADERS = \
     virtualCFG.h \
     cfgToDot.h \
     filteredCFG.h \
     filteredCFGImpl.h \
     staticCFG.h \
     interproceduralCFG.h

if ROSE_BUILD_BINARY_ANALYSIS_SUPPORT
   include_HEADERS += virtualBinCFG.h
endif

EXTRA_DIST = CMakeLists.txt
