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
	(cd tests && ${MAKE} clean-local)

noinst_HEADERS = APIDepAttrib.h APIDepFinder.h APIReader.h APISpec.h \
  DangerousOperationFinder.h GenericDepAttrib.h Outline.h \
  Utils.h ssexpr.h processPragmas.h annotatePragmas.h

TEST_FILES = \
	tests/bcast/bcast.c \
	tests/bcast/files \
	tests/bcast/rose_bcast.c.expected \
	tests/collect/collect.c \
	tests/collect/files \
	tests/diffusion2/diffusion2.c \
	tests/diffusion2/files \
	tests/exchange/exchng.c \
	tests/exchange/files \
	tests/exchange/rose_exchng.c.expected \
	tests/gausselim_par/files \
	tests/gausselim_par/gausselim_par.c \
	tests/hellow/files \
	tests/hellow/helloworld.c \
	tests/hellow/rose_helloworld.c.expected \
	tests/jacobi2/files \
	tests/jacobi2/jacobi2.c \
	tests/jacobicmpl/files \
	tests/jacobicmpl/jacobicmpl.c \
	tests/jacobi/files \
	tests/jacobi/jacobi.c \
	tests/master-slave/files \
	tests/master-slave/io.c \
	tests/master-slave/rose_io.c.expected \
	tests/mpi-ex1/files \
	tests/mpi-ex1/mpi-ex1.c \
	tests/ring/files \
	tests/ring/ring.c \
	tests/ring/rose_ring.c.expected \
	tests/sample1/files \
	tests/sample1/sample1.cc \
	tests/sample2/files \
	tests/sample2/sample2.cc \
	tests/sample3/files \
	tests/sample3/sample3.c \
	tests/sample4/files \
	tests/sample4/sample4.cc \
	tests/sample5/files \
	tests/sample5/sample5.c \
	tests/sample7/files \
	tests/sample7/sample7.c \
	tests/sample8/files \
	tests/sample8/sample8.c \
	tests/sample9/files \
	tests/sample9/sample9.c \
	tests/sampleA/files \
	tests/sampleA/sampleA.c \
	tests/scatterv/files \
	tests/scatterv/scatterv.c \
	tests/t1/files \
	tests/t1/t1.c \
	tests/t4/files \
	tests/t4/rose_t4.c.expected \
	tests/t4/t4.c \
	tests/t5/files \
	tests/t5/rose_t5.c.expected \
	tests/t5/t5.c \
	tests/t6/files \
	tests/t6/t6.c \
	tests/t7/files \
	tests/t7/t7.c \
	tests/t8/files \
	tests/t8/t8.c \
	tests/t9/files \
	tests/t9/t9.c
	tests/t10/files \
	tests/t10/t10.c \
	tests/t11/files \
	tests/t11/t11.c \
	tests/t12/files \
	tests/t12/t12.c \
	tests/t13/files \
	tests/t13/t13.c \
	tests/t14/files \
	tests/t14/t14.c \
	tests/t16/files \
	tests/t16/test.c \
	tests/t17/files \
	tests/t17/test.c

EXTRA_DIST=mpi_api.spec stdio_input.spec apis.coll \
           ${TEST_FILES} mpi-fakeheader/mpi.h doc/user-manual.txt

TAGS: ${CCFILES} ${HFILES}
	etags $^

show:
	@echo $($(VALUE))              # note: this expands!

check-local:
	cp -rf --copy-contents $(srcdir)/tests/* tests
	(cd tests && ${MAKE} test)
	@echo "***********************************************************************************************"
	@echo "*** ROSE/projects/extractMPISkeleton: make check rule complete (terminated normally)"
	@echo "***********************************************************************************************"
