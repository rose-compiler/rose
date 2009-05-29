#------------ MetricsConfig Makefile.am -----------
include $(top_srcdir)/projects/roseQt/Make.inc 

if ROSE_USE_QT

noinst_LTLIBRARIES = libMetricsConfig.la 



libMetricsConfig_la_SOURCES = MetricsConfig.cpp\
                              MetricsInfo.cpp

nodist_libMetricsConfig_la_SOURCES = moc_MetricsConfig.cpp\
                                     ui_MetricsConfig.h

BUILT_SOURCES = ui_MetricsConfig.h

endif
