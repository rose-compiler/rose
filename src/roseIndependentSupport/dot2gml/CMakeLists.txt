
include_directories(${KDE4_INCLUDES} ${KDE4_INCLUDE_DIR})

########### next target ###############
if( DOT_TO_GML_TRANSLATOR )

set(dot2gml_SRCS main.C parseDot.ll parseDotGrammar.yy)

add_executable(dot2gml ${dot2gml_SRCS})

target_link_libraries(dot2gml ${KDE4_KDECORE_LIBS})

install(TARGETS dot2gml DESTINATION bin)
endif()


########### install files ###############
install(FILES  parse_header.h DESTINATION ${INCLUDE_INSTALL_DIR})



#original Makefile.am contents follow:

#include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs
#
#AM_YFLAGS = -d
#
## "if" for DOT_TO_GML_TRANSLATOR (AutoMake conditional block)
#if DOT_TO_GML_TRANSLATOR
#
#bin_PROGRAMS = dot2gml
#
#dot2gml_SOURCES = \
#     main.C parseDot.ll parseDotGrammar.yy
#
#BUILT_SOURCES = parseDotGrammar.h
#DISTCLEANFILES = parseDotGrammar.h parseDotGrammar.cc parseDot.cc
#
## Header files that need to be included in the distribution
#include_HEADERS = parse_header.h
#
#clean-local:
#	rm -f *.o *~ *.gml
#
## DQ (9/10/2005): This should be moved to the tests directory
##check:
##	./dot2gml $(srcdir)/onefileonly.C.dot
#
#endif
#
#EXTRA_DIST = parse_header.h parseDot.ll parseDotGrammar.yy README main.C 
