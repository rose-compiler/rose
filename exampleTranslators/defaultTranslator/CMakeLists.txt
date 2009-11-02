
include_directories(${KDE4_INCLUDES} ${KDE4_INCLUDE_DIR} ${QT_INCLUDES} )


########### next target ###############

set(defaultTranslator_SRCS defaultTranslator.C)

kde4_add_executable(defaultTranslator ${defaultTranslator_SRCS})

target_link_libraries(defaultTranslator ${KDE4_KDECORE_LIBS})

install(TARGETS defaultTranslator ${INSTALL_TARGETS_DEFAULT_ARGS})


########### install files ###############




#original Makefile.am contents follow:

#include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs
#
#INCLUDES = $(ROSE_INCLUDES)
#
#bin_PROGRAMS = defaultTranslator
#
#defaultTranslator_SOURCES = defaultTranslator.C
#
#LDADD = $(LIBS_WITH_RPATH) $(ROSE_LIBS)
#
#EXTRA_DIST = 
#CLEANFILES = 
#
#clean-local:
#	rm -rf Templates.DB ii_files ti_files
#
