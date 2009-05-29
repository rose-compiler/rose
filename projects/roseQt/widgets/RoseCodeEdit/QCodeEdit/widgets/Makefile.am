#------------ widgets Makefile.am -----------
include $(top_srcdir)/projects/roseQt/Make.inc 

if ROSE_USE_QT

noinst_LTLIBRARIES = libwidgets.la 


# Contents of Makefile.am.pre
INCLUDES += -I$(srcdir)/..  -I$(srcdir)/../document -I$(srcdir)/../qnfa 

# End of Makefile.am.pre Content

libwidgets_la_SOURCES = qcalltip.cpp\
                        qeditconfig.cpp\
                        qfoldpanel.cpp\
                        qformatconfig.cpp\
                        qgotolinedialog.cpp\
                        qlinechangepanel.cpp\
                        qlinemarkpanel.cpp\
                        qlinenumberpanel.cpp\
                        qpanel.cpp\
                        qsearchreplacepanel.cpp\
                        qsimplecolorpicker.cpp\
                        qstatuspanel.cpp

nodist_libwidgets_la_SOURCES = moc_qeditconfig.cpp\
                               moc_qfoldpanel.cpp\
                               moc_qformatconfig.cpp\
                               moc_qgotolinedialog.cpp\
                               moc_qlinechangepanel.cpp\
                               moc_qlinemarkpanel.cpp\
                               moc_qlinenumberpanel.cpp\
                               moc_qpanel.cpp\
                               moc_qsearchreplacepanel.cpp\
                               moc_qsimplecolorpicker.cpp\
                               moc_qstatuspanel.cpp\
                               ui_editconfig.h\
                               ui_formatconfig.h\
                               ui_gotoline.h\
                               ui_searchreplace.h

BUILT_SOURCES = ui_editconfig.h\
                ui_formatconfig.h\
                ui_gotoline.h\
                ui_searchreplace.h

# Target for installing header 
headerTargetdir    =  $(roseQtWidgetsIncPath)/QCodeEdit/widgets
headerTarget_HEADERS = qcalltip.h\
                       qeditconfig.h\
                       qfoldpanel.h\
                       qformatconfig.h\
                       qgotolinedialog.h\
                       qlinechangepanel.h\
                       qlinemarkpanel.h\
                       qlinenumberpanel.h\
                       qpanel.h\
                       qsearchreplacepanel.h\
                       qsimplecolorpicker.h\
                       qstatuspanel.h


endif
