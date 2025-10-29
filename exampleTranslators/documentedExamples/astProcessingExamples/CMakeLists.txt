
include_directories(${KDE4_INCLUDES} ${KDE4_INCLUDE_DIR})

add_subdirectory(printVars)
add_subdirectory(printLoopInfo)


########### install files ###############




#original Makefile.am contents follow:

#include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs
#
#SUBDIRS = \
#     printVars printLoopInfo
