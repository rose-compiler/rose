#------------ QCodeEditWidget Makefile.am -----------
include $(top_srcdir)/src/roseExtensions/qtWidgets/Make.inc 

if ROSE_WITH_ROSEQT

noinst_LTLIBRARIES = libQCodeEditWidget.la 


# Contents of Makefile.am.pre
INCLUDES += -I$(srcdir)/QCodeEdit -I$(srcdir)/QCodeEdit/document
SUBDIRS = QCodeEdit
libQCodeEditWidget_la_LIBADD   =  QCodeEdit/libQCodeEdit.la

# End of Makefile.am.pre Content

libQCodeEditWidget_la_SOURCES = QCodeEditWidget.cpp

nodist_libQCodeEditWidget_la_SOURCES = moc_QCodeEditWidget.cxx\
                                       qrc_example.cpp\
                                       qrc_QCodeEdit.cpp



# Target for installing header 
headerTargetdir    =  $(roseQtWidgetsHeaderInstallPath)/
headerTarget_HEADERS = QCodeEditWidget.h


endif

EXTRA_DIST = example.qrc \
	QCodeEdit.qrc \
	goto.png \
	copy.png \
	open.png \
	bold.png \
	paste.png \
	break.png \
	new.png \
	package_settings.png \
	undo.png \
	uncomment.png \
	formats.png \
	settings.png \
	indent.png \
	reload.png \
	exit.png \
	comment.png \
	error.png \
	strikeout.png \
	fillcolor.png \
	find.png \
	replace.png \
	unindent.png \
	editor.png \
	print.png \
	closeall.png \
	italic.png \
	redo.png \
	strokecolor.png \
	cut.png \
	warning.png \
	up.png \
	debug-resume.png \
	textcolor.png \
	saveas.png \
	save.png \
	down.png \
	underline.png \
	qxs/bibtex.qnfa \
	qxs/cpp.qnfa \
	qxs/csharp.qnfa \
	qxs/css.qnfa \
	qxs/dox.qnfa \
	qxs/formats.qxf \
	qxs/php.qnfa \
	qxs/pro.qnfa \
	qxs/python.qnfa \
	qxs/qtscript.qnfa \
	qxs/snippet.qnfa \
	qxs/tex.qxf \
	qxs/xml.qxf \
	qxs/bibtex.qxf \
	qxs/cpp.qxf \
	qxs/csharp.qxf \
	qxs/css.qxf \
	qxs/dox.qxf \
	qxs/marks.qxm \
	qxs/php.qxf \
	qxs/pro.qxf \
	qxs/python.qxf \
	qxs/qtscript.qxf \
	qxs/tex.qnfa \
	qxs/xml.qnfa

