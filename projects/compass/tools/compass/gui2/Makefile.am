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
			-L$(compass_support_bdir) -lCompass \
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
        timer.h \
		ViolationsTableModel.h \
		CompassMainWindow.ui \
		MainWindow.ui \
		CheckerTableModel.h \
		res.qrc \
		icons/application-exit.png \
		icons/chronometer.png \
		icons/compass.gif \
		icons/dialog-cancel.png \
		icons/dialog-close.png \
		icons/dialog-ok-apply.png \
		icons/document-properties.png \
		icons/document-save-as.png \
		icons/document-save.png \
		icons/edit-copy.png \
		icons/edit-cut.png \
		icons/edit-delete.png \
		icons/edit-find.png \
		icons/edit-paste.png \
		icons/edit-redo.png \
		icons/edit-select-all.png \
		icons/edit-undo.png \
		icons/error.gif \
		icons/flag-green.png \
		icons/flag-red.png \
		icons/flag-yellow.png \
		icons/gears.gif \
		icons/HelpIcon.gif \
		icons/mail-mark-notjunk.png \
		icons/ok.gif \
		icons/process-stop.png \
		icons/refresh.gif \
		icons/view-refresh.png \
		icons/view-statistics.png \
		icons/window-close.png


