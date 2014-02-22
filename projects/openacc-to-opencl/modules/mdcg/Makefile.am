include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

bin_PROGRAMS = build-openacc-models

$(top_builddir)/src/backend/MFB/lib/sage/libMFB-sage.la:
	$(MAKE) -C $(top_builddir)/src/backend/MFB/lib/sage libMFB-sage.la

$(top_builddir)/src/backend/MDCG/lib/libMDCG.la:
	$(MAKE) -C $(top_builddir)/src/backend/MDCG/lib libMDCG.la

build_openacc_models_SOURCES = build-openacc-models.cpp
build_openacc_models_CXXFLAGS = \
  -I$(top_srcdir)/src/midend/MFB/include \
  -I$(top_srcdir)/src/midend/MDCG/include \
  -g \
  $(ROSE_INCLUDES)
build_openacc_models_LDADD= \
  $(top_builddir)/src/midend/MFB/lib/sage/libMFB-sage.la \
  $(top_builddir)/src/midend/MDCG/lib/libMDCG.la \
  $(LIBS_WITH_RPATH) $(ROSE_LIBS)

check-local: build-openacc-models
	./build-openacc-models $(top_srcdir)/../libopenacc/ /export/tmp.vanderbrugge1/scratch/OpenCL/

clean-local:
	rm -f *.hpp *.cpp *.dat *.png *.dot

