include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

# . is necessary to ensure building the translator before building the tests
SUBDIRS = . tests

INCLUDES = $(ROSE_INCLUDES) 

# Bison flags
AM_YFLAGS =
# Flex flags
AM_LFLAGS =

# ompparser.cc and omplexer.cc are put into distribution to eliminate
# the dependence on bison and flex for end-users.
# Maintainers should update them whenever .yy or .ll are changed!!

# avoid unused Flex functions which are automatically generated		  
CXXFLAGS+= -DYY_NO_TOP_STATE -DYY_NO_POP_STATE


LDADD = $(LIBS_WITH_RPATH) $(ROSE_LIBS) 

bin_PROGRAMS = parseOmp

parseOmp_SOURCES = parseOmp.C \
                  OmpAttribute.C \
                  omplexer.ll \
                  ompparser.yy

EXTRA_DIST = README OmpAttribute.h omplexer.ll ompparser.yy ompparser.hh omplexer.cc ompparser.h ompparser.cc

clean-local:
	rm -rf parseOmp rose_* *.dot 


