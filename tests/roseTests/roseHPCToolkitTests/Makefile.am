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

# Default link flags for executables
ROSEHPCT_LIBS = $(ROSEHPCT_BUILD_ROOT)/src/librosehpct.la \
                $(ROSE_LIBS) $(RT_LIBS)
LDADD = $(ROSEHPCT_DEP_LIBS)

if ROSE_BUILD_ROSEHPCT

EXTRA_DIST = test_attach01a.sh \
	test_attach02.sh \
	test_hl01.sh \
	test_attach01.sh \
	test_attach03.sh \
	test_prop01.sh \
	test_dump01.sh 

# DQ (4/23/2011): At least one of the files (process_cmdline.o) in this directory does not seem 
# to compile with Insure++ (it hangs for Hudson, but I can't reproduce the problem locally)
# This was not a problem the second time that Insure++ compiled ROSE.
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

# used for invoking individual tests
# test_attach01.sh
TEST1_FLAGS = -rose:hpct:prof $(srcdir)/data/01/PROFILE/PAPI_TOT_CYC.xml \
              -rose:hpct:prof $(srcdir)/data/01/PROFILE/PAPI_FP_OPS.xml \
              -rose:hpct:prof $(srcdir)/data/01/PROFILE/PAPI_L1_DCA.xml \
              -rose:hpct:prof $(srcdir)/data/01/PROFILE/PAPI_L1_DCM.xml \
              -rose:hpct:prof $(srcdir)/data/01/PROFILE/PAPI_L2_TCM.xml \
              -rose:hpct:eqpath .=$(srcdir)/data/01 


test_attach01:attach_metrics
	./attach_metrics$(EXEEXT) $(TEST1_FLAGS) -c  $(srcdir)/data/01/example.c

# test_attach01a.sh
TEST2_FLAGS = -rose:hpct:prof $(srcdir)/data/01/ANALYSIS/scope_tree.hpcviewer \
              -rose:hpct:eqpath .src1=$(srcdir)/data/01 
test_attach01a:attach_metrics
	./attach_metrics$(EXEEXT) $(TEST2_FLAGS) -c  $(srcdir)/data/01/example.c

test_prop01:propagate_metrics
	./propagate_metrics$(EXEEXT) $(TEST1_FLAGS) -c  $(srcdir)/data/01/example.c

test_dump01:dump_ast
	./dump_ast$(EXEEXT) $(TEST1_FLAGS) -c  $(srcdir)/data/01/example.c
.PHONY: test_attach01 test_attach01a test_prop01 test_dump01

# DQ (3/21/2009): These tests don't appear to run under Cygwin.
if ROSE_BUILD_OS_IS_CYGWIN
check-local:
	@echo "********************************************************************************************"
	@echo "****** ROSE/tests/roseTests/roseHPCToolkitTests: (commented out all tests for Cygwin) ******"
	@echo "********************************************************************************************"
else
check-local:
# DQ (9/10/2009): Skip tests on Debian system to test ROSE (and focus first on where it works!)
if !OS_VENDOR_DEBIAN
	env SRC_DIR_PREFIX=$(srcdir)/ $(srcdir)/test_attach01a.sh
	env SRC_DIR_PREFIX=$(srcdir)/ $(srcdir)/test_attach01.sh
#	$(srcdir)/test_attach02.sh
#	$(srcdir)/test_attach03.sh
	env SRC_DIR_PREFIX=$(srcdir)/ $(srcdir)/test_prop01.sh
	env SRC_DIR_PREFIX=$(srcdir)/ $(srcdir)/test_hl01.sh
	env SRC_DIR_PREFIX=$(srcdir)/ $(srcdir)/test_dump01.sh
	@echo "******************************************************************************************************"
	@echo "****** ROSE/tests/roseTests/roseHPCToolkitTests: make check rule complete (terminated normally) ******"
	@echo "******************************************************************************************************"
else
	@echo "***** SKIPPING TESTS IN ROSE/tests/roseTests/roseHPCToolkitTests *****"
endif
endif

clean-local:
	rm -rf $(CXX_TEMPLATE_OBJECTS) Templates.DB ii_files ti_files *.bin
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
	test_dump01.sh \
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
