
include_directories(${KDE4_INCLUDES} ${KDE4_INCLUDE_DIR} ${QT_INCLUDES} )

add_subdirectory(include)
add_subdirectory(src)
add_subdirectory(docs)


########### install files ###############




#original Makefile.am contents follow:

## ROSE-HPCT Module
##examples is moved to tests/roseTests/
#SUBDIRS = include src docs
#
#EXTRA_DIST = Make.inc README
#
## eof
