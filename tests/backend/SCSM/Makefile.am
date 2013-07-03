include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

bin_PROGRAMS = test

test_SOURCES = test.cpp scsm-sage.cpp
test_CXXFLAGS = \
  -g \
  $(ROSE_INCLUDES)
test_LDADD= \
  $(LIBS_WITH_RPATH) $(ROSE_LIBS)

check-local: test
	./test
	cat output.*

clean-local:
	rm -f output.*
