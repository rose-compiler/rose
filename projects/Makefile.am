include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

# Add Projects that are associated with capabilities that belong to ROSE here
SUBDIRS = \
     AstEquivalence \
     autoParallelization \
     autoTuning\
     BinQ \
     CertSecureCodeProject \
     compass \
     BinaryCloneDetection \
     BTORCloneDetection \
     binCompass \
     CloneDetection \
     DatalogAnalysis \
     DocumentationGenerator \
     OpenMP_Translator \
     BabelPreprocessor \
     FiniteStateModelChecker \
     programModeling \
     arrayOptimization \
     highLevelGrammars \
     dataStructureGraphing \
     checkPointExample \
     simpleCallGraphAnalysis \
     C_to_Promela \
     MPICodeMotion \
     javaport \
     haskellport \
     palette \
     reverseComputation \
     DistributedMemoryAnalysisCompass \
     assemblyToSourceAst \
     bugSeeding \
     binaryVisualization \
     MacroRewrapper \
     UpcTranslation \
     RTED \
     taintcheck \
     QtDesignerPlugins \
     RoseQt

# Disable the use of the interpreter except on GNU 4.2
# interpreter

# DQ (9/30/2009): This is classified as an old version of compiler and some parts of
# ROSE are restricted from use (this is currently the only example due to internal 
# compiler errors).
if ! OLD_COMPILER_VERSION
   SUBDIRS += interpreter
endif

# DQ (9/12/2009): This was not added to ROSE correctly, please let me help you do this.
#     StaticDynamicAnalysis/DynamicCPU

EXTRA_DIST = StaticDynamicAnalysis

#AS(3/12/2008) the  NameConsistancyChecker is now part of compass
#     NameConsistancyChecker 

# DQ (9/3/2006): Removed in preference to the newer work on OpenMP.
# Removed the older incomplete (initial attempt) at an OpenMP processor.
#    OpenMPPreprocessor

# Removed Bable from tests since it is currently broken and needs to 
# be fixed (also fails to compile with Intel C++ 9.0 compiler.
#    BabelPreprocessor 

#     AtermTranslation      \ MS10/26/04: deactivated (didn't compile)

# The dataStructureGraphing directory represents Andreas's work 
# on the visualization of complex heap based data structures


# DQ (8/24/2004): This has been moved to the ROSE/src/roseExtensions/databaseConnection
# dataBase

check-local:
	@echo "***************************************************************************"
	@echo "****** ROSE/projects: make check rule complete (terminated normally) ******"
	@echo "***************************************************************************"

