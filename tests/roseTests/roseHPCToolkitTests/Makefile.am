# ROSE-HPCT example programs

# Sample data
SUBDIRS = data


#include ../Make.inc
include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

# Root of ROSE-HPCT source tree
ROSEHPCT_SRC_ROOT = $(top_srcdir)/src/roseExtensions/roseHPCToolkit
ROSEHPCT_BUILD_ROOT = $(top_builddir)/src/roseExtensions/roseHPCToolkit

# Root of ROSE-HPCT header file installation tree
rosehpctincdir = $(includedir)/rosehpct

# Default header file search path
INCLUDES = -I$(top_srcdir) -I$(top_builddir) \
           $(ROSE_INCLUDES) \
           $(ROSEHPCT_DEP_INCLUDES) \
           -I$(ROSEHPCT_SRC_ROOT)/include

# Until we get around to fixing the spelling...
ROSE_SEPARATE_LIBS = $(ROSE_SEPERATE_LIBS)

# Default link flags for executables
ROSEHPCT_LIBS = $(ROSEHPCT_BUILD_ROOT)/src/librosehpct.la \
                $(ROSE_LIBS) -lrt
LDADD = $(ROSEHPCT_DEP_LIBS)

if ROSE_BUILD_ROSEHPCT

EXTRA_DIST = test_attach01a.sh \
	test_attach02.sh \
	test_hl01.sh \
	test_attach01.sh \
	test_attach03.sh \
	test_prop01.sh

noinst_PROGRAMS = convert \
                  simple_read2 \
                  process_cmdline \
                  attach_metrics \
                  propagate_metrics \
                  dump_ast \
                  read_ast \
                  test_highlevel

convert_SOURCES = convert.cc
convert_LDFLAGS = $(ROSEHPCT_LIBS)

simple_read2_SOURCES = simple_read2.cc
simple_read2_LDFLAGS = $(ROSEHPCT_LIBS)

process_cmdline_SOURCES = process_cmdline.cc
process_cmdline_LDFLAGS = $(ROSEHPCT_LIBS)

attach_metrics_SOURCES = attach_metrics.cc
attach_metrics_LDFLAGS = $(ROSEHPCT_LIBS)

propagate_metrics_SOURCES = propagate_metrics.cc
propagate_metrics_LDFLAGS = $(ROSEHPCT_LIBS)

dump_ast_SOURCES = dump_ast.cc
dump_ast_LDFLAGS = $(ROSEHPCT_LIBS)

read_ast_SOURCES = read_ast.cc
read_ast_LDFLAGS = $(ROSEHPCT_LIBS)

test_highlevel_SOURCES = test_highlevel.cc
test_highlevel_LDFLAGS = $(ROSEHPCT_LIBS)

check-local:
	env SRC_DIR_PREFIX=$(srcdir)/ $(srcdir)/test_attach01a.sh
	env SRC_DIR_PREFIX=$(srcdir)/ $(srcdir)/test_attach01.sh
#	$(srcdir)/test_attach02.sh
#	$(srcdir)/test_attach03.sh
	env SRC_DIR_PREFIX=$(srcdir)/ $(srcdir)/test_prop01.sh
	env SRC_DIR_PREFIX=$(srcdir)/ $(srcdir)/test_hl01.sh

clean-local:
	rm -rf $(CXX_TEMPLATE_OBJECTS) Templates.DB ii_files ti_files
	rm -f *.pdf

distclean-local:
	rm -rf Templates.DB

else  #  !ROSE_BUILD_ROSEHPCT

EXTRA_DIST = test_attach01a.sh \
	test_attach02.sh \
	test_hl01.sh \
	test_attach01.sh \
	test_attach03.sh \
	test_prop01.sh \
  convert.cc \
  simple_read2.cc \
  process_cmdline.cc \
  attach_metrics.cc \
  propagate_metrics.cc \
  dump_ast.cc \
  read_ast.cc \
  test_highlevel.cc

endif # ROSE_BUILD_ROSEHPCT

# eof
