
include_directories(${KDE4_INCLUDES} ${KDE4_INCLUDE_DIR} ${QT_INCLUDES} )

add_subdirectory(Framework)
add_subdirectory(Widgets)
add_subdirectory(Components)
add_subdirectory(QRoseLib)


########### install files ###############




#original Makefile.am contents follow:

## the library search path.
#
#SUBDIRS = Framework Widgets Components QRoseLib 
#
#
#
#
#
#
