include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

# DQ (1/18/2008): The visualization work is not superseded by zgrviewer which is availalble on the web.
#                 The graphics directory contains an old STL based graphics interface that is no longer 
#                 used. A newer design is being implemented to support graphs in ROSE.
#                 The dot2gml is only compiled if the DOT_TO_GML_TRANSLATOR configure option is used
#                 The graphicalUserInterface is compiled only if the ROSE_USE_QROSE configure option is used
# SUBDIRS = graphicalUserInterface graphics visualization dot2gml
# SUBDIRS = graphicalUserInterface dot2gml
SUBDIRS = dot2gml

EXTRA_DIST = CMakeLists.txt
