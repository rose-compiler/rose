#------------ document Makefile.am -----------
include $(top_srcdir)/src/roseExtensions/qtWidgets/Make.inc 

if ROSE_WITH_ROSEQT

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

nodist_libdocument_la_SOURCES = moc_qdocument.cxx



EXTRA_DIST = 

# Target for installing header 
headerTargetdir    =  $(roseQtWidgetsHeaderInstallPath)/QCodeEdit
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
