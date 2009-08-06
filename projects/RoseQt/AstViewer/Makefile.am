include $(top_srcdir)/src/roseExtensions/qtWidgets/Make.inc

if ROSE_WITH_ROSEQT

INCLUDES += . $(ROSE_INCLUDES)


bin_PROGRAMS = AstProperties


AstProperties_SOURCES = main.cpp


nodist_AstProperties_SOURCES =  ui_MyWidget.h
                 
BUILT_SOURCES = ui_MyWidget.h


AstProperties_LDADD = $(ROSE_LIBS)
  
endif
