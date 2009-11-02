
include_directories(${KDE4_INCLUDES} ${KDE4_INCLUDE_DIR} ${QT_INCLUDES} )


########### next target ###############

set(pdfGenerator_SRCS pdfGenerator.C)

kde4_add_executable(pdfGenerator ${pdfGenerator_SRCS})

target_link_libraries(pdfGenerator ${KDE4_KDECORE_LIBS})

install(TARGETS pdfGenerator ${INSTALL_TARGETS_DEFAULT_ARGS})


########### install files ###############




#original Makefile.am contents follow:

#include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs
#
#INCLUDES = $(ROSE_INCLUDES)
#
#bin_PROGRAMS = pdfGenerator
#
#pdfGenerator_SOURCES = pdfGenerator.C
#
#LDADD = $(LIBS_WITH_RPATH) $(ROSE_LIBS)
#
#EXTRA_DIST = 
#CLEANFILES = 
#
#clean-local:
#	rm -rf *.o pdfGenerator Templates.DB ii_files ti_files
#
#
