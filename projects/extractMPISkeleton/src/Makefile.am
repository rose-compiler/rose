include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

AM_CPPFLAGS=$(ROSE_INCLUDES) -I$(top_srcdir)/src/midend/programAnalysis/staticInterproceduralSlicing -I$(top_srcdir)/src/midend/programAnalysis/CallGraphAnalysis

bin_PROGRAMS = extractMPISkeleton generateSignatures summarizeSignatures

extractMPISkeleton_SOURCES= extractMPISkeleton.cc APIReader.cc APIReader.h APISpec.cc \
 APISpec.h GenericDepAttrib.cc GenericDepAttrib.h APIDepAttrib.h APIDepFinder.cc APIDepFinder.h \
 SignatureReader.cc SignatureReader.h ssexpr.cc ssexpr.h DangerousOperationFinder.h Utils.cc Utils.h \
 APIDepChecker.h Outline.cc Outline.h processPragmas.cc processPragmas.h \
 annotatePragmas.cc annotatePragmas.h

generateSignatures_SOURCES = generateSignatures.cc APISpec.cc APISpec.h GenericDepAttrib.cc \
  GenericDepAttrib.h APIDepAttrib.h APIReader.cc APIReader.h APIDepFinder.cc APIDepFinder.h \
	SignatureReader.cc SignatureReader.h ssexpr.cc ssexpr.h DangerousOperationFinder.h \
	Utils.cc Utils.h APIDepChecker.h processPragmas.cc processPragmas.h \
	annotatePragmas.cc annotatePragmas.h

summarizeSignatures_SOURCES = summarizeSignatures.cc ssexpr.cc ssexpr.h APISpec.cc APISpec.h \
 GenericDepAttrib.cc GenericDepAttrib.h APIDepAttrib.h APIReader.cc APIReader.h SignatureReader.cc \
 SignatureReader.h

LDADD = $(LIBS_WITH_RPATH) $(ROSE_LIBS)

clean-local:
	rm -rf *.o ${bin_PROGRAMS}

noinst_HEADERS = APIDepAttrib.h APIDepFinder.h APIReader.h APISpec.h \
  DangerousOperationFinder.h GenericDepAttrib.h Outline.h \
  Utils.h ssexpr.h processPragmas.h annotatePragmas.h \
  SignatureReader.h APIDepChecker.h

TAGS: ${CCFILES} ${HFILES}
	etags $^

show:
	@echo $($(VALUE))              # note: this expands!

check-local:
	@echo "***********************************************************************************************"
	@echo "*** ROSE/projects/extractMPISkeleton/src: make check rule complete (terminated normally)"
	@echo "***********************************************************************************************"
