# This is a generated Autoconf/Automake based workspace for using ROSE.

# List subdirectories of the ROSE workspace here (if any).
SUBDIRS = 

AM_CXXFLAGS = $(ROSE_CPPFLAGS) $(BOOST_CPPFLAGS)

INCLUDES = -I@prefix@/include $(ROSE_INCLUDES)

bin_PROGRAMS = exampleTranslator
exampleTranslator_SOURCES = exampleTranslator.C

if OS_MACOSX
  library_extension=dylib
else
  library_extension=so
endif

# Specify the libraries required to link a ROSE application 
# using rpath so that we don't have to set LD_LIBRARY_PATH.
AM_LDFLAGS=-Wl,-rpath -Wl,@prefix@/lib @prefix@/lib/librose.$(library_extension)

# Make rule to automate testing using Automake's "check" rule
# Use the "--help" option to avoid an error.
check-local:
	exampleTranslator --help || (echo "You cannot run a program linked against ROSE.  Please be sure that your LD_LIBRARY_PATH (Linux) or DYLD_LIBRARY_PATH (Mac OSX) contains the correct libraries (JVM, Boost, librose.so, etc.)." >&2; exit 1)


