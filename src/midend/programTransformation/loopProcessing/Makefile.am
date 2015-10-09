# Non-recursive Makefile

include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

# ------------------------------------------------------------------------------
#  Subdirectories
# ------------------------------------------------------------------------------

## Place generated object files (.o) into the same directory as their source
## files, in order to avoid collisions when non-recursive make is used.
AUTOMAKE_OPTIONS = subdir-objects

include $(srcdir)/depInfo/Makefile_variables
include $(srcdir)/depGraph/Makefile_variables
include $(srcdir)/computation/Makefile_variables
include $(srcdir)/outsideInterface/Makefile_variables
include $(srcdir)/slicing/Makefile_variables
include $(srcdir)/driver/Makefile_variables
include $(srcdir)/prepostTransformation/Makefile_variables

#
# Don't use $(srcdir) in conjunction with [subdir-objects], otherwise
# object files will be built in the source tree. Therefore, we need to
# use relative paths.
#
#mptlpDepInfoPath=$(srcdir)/depInfo#
mptlpDepInfoPath=depInfo#
mptlpDepGraphPath=depGraph#
mptlpComputationPath=computation#
mptlpOutsideInterfacePath=outsideInterface#
mptlpSlicingPath=slicing#
mptlpDriverPath=driver#
mptlpPrepostTransformationPath=prepostTransformation#

# ------------------------------------------------------------------------------
#  Library: programTransformationLoopProcessing.la
# ------------------------------------------------------------------------------

## The grammar generator (ROSETTA) should use its own template repository
CXX_TEMPLATE_REPOSITORY_PATH = .

AM_CPPFLAGS=\
	$(ROSE_INCLUDES) \
	-I$(srcdir)/driver \
	-I$(srcdir)/computation \
	-I$(srcdir)/depInfo \
	-I$(srcdir)/depGraph \
	-I$(srcdir)/slicing \
	-I$(srcdir)/outsideInterface \
	-I$(srcdir)/prepostTransformation \
	-I$(top_srcdir)/src/util/support \
	-I$(top_srcdir)/src/util/graphs \
	-I$(top_srcdir)/src/midend/astUtil/astSupport \
	-I$(top_srcdir)/src/midend/astUtil/astInterface \
	-I$(top_srcdir)/src/midend/astUtil/symbolicVal \
	-I$(top_srcdir)/src/midend/programAnalysis/CFG \
	-I$(top_srcdir)/src/midend/programAnalysis/bitvectorDataflow

noinst_LTLIBRARIES=\
	libLoopProcessing.la

libLoopProcessing_la_SOURCES=\
	$(mptlpDepInfo_la_sources) \
	$(mptlpDepGraph_la_sources) \
	$(mptlpComputation_la_sources) \
	$(mptlpOutsideInterface_la_sources) \
	$(mptlpSlicing_la_sources) \
	$(mptlpDriver_la_sources) \
	$(mptlpPrepostTransformation_la_sources)

pkginclude_HEADERS=\
	$(mptlpDepInfo_includeHeaders) \
	$(mptlpDepGraph_includeHeaders) \
	$(mptlpComputation_includeHeaders) \
	$(mptlpOutsideInterface_includeHeaders) \
	$(mptlpSlicing_includeHeaders) \
	$(mptlpDriver_includeHeaders) \
	$(mptlpPrepostTransformation_includeHeaders)

# ------------------------------------------------------------------------------
#
# ------------------------------------------------------------------------------

EXTRA_DIST=\
	CMakeLists.txt \
	$(mptlpDepInfo_extraDist) \
	$(mptlpDepGraph_extraDist) \
	$(mptlpComputation_extraDist) \
	$(mptlpOutsideInterface_extraDist) \
	$(mptlpSlicing_extraDist) \
	$(mptlpDriver_extraDist) \
	$(mptlpPrepostTransformation_extraDist)

clean-local:
	rm -rf Templates.DB ii_files ti_files cxx_templates
	rm -rf rose_mm.C
	$(mptlpDepInfo_cleanLocal)
	$(mptlpDepGraph_cleanLocal)
	$(mptlpComputation_cleanLocal)
	$(mptlpOutsideInterface_cleanLocal)
	$(mptlpSlicing_cleanLocal)
	$(mptlpDriver_cleanLocal)
	$(mptlpPrepostTransformation_cleanLocal)

distclean-local:
	rm -rf Templates.DB
	$(mptlpDepInfo_distCleanLocal)
	$(mptlpDepGraph_distCleanLocal)
	$(mptlpComputation_distCleanLocal)
	$(mptlpSlicing_distCleanLocal)
	$(mptlpDriver_distCleanLocal)
	$(mptlpPrepostTransformation_distCleanLocal)

