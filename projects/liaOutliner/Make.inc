include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

#place to install binary, header, and libraries
liaOutlinerbindir = $(bindir)/liaOutliner
liaOutlinerincdir = $(includedir)/liaOutliner
liaOutlinerlibdir = $(libdir)

# Default header file search path
INCLUDES = \
	-I$(top_srcdir) -I$(top_builddir) \
	$(ROSE_INCLUDES) \
	-I$(top_srcdir)/src/midend/astOutlining/src

# Default link flags for executables
LIAOUTLINER_LIBS = \
	$(top_builddir)/src/midend/astOutlining/src/libliaOutliner.la \
	$(ROSE_LIBS)

# Useful definitions for testing
#   TEST_DIR = Location of ROSE's basic C++ tests.
#   TEST_INCLUDES = Find header files upon which the test files depend.
TEST_DIR = $(top_srcdir)/tests/CompileTests/Cxx_tests
TEST_INCLUDES = \
	-I$(TEST_DIR) \
	-I$(top_srcdir)/tests/CompileTests/A++Code

# eof
