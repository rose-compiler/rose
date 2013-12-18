include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

bin_PROGRAMS = build-model

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

check-model: build-model
	echo "one bar $(srcdir) h" > test.dat
	./build-model test.dat
	echo "pair foo $(srcdir) $(srcdir) h c" > test.dat
	./build-model test.dat
	echo "one bar $(srcdir) h" > test.dat
	echo "pair foo $(srcdir) $(srcdir) h c" >> test.dat
	echo "one test $(srcdir) c" >> test.dat
	./build-model test.dat

check-local: build-model
	$(MAKE) check-model

clean-local:
	rm -f *.hpp *.cpp

