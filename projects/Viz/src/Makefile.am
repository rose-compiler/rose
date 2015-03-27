
include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

##############

bin_PROGRAMS = blank-viz default-viz programmable-viz

blank_viz_SOURCES = viz.cpp
blank_viz_CXXFLAGS = \
  -DVIZ_TRAVERSAL=0 \
  -I$(srcdir)/../include \
  -g \
  $(ROSE_INCLUDES)
blank_viz_LDADD= \
  $(builddir)/../lib/libRoseViz.la \
  $(LIBS_WITH_RPATH) $(ROSE_LIBS)

default_viz_SOURCES = viz.cpp
default_viz_CXXFLAGS = \
  -DVIZ_TRAVERSAL=1 \
  -I$(srcdir)/../include \
  -g \
  $(ROSE_INCLUDES)
default_viz_LDADD= \
  $(builddir)/../lib/libRoseViz.la \
  $(LIBS_WITH_RPATH) $(ROSE_LIBS)

programmable_viz_SOURCES = \
  viz.cpp \
  $(srcdir)/../tools/jsonxx.cc \
  $(builddir)/../tools/sage-variant.cpp
programmable_viz_CXXFLAGS = \
  -DVIZ_TRAVERSAL=2 \
  -I$(srcdir)/../include \
  -I$(srcdir)/../tools \
  -g \
  $(ROSE_INCLUDES)
programmable_viz_LDADD= \
  $(builddir)/../lib/libRoseViz.la \
  $(LIBS_WITH_RPATH) $(ROSE_LIBS)

##############

SVG_TESTS= \
  blank_test_01.svg \
  blank_test_02.svg \
  default_test_01.svg \
  default_test_02.svg \
  programmable_test_01.svg \
  programmable_test_02.svg \
  program_0_test_01.svg \
  program_0_test_02.svg

DOT_BLANK_TESTS= \
  blank_test_01.dot \
  blank_test_02.dot

DOT_DEFAULT_TESTS= \
  default_test_01.dot \
  default_test_02.dot

DOT_PROG_TESTS= \
  programmable_test_01.dot \
  programmable_test_02.dot

DOT_PROG_0_TESTS= \
  program_0_test_01.dot \
  program_0_test_02.dot


check-am: $(SVG_TESTS)

clean-am:
	-rm *.dot *.svg

$(SVG_TESTS): %.svg: %.dot
	dot -Tsvg $< -o $@

$(DOT_BLANK_TESTS): blank_%.dot: $(srcdir)/../examples/%.c blank-viz
	./blank-viz -prefix blank_ -DSKIP_ROSE_BUILTIN_DECLARATIONS $<

$(DOT_DEFAULT_TESTS): default_%.dot: $(srcdir)/../examples/%.c default-viz
	./default-viz -prefix default_ -DSKIP_ROSE_BUILTIN_DECLARATIONS $<

$(DOT_PROG_TESTS): programmable_%.dot: $(srcdir)/../examples/%.c programmable-viz
	./programmable-viz -prefix programmable_ -DSKIP_ROSE_BUILTIN_DECLARATIONS $<

$(DOT_PROG_0_TESTS): program_0_%.dot: $(srcdir)/../examples/%.c $(srcdir)/../examples/program_0.viz programmable-viz
	./programmable-viz -viz_program $(srcdir)/../examples/program_0.viz -prefix program_0_ -DSKIP_ROSE_BUILTIN_DECLARATIONS $<

##############

$(builddir)/../lib/libRoseViz.so:
	make -C $(builddir)/../lib libRoseViz.so

$(builddir)/../tools/sage-variant.cpp:
	make -C $(builddir)/../tools sage-variant.cpp

##############

