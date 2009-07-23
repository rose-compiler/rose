include $(top_srcdir)/src/roseExtensions/qtWidgets/Make.inc

if ROSE_USE_QT

#INCLUDES = $(ROSE_INCLUDES) $(QT_INCLUDES)

#METASOURCES = auto

noinst_PROGRAMS = t1_window t2_layout t3_buttons t4_events t5_components t6_query1 t7_qtcompat
t1_window_SOURCES = t1_window.C 
t2_layout_SOURCES = t2_layout.C
t3_buttons_SOURCES = t3_buttons.C
t4_events_SOURCES = t4_events.C
t5_components_SOURCES = t5_components.C
t6_query1_SOURCES = t6_query1.C
t7_qtcompat_SOURCES = t7_qtcompat.C

t1_window_LDADD =  $(ROSE_LIBS) $(QT_LDFLAGS) $(QROSE_LDFLAGS) 
t2_layout_LDADD =  $(ROSE_LIBS) $(QT_LDFLAGS) $(QROSE_LDFLAGS) 
t3_buttons_LDADD =  $(ROSE_LIBS) $(QT_LDFLAGS) $(QROSE_LDFLAGS)
t4_events_LDADD =  $(ROSE_LIBS) $(QT_LDFLAGS) $(QROSE_LDFLAGS) 
t5_components_LDADD = $(ROSE_LIBS) $(QT_LDFLAGS) $(QROSE_LDFLAGS) 
t6_query1_LDADD = $(ROSE_LIBS) $(QT_LDFLAGS) $(QROSE_LDFLAGS) 
t7_qtcompat_LDADD = $(ROSE_LIBS) $(QT_LDFLAGS) $(QROSE_LDFLAGS) $(top_builddir)/src/roseExtensions/qtWidgets/libRoseQtWidgets.la

EXTRA_DIST = test.C

all-local: test.C
	cp -f $(srcdir)/test.C .
	
clean:
	rm -f *.C *.o t*
	
endif

