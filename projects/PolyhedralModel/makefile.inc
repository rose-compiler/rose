include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

inc_dir=$(top_srcdir)/projects/PolyhedralModel/include/
tpl_dir=$(top_srcdir)/projects/PolyhedralModel/tpl/
src_dir=$(top_srcdir)/projects/PolyhedralModel/src/
obj_dir=$(top_builddir)/projects/PolyhedralModel/obj/
bin_dir=$(top_builddir)/projects/PolyhedralModel/bin/

# PPL information, PPL is needed
CXX_INC_PPL= -I$(PPL_PATH)/include/
CXX_LIB_PPL= -L$(PPL_PATH)/lib/ -lppl

# ROSE information
CXX_INC_ROSE= $(ROSE_INCLUDES) -I$(top_builddir)
CXX_LIB_ROSE= $(LIBS_WITH_RPATH) $(ROSE_LIBS)

# CLOOG information
if ROSE_USE_CLOOG
CXX_OPT_CLOOG= -DCLOOG_INT_GMP
CXX_INC_CLOOG= -I$(CLOOG_PATH)/include/
CXX_LIB_CLOOG= -L$(CLOOG_PATH)/lib/ -lcloog-isl
endif

# SCoPlib information
if ROSE_USE_SCOPLIB
CXX_INC_SCOPLIB= -I$(SCOPLIB_PATH)/include/
CXX_LIB_SCOPLIB= -L$(SCOPLIB_PATH)/lib/ -lscoplib
endif

# CANDL information
if ROSE_USE_CANDL
CXX_INC_CANDL= -I$(CANDL_PATH)/include/
CXX_LIB_CANDL= -L$(CANDL_PATH)/lib/ -lcandl
endif

CXX_INC_PARSER=-I$(top_srcdir)/projects/pragmaParsing/
PARSER_OBJS=$(top_builddir)/projects/pragmaParsing/libparserbuilder.la
#CXX_LIB_PARSER=-L$(top_srcdir)/projects/pragmaParsing/ -lparserbuilder

LT_CXX=@echo "COMPILING $@"; libtool --mode=compile g++
LT_LINK=@echo "LINKING   $@"; libtool --mode=link g++

MAKE=make

# Basic compilation tags
CXX_OPT=
CXX_INC=-I$(inc_dir) -I$(tpl_dir)
CXX_LIB=

if ROSE_USE_CLOOG
CXX_OPT+= -DROSE_USE_CLOOG
endif

CXX_OPT+= $(CXX_OPT_PPL)
CXX_INC+= $(CXX_INC_PPL)
CXX_LIB+= $(CXX_LIB_PPL)

if ROSE_USE_CLOOG
CXX_OPT+= $(CXX_OPT_CLOOG)
CXX_INC+= $(CXX_INC_CLOOG)
CXX_LIB+= $(CXX_LIB_CLOOG)
endif

if ROSE_USE_SCOPLIB
CXX_OPT+= $(CXX_OPT_SCOPLIB)
CXX_INC+= $(CXX_INC_SCOPLIB)
CXX_LIB+= $(CXX_LIB_SCOPLIB)
endif

if ROSE_USE_CANDL
CXX_OPT+= $(CXX_OPT_CANDL)
CXX_INC+= $(CXX_INC_CANDL)
CXX_LIB+= $(CXX_LIB_CANDL)
endif
