include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

###############################################################################
# Subdirectory specifics
# TOO (01/26/11): relative paths must be updated for each propagation upward
# in the flattening of Makefiles
#
# Caution: include path cannot contain variable,
#    e.g. include $(cxxCodeGenerationPath)/Makefile_variables
#
include $(srcdir)/annotation/Makefile_variables
include $(srcdir)/astInterface/Makefile_variables
include $(srcdir)/astSupport/Makefile_variables
include $(srcdir)/symbolicVal/Makefile_variables


mauAnnotationPath=$(srcdir)/annotation#
mauAstInterfacePath=$(srcdir)/astInterface#
mauAstSupportPath=$(srcdir)/astSupport#
mauSymbolicValPath=$(srcdir)/symbolicVal#

###############################################################################

INCLUDES=\
	$(ROSE_INCLUDES) \
	$(mauAnnotation_includes) \
	$(mauAstInterface_includes) \
	$(mauAstSupport_includes) \
	$(mauSymbolicVal_includes)


noinst_LTLIBRARIES=\
	libastUtil.la 


##################################################################

libastUtil_la_SOURCES=\
	$(mauAnnotation_la_sources) \
	$(mauAstInterface_la_sources) \
	$(mauAstSupport_la_sources) \
	$(mauSymbolicVal_la_sources)


include_HEADERS=\
	$(mauAnnotation_includeHeaders) \
	$(mauAstInterface_includeHeaders) \
	$(mauAstSupport_includeHeaders) \
	$(mauSymbolicVal_includeHeaders)


EXTRA_DIST=\
	CMakeLists.txt \
	$(mauAnnotation_extraDist) \
	$(mauAstInterface_extraDist) \
	$(mauAstSupport_extraDist) \
	$(mauSymbolicVal_extraDist)


clean-local:
	$(mauAnnotation_cleanLocal)
	$(mauAstInterface_cleanLocal)
	$(mauAstSupport_cleanLocal)
	$(mauSymbolicVal_cleanLocal)


distclean-local:
	$(mauAnnotation_distCleanLocal)
	$(mauAstInterface_distCleanLocal)
	$(mauAstSupport_distCleanLocal)
	$(mauSymbolicVal_distCleanLocal)



## The grammar generator (ROSETTA) should use its own template repository
CXX_TEMPLATE_REPOSITORY_PATH = .
