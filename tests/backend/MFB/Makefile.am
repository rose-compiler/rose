include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

bin_PROGRAMS = build-test load-test

$(top_builddir)/src/backend/MFB/lib/sage/libMFB-sage.la:
	$(MAKE) -C $(top_builddir)/src/backend/MFB/lib/sage libMFB-sage.la

build_test_SOURCES = build-test.cpp
build_test_CXXFLAGS = \
  -I$(top_srcdir)/src/backend/MFB/include \
  -g \
  $(ROSE_INCLUDES)
build_test_LDADD= \
  $(top_builddir)/src/backend/MFB/lib/sage/libMFB-sage.la \
  $(LIBS_WITH_RPATH) $(ROSE_LIBS)

load_test_SOURCES = load-test.cpp
load_test_CXXFLAGS = \
  -I$(top_srcdir)/src/backend/MFB/include \
  -g \
  $(ROSE_INCLUDES)
load_test_LDADD= \
  $(top_builddir)/src/backend/MFB/lib/sage/libMFB-sage.la \
  $(LIBS_WITH_RPATH) $(ROSE_LIBS)

check-build: build-test
	./build-test

check-load: load-test
	cp $(srcdir)/foo-bar.* .
	./load-test foo-bar

check-local: build-test load-test
	$(MAKE) check-build
	$(MAKE) check-load

clean-local:
	rm -f *.hpp *.cpp
