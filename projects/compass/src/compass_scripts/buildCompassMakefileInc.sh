#!/bin/sh

srcdir="$1"

# CHECKER_LIST_WITHOUT_COMMENTS has two space-separated fields: the original
# checker name, and the checker name with the first character uppercase

echo "# This is a generated file"
echo ""
echo "COMPASS_CHECKER_DIRECTORIES = \\"
cut -f 1 -d \  < CHECKER_LIST_WITHOUT_COMMENTS | sed 's/^.*$/	& \\/'
echo ""
echo "COMPASS_INCLUDES = \\"
cut -f 1 -d \  < CHECKER_LIST_WITHOUT_COMMENTS | sed 's,^.*$,	-I$(compass_checker_dir)/& \\,'
echo ""
echo "COMPASS_CHECKER_SRCS = \\"
cut -f 1 -d \  < CHECKER_LIST_WITHOUT_COMMENTS | sed 's,^.*$,	&/&.C \\,'
echo ""
echo "COMPASS_CHECKER_OBJS = \\"
cut -f 1 -d \  < CHECKER_LIST_WITHOUT_COMMENTS | sed 's,^.*$,	&.lo \\,'
echo ""
echo "COMPASS_CHECKER_TESTS = \\"
cut -f 1 -d \  < CHECKER_LIST_WITHOUT_COMMENTS | sed 's,^.*$,	 &/&Test1.C \\,'
echo ""
echo "COMPASS_CHECKER_TEST_LABELS = \\"
cut -f 2 -d \  < CHECKER_LIST_WITHOUT_COMMENTS | sed 's,^.*$,	test& \\,'
echo ""
echo "COMPASS_CHECKER_TEX_FILES = \\"
cut -f 1 -d \  < CHECKER_LIST_WITHOUT_COMMENTS | sed 's,^.*$,	&Docs.tex \\,'
echo ""
# GMY Jul. 2008 refactored this code directly into projects/compass/Makefile.am
#echo "docs: compass.tex compassCheckerDocs.tex"
#echo '	test -f fixme.sty || ln -s $(top_srcdir)/docs/Rose/fixme.sty fixme.sty'
#for i in compass_arc.png compassdesign.png compass_pic.png acknowledgments.tex introduction.tex design.tex usingCompass.tex emacs_screenshot.jpg compass_vim7.jpg CompassScreenshot.pdf ToolGear_gui_compass_01.pdf appendix.tex; do echo "	test -f $i || ln -s \$(compass_srcdir)/$i $i" ; done
#echo '	test -f compassDocs.tex || ln -s $(compass_srcdir)/compassSupport/compassDocs.tex compassDocs.tex'
#echo '	pdflatex --interaction=batchmode compass.tex'
#echo '	pdflatex --interaction=batchmode compass.tex'
#echo ""
echo "TGUI="
echo ""
#cat CHECKER_LIST_WITHOUT_COMMENTS | while read ch chupper; do sed "s,@CHECKER@,$ch,g" < $srcdir/testMakefile.in | sed "s,@UCHECKER@,$chupper,g" ; done
#echo ""
#echo 'testAllCheckersSeparately: $(COMPASS_CHECKER_TEST_LABELS)'
#echo '	@echo "$@ done"'
#echo ""
#echo "#COMPASS CHECKER INCLUDES:"
#echo ""
#echo ""
#echo "COMPASS_CHECKER_ARCHIVES = \\"
#cut -f 2 -d \  < CHECKER_LIST_WITHOUT_COMMENTS | sed 's,^.*$,	archive& \\,'
#echo ""
#cat CHECKER_LIST_WITHOUT_COMMENTS | while read ch chupper; do sed "s,@CHECKER@,$ch,g" < $srcdir/archiveMakefile.in | sed "s,@UCHECKER@,$chupper,g" ; done
#echo "archiveCheckers: \$(COMPASS_CHECKER_ARCHIVES)"
#echo ""
echo "# Compass checker object file build rules"
cut -f 1 -d \  < CHECKER_LIST_WITHOUT_COMMENTS | sed 's,^.*$,&.lo: $(compass_checker_dir)/&/&.C@	$(LTCXXCOMPILE) -c -o &.lo $(compass_checker_dir)/&/&.C@,' | tr '@' '\n'
