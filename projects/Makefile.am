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
     MPICodeMotion \
     MacroRewrapper \
     OpenMP_Analysis \
     QtDesignerPlugins \
     RoseQt \
     UpcTranslation \
     arrayOptimization \
     autoParallelization \
     binaryVisualization \
     bugSeeding \
     checkPointExample \
     highLevelGrammars \
     javaport \
     programModeling \
     pragmaParsing \
     roseToLLVM \
     symbolicAnalysisFramework \
     taintcheck \
     PowerAwareCompiler

# DQ (4/8/2011): Directories of tests are a current problem for Insure++.
if !ROSE_USE_INSURE
   SUBDIRS += SemanticSignatureVectors \
              assemblyToSourceAst
endif

if !ROSE_USING_BOOST_VERSION_1_46
   SUBDIRS += HeaderFilesInclusion
endif

# TOO (2/16/2011): Errors with Tensilica's Xtensa compilers as alternative backend compilers. We can
# gradually enable these tests at a later stage if necessary.
if !USING_XTENSA_BACKEND_COMPILER
   SUBDIRS += dataStructureGraphing OpenMP_Translator autoTuning

# TOO (2/16/2011): Disabled for Thrifty compiler (gcc 3.4.4) due to errors during make check
if !USING_GCC_3_4_4_BACKEND_COMPILER
#  SUBDIRS += backstroke haskellport 
   SUBDIRS += backstroke

# DQ (4/17/2011):This is hanging (worst kind of failing) on tux324 (for two Hudson tests a50-ROSE-BOOST-matrix-scratch in a row #334 and #335).
if !ROSE_USING_BOOST_VERSION_1_36
# DQ (4/16/2011): This is hanging (worst kind of failing) on tux324 (for two Hudson tests in a row #2290 and #2291).
if !USING_GCC_4_0_4_BACKEND_COMPILER
# DQ (4/8/2011): Directories of tests are a current problem for Insure++.
if !ROSE_USE_INSURE
   SUBDIRS += haskellport 
endif
endif
endif

endif
endif

# DQ (10/15/2010): Adding initial trace analysis support.
if ROSE_USE_TRACE_ANALYSIS
   SUBDIRS += traceAnalysis
endif

# DQ (7/21/2010): Removed RTED since it is a problem to compile with ROSE (projects/RTED/CppRuntimeSystem/test.cpp).
# DQ (7/21/2010): Removed simulator since it is a problem to compile with ROSE (projects/simulator/emulator.C).
if !ROSE_USING_ROSE_ANALYSIS
if !ROSE_USING_BOOST_VERSION_1_46
   SUBDIRS += RTED
endif
   SUBDIRS += simulator
endif

# DQ (2/12/2010): compass appears to be a problem for the code generation support
# in ROSE, so lets' move on and return to compass later for this level of support.
#    compass

if !ROSE_USING_ROSE_CODE_GENERATION

# DQ (10/17/2010): This seems to be problem for Java, we can fix this later.
if !ROSE_BUILD_JAVA_LANGUAGE_SUPPORT
   SUBDIRS += compass binCompass
endif

# DQ (10/17/2010): Since we can't process Compass with Java we can't compiler palette (which requires Compass).
if !ROSE_BUILD_JAVA_LANGUAGE_SUPPORT
 # DQ (2/13/2010): This appears to have a dependence on compass, but I don't know why.
 # All projects should be independent of each other so that they should be able to be
 # compiled in any order. It was originally listed after "haskellport".
   SUBDIRS += palette
endif
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

# DQ (4/8/2011): Directories of tests are a current problem for Insure++.
if !ROSE_USE_INSURE
   SUBDIRS += interpreter
endif

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

