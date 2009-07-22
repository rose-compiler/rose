#------------ MetricsKiviat Makefile.am -----------
include $(top_srcdir)/src/roseExtensions/qtWidgets/Make.inc 

if ROSE_WITH_ROSEQT

noinst_LTLIBRARIES = libMetricsKiviat.la 



libMetricsKiviat_la_SOURCES = KiviatInfo.cpp\
                              MetricsKiviat.cpp

nodist_libMetricsKiviat_la_SOURCES = moc_KiviatInfo.cxx\
                                     moc_MetricsKiviat.cxx\
                                     ui_KiviatInfo.h\
                                     ui_MetricsWidget.h

BUILT_SOURCES = ui_KiviatInfo.h\
                ui_MetricsWidget.h

EXTRA_DIST = KiviatInfo.ui \
	MetricsWidget.ui

# Target for installing header 
headerTargetdir    =  $(roseQtWidgetsHeaderInstallPath)/
headerTarget_HEADERS = KiviatInfo.h\
                       MetricsKiviat.h


endif
