#------------ MetricsConfig Makefile.am -----------
include $(top_srcdir)/src/roseExtensions/qtWidgets/Make.inc 

if ROSE_WITH_ROSEQT

noinst_LTLIBRARIES = libMetricsConfig.la 



libMetricsConfig_la_SOURCES = MetricsConfig.cpp\
                              MetricsInfo.cpp

nodist_libMetricsConfig_la_SOURCES = moc_MetricsConfig.cxx\
                                     ui_MetricsConfig.h

BUILT_SOURCES = ui_MetricsConfig.h

# Target for installing header 
headerTargetdir    =  $(roseQtWidgetsHeaderInstallPath)/
headerTarget_HEADERS = MetricsConfig.h\
                       MetricsInfo.h


endif
