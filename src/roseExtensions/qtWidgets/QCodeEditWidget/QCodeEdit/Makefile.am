#------------ QCodeEdit Makefile.am -----------
include $(top_srcdir)/src/roseExtensions/qtWidgets/Make.inc 

if ROSE_WITH_ROSEQT

noinst_LTLIBRARIES = libQCodeEdit.la 


# Contents of Makefile.am.pre
INCLUDES += -I$(srcdir)/widgets  -I$(srcdir)/document -I$(srcdir)/qnfa -Iwidgets
SUBDIRS = qnfa widgets document
libQCodeEdit_la_LIBADD   =   document/libdocument.la\
			     qnfa/libqnfa.la\
			     widgets/libwidgets.la 
# End of Makefile.am.pre Content

libQCodeEdit_la_SOURCES = qcodecompletionengine.cpp\
                          qcodeedit.cpp\
                          qeditor.cpp\
                          qeditorfactory.cpp\
                          qformatscheme.cpp\
                          qlanguagedefinition.cpp\
                          qlanguagefactory.cpp\
                          qlinemarksinfocenter.cpp\
                          qpanellayout.cpp\
                          qreliablefilewatch.cpp

nodist_libQCodeEdit_la_SOURCES = moc_qcodecompletionengine.cxx\
                                 moc_qeditor.cxx\
                                 moc_qeditorfactory.cxx\
                                 moc_qformatscheme.cxx\
                                 moc_qlanguagefactory.cxx\
                                 moc_qlinemarksinfocenter.cxx\
                                 moc_qpanellayout.cxx\
                                 moc_qreliablefilewatch.cxx



# Target for installing header 
headerTargetdir    =  $(roseQtWidgetsHeaderInstallPath)/QCodeEdit
headerTarget_HEADERS = qce-config.h\
                       qcodecompletionengine.h\
                       qcodeedit.h\
                       qeditor.h\
                       qeditorfactory.h\
                       qformat.h\
                       qformatfactory.h\
                       qformatscheme.h\
                       qlanguagedefinition.h\
                       qlanguagefactory.h\
                       qlinemarksinfocenter.h\
                       qpanellayout.h\
                       qreliablefilewatch.h


endif

EXTRA_DIST = 

