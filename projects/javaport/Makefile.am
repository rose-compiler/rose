include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

EXTRA_DIST	= rose.i test.java input.cc GenericVector.java testjava.sh generate_casts.sh Ptr.java Log.java MakeSgNode.java std_list.i std_container.i std_set.i std_alloc.i frontendAndDump.cc README testlink.cc

if ENABLE_JAVAPORT

# I would like to not have this be removed when we kill SWIG.
.PRECIOUS: rose_wrap.cxx

# Make sure that no intermediate files are removed by make when SWIG is killed.
.SECONDARY:	

# DQ (3/6/2013): We need a version of the ROSE_INLCUDES paths that excludes any possability of including the -isystem option.
# To keep compile commands tidy, please ONLY put required include flags here.
JAVA_INCLUDES = -I$(JAVA_PATH)/include -I$(JAVA_PATH)/include/linux
if ROSE_MPI
# This case is not handled.
else
AM_CPPFLAGS = $(ROSE_INCLUDES) $(JAVA_INCLUDES)
endif

# This is just to get the environment for Java.
dummy_JAVA=
dummydir=$(datadir)/java

# This programs just make sure that no symbols are missing.
noinst_PROGRAMS = testlink
testlink_SOURCES = testlink.cc
testlink_LDADD = $(LIBS_WITH_RPATH) $(ROSE_SEPARATE_LIBS) librose_wrap.la

bin_PROGRAMS = frontendAndDump
frontendAndDump_SOURCES = frontendAndDump.cc
frontendAndDump_LDADD = $(LIBS_WITH_RPATH) $(ROSE_SEPARATE_LIBS)

javalib_DATA	= rose.jar
javalibdir	= $(datadir)/java

lib_LTLIBRARIES	= librose_wrap.la 
nodist_librose_wrap_la_SOURCES	= rose_wrap.cxx
librose_wrap_la_CPPFLAGS	= $(AM_CPPFLAGS)

librose_wrap_la_LIBADD		= -lrose $(RT_LIBS) 
librose_wrap_la_LDFLAGS	= -L../../src 

# SWIG_DEBUG_OPTIONS = 
SWIG_DEBUG_OPTIONS = -Wall -v -debug-classes

# DQ (3/10/2013): This will only suppress warnings if -Wall and/or -v are NOT used.
# Some of these are useful to track progress though large files (which takes a few minutes or could just hang).
# SWIG_OPTIONS = -w315,325,350,351,401,451,503,516,822
SWIG_OPTIONS = 

rose_wrap.cxx:	rose.i
	mkdir -p gov/llnl/casc/rose
	$(SWIG) `echo $(AM_CPPFLAGS) | sed 's/ -pthread / /g' | sed 's/ -Winvalid-pch / /g'  | sed 's/ -fPIC / /g'` \
	  -DUSE_ROSE -DROSE_USE_SWIG_SUPPORT \
	  -c++ -java $(SWIG_OPTIONS) $(SWIG_DEBUG_OPTIONS) -I$(srcdir) \
	  -package gov.llnl.casc.rose -outdir gov/llnl/casc/rose \
	  -o rose_wrap.cxx \
	  $(srcdir)/rose.i

casts.i: $(srcdir)/generate_casts.sh
	$(srcdir)/generate_casts.sh

compile_java_files: rose_wrap.cxx MakeSgNode.java
	mkdir -p class/gov/llnl/casc/rose
	rm -f class/gov/llnl/casc/rose/*.class
	$(CLASSPATH_ENV) $(JAVAC) $(AM_JAVACFLAGS) $(JAVACFLAGS) \
	  gov/llnl/casc/rose/*.java \
	  $(srcdir)/GenericVector.java \
	  $(srcdir)/Log.java \
	  $(srcdir)/MakeSgNode.java \
	  -d class
	touch compile_java_files

rose.jar: compile_java_files
	$(JAR) cf $@ -C class .

# TESTS=testjava.sh

check-local: test

test: test.class rose.jar .libs/librose_wrap.so input.cc
	env LD_LIBRARY_PATH="../.libs:.libs:${LD_LIBRARY_PATH}" \
	  $(JAVA) -classpath rose.jar:. test $(srcdir)/input.cc -o /dev/null

test.class: test.java rose.jar
	$(JAVAC) -classpath rose.jar:. $(srcdir)/test.java -d .

CLEANFILES = rose_wrap.cxx rose_wrap.h compile_java_files rose.jar rose_input.cc casts.i Cxx_Grammar.i

clean-local:
	rm -rf gov class
	rm -rf *.loT

else

# Put the source files into the distribution.
EXTRA_DIST += testlink.cc frontendAndDump.cc

endif
