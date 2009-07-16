include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

if ROSE_USE_QT

INCLUDES = $(ROSE_INCLUDES) $(QT_INCLUDES) 

  
#LD_FLAGS = @LIB_ROSE@ @ROSE_LDFLAGS@  @LIB_QT@ @QT_LDFLAGS@
METASOURCES = auto
include_HEADERS = qrose.h QRMacros.h QRMain.h QRException.h QRTiledWidget.h QRWidgetAccess.h QRWindow.h QRPanel.h QRSpaceWidget.h QRGroup.h
noinst_LTLIBRARIES = libqrframework.la 

MOC_SOURCES = moc_QRMain_p.C moc_QRWindow.C moc_QRTiledWidget.C moc_QRSpaceWidget.C moc_QRGroup.C

# DQ (2/11/2009): Using nodist_ rule to support specification of files to be built at compile time and not distrivuted in the distribution.
libqrframework_la_SOURCES = QRMain.C QRMain_p.C QRException.C QRTiledWidget.C QRWidgetAccess.C QRWindow.C QRPanel.C QRSpaceWidget.C QRGroup.C
nodist_libqrframework_la_SOURCES = $(MOC_SOURCES)

CLEANFILES = moc_*.C

if QROSE_USE_WINDOWS
moc_%.C: $(srcdir)/%.h 
	$(MOC) -o $@ `cygpath -w $<` -p$(srcdir)
else
moc_%.C: $(srcdir)/%.h 
	$(MOC) -o $@ $<
endif 

else

# DQ (2/11/2009): If not configured to use Qt then we can't save the $(MOC_SOURCES) files
# MOC_SOURCES = 
# CLEANFILES =
# libqrframework_la_SOURCES = 

# moc_%.C: $(srcdir)/%.h 
# 	touch $<

endif

# DQ (2/11/2009): Save the $(MOC_SOURCES) files into the distribution.
EXTRA_DIST = QRException.h \
             QRMacros.h \
             QRMain.h \
             QRMain_p.h \
             qrose.h \
             QRPanel.h \
             QRSpaceWidget.h \
             QRTiledWidget.h \
             QRWidgetAccess.h \
             QRWindow.h \
             QRGroup.h

