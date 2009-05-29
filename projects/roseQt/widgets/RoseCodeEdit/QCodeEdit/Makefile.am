#------------ QCodeEdit Makefile.am -----------
include $(top_srcdir)/projects/roseQt/Make.inc 

if ROSE_USE_QT

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

nodist_libQCodeEdit_la_SOURCES = moc_qcodecompletionengine.cpp\
                                 moc_qeditor.cpp\
                                 moc_qeditorfactory.cpp\
                                 moc_qformatscheme.cpp\
                                 moc_qlanguagefactory.cpp\
                                 moc_qlinemarksinfocenter.cpp\
                                 moc_qpanellayout.cpp\
                                 moc_qreliablefilewatch.cpp



# Target for installing header 
headerTargetdir    =  $(roseQtWidgetsIncPath)/QCodeEdit
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
