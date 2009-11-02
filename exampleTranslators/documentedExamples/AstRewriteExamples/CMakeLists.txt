
include_directories(${KDE4_INCLUDES} ${KDE4_INCLUDE_DIR} ${QT_INCLUDES} )


########### next target ###############

set(astRewriteExample1_SRCS astRewriteExample1.C)

kde4_add_executable(astRewriteExample1 ${astRewriteExample1_SRCS})

target_link_libraries(astRewriteExample1 ${KDE4_KDECORE_LIBS})

install(TARGETS astRewriteExample1 ${INSTALL_TARGETS_DEFAULT_ARGS})


########### install files ###############




#original Makefile.am contents follow:

#include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs
#
#INCLUDES = $(ROSE_INCLUDES)
#
## DQ (3/21/2009): fails to compile under Cygwin.
#if !ROSE_BUILD_OS_IS_CYGWIN
#bin_PROGRAMS = astRewriteExample1
#astRewriteExample1_SOURCES = astRewriteExample1.C
#endif
#
#LDADD = $(LIBS_WITH_RPATH) $(ROSE_LIBS)
#
## DQ this file is used in the ROSE User Manual so we need it here
#EXTRA_DIST = inputRewrite1.C exampleRewrite1.C
#
#CLEANFILES = 
#
#clean-local:
#	rm -rf Templates.DB ii_files ti_files
#
