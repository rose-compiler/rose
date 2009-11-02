
include_directories(${KDE4_INCLUDES} ${KDE4_INCLUDE_DIR} ${QT_INCLUDES} )


########### install files ###############

install(FILES  cpp2cpp cpp2pdf cpp2ps dot2ps lsviz cpp2beauty indent.cfg pragmarize DESTINATION )



#original Makefile.am contents follow:

## copy the tools (scripts only) from the source tree to the source destination in the distribution
#EXTRA_DIST = cpp2cpp cpp2pdf cpp2ps dot2ps lsviz cpp2beauty indent.cfg pragmarize
#
## install the tools in 'bin'
#bin_SCRIPTS = cpp2cpp cpp2pdf cpp2ps dot2ps lsviz cpp2beauty indent.cfg pragmarize
#
