include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

if ROSE_USE_QT

AM_CPPFLAGS = $(ROSE_INCLUDES) $(QT_AM_CPPFLAGS) 

#LD_FLAGS = @LIB_ROSE@ @ROSE_LDFLAGS@  @LIB_QT@ @QT_LDFLAGS@
METASOURCES = auto

pkginclude_HEADERS = QRTreeBox.h

noinst_LTLIBRARIES = libqrtreebox.la

MOC_SOURCES = moc_QRTreeBox.C 

libqrtreebox_la_SOURCES = QRListViewItem.C QRTreeBox.C QRTreeVisitor.C
  
nodist_libqrtreebox_la_SOURCES = $(MOC_SOURCES)

CLEANFILES = moc_*.C

if QROSE_USE_WINDOWS
moc_%.C: $(srcdir)/%.h 
	$(MOC) -o $@ `cygpath -w $<` -p$(srcdir)
else
moc_%.C: $(srcdir)/%.h 
	$(MOC) -o $@ $<
endif 

endif

EXTRA_DIST = CMakeLists.txt QRListViewItem.h \
             QRTreeBox.h \
             QRTreeVisitor.h
              
