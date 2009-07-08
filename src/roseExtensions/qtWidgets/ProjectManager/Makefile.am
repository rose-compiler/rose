#------------ ProjectManager Makefile.am -----------
include $(top_srcdir)/src/roseExtensions/qtWidgets/Make.inc 

if ROSE_WITH_ROSEQT

noinst_LTLIBRARIES = libProjectManager.la 



libProjectManager_la_SOURCES = DependencyInfo.cpp\
                               Project.cpp\
                               ProjectManager.cpp\
                               ProjectView.cpp

nodist_libProjectManager_la_SOURCES = moc_DependencyInfo.cxx\
                                      moc_Project.cxx\
                                      moc_ProjectView.cxx



# Target for installing header 
headerTargetdir    =  $(roseQtWidgetsHeaderInstallPath)/
headerTarget_HEADERS = DependencyInfo.h\
                       Project.h\
                       ProjectManager.h\
                       ProjectView.h


endif
