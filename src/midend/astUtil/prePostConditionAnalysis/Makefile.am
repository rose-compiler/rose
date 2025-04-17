# Include the top-level ROSE configuration
include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

# Define flags for OpenMP if used
if ROSE_USE_GCC_OMP
AM_CPPFLAGS_OMP = -DROSE_GCC_OMP 
endif

# Define include paths for this subdirectory
AM_CPPFLAGS =  $(ROSE_INCLUDES) \
   -I$(srcdir)/.. -I$(ROSE_SOURCE)/src/midend/astUtil/astSupport \
   -I$(BOOST_INCLUDEDIR) \
   -I./src
   $(AM_CPPFLAGS_OMP)

# Define the library to be built
noinst_LTLIBRARIES = libPrePostConditionAnalysis.la

# Source files for the library
libPrePostConditionAnalysis_la_SOURCES = src/PrePostConditionAnalysis.C \
                                src/PrePostCondition.C

# Define the executable to be built
bin_PROGRAMS = PrePostConditionDriver

# Source files for the executable
PrePostConditionDriver_SOURCES = src/PrePostConditionDriver.C

# Link the executable with the library and other dependencies
PrePostConditionDriver_LDADD = libPrePostConditionAnalysis.la $(ROSE_LIBS) $(BOOST_LIBS)

# Install headers
pkginclude_HEADERS = src/PrePostConditionAnalysis.h \
                     src/PrePostCondition.h

# Clean up build artifacts
clean-local:
	rm -rf ./Templates.DB ./ii_files ./ti_files ./cxx_templates

distclean-local:
	rm -rf ./Templates.DB

EXTRA_DIST = CMakeLists.txt
