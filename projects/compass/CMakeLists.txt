
include_directories(${KDE4_INCLUDES} ${KDE4_INCLUDE_DIR} ${QT_INCLUDES} )

add_subdirectory(src)
add_subdirectory(tools)


########### install files ###############




#original Makefile.am contents follow:

#include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs
#
#SUBDIRS = src tools
#
#EXTRA_DIST = extensions
#
#check-local:
#	@echo "***************************************************************************"
#	@echo "****** ROSE/projects/compass: make check rule complete (terminated normally) ******"
#	@echo "***************************************************************************"
