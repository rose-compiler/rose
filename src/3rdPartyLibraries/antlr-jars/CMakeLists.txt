
include_directories(${KDE4_INCLUDES} ${KDE4_INCLUDE_DIR} ${QT_INCLUDES} )


########### install files ###############

# DQ (3/11/2010): Updating to new Fortran OFP version 0.7.2 with Craig.
# install(FILES  antlr-2.7.7.jar     antlr-3.0.1.jar     antlr-runtime-3.0.1.jar     stringtemplate-3.1b1.jar DESTINATION ${INCLUDE_INSTALL_DIR} )
install(FILES  antlr-3.2.jar DESTINATION ${INCLUDE_INSTALL_DIR} )



#original Makefile.am contents follow:

#javadir = $(libdir)
#
#dist_java_DATA = \
#   antlr-2.7.7.jar \
#   antlr-3.0.1.jar \
#   antlr-runtime-3.0.1.jar \
#   stringtemplate-3.1b1.jar
