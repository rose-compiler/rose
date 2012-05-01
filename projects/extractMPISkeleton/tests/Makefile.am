include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

EXEC=../../src/extractMPISkeleton

EXTRA_DIST = \
	bcast/bcast.c \
	bcast/files \
	bcast/rose_bcast.c.expected \
	collect/collect.c \
	collect/files \
	diffusion2/diffusion2.c \
	diffusion2/files \
	exchange/exchng.c \
	exchange/files \
	exchange/rose_exchng.c.expected \
	gausselim_par/files \
	gausselim_par/gausselim_par.c \
	hellow/files \
	hellow/helloworld.c \
	hellow/rose_helloworld.c.expected \
	jacobi2/files \
	jacobi2/jacobi2.c \
	jacobicmpl/files \
	jacobicmpl/jacobicmpl.c \
	jacobi/files \
	jacobi/jacobi.c \
	master-slave/files \
	master-slave/io.c \
	master-slave/rose_io.c.expected \
	mpi-ex1/files \
	mpi-ex1/mpi-ex1.c \
	ring/files \
	ring/ring.c \
	ring/rose_ring.c.expected \
	sample1/files \
	sample1/sample1.cc \
	sample2/files \
	sample2/sample2.cc \
	sample3/files \
	sample3/sample3.c \
	sample4/files \
	sample4/sample4.cc \
	sample5/files \
	sample5/sample5.c \
	sample7/files \
	sample7/sample7.c \
	sample8/files \
	sample8/sample8.c \
	sample9/files \
	sample9/sample9.c \
	sampleA/files \
	sampleA/sampleA.c \
	scatterv/files \
	scatterv/scatterv.c \
	t1/files \
	t1/t1.c \
	t4/files \
	t4/rose_t4.c.expected \
	t4/t4.c \
	t5/files \
	t5/rose_t5.c.expected \
	t5/t5.c \
	t6/files \
	t6/t6.c \
	t7/files \
	t7/t7.c \
	t8/files \
	t8/t8.c \
	t9/files \
	t9/t9.c \
	t10/files \
	t10/t10.c \
	t11/files \
	t11/t11.c \
	t12/files \
	t12/t12.c \
	t13/files \
	t13/t13.c \
	t14/files \
	t14/t14.c \
	t16/files \
	t16/test.c \
	t17/files \
	t17/test.c

include Makefile.common

check-local: ../src/extractMPISkeleton
	${MAKE} ${MARKS}
	${MAKE} ${ALL_OBJS}
	${MAKE} ${ALL_EXPECTED:.expected=.comp}
	@echo "***********************************************************************************************"
	@echo "*** ROSE/projects/extractMPISkeleton/tests: make check rule complete (terminated normally)"
	@echo "***********************************************************************************************"

