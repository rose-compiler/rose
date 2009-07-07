#------------ QtGradientEditor Makefile.am -----------
include ../Make.inc 

if ROSE_WITH_ROSEQT

INCLUDES += -I.

QTGRADIENT_MOC_FILES = moc_qtcolorbutton.cxx \
		       moc_qtcolorline.cxx \
		       moc_qtgradientdialog.cxx \
		       moc_qtgradienteditor.cxx \
		       moc_qtgradientmanager.cxx \
		       moc_qtgradientstopscontroller.cxx \
		       moc_qtgradientstopsmodel.cxx \
		       moc_qtgradientstopswidget.cxx \
		       moc_qtgradientutils.cxx \
		       moc_qtgradientview.cxx \
		       moc_qtgradientviewdialog.cxx \
		       moc_qtgradientwidget.cxx


noinst_LTLIBRARIES = libQtGradientEditor.la 

libQtGradientEditor_la_SOURCES = qtcolorbutton.cpp \
                                 qtcolorline.cpp \
                                 qtgradientdialog.cpp \
                                 qtgradienteditor.cpp \
                                 qtgradientmanager.cpp \
                                 qtgradientstopscontroller.cpp \
                                 qtgradientstopsmodel.cpp \
                                 qtgradientstopswidget.cpp \
                                 qtgradientutils.cpp \
                                 qtgradientview.cpp \
                                 qtgradientviewdialog.cpp \
                                 qtgradientwidget.cpp \
				 ui_qtgradientdialog.h \
				 ui_qtgradienteditor.h \
				 ui_qtgradientviewdialog.h \
				 ui_qtgradientview.h

BUILT_SOURCES = ui_qtgradientdialog.h \
		ui_qtgradienteditor.h \
		ui_qtgradientviewdialog.h \
		ui_qtgradientview.h 

libQtGradientEditor.la: $(QTGRADIENT_MOC_FILES) $(libQtGradientEditor_la_OBJECTS)
	$(CXXLINK) $(am_libQtGradientEditor_la_rpath) $(libQtGradientEditor_la_LDFLAGS) $(libQtGradientEditor_la_OBJECTS) $(libQtGradientEditor_la_LIBADD) $(LIBS)

# Target for installing header 
headerTargetdir    =  $(roseQtWidgetsIncPath)/
headerTarget_HEADERS = qtcolorbutton.h \
		       qtcolorline.h \
		       qtgradientdialog.h \
		       qtgradienteditor.h \
		       qtgradientmanager.h \
		       qtgradientstopscontroller.h \
		       qtgradientstopsmodel.h \
		       qtgradientstopswidget.h \
		       qtgradientutils.h \
		       qtgradientview.h \
		       qtgradientviewdialog.h \
		       qtgradientwidget.h

endif
