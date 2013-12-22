include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

bin_PROGRAMS=looptree-to-kernel

looptree_to_kernel_SOURCES= \
  looptree-to-kernel.cpp
looptree_to_kernel_CXXFLAGS= \
  -g \
  -I$(top_srcdir)/src/frontend/DLX/include \
  -I$(top_srcdir)/src/backend/KLT/include \
  $(ROSE_INCLUDES)
looptree_to_kernel_LDADD = \
  $(top_builddir)/src/frontend/DLX/lib/core/libDLX-core.la \
  $(top_builddir)/src/frontend/DLX/lib/openacc/libDLX-openacc.la \
  $(top_builddir)/src/backend/KLT/lib/libKLT.la \
  $(top_builddir)/src/midend/MFB/lib/sage/libMFB-sage.la \
  $(LIBS_WITH_RPATH) \
  $(ROSE_LIBS)

check-local: looptree-to-kernel
	./looptree-to-kernel $(srcdir)/../../tests/101-vector-constant-addition/101-klt.lt 101-klt.out
	./looptree-to-kernel $(srcdir)/../../tests/102-vector-constant-product/102-klt.lt 102-klt.out
	./looptree-to-kernel $(srcdir)/../../tests/103-vector-vector-addition/103-klt.lt 103-klt.out

$(top_builddir)/src/backend/KLT/lib/libKLT.la:
	make -C $(top_builddir)/src/backend/KLT/lib libKLT.la

$(top_builddir)/src/midend/MFB/lib/sage/libMFB-sage.la:
	make -C $(top_builddir)/src/midend/MFB/lib/sage libMFB-sage.la

