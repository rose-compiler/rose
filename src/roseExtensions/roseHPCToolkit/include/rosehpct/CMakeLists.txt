
include_directories(${KDE4_INCLUDES} ${KDE4_INCLUDE_DIR})

add_subdirectory(util)
add_subdirectory(xml)
add_subdirectory(profir)
add_subdirectory(xml2profir)
add_subdirectory(sage)
add_subdirectory(profir2sage)
add_subdirectory(xml-xercesc)
add_subdirectory(gprof)


########### install files ###############




#original Makefile.am contents follow:

## ROSE-HPCT header files
#
#SUBDIRS = util xml profir xml2profir sage profir2sage \
#          xml-xercesc gprof
#
#include ../../Make.inc
#
#rosehpctinc_HEADERS = rosehpct.hh
#
## eof
