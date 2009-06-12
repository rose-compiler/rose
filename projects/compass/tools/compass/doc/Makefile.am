include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs
include $(top_srcdir)/projects/compass/src/compassSupport/compass_dirs.inc


../CHECKER_LIST_WITHOUT_COMMENTS:
	cd ..; $(MAKE) CHECKER_LIST_WITHOUT_COMMENTS


compassCheckerDocs.tex: ../CHECKER_LIST_WITHOUT_COMMENTS $(compass_scripts_dir)/compass_submission_setup_docsonly.sh
	cut -f 1 -d \  < ../CHECKER_LIST_WITHOUT_COMMENTS | $(compass_scripts_dir)/compass_submission_setup_docsonly.sh $(compass_checker_dir) > compassCheckerDocs.tex

docs: compass.tex compassCheckerDocs.tex
	test -f fixme.sty || ln -s $(top_srcdir)/docs/Rose/fixme.sty fixme.sty
	test -f html.sty || ln -s $(top_srcdir)/docs/Rose/html.sty html.sty
	sh -c '\
	for file in `ls $(srcdir)` ;\
	do \
	        test -f $${file} || ln -s $(srcdir)/$${file} $${file} ;\
	done ;\
	'
	@rm -f Makefile.*
	pdflatex --interaction=batchmode compass.tex 
	pdflatex --interaction=batchmode compass.tex
# pointless file moving	??
#	@mv -f compass.pdf ..

EXTRA_DIST = acknowledgments.tex \
	appendix.tex \
	compass_arc.png \
	compassdesign.png \
	compass_pic.png \
	CompassScreenshot.pdf \
	CompassScreenshot.png \
	compass.tex.in \
	compass_vim7.jpg \
	design.tex \
	emacs_screenshot.jpg \
	introduction.tex \
	Makefile.am \
	ToolGear_gui_compass_01.pdf \
	ToolGear_gui_compass_01.png \
	usingCompass.tex \
        usingCompassGui.tex \
	compass_categories.tex \
	usingCompassVerifier.tex

clean-local:
	find . -type l -maxdepth 1 -exec rm {} \;
	rm -f *.log *.out *.aux *.toc

CLEANFILES = compassCheckerDocs.tex compass.pdf 
