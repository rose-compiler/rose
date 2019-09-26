# When debugging latex documentation skip using the --interaction=batchmode option
# PDFLATEX = env TEXINPUTS=$(srcdir):$(srcdir)/..:$(top_srcdir)/tests:.: pdflatex --interaction=batchmode
PDFLATEX = env TEXINPUTS=$(srcdir):$(srcdir)/..:$(top_srcdir)/tests:.: pdflatex

# We can't specify the source directory to latex so we have to build
# links to the files in the source directory.  Other ideas?
# I have been unable to have the links be built unless I build them all the file.
latexFiles = \
   introduction.tex ASTGraphGenerator.tex AST_PDF_Generator.tex \
   identityTranslator.tex queryLibraryExample.tex abstractHandle.tex \
   traversal.tex loopRecognition.tex typeInfoFromFunctionParameters.tex resolveOverloadedFunction.tex \
   templateParameter.tex \
   buildCFG.tex parallelChecker.tex defuseAnalysis.tex \
   binaryAnalysis.tex GenericDFA.tex buildCallGraph.tex buildVFA.tex classHierarchyGraph.tex loopOptimization.tex inliner.tex outliner.tex \
   parser-docs.tex codeCoverage.tex scopeInformation.tex sideeffect-docs.tex \
   tauInstrumentation.tex templateSupport.tex dataBaseSupport.tex recognitionOfAbstractions.tex \
   debuggingSupport.tex commandlineProcessing.tex customGraphs.tex tutorialMakefile.tex wrapup.tex \
   appendix.tex astFileIO.tex uniqueNames.tex wholeGraphAST.tex addingComments.tex \
   partialRedundancyElimination.tex codeGenerationFormatControl.tex copyHelp.tex \
   roseHPCT.tex sharedMemoryTraversals.tex distributedMemoryTraversals.tex bugSeeding.tex \
   modifiers.tex howToContribute.tex virtualCFG.tex binaryConstruction.tex reductionRecognition.tex \
   livenessAnalysis.tex CFGTraversalTutorial.tex parserBuildingBlocks.tex astConstruction.tex

# This is an evolving list, but these are a few of the required files
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
   $(top_builddir)/tutorial/vcfg.pdf \
   $(top_builddir)/tutorial/rose_inputCode_inlineTransformations.C \
   $(top_builddir)/tutorial/roseHPCT/profiled.out \
   $(top_builddir)/tutorial/sourceTraversalTutorial.C \
   $(top_builddir)/tutorial/binaryTraversalTutorial.C

$(generatedCodeExamples):
	cd $(top_builddir)/tutorial; $(MAKE) docs


latex-docs: pdf

TUTORIAL_NAME = $(PACKAGE_NAME)-$(PACKAGE_VERSION)-Tutorial.pdf

pdf-local: $(TUTORIAL_NAME)

tutorial:$(TUTORIAL_NAME)
	cp $(PACKAGE_NAME)-$(PACKAGE_VERSION)-Tutorial.pdf tutorial.pdf

make-web-pages: $(TUTORIAL_NAME)
	mkdir -p ../ROSE_WebPages/ROSE_Tutorial
	cp -fp $(TUTORIAL_NAME) ../ROSE_WebPages/ROSE_Tutorial/
# a copy without version number as a permanent URL used for external references, Liao	
	cp -fp $(TUTORIAL_NAME) ../ROSE_WebPages/ROSE_Tutorial/ROSE-Tutorial.pdf

#Liao, 10/21/2009, run external tests before making tutorial
# I do this to avoid duplicated test/translator files in both tests and doc directories
#for ROSE turtorial: it needs sageInterface tests in addition to the examples from tutorial/
$(TUTORIAL_NAME): tutorial.tex gettingStarted.tex $(latexFiles)
	$(MAKE) -C$(top_builddir)/tutorial docs && \
	$(MAKE) -C$(top_builddir)/tests/nonsmoke/functional/roseTests/astInterfaceTests check && \
	$(PDFLATEX) tutorial.tex && \
	$(PDFLATEX) tutorial.tex && \
	$(PDFLATEX) tutorial.tex && \
	mv tutorial.pdf $(TUTORIAL_NAME)

docs: pdf

EXTRA_DIST = CMakeLists.txt $(latexFiles)

CLEANFILES = *.out *.log *.aux *.lof *.toc *.dvi *.ps *.pdf *.gz
