include $(top_srcdir)/src/roseExtensions/qtWidgets/Make.inc
include $(top_srcdir)/projects/compass/src/compassSupport/compass_dirs.inc

# Include Paths for the roseQt-widgets  and rules for Qt's metacompiler moc,uic,rcc

if ROSE_WITH_ROSEQT

if ROSE_MPI
INCLUDES += -DROSE_MPI $(ROSE_INCLUDES) -I$(compass_support_dir) -I$(compass_srcdir) -I$(compass_checker_dir) -I$(compass_prereqs_dir) -I$(compass_support_bdir) -I. $(ROSE_QT_WIDGETS_INCLUDES)
else
INCLUDES += $(ROSE_INCLUDES) -I$(compass_support_dir) -I$(compass_srcdir) -I$(compass_checker_dir) -I$(compass_prereqs_dir) -I$(compass_support_bdir) -I. $(ROSE_QT_WIDGETS_INCLUDES)
endif

$(compass_build_tooldir)/compass/libCompassCheckers.la:
	cd .. && $(MAKE) libCompassCheckers.la
  
  
bin_PROGRAMS = compassMainGui2

roseQtDir = $(top_srcdir)/src/roseExtensions/qtWidgets
roseQtBuildDir = $(top_builddir)/src/roseExtensions/qtWidgets


compassMainGui2_SOURCES =  timer.cpp \
                            CompassMainWindow.cpp \
                            compassResult.cpp \
                            compassViolation.cpp \
                            compassChecker.cpp \
                            compassInterface.cpp \
                            CheckerTableModel.cpp \
                            ViolationsTableModel.cpp \
                            main.cpp

nodist_compassMainGui2_SOURCES =    moc_CompassMainWindow.cxx \
                                    moc_CheckerTableModel.cxx \
                                    qrc_res.cpp\
                                    ui_CompassMainWindow.h


BUILT_SOURCES = ui_CompassMainWindow.h 

compassMainGui2_LDADD = $(compass_build_tooldir)/compass/libCompassCheckers.la \
                        $(ROSE_LIBS) \
                        $(QT_LDFLAGS) \
                        -lrt


compassMainGui2_DEPENDENCIES = $(compass_build_tooldir)/compass/libCompassCheckers.la

endif

EXTRA_DIST = \
        compassChecker.h \
        CompassMainWindow.h \
        compassInterface.h \
        compassResult.h \
        compassViolation.h \
        timer.h
