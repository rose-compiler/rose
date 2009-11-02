
include_directories(${KDE4_INCLUDES} ${KDE4_INCLUDE_DIR} ${QT_INCLUDES} )

add_subdirectory(Common)
add_subdirectory(QueryBox)
add_subdirectory(SourceBox)
add_subdirectory(TreeBox)


########### install files ###############




#original Makefile.am contents follow:

## the library search path.
#
#SUBDIRS = Common QueryBox SourceBox TreeBox
# 
#
#
#
#
#
#
