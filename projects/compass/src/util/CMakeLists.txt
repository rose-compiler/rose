
include_directories(${KDE4_INCLUDES} ${KDE4_INCLUDE_DIR} ${QT_INCLUDES} )

add_subdirectory(C-API)
add_subdirectory(MPIAbstraction)


########### install files ###############




#original Makefile.am contents follow:

## Utility support for Compass-based checkers.
#
#SUBDIRS = C-API MPIAbstraction
#
## eof
