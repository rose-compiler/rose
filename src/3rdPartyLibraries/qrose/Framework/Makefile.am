include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

if ROSE_USE_QT

INCLUDES = $(ROSE_INCLUDES) $(QT_INCLUDES) 

  
#LD_FLAGS = @LIB_ROSE@ @ROSE_LDFLAGS@  @LIB_QT@ @QT_LDFLAGS@
METASOURCES = auto
include_HEADERS = qrose.h QRMacros.h QRMain.h QRException.h QRTiledWidget.h QRWidgetAccess.h QRWindow.h QRPanel.h QRSpaceWidget.h 
noinst_LTLIBRARIES = libqrframework.la

MOC_SOURCES = moc_QRMain_p.C moc_QRWindow.C moc_QRTiledWidget.C moc_QRSpaceWidget.C

libqrframework_la_SOURCES = $(MOC_SOURCES) \
     QRMain.C QRMain_p.C QRException.C QRTiledWidget.C QRWidgetAccess.C QRWindow.C QRPanel.C QRSpaceWidget.C
CLEANFILES=moc_*.C

if QROSE_USE_WINDOWS
moc_%.C: $(srcdir)/%.h 
	$(MOC) -o $@ `cygpath -w $<` -p$(srcdir)
else
moc_%.C: $(srcdir)/%.h 
	$(MOC) -o $@ $<
endif 

endif

EXTRA_DIST = QRException.h \
             QRMacros.h \
             QRMain.h \
             QRMain_p.h \
             qrose.h \
             QRPanel.h \
             QRSpaceWidget.h \
             QRTiledWidget.h \
             QRWidgetAccess.h \
             QRWidgetX.h \
             QRWindow.h 
