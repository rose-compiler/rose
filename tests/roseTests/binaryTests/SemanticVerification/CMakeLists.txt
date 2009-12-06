
include_directories(${KDE4_INCLUDES} ${KDE4_INCLUDE_DIR} ${QT_INCLUDES} )


########### next target ###############

set(verifySemantics_SRCS verifySemantics.C)

kde4_add_executable(verifySemantics ${verifySemantics_SRCS})

target_link_libraries(verifySemantics ${KDE4_KDECORE_LIBS})


########### install files ###############

install(FILES  Debugger.h DESTINATION )



#original Makefile.am contents follow:

#include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs
#
## Automake's testing mechanism (which defines the "make check" rule) requires passing tests.
#TESTCODES=
#
#INCLUDES=$(ROSE_INCLUDES)
#
#dist_include_HEADERS=Debugger.h
#noinst_PROGRAMS=verifySemantics
#
## Program to verify that ROSE's instruction semantics layer functions correctly.
#verifySemantics_SOURCES=verifySemantics.C
#verifySemantics_LDADD=$(ROSE_LIBS_WITH_PATH) $(ROSE_SEPARATE_LIBS) $(RT_LIBS)
