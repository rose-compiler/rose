# Example Makefile for ROSE users
# This makefile is provided as an example of how to use ROSE when ROSE is
# installed (using "make install").  This makefile is tested as part of the
# "make distcheck" rule (run as part of tests before any CVS checkin).
# The test of this makefile can also be run by using the "make installcheck"
# rule (run as part of "make distcheck").

top_srcdir =/home/nguyenthanh1/Rose_compile_tree

# Location of Boost include directory
BOOST_CPPFLAGS = -pthread -I/home/nguyenthanh1/boost/include

#use -DPRE_ORDER for preorder and -DPOST_ORDER for postorder traversal
export ORDER_FLAG = -DPOST_ORDER

# -DFUNC_DECL or -DFUNC_DEF or VAR_DECL or -DGLOBAL 
export NODE_DELETE_FLAG = -DGLOBAL



# Location of include directory after "make install"
ROSE_INCLUDE_DIR = $(top_srcdir)/include

ROSE_INCLUDES = \
     $(ROSE_BOOST_PREINCLUDE_PATH) \
     -I$(top_srcdir)/src \
     -I$(top_srcdir)/src/frontend/SageIII \
     -I$(top_srcdir)/src/frontend/SageIII/astPostProcessing \
     -I$(top_srcdir)/src/frontend/SageIII/astVisualization \
     -I$(top_srcdir)/src/frontend/SageIII/sageInterface \


# Location of Dwarf include and lib (if ROSE is configured to use Dwarf)
ROSE_DWARF_INCLUDES = 
ROSE_DWARF_LIBS_WITH_PATH = 
ROSE_INCLUDE_DIR += $(ROSE_DWARF_INCLUDES)
ROSE_LIBS += $(ROSE_DWARF_LIBS_WITH_PATH)

CC                    = gcc
CXX                   = g++
CPPFLAGS              =  -I/usr/apps/java/jdk1.5.0_11/include -I/usr/apps/java/jdk1.5.0_11/include/linux
#CXXCPPFLAGS           = @CXXCPPFLAGS@
CXXFLAGS              =
LDFLAGS               = 

# Location of library directory after "make install"
ROSE_LIB_DIR = $(top_srcdir)//lib

ROSE_LIBS = $(ROSE_LIB_DIR)/librose.la

ROSE_SOURCE_DIR = $(top_srcdir)/testDeepDelete 

# Default make rule to use
all: testDeepDelete 
	@if [ x$${ROSE_IN_BUILD_TREE:+present} = xpresent ]; then echo "ROSE_IN_BUILD_TREE should not be set" >&2; exit 1; fi

# Example suffix rule for more experienced makefile users
# .C.o:
#	g++ -c -I$(ROSE_INCLUDE_DIR) -o $@ $(@:.o=.C)

# Compile the exampleTranslator using the file identityTranslator.C
testDeepDelete.lo:
	/bin/sh $(top_srcdir)/libtool --mode=compile $(CXX) $(CXXFLAGS)  $(CPPFLAGS) $(ROSE_INCLUDES) -I$(ROSE_INCLUDE_DIR) $(BOOST_CPPFLAGS) $(ORDER_FLAG) $(NODE_DELETE_FLAG)  -c -o testDeepDelete.lo testDeepDelete.C

testDeepDelete: testDeepDelete.lo
	/bin/sh $(top_srcdir)/libtool --mode=link $(CXX) $(CXXFLAGS) $(LDFLAGS) $(ORDER_FLAG) $(NODE_DELETE_FLAG) -o testDeepDelete testDeepDelete.lo $(ROSE_LIBS)

# Rule used by make installcheck to verify correctness of installed libraries
check:
	./testDeepDelete -c input.C 
clean:
	$(RM) testDeepDelete 
	$(RM) testDeepDelete.lo
	$(RM) *.dot
