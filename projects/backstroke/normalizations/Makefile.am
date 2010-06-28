include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

bin_PROGRAMS = normalizationTranslator 
normalizationTranslator_SOURCES = ExtractFunctionArguments.C NormalizationTranslator.C \
	Utilities.C CFGReverserProofOfConcept.C

INCLUDES = $(ROSE_INCLUDES)
LDADD    = $(LIBS_WITH_RPATH) $(ROSE_LIBS)

EXTRA_DIST = ExtractFunctionArguments.h Utilities.h CPPDefinesAndNamespaces.h \
	CFGReverserProofOfConcept.h

CLEANFILES = 

include_HEADERS =

check-local:
	@echo "*********************************************************************************************************"
	@echo "****** ROSE/projects/backstroke/normalizations: make check rule complete (terminated normally) ******"
	@echo "*********************************************************************************************************"
