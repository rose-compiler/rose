#------------ MetricFilter Makefile.am -----------
include $(top_srcdir)/projects/roseQt/Make.inc 

if ROSE_WITH_ROSEQT

noinst_LTLIBRARIES = libMetricFilter.la 



libMetricFilter_la_SOURCES = MetricFilter.cpp

nodist_libMetricFilter_la_SOURCES = moc_MetricFilter.cxx\
                                    ui_MetricFilter.h

BUILT_SOURCES = ui_MetricFilter.h

# Target for installing header 
headerTargetdir    =  $(roseQtWidgetsIncPath)/
headerTarget_HEADERS = MetricFilter.h


endif
