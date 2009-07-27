#------------ widgets Makefile.am -----------
include $(top_srcdir)/src/roseExtensions/qtWidgets/Make.inc 

if ROSE_WITH_ROSEQT

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

nodist_libwidgets_la_SOURCES = moc_qeditconfig.cxx\
                               moc_qfoldpanel.cxx\
                               moc_qformatconfig.cxx\
                               moc_qgotolinedialog.cxx\
                               moc_qlinechangepanel.cxx\
                               moc_qlinemarkpanel.cxx\
                               moc_qlinenumberpanel.cxx\
                               moc_qpanel.cxx\
                               moc_qsearchreplacepanel.cxx\
                               moc_qsimplecolorpicker.cxx\
                               moc_qstatuspanel.cxx\
                               ui_editconfig.h\
                               ui_formatconfig.h\
                               ui_gotoline.h\
                               ui_searchreplace.h

BUILT_SOURCES = ui_editconfig.h\
                ui_formatconfig.h\
                ui_gotoline.h\
                ui_searchreplace.h

EXTRA_DIST = editconfig.ui \
	formatconfig.ui \
	gotoline.ui \
	searchreplace.ui

# Target for installing header 
headerTargetdir    =  $(roseQtWidgetsHeaderInstallPath)/QCodeEdit
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
