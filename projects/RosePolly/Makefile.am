include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

## Place generated object files (.o) into the same directory as their source
## files, in order to avoid collisions when non-recursive make is used.
AUTOMAKE_OPTIONS = subdir-objects

# ------------------------------------------------------------------------------
#  rosepoly
# ------------------------------------------------------------------------------

# GMP_PATH = /Users/konstantinid1/Development/opt
# ISL_PATH = /Users/konstantinid1/Internship/Tools/ISL
# CLOOG_PATH = /Users/konstantinid1/Internship/Tools/CLOOG
AM_CPPFLAGS = \
  -I$(srcdir)/include \
  -I$(ISL_INSTALL_PATH)/include \
  -I$(CLOOG_PATH)/include \
  $(ROSE_INCLUDES)

# -L$(GMP_PATH)/lib    -lgmp
LDADD = \
  $(ROSE_LIBS) \
  -L$(ISL_INSTALL_PATH)/lib -lisl \
  -L$(CLOOG_PATH)/lib -lcloog-isl

# ------------------------------------------------------------------------------
#  rosepoly.la
# ------------------------------------------------------------------------------

rosepoly_API_VERSION=0.1.0
exec_prefix=${prefix}/rosepoly-$(rosepoly_API_VERSION)
librosepoly_la_includedir = ${exec_prefix}/include/rosepoly

if ROSE_WITH_ISL
lib_LTLIBRARIES = librosepoly.la
else
lib_LTLIBRARIES = 
endif	

librosepoly_la_CPPFLAGS = \
  -DISL \
  -DCLOOG_INT_GMP

librosepoly_la_SOURCES = \
  src/RosePollyInterface.cpp \
  src/Kernel.cpp \
  src/FlowGraph.cpp \
  src/Nodes.cpp \
  src/RosePollyModel.cpp \
  src/data.cpp \
  src/Utilities.cpp \
  src/error.cpp \
  src/access_pattern.cpp \
  src/traversals.cpp \
  src/RosePollyMath.cpp \
  src/isl_interface.cpp \
  src/simple_multi_graph.cpp \
  src/RosePluto.cpp \
  src/simple_matrix.cpp \
  src/RoseCloog.cpp \
  src/RosePollyCustom.cpp

librosepoly_la_include_HEADERS = \
  include/rosepoly/access_pattern.h \
  include/rosepoly/data.h \
  include/rosepoly/error.h \
  include/rosepoly/FlowGraph.h \
  include/rosepoly/isl_interface.h \
  include/rosepoly/Kernel.h \
  include/rosepoly/Makefile.am \
  include/rosepoly/Nodes.h \
  include/rosepoly/RoseCloog.h \
  include/rosepoly/RosePluto.h \
  include/rosepoly/RosePollyCustom.h \
  include/rosepoly/RosePollyInterface.h \
  include/rosepoly/RosePollyMath.h \
  include/rosepoly/RosePollyModel.h \
  include/rosepoly/simple_matrix.h \
  include/rosepoly/simple_multi_graph.h \
  include/rosepoly/traversals.h \
  include/rosepoly/Utilities.h

