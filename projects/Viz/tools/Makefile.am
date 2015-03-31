
include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

##############

bin_PROGRAMS = build-sage-variant

build_sage_variant_SOURCES = \
  build-sage-variant.cpp \
  jsonxx.cc
build_sage_variant_CXXFLAGS = -g

############

check-am: check-sage-variant

clean-am:
	rm -f sage.dot sage.svg
	rm -f sage-variant.cpp sage-variant.o

sage.svg: sage.dot
	dot -Tsvg $< -o $@

check-sage-variant: sage.svg sage-variant.o

sage-variant.o: sage-variant.cpp
	c++ $(ROSE_INCLUDES) -I$(top_builddir) -g -c sage-variant.cpp -o sage-variant.o

sage-variant.cpp: sage.json build-sage-variant
	./build-sage-variant $< $@

sage.dot: sage.json build-sage-variant
	./build-sage-variant $< $@

############

