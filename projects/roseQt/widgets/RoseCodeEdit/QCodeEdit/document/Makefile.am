#------------ document Makefile.am -----------
include $(top_srcdir)/projects/roseQt/Make.inc 

if ROSE_USE_QT

noinst_LTLIBRARIES = libdocument.la 


# Contents of Makefile.am.pre
INCLUDES += -I$(srcdir)/..  -I$(srcdir)/../qnfa -I$(srcdir)/../widgets

# End of Makefile.am.pre Content

libdocument_la_SOURCES = qdocument.cpp\
                         qdocumentbuffer.cpp\
                         qdocumentcommand.cpp\
                         qdocumentcursor.cpp\
                         qdocumentline.cpp\
                         qdocumentsearch.cpp

nodist_libdocument_la_SOURCES = moc_qdocument.cpp



# Target for installing header 
headerTargetdir    =  $(roseQtWidgetsIncPath)/QCodeEdit/document
headerTarget_HEADERS = qdocument.h\
                       qdocument_p.h\
                       qdocumentbuffer.h\
                       qdocumentcommand.h\
                       qdocumentcursor.h\
                       qdocumentcursor_p.h\
                       qdocumentline.h\
                       qdocumentline_p.h\
                       qdocumentsearch.h


endif
