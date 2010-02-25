
include_directories(${CMAKE_SOURCE_DIR}/src/util/support ${CMAKE_SOURCE_DIR}/src/util/graphs ${CMAKE_SOURCE_DIR}/src/midend/astUtil/astSupport ${CMAKE_SOURCE_DIR}/src/midend/astUtil/symbolicVal ${CMAKE_SOURCE_DIR}/src/midend/astUtil/astInterface ${KDE4_INCLUDES} ${KDE4_INCLUDE_DIR} ${QT_INCLUDES} )

add_subdirectory(depInfo)
add_subdirectory(depGraph)
add_subdirectory(computation)
add_subdirectory(outsideInterface)
add_subdirectory(slicing)
add_subdirectory(driver)
add_subdirectory(prepostTransformation)


########### install files ###############


#add_library( midend_loopproc STATIC ${midend_loopproc_src}  )
#add_dependencies( midend_loopproc ROSETTA )




#original Makefile.am contents follow:

#include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs
#
#SUBDIRS = depInfo depGraph computation outsideInterface slicing driver prepostTransformation
#
#INCLUDES = \
#   $(ROSE_INCLUDES) -I${srcdir}/driver -I${srcdir}/prepostTransformation -I$(top_srcdir)/src/util/support -I$(top_srcdir)/src/util/graphs  -I$(top_srcdir)/src/midend/astUtil/astSupport -I$(top_srcdir)/src/midend/astUtil/symbolicVal -I$(top_srcdir)/src/midend/astUtil/astInterface
#
### The grammar generator (ROSETTA) should use its own template repository
#CXX_TEMPLATE_REPOSITORY_PATH = .
#
#clean-local:
#	rm -rf Templates.DB ii_files ti_files cxx_templates
#	rm -rf rose_mm.C
#
#distclean-local:
#	rm -rf Templates.DB
#
