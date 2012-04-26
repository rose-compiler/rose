include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

EXEC=../../src/extractMPISkeleton

EXTRA_DIST = \
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

include Makefile.common

check-local: ../src/extractMPISkeleton
	cp -fR $(srcdir)/* .
	${MAKE} ${MARKS}
	${MAKE} ${ALL_OBJS}
	${MAKE} ${ALL_EXPECTED:.expected=.comp}
	@echo "***********************************************************************************************"
	@echo "*** ROSE/projects/extractMPISkeleton/tests: make check rule complete (terminated normally)"
	@echo "***********************************************************************************************"

