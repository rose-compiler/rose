include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

bin_PROGRAMS = build-model static-initializer

$(top_builddir)/src/backend/MFB/lib/sage/libMFB-sage.la:
	$(MAKE) -C $(top_builddir)/src/backend/MFB/lib/sage libMFB-sage.la

$(top_builddir)/src/backend/MDCG/lib/libMDCG.la:
	$(MAKE) -C $(top_builddir)/src/backend/MDCG/lib libMDCG.la

build_model_SOURCES = build-model.cpp
build_model_CXXFLAGS = \
  -I$(top_srcdir)/src/midend/MFB/include \
  -I$(top_srcdir)/src/midend/MDCG/include \
  -g \
  $(ROSE_INCLUDES)
build_model_LDADD= \
  $(top_builddir)/src/midend/MFB/lib/sage/libMFB-sage.la \
  $(top_builddir)/src/midend/MDCG/lib/libMDCG.la \
  $(LIBS_WITH_RPATH) $(ROSE_LIBS)

static_initializer_SOURCES = static-initializer.cpp
static_initializer_CXXFLAGS = \
  -I$(top_srcdir)/src/midend/MFB/include \
  -I$(top_srcdir)/src/midend/MDCG/include \
  -g \
  $(ROSE_INCLUDES)
static_initializer_LDADD= \
  $(top_builddir)/src/midend/MFB/lib/sage/libMFB-sage.la \
  $(top_builddir)/src/midend/MDCG/lib/libMDCG.la \
  $(LIBS_WITH_RPATH) $(ROSE_LIBS)

check-model: build-model test_01.h test_02.h test_02.c
	./build-model $(srcdir)

check-static-init: static-initializer test_02.h
	./static-initializer $(srcdir)

check-local:
	$(MAKE) check-model
	$(MAKE) check-static-init

clean-local:
	rm -f *.hpp *.cpp *.dat *.png *.dot

