# This is a generated file

COMPASS_CHECKER_DIRECTORIES = \
	fileReadOnlyAccess \
	forbiddenFunctions \
	noAsmStmtsOps

COMPASS_INCLUDES = \
	-I$(srcdir)/fileReadOnlyAccess \
	-I$(srcdir)/forbiddenFunctions \
	-I$(srcdir)/noAsmStmtsOps

COMPASS_CHECKER_SRCS = \
	$(srcdir)/fileReadOnlyAccess/fileReadOnlyAccess.C \
	$(srcdir)/forbiddenFunctions/forbiddenFunctions.C \
	$(srcdir)/noAsmStmtsOps/noAsmStmtsOps.C

COMPASS_CHECKER_TESTS = \
	 $(srcdir)/fileReadOnlyAccess/fileReadOnlyAccessTest1.C \
	 $(srcdir)/forbiddenFunctions/forbiddenFunctionsTest1.C \
	 $(srcdir)/noAsmStmtsOps/noAsmStmtsOpsTest1.C

COMPASS_CHECKER_TEST_LABELS = \
	testFileReadOnlyAccess \
	testForbiddenFunctions \
	testNoAsmStmtsOps

COMPASS_CHECKER_TEX_FILES = \
	fileReadOnlyAccessDocs.tex \
	forbiddenFunctionsDocs.tex \
	noAsmStmtsOpsDocs.tex

docs: compass.tex
	test -f fixme.sty || ln -s $(top_srcdir)/docs/Rose/fixme.sty fixme.sty
	test -f compassCheckerDocs.tex || ln -s $(srcdir)/compassCheckerDocs.tex compassCheckerDocs.tex
	test -f acknowledgments.tex || ln -s $(srcdir)/acknowledgments.tex acknowledgments.tex
	test -f introduction.tex || ln -s $(srcdir)/introduction.tex introduction.tex
	test -f addYourOwnDetector.tex || ln -s $(srcdir)/addYourOwnDetector.tex addYourOwnDetector.tex
	test -f fileReadOnlyAccessDocs.new.tex || ln -s $(srcdir)/fileReadOnlyAccess/fileReadOnlyAccessDocs.new.tex fileReadOnlyAccessDocs.new.tex
	test -f forbiddenFunctionsDocs.new.tex || ln -s $(srcdir)/forbiddenFunctions/forbiddenFunctionsDocs.new.tex forbiddenFunctionsDocs.new.tex
	test -f noAsmStmtsOpsDocs.new.tex || ln -s $(srcdir)/noAsmStmtsOps/noAsmStmtsOpsDocs.new.tex noAsmStmtsOpsDocs.new.tex
	test -f compassDocs.tex || ln -s $(srcdir)/compassSupport/compassDocs.tex compassDocs.tex
	pdflatex compass.tex
	pdflatex compass.tex

TGUI=

testFileReadOnlyAccess: compassMain
	@echo -e "\nRunning testFileReadOnlyAccess"
	cp -f $(srcdir)/compass_parameters compass_parameters
	@echo -e "./compassMain $(srcdir)/fileReadOnlyAccess/fileReadOnlyAccessTest1.C $(TGUI)"
	@if ! test -f $(srcdir)/fileReadOnlyAccess/fileReadOnlyAccessTestOutput.txt; then \
		echo "creating $(srcdir)/fileReadOnlyAccess/fileReadOnlyAccessTestOutput.txt" ;\
		./compassMain $(srcdir)/fileReadOnlyAccess/fileReadOnlyAccessTest1.C $(TGUI) &> $(srcdir)/fileReadOnlyAccess/fileReadOnlyAccessTestOutput.txt ;\
		if(($$?!=0)) ; then \
			echo -e "\nError: compassMain EXIT FAILURE, see $(srcdir)/fileReadOnlyAccess/fileReadOnlyAccessTestOutput.txt" ;\
			exit 1 ;\
		fi ;\
		cp -f $(srcdir)/fileReadOnlyAccess/fileReadOnlyAccessTestOutput.txt $(top_builddir)/projects/compass/fileReadOnlyAccessTestOutput.txt.00 ;\
		grep "FileReadOnlyAccess:" $(top_builddir)/projects/compass/fileReadOnlyAccessTestOutput.txt.00 | sed -e s@\/.*\/@@g > $(srcdir)/fileReadOnlyAccess/fileReadOnlyAccessTestOutput.txt ;\
		rm -f $(top_builddir)/projects/compass/fileReadOnlyAccessTestOutput.txt.00 ;\
		if ! test -e $(srcdir)/fileReadOnlyAccess/fileReadOnlyAccessTestOutput.txt; then  \
			echo "error in compass tests: failed to create $(srcdir)/fileReadOnlyAccess/fileReadOnlyAccessTestOutput.txt" ;\
			exit 1 ;\
		elif ! test -s $(srcdir)/fileReadOnlyAccess/fileReadOnlyAccessTestOutput.txt; then \
			if test -e $(srcdir)/fileReadOnlyAccess/regressionException; then \
				echo "Warning this checker uses `cat $(srcdir)/fileReadOnlyAccess/regressionException`" ;\
			else \
				echo "error in compass tests: Created empty file $(srcdir)/fileReadOnlyAccess/fileReadOnlyAccessTestOutput.txt" ;\
				exit 1 ;\
			fi ;\
		fi ;\
	else \
		./compassMain $(srcdir)/fileReadOnlyAccess/fileReadOnlyAccessTest1.C $(TGUI) &> $(top_builddir)/projects/compass/fileReadOnlyAccessTestOutput.txt ;\
		if(($$?!=0)) ; then \
			echo -e "\nError: compassMain EXIT FAILURE, see $(top_builddir)/projects/compass/fileReadOnlyAccessTestOutput.txt" ;\
			exit 1 ;\
		fi ;\
		cp -f $(top_builddir)/projects/compass/fileReadOnlyAccessTestOutput.txt $(top_builddir)/projects/compass/fileReadOnlyAccessTestOutput.txt.00 ;\
		grep "FileReadOnlyAccess:" $(top_builddir)/projects/compass/fileReadOnlyAccessTestOutput.txt.00 | sed -e s@\/.*\/@@g > $(top_builddir)/projects/compass/fileReadOnlyAccessTestOutput.txt ;\
		rm -f $(top_builddir)/projects/compass/fileReadOnlyAccessTestOutput.txt.00 ;\
		if ! test -s $(top_builddir)/projects/compass/fileReadOnlyAccessTestOutput.txt || ! test -s $(srcdir)/fileReadOnlyAccess/fileReadOnlyAccessTestOutput.txt ; then \
			if test -e $(srcdir)/fileReadOnlyAccess/regressionException ; then \
				echo "Warning this checker uses `cat $(srcdir)/fileReadOnlyAccess/regressionException`" ;\
			fi ;\
			echo "warning in compass tests: Comparison between empty file $(top_builddir)/projects/compass/fileReadOnlyAccessTestOutput.txt or $(srcdir)/fileReadOnlyAccess/fileReadOnlyAccessTestOutput.txt" ;\
		fi ;\
		if ! diff -q $(top_builddir)/projects/compass/fileReadOnlyAccessTestOutput.txt $(srcdir)/fileReadOnlyAccess/fileReadOnlyAccessTestOutput.txt ; then \
			echo "error in compass tests: $(top_builddir)/projects/compass/fileReadOnlyAccessTestOutput.txt and $(srcdir)/fileReadOnlyAccess/fileReadOnlyAccessTestOutput.txt differ" ;\
			exit 1 ;\
		fi ;\
		rm $(top_builddir)/projects/compass/fileReadOnlyAccessTestOutput.txt ;\
	fi
	@echo "done testfileReadOnlyAccess"

testForbiddenFunctions: compassMain
	@echo -e "\nRunning testForbiddenFunctions"
	cp -f $(srcdir)/compass_parameters compass_parameters
	@echo -e "./compassMain $(srcdir)/forbiddenFunctions/forbiddenFunctionsTest1.C $(TGUI)"
	@if ! test -f $(srcdir)/forbiddenFunctions/forbiddenFunctionsTestOutput.txt; then \
		echo "creating $(srcdir)/forbiddenFunctions/forbiddenFunctionsTestOutput.txt" ;\
		./compassMain $(srcdir)/forbiddenFunctions/forbiddenFunctionsTest1.C $(TGUI) &> $(srcdir)/forbiddenFunctions/forbiddenFunctionsTestOutput.txt ;\
		if(($$?!=0)) ; then \
			echo -e "\nError: compassMain EXIT FAILURE, see $(srcdir)/forbiddenFunctions/forbiddenFunctionsTestOutput.txt" ;\
			exit 1 ;\
		fi ;\
		cp -f $(srcdir)/forbiddenFunctions/forbiddenFunctionsTestOutput.txt $(top_builddir)/projects/compass/forbiddenFunctionsTestOutput.txt.00 ;\
		grep "ForbiddenFunctions:" $(top_builddir)/projects/compass/forbiddenFunctionsTestOutput.txt.00 | sed -e s@\/.*\/@@g > $(srcdir)/forbiddenFunctions/forbiddenFunctionsTestOutput.txt ;\
		rm -f $(top_builddir)/projects/compass/forbiddenFunctionsTestOutput.txt.00 ;\
		if ! test -e $(srcdir)/forbiddenFunctions/forbiddenFunctionsTestOutput.txt; then  \
			echo "error in compass tests: failed to create $(srcdir)/forbiddenFunctions/forbiddenFunctionsTestOutput.txt" ;\
			exit 1 ;\
		elif ! test -s $(srcdir)/forbiddenFunctions/forbiddenFunctionsTestOutput.txt; then \
			if test -e $(srcdir)/forbiddenFunctions/regressionException; then \
				echo "Warning this checker uses `cat $(srcdir)/forbiddenFunctions/regressionException`" ;\
			else \
				echo "error in compass tests: Created empty file $(srcdir)/forbiddenFunctions/forbiddenFunctionsTestOutput.txt" ;\
				exit 1 ;\
			fi ;\
		fi ;\
	else \
		./compassMain $(srcdir)/forbiddenFunctions/forbiddenFunctionsTest1.C $(TGUI) &> $(top_builddir)/projects/compass/forbiddenFunctionsTestOutput.txt ;\
		if(($$?!=0)) ; then \
			echo -e "\nError: compassMain EXIT FAILURE, see $(top_builddir)/projects/compass/forbiddenFunctionsTestOutput.txt" ;\
			exit 1 ;\
		fi ;\
		cp -f $(top_builddir)/projects/compass/forbiddenFunctionsTestOutput.txt $(top_builddir)/projects/compass/forbiddenFunctionsTestOutput.txt.00 ;\
		grep "ForbiddenFunctions:" $(top_builddir)/projects/compass/forbiddenFunctionsTestOutput.txt.00 | sed -e s@\/.*\/@@g > $(top_builddir)/projects/compass/forbiddenFunctionsTestOutput.txt ;\
		rm -f $(top_builddir)/projects/compass/forbiddenFunctionsTestOutput.txt.00 ;\
		if ! test -s $(top_builddir)/projects/compass/forbiddenFunctionsTestOutput.txt || ! test -s $(srcdir)/forbiddenFunctions/forbiddenFunctionsTestOutput.txt ; then \
			if test -e $(srcdir)/forbiddenFunctions/regressionException ; then \
				echo "Warning this checker uses `cat $(srcdir)/forbiddenFunctions/regressionException`" ;\
			fi ;\
			echo "warning in compass tests: Comparison between empty file $(top_builddir)/projects/compass/forbiddenFunctionsTestOutput.txt or $(srcdir)/forbiddenFunctions/forbiddenFunctionsTestOutput.txt" ;\
		fi ;\
		if ! diff -q $(top_builddir)/projects/compass/forbiddenFunctionsTestOutput.txt $(srcdir)/forbiddenFunctions/forbiddenFunctionsTestOutput.txt ; then \
			echo "error in compass tests: $(top_builddir)/projects/compass/forbiddenFunctionsTestOutput.txt and $(srcdir)/forbiddenFunctions/forbiddenFunctionsTestOutput.txt differ" ;\
			exit 1 ;\
		fi ;\
		rm $(top_builddir)/projects/compass/forbiddenFunctionsTestOutput.txt ;\
	fi
	@echo "done testforbiddenFunctions"

testNoAsmStmtsOps: compassMain
	@echo -e "\nRunning testNoAsmStmtsOps"
	cp -f $(srcdir)/compass_parameters compass_parameters
	@echo -e "./compassMain $(srcdir)/noAsmStmtsOps/noAsmStmtsOpsTest1.C $(TGUI)"
	@if ! test -f $(srcdir)/noAsmStmtsOps/noAsmStmtsOpsTestOutput.txt; then \
		echo "creating $(srcdir)/noAsmStmtsOps/noAsmStmtsOpsTestOutput.txt" ;\
		./compassMain $(srcdir)/noAsmStmtsOps/noAsmStmtsOpsTest1.C $(TGUI) &> $(srcdir)/noAsmStmtsOps/noAsmStmtsOpsTestOutput.txt ;\
		if(($$?!=0)) ; then \
			echo -e "\nError: compassMain EXIT FAILURE, see $(srcdir)/noAsmStmtsOps/noAsmStmtsOpsTestOutput.txt" ;\
			exit 1 ;\
		fi ;\
		cp -f $(srcdir)/noAsmStmtsOps/noAsmStmtsOpsTestOutput.txt $(top_builddir)/projects/compass/noAsmStmtsOpsTestOutput.txt.00 ;\
		grep "NoAsmStmtsOps:" $(top_builddir)/projects/compass/noAsmStmtsOpsTestOutput.txt.00 | sed -e s@\/.*\/@@g > $(srcdir)/noAsmStmtsOps/noAsmStmtsOpsTestOutput.txt ;\
		rm -f $(top_builddir)/projects/compass/noAsmStmtsOpsTestOutput.txt.00 ;\
		if ! test -e $(srcdir)/noAsmStmtsOps/noAsmStmtsOpsTestOutput.txt; then  \
			echo "error in compass tests: failed to create $(srcdir)/noAsmStmtsOps/noAsmStmtsOpsTestOutput.txt" ;\
			exit 1 ;\
		elif ! test -s $(srcdir)/noAsmStmtsOps/noAsmStmtsOpsTestOutput.txt; then \
			if test -e $(srcdir)/noAsmStmtsOps/regressionException; then \
				echo "Warning this checker uses `cat $(srcdir)/noAsmStmtsOps/regressionException`" ;\
			else \
				echo "error in compass tests: Created empty file $(srcdir)/noAsmStmtsOps/noAsmStmtsOpsTestOutput.txt" ;\
				exit 1 ;\
			fi ;\
		fi ;\
	else \
		./compassMain $(srcdir)/noAsmStmtsOps/noAsmStmtsOpsTest1.C $(TGUI) &> $(top_builddir)/projects/compass/noAsmStmtsOpsTestOutput.txt ;\
		if(($$?!=0)) ; then \
			echo -e "\nError: compassMain EXIT FAILURE, see $(top_builddir)/projects/compass/noAsmStmtsOpsTestOutput.txt" ;\
			exit 1 ;\
		fi ;\
		cp -f $(top_builddir)/projects/compass/noAsmStmtsOpsTestOutput.txt $(top_builddir)/projects/compass/noAsmStmtsOpsTestOutput.txt.00 ;\
		grep "NoAsmStmtsOps:" $(top_builddir)/projects/compass/noAsmStmtsOpsTestOutput.txt.00 | sed -e s@\/.*\/@@g > $(top_builddir)/projects/compass/noAsmStmtsOpsTestOutput.txt ;\
		rm -f $(top_builddir)/projects/compass/noAsmStmtsOpsTestOutput.txt.00 ;\
		if ! test -s $(top_builddir)/projects/compass/noAsmStmtsOpsTestOutput.txt || ! test -s $(srcdir)/noAsmStmtsOps/noAsmStmtsOpsTestOutput.txt ; then \
			if test -e $(srcdir)/noAsmStmtsOps/regressionException ; then \
				echo "Warning this checker uses `cat $(srcdir)/noAsmStmtsOps/regressionException`" ;\
			fi ;\
			echo "warning in compass tests: Comparison between empty file $(top_builddir)/projects/compass/noAsmStmtsOpsTestOutput.txt or $(srcdir)/noAsmStmtsOps/noAsmStmtsOpsTestOutput.txt" ;\
		fi ;\
		if ! diff -q $(top_builddir)/projects/compass/noAsmStmtsOpsTestOutput.txt $(srcdir)/noAsmStmtsOps/noAsmStmtsOpsTestOutput.txt ; then \
			echo "error in compass tests: $(top_builddir)/projects/compass/noAsmStmtsOpsTestOutput.txt and $(srcdir)/noAsmStmtsOps/noAsmStmtsOpsTestOutput.txt differ" ;\
			exit 1 ;\
		fi ;\
		rm $(top_builddir)/projects/compass/noAsmStmtsOpsTestOutput.txt ;\
	fi
	@echo "done testnoAsmStmtsOps"


testAllCheckersSeparately: $(COMPASS_CHECKER_TEST_LABELS)
	@echo "$@ done"

#COMPASS CHECKER INCLUDES:


COMPASS_CHECKER_ARCHIVES = \
	archiveFileReadOnlyAccess \
	archiveForbiddenFunctions \
	archiveNoAsmStmtsOps

archiveFileReadOnlyAccess:
	rm -f $(srcdir)/compassRepository/fileReadOnlyAccess.*
	cd $(srcdir); tar -zcf compassRepository/fileReadOnlyAccess.tar.gz fileReadOnlyAccess
	@echo "updated fileReadOnlyAccess archive"

archiveForbiddenFunctions:
	rm -f $(srcdir)/compassRepository/forbiddenFunctions.*
	cd $(srcdir); tar -zcf compassRepository/forbiddenFunctions.tar.gz forbiddenFunctions
	@echo "updated forbiddenFunctions archive"

archiveNoAsmStmtsOps:
	rm -f $(srcdir)/compassRepository/noAsmStmtsOps.*
	cd $(srcdir); tar -zcf compassRepository/noAsmStmtsOps.tar.gz noAsmStmtsOps
	@echo "updated noAsmStmtsOps archive"

archiveCheckers: $(COMPASS_CHECKER_ARCHIVES)
