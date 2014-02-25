include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

bin_PROGRAMS = api-extractor # build-test load-test

$(top_builddir)/src/midend/MFB/lib/sage/libMFB-sage.la:
	$(MAKE) -C $(top_builddir)/src/midend/MFB/lib/sage libMFB-sage.la

api_extractor_SOURCES = api-extractor.cpp
api_extractor_CXXFLAGS = \
  -I$(top_srcdir)/src/midend/MFB/include \
  -g \
  $(ROSE_INCLUDES)
api_extractor_LDADD= \
  $(top_builddir)/src/midend/MFB/lib/sage/libMFB-sage.la \
  $(LIBS_WITH_RPATH) $(ROSE_LIBS)

build_test_SOURCES = build-test.cpp
build_test_CXXFLAGS = \
  -I$(top_srcdir)/src/midend/MFB/include \
  -g \
  $(ROSE_INCLUDES)
build_test_LDADD= \
  $(top_builddir)/src/midend/MFB/lib/sage/libMFB-sage.la \
  $(LIBS_WITH_RPATH) $(ROSE_LIBS)

load_test_SOURCES = load-test.cpp
load_test_CXXFLAGS = \
  -I$(top_srcdir)/src/midend/MFB/include \
  -g \
  $(ROSE_INCLUDES)
load_test_LDADD= \
  $(top_builddir)/src/midend/MFB/lib/sage/libMFB-sage.la \
  $(LIBS_WITH_RPATH) $(ROSE_LIBS)

check-build: build-test
	rm -f *.hpp *.cpp
	./build-test

check-load: load-test
	rm -f *.hpp *.cpp
	cp $(srcdir)/foo-bar.* .
	./load-test foo-bar

check-api: api-extractor
	rm -f *.hpp *.cpp
	cp $(srcdir)/foo-bar.* .
	./api-extractor foo-bar

check-local: api-extractor # build-test load-test
#	$(MAKE) check-load
#	$(MAKE) check-build
	$(MAKE) check-api

clean-local:
	rm -f *.hpp *.cpp

