
include_directories(${KDE4_INCLUDES} ${KDE4_INCLUDE_DIR} ${QT_INCLUDES} )

add_subdirectory(src)
#add_subdirectory(include)


########### install files ###############




#original Makefile.am contents follow:

#AUTOMAKE_OPTIONS=foreign no-dependencies
#SUBDIRS=src include
