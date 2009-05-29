#------------ MetricsKiviat Makefile.am -----------
include $(top_srcdir)/projects/roseQt/Make.inc 

if ROSE_USE_QT

noinst_LTLIBRARIES = libMetricsKiviat.la 



libMetricsKiviat_la_SOURCES = KiviatInfo.cpp\
                              MetricsKiviat.cpp

nodist_libMetricsKiviat_la_SOURCES = moc_KiviatInfo.cpp\
                                     moc_MetricsConfig.cpp\
                                     moc_MetricsKiviat.cpp\
                                     ui_KiviatInfo.h\
                                     ui_MetricsWidget.h

BUILT_SOURCES = ui_KiviatInfo.h\
                ui_MetricsWidget.h

endif
