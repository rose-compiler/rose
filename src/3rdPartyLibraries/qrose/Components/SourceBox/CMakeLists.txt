include_directories(${QT_QTXML_INCLUDE_DIR})

set(SOURCEBOX_MOC_SOURCES
  markerwidget.h qeditor.h levelwidget.h linenumberwidget.h q3richtext_px.h
  QRSourceBox.h)
QT4_WRAP_CPP(SOURCEBOX_MOC_CPP ${SOURCEBOX_MOC_SOURCES})

set(SOURCEBOX_SOURCES
  cindent.C markerwidget.C  q3richtextx.C QRUnparser.C cpp_colorizer.C
  paragdata.C qeditor.C qsourcecolorizer.C levelwidget.C parenmatcher.C
  qeditor_indenter.C  simple_indent.C linenumberwidget.C  q3richtext_px.C
  QRSourceBox.C yyindent.C)
add_library(qrsourcebox ${SOURCEBOX_SOURCES} ${SOURCEBOX_MOC_CPP})
add_dependencies(qrsourcebox rosetta_generated)

########### install files ###############

install(FILES  QRSourceBox.h  DESTINATION ${INCLUDE_INSTALL_DIR})



#original Makefile.am contents follow:

#include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs
#
#if ROSE_USE_QT
#
#INCLUDES = $(ROSE_INCLUDES) $(QT_INCLUDES) 
#
##LD_FLAGS = @LIB_ROSE@ @ROSE_LDFLAGS@  @LIB_QT@ @QT_LDFLAGS@
#METASOURCES = auto
#
#include_HEADERS = QRSourceBox.h 
#
#noinst_LTLIBRARIES = libqrsourcebox.la
#
#MOC_SOURCES = moc_markerwidget.C moc_qeditor.C moc_levelwidget.C \
#              moc_linenumberwidget.C  moc_q3richtext_px.C  moc_QRSourceBox.C 
#
#libqrsourcebox_la_SOURCES = cindent.C markerwidget.C  q3richtextx.C  \
#                            QRUnparser.C cpp_colorizer.C paragdata.C \
#                            qeditor.C qsourcecolorizer.C levelwidget.C \
#                            parenmatcher.C  qeditor_indenter.C  simple_indent.C \
#                            linenumberwidget.C  q3richtext_px.C  QRSourceBox.C  \
#                            yyindent.C
#  
#nodist_libqrsourcebox_la_SOURCES = $(MOC_SOURCES)
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
#EXTRA_DIST = cindent.h \
#             markerwidget.h \
#             qeditor.h   \
#             qsourcecolorizer.h \
#             cpp_colorizer.h  \
#             paragdata.h \
#             qeditor_indenter.h \
#             simple_indent.h \
#             levelwidget.h  \
#             parenmatcher.h  \
#             QRSourceBox.h \
#             linenumberwidget.h \
#             q3richtext_px.h \
#             QRUnparser.h
#             
