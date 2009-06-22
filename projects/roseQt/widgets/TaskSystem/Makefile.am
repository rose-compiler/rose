#------------ TaskSystem Makefile.am -----------
include $(top_srcdir)/projects/roseQt/Make.inc 

if ROSE_WITH_ROSEQT

noinst_LTLIBRARIES = libTaskSystem.la 



libTaskSystem_la_SOURCES = CompilerOutputWidget.cpp\
                           GccTask.cpp\
                           RoseFrontendTask.cpp\
                           TaskList.cpp

nodist_libTaskSystem_la_SOURCES = moc_CompilerOutputWidget.cxx\
                                  moc_GccTask.cxx\
                                  moc_TaskInterface.cxx\
                                  moc_TaskList.cxx\
                                  qrc_res.cpp



# Target for installing header 
headerTargetdir    =  $(roseQtWidgetsIncPath)/
headerTarget_HEADERS = CompilerOutputWidget.h\
                       GccTask.h\
                       RoseFrontendTask.h\
                       TaskInterface.h\
                       TaskList.h


endif
