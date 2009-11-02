
include_directories(${KDE4_INCLUDES} ${KDE4_INCLUDE_DIR} ${QT_INCLUDES} )


########### next target ###############

set(astCopyReplTest_SRCS astCopyReplTest.C)

kde4_add_executable(astCopyReplTest ${astCopyReplTest_SRCS})

target_link_libraries(astCopyReplTest ${KDE4_KDECORE_LIBS})

install(TARGETS astCopyReplTest ${INSTALL_TARGETS_DEFAULT_ARGS})


########### install files ###############




#original Makefile.am contents follow:

#include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs
#
#INCLUDES = $(ROSE_INCLUDES)
#
#bin_PROGRAMS = astCopyReplTest 
#
#astCopyReplTest_SOURCES = astCopyReplTest.C
#
#LDADD = $(LIBS_WITH_RPATH) $(ROSE_LIBS)
#
#EXTRA_DIST = 
#CLEANFILES = 
#
#clean-local:
#	rm -rf Templates.DB ii_files ti_files
#
