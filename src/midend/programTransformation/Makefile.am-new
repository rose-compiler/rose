include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

###############################################################################
# Subdirectory specifics
# TOO (01/26/11): relative paths must be updated for each propagation upward
# in the flattening of Makefiles
#
# Caution: include path cannot contain variable,
#    e.g. include cxxCodeGenerationPath/Makefile_variables
#
include $(srcdir)/astInlining/Makefile_variables


mptAstInliningPath=$(srcdir)/astInlining#


###############################################################################

INCLUDES=\
	$(ROSE_INCLUDES)


noinst_LTLIBRARIES = libprogramTransformation.la

##################################################################

libprogramTransformation_la_SOURCES=\
	$(mptAstInlining_la_sources)


include_HEADERS=\
	$(mptAstInlining_includeHeaders)


EXTRA_DIST=\
	CMakeLists.txt \
	$(mptAstInlining_extraDist)


clean-local:
	$(mptAstInlining_cleanLocal)


#SUBDIRS = partialRedundancyElimination finiteDifferencing \
#          functionCallNormalization constantFolding implicitCodeGeneration \
#          astInlining astOutlining loopProcessing ompLowering
