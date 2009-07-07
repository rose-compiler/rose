#------------ RoseCodeEdit Makefile.am -----------
include $(top_srcdir)/src/roseExtensions/qtWidgets/Make.inc 

if ROSE_WITH_ROSEQT

noinst_LTLIBRARIES = libRoseCodeEdit.la 


# Contents of Makefile.am.pre
INCLUDES += -I$(srcdir)/QCodeEdit -I$(srcdir)/QCodeEdit/document
SUBDIRS = QCodeEdit
libRoseCodeEdit_la_LIBADD   =  QCodeEdit/libQCodeEdit.la
# End of Makefile.am.pre Content

libRoseCodeEdit_la_SOURCES = RoseCodeEdit.cpp

nodist_libRoseCodeEdit_la_SOURCES = moc_RoseCodeEdit.cxx\
                                    qrc_example.cpp\
                                    qrc_QCodeEdit.cpp



# Target for installing header 
headerTargetdir    =  $(roseQtWidgetsIncPath)/
headerTarget_HEADERS = RoseCodeEdit.h


endif
