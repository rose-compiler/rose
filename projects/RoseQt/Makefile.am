#------------ demo Makefile.am -----------
include $(top_srcdir)/src/roseExtensions/qtWidgets/Make.inc

SUBDIRS = AstViewer

if ROSE_WITH_ROSEQT

ROSEHPCT_BUILD_ROOT = $(top_builddir)/src/roseExtensions/roseHPCToolkit

CXXFLAGS += -DFRONTEND_CALL=\"$(top_builddir)/src/roseExtensions/qtWidgets/frontend_process\"
      
#INCLUDES += $(ROSEQT_INCLUDES) $(QT_INCLUDES) $(ROSE_INCLUDES) $(ROSEHPCT_DEP_INCLUDES)
INCLUDES +=  $(ROSEHPCT_DEP_INCLUDES) -I$(top_srcdir)/src/roseExtensions/roseHPCToolkit/include

print:
	@echo $(INCLUDES)
	@echo $(top_srcdir)/src/roseExtensions/roseHPCToolkit/include

bin_PROGRAMS = roseQt

roseQt_SOURCES        = MainWindow.cpp  main.cpp
nodist_roseQt_SOURCES = moc_MainWindow.cxx ui_MainWindow.h

BUILT_SOURCES = ui_MainWindow.h

EXTRA_DIST = MainWindow.ui \
			 MainWindow.h
			 res.qrc


roseQt_LDADD = $(ROSEHPCT_BUILD_ROOT)/src/librosehpct.la \
               $(ROSE_LIBS) \
               $(ROSEHPCT_DEP_LIBS)\
               $(ROSE_SEPARATE_LIBS)\
               $(LIBS_WITH_RPATH)
endif
