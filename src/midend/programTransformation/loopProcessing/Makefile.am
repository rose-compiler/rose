# Non-recursive Makefile

include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

# ------------------------------------------------------------------------------
#  Subdirectories
# ------------------------------------------------------------------------------

include $(srcdir)/depInfo/Makefile_variables
include $(srcdir)/depGraph/Makefile_variables

mptlpDepInfoPath=$(srcdir)/depInfo#
mptlpDepGraphPath=$(srcdir)/depGraph#

SUBDIRS=\
	computation \
	outsideInterface \
	slicing \
	driver \
	prepostTransformation

# ------------------------------------------------------------------------------
#  Library: programTransformationLoopProcessing.la
# ------------------------------------------------------------------------------

## The grammar generator (ROSETTA) should use its own template repository
CXX_TEMPLATE_REPOSITORY_PATH = .

INCLUDES=\
	$(ROSE_INCLUDES) \
	-I$(srcdir)/driver \
	-I$(srcdir)/depInfo \
	-I$(srcdir)/prepostTransformation \
	-I$(top_srcdir)/src/util/support \
	-I$(top_srcdir)/src/util/graphs \
	-I$(top_srcdir)/src/midend/astUtil/astSupport \
	-I$(top_srcdir)/src/midend/astUtil/astInterface \
	-I$(top_srcdir)/src/midend/astUtil/symbolicVal

noinst_LTLIBRARIES=\
	libLoopProcessing.la

libLoopProcessing_la_SOURCES = \
	$(mptlpDepInfo_la_sources) \
	$(mptlpDepGraph_la_sources)

include_HEADERS = \
	$(mptlpDepInfo_includeHeaders) \
	$(mptlpDepGraph_includeHeaders)

# ------------------------------------------------------------------------------
#
# ------------------------------------------------------------------------------

EXTRA_DIST=\
	CMakeLists.txt \
	$(mptlpDepInfo_extraDist) \
	$(mptlpDepGraph_extraDist)

clean-local:
	rm -rf Templates.DB ii_files ti_files cxx_templates
	rm -rf rose_mm.C
	$(mptlpDepInfo_cleanLocal)
	$(mptlpDepGraph_cleanLocal)

distclean-local:
	rm -rf Templates.DB
	$(mptlpDepInfo_distCleanLocal)
	$(mptlpDepGraph_distCleanLocal)

