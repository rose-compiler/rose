#------------ TaskSystem Makefile.am -----------
include $(top_srcdir)/src/roseExtensions/qtWidgets/Make.inc 

if ROSE_WITH_ROSEQT

noinst_LTLIBRARIES = libTaskSystem.la 



libTaskSystem_la_SOURCES = CompilerOutputWidget.cpp\
                           GccTask.cpp\
                           RoseFrontendTask.cpp\
                           TaskList.cpp

nodist_libTaskSystem_la_SOURCES = moc_CompilerOutputWidget.cxx\
                                  moc_GccTask.cxx\
                                  moc_RoseFrontendTask.cxx\
                                  moc_TaskInterface.cxx\
                                  moc_TaskList.cxx\
                                  qrc_res.cpp



EXTRA_DIST = res.qrc \
	images/gears.gif \
	images/compile_warning.png \
	images/compile_error.png \
	images/compile_unspecified.png

# Target for installing header 
headerTargetdir    =  $(roseQtWidgetsHeaderInstallPath)/
headerTarget_HEADERS = CompilerOutputWidget.h\
                       GccTask.h\
                       RoseFrontendTask.h\
                       TaskInterface.h\
                       TaskList.h


endif
