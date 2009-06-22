#------------ demo Makefile.am -----------
include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs
include $(top_srcdir)/projects/roseQt/UseQtWidgets.inc

if ROSE_WITH_ROSEQT


# Include Paths for the roseQt-widgets  and rules for Qt's metacompiler moc,uic,rcc
roseQtDir = $(top_srcdir)/projects/roseQt/



ROSEHPCT_BUILD_ROOT = $(top_builddir)/src/roseExtensions/roseHPCToolkit

      
INCLUDES = $(ROSEQT_INCLUDES) $(QT_INCLUDES) $(ROSE_INCLUDES) $(ROSEHPCT_DEP_INCLUDES)\
           -I$(top_srcdir)/src/roseExtensions/roseHPCToolkit/include
                         

bin_PROGRAMS = roseQtDemo

roseQtDemo_SOURCES        = MainWindow.cpp  main.cpp
nodist_roseQtDemo_SOURCES = moc_MainWindow.cxx ui_MainWindow.h

BUILT_SOURCES = ui_MainWindow.h


roseQtDemo_LDADD = $(top_builddir)/projects/roseQt/widgets/libRoseQtWidgets.la \
                   $(ROSEHPCT_BUILD_ROOT)/src/librosehpct.la \
                   $(ROSE_LIBS) \
                   $(ROSEHPCT_DEP_LIBS)
                   
endif
