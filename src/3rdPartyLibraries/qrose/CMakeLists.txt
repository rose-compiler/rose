add_definitions(-DQT3_SUPPORT)
include_directories(${QT_QT3SUPPORT_INCLUDE_DIR})

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
