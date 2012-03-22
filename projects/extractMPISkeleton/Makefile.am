include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

INCLUDES=$(ROSE_INCLUDES) -I$(top_srcdir)/src/midend/programAnalysis/staticInterproceduralSlicing -I$(top_srcdir)/src/midend/programAnalysis/CallGraphAnalysis

EXEC=extractMPISkeleton
bin_PROGRAMS=${EXEC}

extractMPISkeleton_SOURCES = APIDepFinder.cc APIReader.cc APISpec.cc \
  Outline.cc Utils.cc extractMPISkeleton.cc ssexpr.cc processPragmas.cc

LDADD = $(LIBS_WITH_RPATH) $(ROSE_LIBS)

clean-local:
	rm -rf *.o ${bin_PROGRAMS} out out-o *.pdf rose_*.c rose_*.C

noinst_HEADERS = APIDepAttrib.h APIDepFinder.h APIReader.h APISpec.h \
  DangerousOperationFinder.h GenericDepAttrib.h Outline.h \
  Utils.h ssexpr.h processPragmas.h

EXTRA_DIST=mpi_api.spec stdio_input.spec apis.coll \
           examples test-sources mpi-fakeheader

EXAMPLES=$(shell find $(srcdir)/examples -name "*.c")
SOURCES=${EXAMPLES} $(shell find $(srcdir)/test-sources -name "*.c")
OUTS=$(addsuffix .o, $(addprefix out/, $(basename $(notdir ${SOURCES}))))
OUTS_O=$(addsuffix .o, (addprefix out-o/, $(basename $(notdir ${SOURCES}))))

run: out

.PHONY: out
out: ${EXEC}
	rm -rf $@
	mkdir $@
	for f in ${SOURCES} ; do b=$$(basename "$$f") ; echo ----- $$b ----- ; ./${EXEC} -I${srcdir}/mpi-fakeheader -rose:o out/$$b -skel:spec $(srcdir)/apis.coll $$f ; done

out/%.o: out/%.c
	${CC} -I${srcdir}/mpi-fakeheader -c -o $@ $<

.PHONY: mpibuild
mpibuild: ${OUTS}

# Create skeletons via outlining, not removing

.PHONY: out-o
out-o: ${EXEC}
	rm -rf $@
	mkdir $@
	for f in ${SOURCES} ; do b=$$(basename "$$f") ; echo ----- $$b ----- ; ./${EXEC} -I${srcdir}/mpi-fakeheader -rose:o out-o/$$b -skel:spec $(srcdir)/apis.coll -skel:outline $$f ; done

out-o/%: out-o/%.c
	mpicc -o $@ $<

.PHONY: mpibuild
mpibuild-o: ${OUTS_O}

  # ^ not fully successful, use "make -k mpibuild-o" for now.

.PHONY: compare
compare: ${OUTS}
	@for f in out/*.c ; do b=$$(basename "$$f") ; if [ -e ${srcdir}/expected/$$b.expected ] ; then diff $$f ${srcdir}/expected/$$b.expected ; fi ; done

.PHONY: test
test: ${EXEC}
	${MAKE} run
	${MAKE} mpibuild
	${MAKE} compare

TAGS: ${CCFILES} ${HFILES}
	etags $^

show:
	@echo $($(VALUE))              # note: this expands!

check-local: test
	@echo "***********************************************************************************************"
	@echo "*** ROSE/projects/extractMPISkeletons: make check rule complete (terminated normally)"
	@echo "***********************************************************************************************"
