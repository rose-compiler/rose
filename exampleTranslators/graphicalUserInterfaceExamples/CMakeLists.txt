
include_directories(${KDE4_INCLUDES} ${KDE4_INCLUDE_DIR})

add_subdirectory(slicing)
add_subdirectory(attributes)
add_subdirectory(query)
add_subdirectory(layout)


########### install files ###############




#original Makefile.am contents follow:

#include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs
#
#SUBDIRS = slicing attributes query layout
#
#EXTRA_DIST = test_01.cpp
