include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

###############################################################################
# Subdirectory specifics
# TOO (01/26/11): relative paths must be updated for each propagation upward
# in the flattening of Makefiles
#
# Caution: include path cannot contain variable,
#    e.g. include cxxCodeGenerationPath/Makefile_variables
#
include $(srcdir)/annotation/Makefile_variables
include $(srcdir)/astInterface/Makefile_variables


mauAnnotationPath=$(srcdir)/annotation#
mauAstInterfacePath=$(srcdir)/astInterface#

###############################################################################

INCLUDES=\
	$(ROSE_INCLUDES) \
	$(mauAnnotation_includes) \
	$(mauAstInterface_includes)


noinst_LTLIBRARIES=\
	libastUtil.la 


##################################################################

libastUtil_la_sources=\
	$(mauAnnotation_la_sources) \
	$(mauAstInterface_la_sources)


include_HEADERS=\
	$(mauAnnotation_includeHeaders) \
	$(mauAstInterface_includeHeaders)


EXTRA_DIST=\
	CMakeLists.txt \
	$(mauAnnotation_extraDist) \
	$(mauAstInterface_extraDist)


clean-local:
	$(mauAnnotation_cleanLocal)
	$(mauAstInterface_cleanLocal)


distclean-local:
	$(mauAnnotation_distCleanLocal)
	$(mauAstInterface_distCleanLocal)





#SUBDIRS = astSupport  symbolicVal 




## The grammar generator (ROSETTA) should use its own template repository
CXX_TEMPLATE_REPOSITORY_PATH = .
