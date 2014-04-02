
include_directories(${KDE4_INCLUDES} ${KDE4_INCLUDE_DIR} ${QT_INCLUDES} ${Boost_INCLUDE_DIRS})


########### next target ###############

#add_library(PHP_FRONTEND_LIB STATIC ${phpFrontend_SRC} )
#add_dependencies(PHP_FRONTEND_LIB ROSETTA  ) 

########### install files ###############

install(FILES  PHPFrontend.h DESTINATION include)



#original Makefile.am contents follow:

#include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs
#
#INCLUDES = $(ROSE_INCLUDES)
#
## This will cause the dynamic library to be built (as librosePHPFrontend.so)
#noinst_LTLIBRARIES = librosePHPFrontend.la
#
#librosePHPFrontend_la_DEPENDENCIES =
#librosePHPFrontend_la_SOURCES = PHPFrontend.C PHPFrontend.h
#
#dist_include_HEADERS = PHPFrontend.h
#
#include_HEADERS = 
#
