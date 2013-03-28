set(WIDGETS_MOC_SOURCES
  QRButtons.h QRToolBar.h QRTable.h QREdit.h QRSelect.h QRProgress.h
  QRSeparator.h QRSelect_p.h QRTree.h)
QT4_WRAP_CPP(WIDGETS_MOC_CPP ${WIDGETS_MOC_SOURCES})

set(WIDGETS_SOURCES
  QRTable_p.C QREdit_p.C QRProgress_p.C QRSeparator_p.C QRButtons.C
  QRButtons_p.C QRToolBar.C QRTable.C  QREdit.C  QRSelect.C QRSelect_p.C
  QRProgress.C QRSeparator.C QRTree.C)
add_library(qrwidgets ${WIDGETS_SOURCES} ${WIDGETS_MOC_CPP})

########### install files ###############

install(FILES  QRButtons.h QRToolBar.h QRTable.h QREdit.h QRSelect.h QRProgress.h QRSeparator.h QRTree.h  DESTINATION ${INCLUDE_INSTALL_DIR})



#original Makefile.am contents follow:

#
#include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs
#
#if ROSE_USE_QT
#
#INCLUDES = $(ROSE_INCLUDES) $(QT_INCLUDES) 
#
##LD_FLAGS = @LIB_ROSE@ @ROSE_LDFLAGS@  @LIB_QT@ @QT_LDFLAGS@
#METASOURCES = auto
#
#include_HEADERS = QRButtons.h QRToolBar.h QRTable.h QREdit.h QRSelect.h QRProgress.h QRSeparator.h QRTree.h 
#
#noinst_LTLIBRARIES = libqrwidgets.la
#
#MOC_SOURCES = moc_QRButtons.C moc_QRToolBar.C moc_QRTable.C moc_QREdit.C moc_QRSelect.C moc_QRProgress.C moc_QRSeparator.C moc_QRSelect_p.C moc_QRTree.C 
#
#libqrwidgets_la_SOURCES = QRTable_p.C QREdit_p.C QRProgress_p.C QRSeparator_p.C QRButtons.C QRButtons_p.C QRToolBar.C QRTable.C  QREdit.C  QRSelect.C QRSelect_p.C QRProgress.C QRSeparator.C QRTree.C 
#nodist_libqrwidgets_la_SOURCES = $(MOC_SOURCES)
#
#CLEANFILES = moc_*.C
#
#if QROSE_USE_WINDOWS
#moc_%.C: $(srcdir)/%.h 
#	$(MOC) -o $@ `cygpath -w $<` -p$(srcdir)
#else
#moc_%.C: $(srcdir)/%.h 
#	$(MOC) -o $@ $<
#endif 
#
#endif
#
#EXTRA_DIST = QRButtons.h  \
#	     QRButtons_p.h \
#             QREdit_p.h \
#             QRProgress_p.h \
#             QRSelect_p.h \
#             QRSeparator_p.h \
#             QRTable_p.h \
#             QREdit.h \
#             QRProgress.h \
#             QRSelect.h \
#             QRSeparator.h \
#             QRTable.h \
#             QRToolBar.h 
