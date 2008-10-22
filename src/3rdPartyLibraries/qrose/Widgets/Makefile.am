
include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

if ROSE_USE_QT

INCLUDES = $(ROSE_INCLUDES) $(QT_INCLUDES) 

#LD_FLAGS = @LIB_ROSE@ @ROSE_LDFLAGS@  @LIB_QT@ @QT_LDFLAGS@
METASOURCES = auto

include_HEADERS = QRButtons.h QRToolBar.h QRTable.h QREdit.h QRSelect.h QRProgress.h QRSeparator.h

noinst_LTLIBRARIES = libqrwidgets.la

MOC_SOURCES = moc_QRButtons.cpp moc_QRToolBar.cpp moc_QRTable.cpp moc_QREdit.cpp moc_QRSelect.cpp moc_QRProgress.cpp moc_QRSeparator.cpp moc_QRSelect_p.cpp

libqrwidgets_la_SOURCES = $(MOC_SOURCES) QRTable_p.cpp QREdit_p.cpp QRProgress_p.cpp QRSeparator_p.cpp QRButtons.cpp QRToolBar.cpp QRTable.cpp  QREdit.cpp  QRSelect.cpp QRSelect_p.cpp QRProgress.cpp QRSeparator.cpp

CLEANFILES=moc_*.cpp

if QROSE_USE_WINDOWS
moc_%.cpp: $(srcdir)/%.h 
	$(MOC) -o $@ `cygpath -w $<` -p$(srcdir)
else
moc_%.cpp: $(srcdir)/%.h 
	$(MOC) -o $@ $<
endif 

endif

EXTRA_DIST = QRButtons.h  \
             QREdit_p.h \
             QRProgress_p.h \
             QRSelect_p.h \
             QRSeparator_p.h \
             QRTable_p.h \
             QREdit.h \
             QRProgress.h \
             QRSelect.h \
             QRSeparator.h \
             QRTable.h \
             QRToolBar.h
