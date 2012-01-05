include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

INCLUDES=$(ROSE_INCLUDES) -I$(top_srcdir)/src/midend/programAnalysis/staticInterproceduralSlicing -I$(top_srcdir)/src/midend/programAnalysis/CallGraphAnalysis

EXEC=extractMPISkeleton
bin_PROGRAMS=${EXEC}

extractMPISkeleton_SOURCES = APIDepFinder.cc APIReader.cc APISpec.cc \
  Outline.cc Utils.cc extractMPISkeleton.cc ssexpr.cc

LDADD = $(LIBS_WITH_RPATH) $(ROSE_LIBS)

clean-local:
	rm -f *.o ${bin_PROGRAMS} out out-o *.pdf rose_*.c rose_*.C

noinst_HEADERS = APIDepAttrib.h APIDepFinder.h APIReader.h APISpec.h \
  DangerousOperationFinder.h GenericDepAttrib.h Outline.h \
  Utils.h ssexpr.h 

EXTRA_DIST=mpi_api.spec stdio_input.spec

EXAMPLES=$(shell ls $(srcdir)/examples/*/*.c)
TESTS=$(shell ls $(srcdir)/tests/*.c)
SOURCES=${EXAMPLES} ${TESTS}
OUTS=$(addprefix out/, $(basename $(notdir ${SOURCES})))
OUTS_O=$(addprefix out-o/, $(basename $(notdir ${SOURCES})))

run: out

.PHONY: out
out: ${EXEC}
	rm -rf $@
	mkdir $@
	for f in ${EXAMPLES} ${TESTS} ; do b=$$(basename "$$f") ; echo ----- $$b ----- ; ${LIBTOOL} --mode=execute ./${EXEC} -rose:o out/$$b $$f ; done

out/%: out/%.c
	mpicc -o $@ $<

.PHONY: mpibuild
mpibuild: ${OUTS}

# Create skeletons via outlining, not removing

.PHONY: out-o
out-o: ${EXEC}
	rm -rf $@
	mkdir $@
	for f in ${EXAMPLES} ${TESTS} ; do b=$$(basename "$$f") ; echo ----- $$b ----- ; ${LIBTOOL} --mode=execute ./${EXEC} -rose:o out-o/$$b -local:outline $$f ; done

out-o/%: out-o/%.c
	mpicc -o $@ $<

.PHONY: mpibuild
mpibuild-o: ${OUTS_O}

  # ^ not fully successful, use "make -k mpibuild-o" for now.

.PHONY: compare
compare: ${OUTS}
	@for f in out/*.c ; do b=$$(basename "$$f") ; if [ -e expected/$$b.expected ] ; then diff $$f expected/$$b.expected ; fi ; done

.PHONY: test
test: ${EXEC}
	${MAKE} run
	${MAKE} mpibuild
	${MAKE} compare

TAGS: ${CCFILES} ${HFILES}
	etags $^

show:
	@echo $($(VALUE))              # note: this expands!
