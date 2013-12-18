include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

bin_PROGRAMS = build-model

$(top_builddir)/src/backend/MFB/lib/sage/libMFB-sage.la:
	$(MAKE) -C $(top_builddir)/src/backend/MFB/lib/sage libMFB-sage.la

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
	./build-test

check-load: load-test
	cp $(srcdir)/foo-bar.* .
	./load-test foo-bar

check-local: build-test load-test
	$(MAKE) check-load
	$(MAKE) check-build

clean-local:
	rm -f *.hpp *.cpp

