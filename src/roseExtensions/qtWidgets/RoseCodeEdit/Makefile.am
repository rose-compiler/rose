#------------ RoseCodeEdit Makefile.am -----------
include $(top_srcdir)/src/roseExtensions/qtWidgets/Make.inc 

if ROSE_WITH_ROSEQT

noinst_LTLIBRARIES = libRoseCodeEdit.la 


# Contents of Makefile.am.pre
INCLUDES += -I$(srcdir)/QCodeEdit -I$(srcdir)/QCodeEdit/document
SUBDIRS = QCodeEdit
libRoseCodeEdit_la_LIBADD   =  QCodeEdit/libQCodeEdit.la
# End of Makefile.am.pre Content

libRoseCodeEdit_la_SOURCES = RoseCodeEdit.cpp

nodist_libRoseCodeEdit_la_SOURCES = moc_RoseCodeEdit.cxx\
                                    qrc_example.cpp\
                                    qrc_QCodeEdit.cpp



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
	underline.png

# Target for installing header 
headerTargetdir    =  $(roseQtWidgetsHeaderInstallPath)/
headerTarget_HEADERS = RoseCodeEdit.h


endif
