include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

########################################################################################################################
# List of all project subdirectories (SUBDIRS)
########################################################################################################################

# Ideally, each project's makefile should know whether it should build anything based on what features (such as
# dependencies, frontend languages) are available and selected by the configuration.  However, some of the earlier
# projects have errors if you run "make" in the project directory when ROSE is not configured to support the project,
# thus we have various "if" statements here to support that.
#
# Projects that use the "rose.config" mechanism need not be listed here. Their makefile MUST know how to avoid building
# the project if its dependencies are not met.
#
# To use the "rose.config" mechanism: add a "rose.config" file to your project directory and then you don't have to
# change any of the ROSE build system -- your project will be found automatically by the "build" script, your
# rose.config will be incorporated into the configuration system, and your project will become part of the recursive
# "make" commands.  To disable your project, just rename its rose.config file and rerun "build" and "configure".

SUBDIRS = \
	ArithmeticMeasureTool \
        POET_ROSE \
	BinaryCloneDetection \
	CertSecureCodeProject \
	CloneDetection \
	EditDistanceMetric \
	fuse \
	HeaderFilesInclusion \
	MPI_Tools \
	MapleDSL \
	arrayOptimization \
	UpcTranslation \
	autoParallelization \
	pragmaParsing \
	ManyCoreRuntime \
	Fortran_to_C \
	Jovial_to_C \
	extractMPISkeleton \
	RoseBlockLevelTracing \
	xgenTranslator \
        TemplateAnalysis \
	$(OPTIONAL_PROJECTS)

#QY (6/11/2018): this directory is no longer used; moved code to POET-ROSE/manycore-stencil
# DQ (4/20/2016): Clang has a problem within compiling POET, which is used in the StencilManyCore project.
#if !USING_CLANG_COMPILER
#SUBDIRS += StencilManyCore 
#endif

# DQ (3/16/2015): Test for a version of g++ that will allow use of C++11 for the AMR Shift Calculs DSL work.
if ROSE_USING_GCC_VERSION_LATER_4_8
  # DQ (3/23/2016): Note that we currently require the dsl_infrastructure directory to be built before both of 
  # the arrayDSLcompiler and amrShiftDSLcompiler directories.
  SUBDIRS += \
    ShiftCalculus4 \
    dsl_infrastructure \
    arrayDSLcompiler \
    amrShiftDSLcompiler
endif

# Liao, 2014/2/11, with Dan's permission, comment out this project since it is broken now.
#   BinaryDataStructureRecognition 

# DQ (9/26/2011): Directory removed for failing new AST consistancy test for references to deleted IR nodes.
# UpcTranslation (fails only a single test code shared_2.upc)

# TOO (2/16/2011): Errors with Tensilica's Xtensa compilers as alternative backend compilers. We can
# gradually enable these tests at a later stage if necessary.
if !USING_XTENSA_BACKEND_COMPILER
   SUBDIRS += autoTuning
endif

# DQ (7/21/2010): Removed RTED since it is a problem to compile with ROSE (projects/RTED/CppRuntimeSystem/test.cpp).
if !ROSE_USING_ROSE_ANALYSIS
   SUBDIRS += RTED
endif

SUBDIRS += simulator2

# DQ (2/12/2010): compass appears to be a problem for the code generation support
# in ROSE, so lets' move on and return to compass later for this level of support
# of ROSE compiling the compass tool as a test of ROSE.
#    compass

if !ROSE_USING_ROSE_CODE_GENERATION

# DQ (10/17/2010): This seems to be problem for Java, we can fix this later.
if !ROSE_BUILD_JAVA_LANGUAGE_SUPPORT

# DQ (4/17/2016): Restrict this to requiring both C/C++ language support and Binary analysis support.
if ROSE_BUILD_CXX_LANGUAGE_SUPPORT
if ROSE_BUILD_BINARY_ANALYSIS_SUPPORT

   SUBDIRS += compass
 # DQ (10/17/2010): Since we can't process Compass with Java we can't compiler palette (which requires Compass).
 # DQ (2/13/2010): This appears to have a dependence on compass, but I don't know why.
 # All projects should be independent of each other so that they should be able to be
 # compiled in any order. It was originally listed after "haskellport".
   SUBDIRS += palette
endif

endif

endif
endif

# Disable the use of the interpreter except on GNU 4.2
# interpreter

if !ROSE_USING_ROSE_CODE_GENERATION
# DQ (9/30/2009): This is classified as an old version of compiler and some parts of
# ROSE are restricted from use (this is currently the only example due to internal 
# compiler errors).
if !OLD_COMPILER_VERSION

# DQ (4/8/2011): Directories of tests are a current problem for Insure++.
if !ROSE_USE_INSURE
   SUBDIRS += interpreter
endif

endif
endif

# Pei-Hung (12/17/2014): adding PolyOpt only when POCC is available
# Pei-Hung (09/24/2015): MultiLevelMemory requires POCC
if ROSE_WITH_POCC
   SUBDIRS += PolyOpt2
endif

SUBDIRS += TileK
SUBDIRS += RaaS
SUBDIRS += CompilationDB

########################################################################################################################
# Associate projects with developers
########################################################################################################################

# The developer-specific project lists used for portability testing have been moved to
# project/MatrixTesting/lib/matrix/docker-scripts/steps so they can be used by all
# supported build systems. [Matzke, 2019-04-24]

########################################################################################################################
# Rules for testing projects having particular core developer sponsors
########################################################################################################################

developer_projects = $(foreach subdir, $(1), $(if $(findstring $(subdir), $(SUBDIRS)), $(subdir)))

check-projects-robb:
	@for project in $(call developer_projects, $(PROJECTS_ROBB)); do	\
		$(MAKE) -C $$project check || exit 1;				\
	done
check-projects-justin:
	@for project in $(call developer_projects, $(PROJECTS_JUSTIN)); do	\
		$(MAKE) -C $$project check || exit 1;				\
	done
check-projects-dan:
	@for project in $(call developer_projects, $(PROJECTS_DAN)); do	\
		$(MAKE) -C $$project check || exit 1;				\
	done
check-projects-markus:
	@for project in $(call developer_projects, $(PROJECTS_MARKUS)); do	\
		$(MAKE) -C $$project check || exit 1;				\
	done
check-projects-peihung:
	@for project in $(call developer_projects, $(PROJECTS_PEIHUNG)); do	\
		$(MAKE) -C $$project check || exit 1;				\
	done
check-projects-leo:
	@for project in $(call developer_projects, $(PROJECTS_LEO)); do		\
		$(MAKE) -C $$project check || exit 1;				\
	done



EXTRA_DIST = StaticDynamicAnalysis
