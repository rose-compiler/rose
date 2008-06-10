
# Permit switch to pdflatex from latex.
LATEX = pdflatex

# We can't specify the source directory to latex so we have to build
# links to the files in the source directory.  Other ideas?
# I have been unable to have the links be built unless I build them all the file.
latexFiles = \
   introduction.tex ASTGraphGenerator.tex AST_PDF_Generator.tex \
   identityTranslator.tex queryLibraryExample.tex \
   traversal.tex loopRecognition.tex typeInfoFromFunctionParameters.tex resolveOverloadedFunction.tex \
   templateParameter.tex instrumentationExample.tex addVariableDeclaration.tex \
   addAssignmentStmt.tex addExpression.tex\
   addFunctionDeclaration.tex buildCFG.tex parallelChecker.tex runtimeErrorCheck.tex defuseAnalysis.tex binaryAnalysis.tex buildCG.tex classHierarchyGraph.tex loopOptimization.tex inliner.tex outliner.tex \
   parser-docs.tex codeCoverage.tex globalVariableHandling.tex scopeInformation.tex sideeffect-docs.tex \
   tauInstrumentation.tex templateSupport.tex dataBaseSupport.tex recognitionOfAbstractions.tex \
   debuggingSupport.tex commandlineProcessing.tex customGraphs.tex tutorialMakefile.tex wrapup.tex \
   appendix.tex astFileIO.tex uniqueNames.tex wholeGraphAST.tex addingComments.tex \
   partialRedundancyElimination.tex codeGenerationFormatControl.tex copyHelp.tex \
   roseHPCT.tex sharedMemoryTraversals.tex distributedMemoryTraversals.tex \
   howToContribute.tex

# glossary.tex FAQ.tex fixme.sty

FORCE_BUILD:

# To build this once make the target a single name (instead of a list like $(latexFiles))
#$(latexFiles): FORCE_BUILD
link_LaTeX_files:
#	test -f gettingStarted.tex || ln -s $(top_srcdir)/docs/Rose/Tutorial/gettingStarted.tex gettingStarted.tex
	test -f introduction.tex || ln -s $(top_srcdir)/docs/Rose/Tutorial/introduction.tex introduction.tex
	test -f ASTGraphGenerator.tex || ln -s $(top_srcdir)/docs/Rose/Tutorial/ASTGraphGenerator.tex ASTGraphGenerator.tex
	test -f AST_PDF_Generator.tex || ln -s $(top_srcdir)/docs/Rose/Tutorial/AST_PDF_Generator.tex AST_PDF_Generator.tex
	test -f identityTranslator.tex || ln -s $(top_srcdir)/docs/Rose/Tutorial/identityTranslator.tex identityTranslator.tex
	test -f queryLibraryExample.tex || ln -s $(top_srcdir)/docs/Rose/Tutorial/queryLibraryExample.tex queryLibraryExample.tex
	test -f traversal.tex || ln -s $(top_srcdir)/docs/Rose/Tutorial/traversal.tex traversal.tex
	test -f loopRecognition.tex || ln -s $(top_srcdir)/docs/Rose/Tutorial/loopRecognition.tex loopRecognition.tex
	test -f dataBaseSupport.tex || ln -s $(top_srcdir)/docs/Rose/Tutorial/dataBaseSupport.tex dataBaseSupport.tex
	test -f typeInfoFromFunctionParameters.tex || ln -s $(top_srcdir)/docs/Rose/Tutorial/typeInfoFromFunctionParameters.tex typeInfoFromFunctionParameters.tex
	test -f resolveOverloadedFunction.tex || ln -s $(top_srcdir)/docs/Rose/Tutorial/resolveOverloadedFunction.tex resolveOverloadedFunction.tex
	test -f templateParameter.tex || ln -s $(top_srcdir)/docs/Rose/Tutorial/templateParameter.tex templateParameter.tex
	test -f templateSupport.tex || ln -s $(top_srcdir)/docs/Rose/Tutorial/templateSupport.tex templateSupport.tex
	test -f instrumentationExample.tex || ln -s $(top_srcdir)/docs/Rose/Tutorial/instrumentationExample.tex instrumentationExample.tex
	test -f addVariableDeclaration.tex || ln -s $(top_srcdir)/docs/Rose/Tutorial/addVariableDeclaration.tex addVariableDeclaration.tex
	test -f addAssignmentStmt.tex || ln -s $(top_srcdir)/docs/Rose/Tutorial/addAssignmentStmt.tex addAssignmentStmt.tex
	test -f addExpression.tex || ln -s $(top_srcdir)/docs/Rose/Tutorial/addExpression.tex addExpression.tex
	test -f addFunctionDeclaration.tex || ln -s $(top_srcdir)/docs/Rose/Tutorial/addFunctionDeclaration.tex addFunctionDeclaration.tex
	test -f codeCoverage.tex || ln -s $(top_srcdir)/docs/Rose/Tutorial/codeCoverage.tex codeCoverage.tex
	test -f globalVariableHandling.tex || ln -s $(top_srcdir)/docs/Rose/Tutorial/globalVariableHandling.tex globalVariableHandling.tex
	test -f buildCFG.tex || ln -s $(top_srcdir)/docs/Rose/Tutorial/buildCFG.tex buildCFG.tex
	test -f runtimeErrorCheck.tex || ln -s $(top_srcdir)/docs/Rose/Tutorial/runtimeErrorCheck.tex runtimeErrorCheck.tex	
	test -f parallelChecker.tex || ln -s $(top_srcdir)/docs/Rose/Tutorial/parallelChecker.tex parallelChecker.tex	
	test -f defuseAnalysis.tex || ln -s $(top_srcdir)/docs/Rose/Tutorial/defuseAnalysis.tex defuseAnalysis.tex	
	test -f binaryAnalysis.tex || ln -s $(top_srcdir)/docs/Rose/Tutorial/binaryAnalysis.tex binaryAnalysis.tex	
	test -f buildCG.tex || ln -s $(top_srcdir)/docs/Rose/Tutorial/buildCG.tex buildCG.tex
	test -f classHierarchyGraph.tex || ln -s $(top_srcdir)/docs/Rose/Tutorial/classHierarchyGraph.tex classHierarchyGraph.tex
	test -f loopOptimization.tex || ln -s $(top_srcdir)/docs/Rose/Tutorial/loopOptimization.tex loopOptimization.tex
	test -f inliner.tex || ln -s $(top_srcdir)/docs/Rose/Tutorial/inliner.tex inliner.tex
	test -f outliner.tex || ln -s $(top_srcdir)/docs/Rose/Tutorial/outliner.tex outliner.tex
	test -f recognitionOfAbstractions.tex || ln -s $(top_srcdir)/docs/Rose/Tutorial/recognitionOfAbstractions.tex recognitionOfAbstractions.tex
	test -f debuggingSupport.tex || ln -s $(top_srcdir)/docs/Rose/Tutorial/debuggingSupport.tex debuggingSupport.tex
	test -f tauInstrumentation.tex || ln -s $(top_srcdir)/docs/Rose/Tutorial/tauInstrumentation.tex tauInstrumentation.tex
	test -f uniqueNames.tex || ln -s $(top_srcdir)/docs/Rose/Tutorial/uniqueNames.tex uniqueNames.tex
	test -f scopeInformation.tex || ln -s $(top_srcdir)/docs/Rose/Tutorial/scopeInformation.tex scopeInformation.tex
	test -f commandlineProcessing.tex || ln -s $(top_srcdir)/docs/Rose/Tutorial/commandlineProcessing.tex commandlineProcessing.tex
	test -f customGraphs.tex || ln -s $(top_srcdir)/docs/Rose/Tutorial/customGraphs.tex customGraphs.tex
	test -f addingComments.tex || ln -s $(top_srcdir)/docs/Rose/Tutorial/addingComments.tex addingComments.tex
	test -f partialRedundancyElimination.tex || ln -s $(top_srcdir)/docs/Rose/Tutorial/partialRedundancyElimination.tex partialRedundancyElimination.tex
	test -f codeGenerationFormatControl.tex || ln -s $(top_srcdir)/docs/Rose/Tutorial/codeGenerationFormatControl.tex codeGenerationFormatControl.tex
	test -f roseHPCT.tex || ln -s $(top_srcdir)/docs/Rose/Tutorial/roseHPCT.tex roseHPCT.tex
	test -f sharedMemoryTraversals.tex || ln -s $(top_srcdir)/docs/Rose/Tutorial/sharedMemoryTraversals.tex sharedMemoryTraversals.tex
	test -f distributedMemoryTraversals.tex || ln -s $(top_srcdir)/docs/Rose/Tutorial/distributedMemoryTraversals.tex distributedMemoryTraversals.tex
	test -f copyHelp.tex || ln -s $(top_srcdir)/docs/Rose/Tutorial/copyHelp.tex copyHelp.tex
	test -f howToContribute.tex || ln -s $(top_srcdir)/docs/Rose/Tutorial/howToContribute.tex howToContribute.tex
	test -f astFileIO.tex || ln -s $(top_srcdir)/docs/Rose/Tutorial/astFileIO.tex astFileIO.tex
	test -f wholeGraphAST.tex || ln -s $(top_srcdir)/docs/Rose/Tutorial/wholeGraphAST.tex wholeGraphAST.tex
	test -f tutorialMakefile.tex || ln -s $(top_srcdir)/docs/Rose/Tutorial/tutorialMakefile.tex tutorialMakefile.tex
	test -f wrapup.tex || ln -s $(top_srcdir)/docs/Rose/Tutorial/wrapup.tex wrapup.tex
	test -f appendix.tex || ln -s $(top_srcdir)/docs/Rose/Tutorial/appendix.tex appendix.tex
	test -f glossary.tex || ln -s $(top_srcdir)/docs/Rose/glossary.tex glossary.tex
	test -f FAQ.tex || ln -s $(top_srcdir)/docs/Rose/FAQ.tex FAQ.tex
	test -f fixme.sty || ln -s $(top_srcdir)/docs/Rose/fixme.sty fixme.sty

# This is an evloving list, but these are a few of the required files
generatedCodeExamples = \
   $(top_builddir)/tutorial/inputCode_ASTGraphGenerator.ps \
   $(top_builddir)/tutorial/rose_inputCode_AddFunctionDeclaration.C \
   $(top_builddir)/tutorial/rose_inputCode_AddFunctionDeclaration2.C \
   $(top_builddir)/tutorial/rose_inputCode_AddFunctionDeclaration3.C \
   $(top_builddir)/tutorial/rose_inputCode_IdentityTranslator.C \
   $(top_builddir)/tutorial/rose_inputCode_AddVariableDeclaration.C \
   $(top_builddir)/tutorial/rose_inputCode_AddVariableDeclaration2.C \
   $(top_builddir)/tutorial/rose_transformation_inputCode_InstrumentationTranslator1.C \
   $(top_builddir)/tutorial/rose_inputCode_ExampleDebugging.C \
   $(top_builddir)/tutorial/codeCoverage.out \
   $(top_builddir)/tutorial/rose_inputCode_inlineTransformations.C \
   $(top_builddir)/tutorial/roseHPCT/profiled.out

$(generatedCodeExamples):
	cd $(top_builddir)/tutorial; $(MAKE) docs

latex-docs-tutorial: $(srcdir)/$(PACKAGE_NAME)-$(PACKAGE_VERSION)-Tutorial.pdf

# Development rule to force build of latex documentation
# force-tutorial-docs: tutorial.tex $(latexFiles) $(generatedCodeExamples)
force: force-tutorial-docs
force-tutorial-docs: tutorial.tex gettingStarted.tex link_LaTeX_files logo
	echo "Building tutorial documentation ($(srcdir)/$(PACKAGE_NAME)-$(PACKAGE_VERSION)-Tutorial.pdf ... ";
	$(LATEX) tutorial.tex;
	$(LATEX) tutorial.tex;
#	$(LATEX) tutorial.tex;
	mv tutorial.pdf $(PACKAGE_NAME)-$(PACKAGE_VERSION)-Tutorial.pdf
	cp -f $(PACKAGE_NAME)-$(PACKAGE_VERSION)-Tutorial.pdf $(PACKAGE_NAME)-$(PACKAGE_VERSION)-Tutorial_uncompressed.pdf;
#	DQ (7/13/2006): Change so that Type 1 (outline) fonts will be used rather than Type 3 (bitmaps).  
#	This should make the manuals look better on some viewers, and makes xpdf a lot faster when viewing 
#	the tutorial (suggested by Jeremiah).
#	dvips -Pcmz -o $(PACKAGE_NAME)-$(PACKAGE_VERSION)-Tutorial.ps tutorial.dvi;
	mkdir -p ../ROSE_WebPages/ROSE_Tutorial
#	cp -f $(PACKAGE_NAME)-$(PACKAGE_VERSION)-Tutorial.ps ../ROSE_WebPages/ROSE_Tutorial/$(PACKAGE_NAME)-$(PACKAGE_VERSION)-Tutorial.ps;
#	ps2pdf $(PACKAGE_NAME)-$(PACKAGE_VERSION)-Tutorial.ps
	cp -f $(PACKAGE_NAME)-$(PACKAGE_VERSION)-Tutorial.pdf ../ROSE_WebPages/ROSE_Tutorial/$(PACKAGE_NAME)-$(PACKAGE_VERSION)-Tutorial.pdf;
#	latex2html -dir ../ROSE_WebPages/ROSE_Tutorial -split 2 -noinfo -local_icons tutorial.tex;
	echo "Copy the tutorial documentation to the source tree so it can be saved (running gzip) ... ";
#	rm -rf $(PACKAGE_NAME)-$(PACKAGE_VERSION)-Tutorial.ps.gz
#	gzip --fast $(PACKAGE_NAME)-$(PACKAGE_VERSION)-Tutorial.ps;
#	This rest of these lines are not really requires since in the automake tests we don't run "make force"
#	echo "Make the source directory writable (automake generated test directories are not)"
#	chmod +w $(srcdir)
#	cp $(PACKAGE_NAME)-$(PACKAGE_VERSION)-Tutorial.ps.gz $(srcdir)/$(PACKAGE_NAME)-$(PACKAGE_VERSION)-Tutorial.ps.gz;
	cp -f $(PACKAGE_NAME)-$(PACKAGE_VERSION)-Tutorial.pdf $(srcdir)/$(PACKAGE_NAME)-$(PACKAGE_VERSION)-Tutorial.pdf;

logo:
	mkdir -p ../ROSE_WebPages;
	cp -f @top_srcdir@/docs/Rose/compass_rose.gif ../ROSE_WebPages

$(srcdir)/$(PACKAGE_NAME)-$(PACKAGE_VERSION)-Tutorial.pdf $(PACKAGE_NAME)-$(PACKAGE_VERSION)-Tutorial.pdf: tutorial.tex gettingStarted.tex link_LaTeX_files logo
	@echo "Building Latex documentation for Rose Project (tutorial) ... "
#	@echo "Calling \"$(PACKAGE_NAME)-$(PACKAGE_VERSION)-Tutorial.ps.gz\" make rule in ROSE/docs/Rose/Makefile ... (MAKE = $(MAKE) AM_MAKEFLAGS = $(AM_MAKEFLAGS))"
#	@echo "PWD = $(PWD) top_pwd = $(top_pwd) top_srcdir = $(top_srcdir)"
#	Need to make this conditional so that the make distcheck will work (and not rebuild the documentation)
#  Also copy a version to the ROSE_WebPages/ROSE_Tutorial directory 
#  to support relative links to postscript version from html (then run gzip to bild a compressed version)
	@if (test -f $(srcdir)/$(PACKAGE_NAME)-$(PACKAGE_VERSION)-Tutorial.pdf); then \
        echo "***********************************************************************************"; \
        echo "$(PACKAGE_NAME)-$(PACKAGE_VERSION)-Tutorial.pdf has been found and is"; \
        echo "already provided in source directory for your convience."; \
        echo "User's cannot build documentation from a ROSE distribution."; \
        echo "Only ROSE developers can build the Web and LaTeX documentation (access to CVS version is required) "; \
        echo "If this is a development version and your are building ROSE in the source tree"; \
        echo "then remove $(PACKAGE_NAME)-$(PACKAGE_VERSION)-Tutorial.pdf"; \
        echo "before rebuilding documentation (retry \"make docs\")"; \
        echo "***********************************************************************************"; \
      else \
	     echo "Building tutorial documentation ($(srcdir)/$(PACKAGE_NAME)-$(PACKAGE_VERSION)-Tutorial.pdf ... "; \
	     echo "Build the files that the tutorial documentation requires ..."; \
	     cd $(top_builddir)/tutorial; \
	     $(MAKE) docs; \
	     cd $(PWD); \
	     echo "DONE building the files that the tutorial documentation requires ..."; \
	     $(LATEX) tutorial.tex; \
	     $(LATEX) tutorial.tex; \
	     mv tutorial.pdf $(PACKAGE_NAME)-$(PACKAGE_VERSION)-Tutorial.pdf; \
        cp -f $(PACKAGE_NAME)-$(PACKAGE_VERSION)-Tutorial.pdf $(PACKAGE_NAME)-$(PACKAGE_VERSION)-Tutorial_uncompressed.pdf; \
        mkdir -p ../ROSE_WebPages/ROSE_Tutorial; \
	     echo "Copy the tutorial documentation to the source tree so it can be saved (running gzip) ... "; \
	     cp -f $(PACKAGE_NAME)-$(PACKAGE_VERSION)-Tutorial.pdf ../ROSE_WebPages/ROSE_Tutorial/$(PACKAGE_NAME)-$(PACKAGE_VERSION)-Tutorial.pdf; \
	     cp -f $(PACKAGE_NAME)-$(PACKAGE_VERSION)-Tutorial.pdf $(srcdir)/$(PACKAGE_NAME)-$(PACKAGE_VERSION)-Tutorial.pdf; \
      fi

#       latex2html -dir ../ROSE_WebPages/ROSE_Tutorial -split 2 -noinfo -local_icons tutorial.tex;

# dvips -Pcmz -o $(PACKAGE_NAME)-$(PACKAGE_VERSION)-Tutorial.ps tutorial.dvi;
# cp $(PACKAGE_NAME)-$(PACKAGE_VERSION)-Tutorial.ps ../ROSE_WebPages/ROSE_Tutorial/$(PACKAGE_NAME)-$(PACKAGE_VERSION)-Tutorial.ps; 
# ps2pdf $(PACKAGE_NAME)-$(PACKAGE_VERSION)-Tutorial.ps; 
# rm -rf $(PACKAGE_NAME)-$(PACKAGE_VERSION)-Tutorial.ps.gz;
# gzip $(PACKAGE_NAME)-$(PACKAGE_VERSION)-Tutorial.ps;
# cp $(PACKAGE_NAME)-$(PACKAGE_VERSION)-Tutorial.ps.gz $(srcdir)/$(PACKAGE_NAME)-$(PACKAGE_VERSION)-Tutorial.ps.gz;

docs:
#	@echo "Calling \"docs\" make rule in ROSE/docs/Rose/Makefile ... (MAKE = $(MAKE) AM_MAKEFLAGS = $(AM_MAKEFLAGS))"
#	@echo "PWD = $(PWD) top_pwd = $(top_pwd) top_srcdir = $(top_srcdir)"
	@if (test -f $(srcdir)/$(PACKAGE_NAME)-$(PACKAGE_VERSION)-Tutorial.pdf); then \
        echo "***********************************************************************************"; \
        echo "$(PACKAGE_NAME)-$(PACKAGE_VERSION)-Tutorial.pdf"; \
        echo "is already provided in source directory for your convience."; \
        echo "User's cannot build documentation from a ROSE distribution.";\
        echo "Only ROSE developers can build the Web and LaTeX documentation (access to CVS version is required) "; \
        echo "If this is a development version and your are building ROSE in the source tree,"; \
        echo "then remove $(srcdir)/$(PACKAGE_NAME)-$(PACKAGE_VERSION)-Tutorial.pdf"; \
        echo "before rebuilding documentation (retry \"make docs\")"; \
        echo "***********************************************************************************"; \
      else \
        echo "Building documentation ($(srcdir)/$(PACKAGE_NAME)-$(PACKAGE_VERSION)-Tutorial.pdf not found) ... "; \
        $(MAKE) $(PACKAGE_NAME)-$(PACKAGE_VERSION)-Tutorial.pdf; \
      fi
	@echo ""
	@echo ""
	@echo "************************************************************************"
	@echo "****** All possible tutorial documentation has been generated ... ******"
	@echo "************************************************************************"
	@echo ""

#  $(MAKE) latex-docs-tutorial;

EXTRA_DIST = $(latexFiles) $(PACKAGE_NAME)-$(PACKAGE_VERSION)-Tutorial.pdf 

cleanSource: clean
	rm -rf $(srcdir)/$(PACKAGE_NAME)-$(PACKAGE_VERSION)-Tutorial.pdf

clean-local:
#	Note that when the compile tree and the source tree are the same this will whip out the documentation!
#	Later we need to detect this case and avoid removing the *.tex files and anything else required.
	rm -rf $(latexFiles) glossary.tex FAQ.tex fixme.sty *.tex *.out
	rm -rf tutorial.tex gettingStarted.tex *.log *.aux *.lof *.toc *.dvi *.sty *.ps *.pdf *.gz








