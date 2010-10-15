include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

# Add Projects that are associated with capabilities that belong to ROSE here
SUBDIRS = \
     AstEquivalence \
     BabelPreprocessor \
     BinQ \
     BinaryCloneDetection \
     C_to_Promela \
     CertSecureCodeProject \
     CloneDetection \
     DatalogAnalysis \
     DocumentationGenerator \
     FiniteStateModelChecker \
     HeaderFilesInclusion \
     MPICodeMotion \
     MacroRewrapper \
     OpenMP_Analysis \
     OpenMP_Translator \
     PolyhedralDependenceAnalysis \
     QtDesignerPlugins \
     RoseQt \
     SemanticSignatureVectors \
     UpcTranslation \
     arrayOptimization \
     assemblyToSourceAst \
     autoParallelization \
     autoTuning\
     backstroke \
     binCompass \
     binaryVisualization \
     bugSeeding \
     checkPointExample \
     dataStructureGraphing \
     haskellport \
     highLevelGrammars \
     javaport \
     programModeling \
     roseToLLVM \
     symbolicAnalysisFramework \
     taintcheck \
     PowerAwareCompiler

# DQ (7/21/2010): Removed RTED since it is a problem to compile with ROSE (projects/RTED/CppRuntimeSystem/test.cpp).
# DQ (7/21/2010): Removed simulator since it is a problem to compile with ROSE (projects/simulator/emulator.C).
if !ROSE_USING_ROSE_ANALYSIS
   SUBDIRS += RTED simulator
endif

# DQ (2/12/2010): compass appears to be a problem for the code generation support
# in ROSE, so lets' move on and return to compass later for this level of support.
#    compass

if !ROSE_USING_ROSE_CODE_GENERATION
   SUBDIRS += compass


 # DQ (2/13/2010): This appears to have a dependence on compass, but I don't know why.
 # All projects should be independent of each other so that they should be able to be
 # compiled in any order. It was originally listed after "haskellport".
   SUBDIRS += palette
endif

if ROSE_MPI
   SUBDIRS += DistributedMemoryAnalysisCompass 
endif

# Disable the use of the interpreter except on GNU 4.2
# interpreter

if !ROSE_USING_ROSE_CODE_GENERATION
# DQ (9/30/2009): This is classified as an old version of compiler and some parts of
# ROSE are restricted from use (this is currently the only example due to internal 
# compiler errors).
if ! OLD_COMPILER_VERSION
   SUBDIRS += interpreter
endif
endif

# DQ (2/28/2010): We can't make this conditionsl this since then the Makefile.in will 
# be missing at configure time.
# if ROSE_USE_SAT
SUBDIRS += SatSolver
# endif



# DQ (9/12/2009): This was not added to ROSE correctly, please let me help you do this.
#     StaticDynamicAnalysis/DynamicCPU

EXTRA_DIST = StaticDynamicAnalysis


check-local:
	@echo "***************************************************************************"
	@echo "****** ROSE/projects: make check rule complete (terminated normally) ******"
	@echo "***************************************************************************"

