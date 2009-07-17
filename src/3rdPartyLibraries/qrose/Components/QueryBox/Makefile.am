include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

if ROSE_USE_QT

INCLUDES = $(ROSE_INCLUDES) $(QT_INCLUDES) 

#LD_FLAGS = @LIB_ROSE@ @ROSE_LDFLAGS@  @LIB_QT@ @QT_LDFLAGS@
METASOURCES = auto

include_HEADERS = QRQueryBox.h QRQueryDomain.h QRQueryOp.h QRQueryRange.h 

noinst_LTLIBRARIES = libqrquerybox.la

MOC_SOURCES = moc_QRQueryBox.C 

libqrquerybox_la_SOURCES = QRQueryBox.C QRQueryDomain.C QRQueryOp.C QRQueryRange.C  
nodist_libqrquerybox_la_SOURCES = $(MOC_SOURCES)

CLEANFILES = moc_*.C

if QROSE_USE_WINDOWS
moc_%.C: $(srcdir)/%.h 
	$(MOC) -o $@ `cygpath -w $<` -p$(srcdir)
else
moc_%.C: $(srcdir)/%.h 
	$(MOC) -o $@ $< -frose.h -f$(notdir $<)
endif 

endif

EXTRA_DIST = QRQueryBox.h \
             QRQueryDomain.h \
             QRQueryOp.h \
             QRQueryRange.h 
