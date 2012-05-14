include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

INCLUDES=$(ROSE_INCLUDES) -I$(top_srcdir)/src/midend/programAnalysis/staticInterproceduralSlicing -I$(top_srcdir)/src/midend/programAnalysis/CallGraphAnalysis

EXEC=extractMPISkeleton
bin_PROGRAMS=${EXEC}

extractMPISkeleton_SOURCES = APIDepFinder.cc APIReader.cc APISpec.cc \
  Outline.cc Utils.cc extractMPISkeleton.cc ssexpr.cc processPragmas.cc \
  annotatePragmas.cc

LDADD = $(LIBS_WITH_RPATH) $(ROSE_LIBS)

clean-local:
	rm -rf *.o ${bin_PROGRAMS}

noinst_HEADERS = APIDepAttrib.h APIDepFinder.h APIReader.h APISpec.h \
  DangerousOperationFinder.h GenericDepAttrib.h Outline.h \
  Utils.h ssexpr.h processPragmas.h annotatePragmas.h

TAGS: ${CCFILES} ${HFILES}
	etags $^

show:
	@echo $($(VALUE))              # note: this expands!

check-local:
	@echo "***********************************************************************************************"
	@echo "*** ROSE/projects/extractMPISkeleton/src: make check rule complete (terminated normally)"
	@echo "***********************************************************************************************"
