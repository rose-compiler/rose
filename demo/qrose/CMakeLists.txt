
include_directories(${KDE4_INCLUDES} ${KDE4_INCLUDE_DIR} ${QT_INCLUDES} )


########### next target ###############

set(t1_window_SRCS t1_window.C)

kde4_add_executable(t1_window ${t1_window_SRCS})

target_link_libraries(t1_window ${KDE4_KDECORE_LIBS})


########### next target ###############

set(t2_layout_SRCS t2_layout.C)

kde4_add_executable(t2_layout ${t2_layout_SRCS})

target_link_libraries(t2_layout ${KDE4_KDECORE_LIBS})


########### next target ###############

set(t3_buttons_SRCS t3_buttons.C)

kde4_add_executable(t3_buttons ${t3_buttons_SRCS})

target_link_libraries(t3_buttons ${KDE4_KDECORE_LIBS})


########### next target ###############

set(t4_events_SRCS t4_events.C)

kde4_add_executable(t4_events ${t4_events_SRCS})

target_link_libraries(t4_events ${KDE4_KDECORE_LIBS})


########### next target ###############

set(t5_components_SRCS t5_components.C)

kde4_add_executable(t5_components ${t5_components_SRCS})

target_link_libraries(t5_components ${KDE4_KDECORE_LIBS})


########### next target ###############

set(t6_query1_SRCS t6_query1.C)

kde4_add_executable(t6_query1 ${t6_query1_SRCS})

target_link_libraries(t6_query1 ${KDE4_KDECORE_LIBS})


########### install files ###############




#original Makefile.am contents follow:

#include $(top_srcdir)/src/roseExtensions/qtWidgets/Make.inc
#
#if ROSE_USE_QT
#
##INCLUDES = $(ROSE_INCLUDES) $(QT_INCLUDES)
#
##METASOURCES = auto
#
#noinst_PROGRAMS = t1_window t2_layout t3_buttons t4_events t5_components t6_query1
#if ROSE_WITH_ROSEQT
#noinst_PROGRAMS += t7_qtcompat
#endif
#
#t1_window_SOURCES = t1_window.C 
#t2_layout_SOURCES = t2_layout.C
#t3_buttons_SOURCES = t3_buttons.C
#t4_events_SOURCES = t4_events.C
#t5_components_SOURCES = t5_components.C
#t6_query1_SOURCES = t6_query1.C
#
#if ROSE_WITH_ROSEQT
#t7_qtcompat_SOURCES = t7_qtcompat.C
#endif
#
#t1_window_LDADD =  $(ROSE_LIBS) $(QT_LDFLAGS) $(QROSE_LDFLAGS) 
#t2_layout_LDADD =  $(ROSE_LIBS) $(QT_LDFLAGS) $(QROSE_LDFLAGS) 
#t3_buttons_LDADD =  $(ROSE_LIBS) $(QT_LDFLAGS) $(QROSE_LDFLAGS)
#t4_events_LDADD =  $(ROSE_LIBS) $(QT_LDFLAGS) $(QROSE_LDFLAGS) 
#t5_components_LDADD = $(ROSE_LIBS) $(QT_LDFLAGS) $(QROSE_LDFLAGS) 
#t6_query1_LDADD = $(ROSE_LIBS) $(QT_LDFLAGS) $(QROSE_LDFLAGS) 
#if ROSE_WITH_ROSEQT
#t7_qtcompat_LDADD = $(ROSE_LIBS) $(QT_LDFLAGS) $(QROSE_LDFLAGS) $(top_builddir)/src/roseExtensions/qtWidgets/libRoseQtWidgets.la
#endif
#
#EXTRA_DIST = test.C icons.h
#
#all-local: test.C
#	cp -f $(srcdir)/test.C .
#	
#clean:
#	rm -f *.C *.o t*
#	
#endif
#
