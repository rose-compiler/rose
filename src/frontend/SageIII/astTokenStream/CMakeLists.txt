
include_directories(${KDE4_INCLUDES} ${KDE4_INCLUDE_DIR} ${QT_INCLUDES} )


########### next target ###############


#add_library( libastTokenStream STATIC ${libastTokenStream_SRC}  )
#add_dependencies( libastTokenStream ROSETTA )

########### install files ###############

install(FILES  createMap.h  doCompleteMapping.h  linearizeAST.h unparseMacro.h tokenStreamMapping.h frontierDetection.h simpleFrontierDetection.h previousAndNextNode.h DESTINATION ${INCLUDE_INSTALL_DIR})



#original Makefile.am contents follow:

#include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs
#
#INCLUDES = $(ROSE_INCLUDES)
#
#if ROSE_USE_BOOST_WAVE
#
#noinst_LTLIBRARIES = libastTokenStream.la
#
#libastTokenStream_la_SOURCES      = \
#     createMap.C  doCompleteMapping.C  linearizeAST.C unparseMacro.C
#
#libastTokenStream_la_LIBADD       = 
#libastTokenStream_la_DEPENDENCIES = 
#
#include_HEADERS = \
#     createMap.h  doCompleteMapping.h  linearizeAST.h unparseMacro.h
#
#endif
