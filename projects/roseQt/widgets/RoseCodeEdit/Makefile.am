#------------ RoseCodeEdit Makefile.am -----------
include $(top_srcdir)/projects/roseQt/Make.inc 

if ROSE_USE_QT

noinst_LTLIBRARIES = libRoseCodeEdit.la 


# Contents of Makefile.am.pre
INCLUDES += -I$(srcdir)/QCodeEdit -I$(srcdir)/QCodeEdit/document
SUBDIRS = QCodeEdit
libRoseCodeEdit_la_LIBADD   =  QCodeEdit/libQCodeEdit.la
# End of Makefile.am.pre Content

libRoseCodeEdit_la_SOURCES = RoseCodeEdit.cpp

nodist_libRoseCodeEdit_la_SOURCES = moc_RoseCodeEdit.cpp\
                                    qrc_example.cpp\
                                    qrc_QCodeEdit.cpp



endif
