include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs
include $(top_srcdir)/projects/compass/src/compassSupport/compass_dirs.inc

#							-*- Makefile -*-

EXTRA_DIST	= sage3.i sgname.i wchart.i test.java input.cc	\
		  GenericVector.java testjava.sh		\
		  generate_casts.sh Ptr.java Log.java           \
		  MakeSgNode.java std_list.i frontendAndDump.cc \
		  README testlink.cc


if ENABLE_JAVAPORT

# GMY Javaport links to libCompass (sometimes --with-mpi) needs to find
# MPI symbols 
if ROSE_MPI
#LTCXXCOMPILE = \
#        $(LIBTOOL) --tag=CXX --mode=compile $(MPICXX) $(DEFS) \
#        $(DEFAULT_INCLUDES) $(INCLUDES) $(AM_CPPFLAGS) $(CPPFLAGS) \
#        $(AM_CXXFLAGS) $(CXXFLAGS)
#CXXCOMPILE = \
#        $(MPICXX) $(DEFS) \
#        $(DEFAULT_INCLUDES) $(INCLUDES) $(AM_CPPFLAGS) $(CPPFLAGS) \
#        $(AM_CXXFLAGS) $(CXXFLAGS)
#CXXLINK = $(LIBTOOL) --tag=CXX --mode=link $(MPICXX) $(AM_CXXFLAGS) \
#        $(CXXFLAGS) $(AM_LDFLAGS) $(LDFLAGS) -o $@
endif


# To keep compile commands tidy, please ONLY put required include flags here.
JAVA_INCLUDES = -I$(JAVA_PATH)/include -I$(JAVA_PATH)/include/linux
if ROSE_MPI
INCLUDES = $(ROSE_INCLUDES) -I$(compass_support_dir) $(JAVA_INCLUDES) -DROSE_MPI  -I$(compass_prereqs_dir) -I$(compass_support_bdir)
else
INCLUDES = $(ROSE_INCLUDES) -I$(compass_support_dir) $(JAVA_INCLUDES)  -I$(compass_prereqs_dir) -I$(compass_support_bdir)
endif

#INCLUDES	=						\
#	-I.							\
#	-I$(top_srcdir)/src/frontend/SageIII			\
#	-I$(top_builddir)/src/frontend/SageIII			\
#	-I$(top_srcdir)/src/midend/astProcessing		\
#	-I$(top_srcdir)/src/3rdPartyLibraries/MSTL		\
#	-I$(top_srcdir)/src/frontend/SageIII/virtualCFG		\
#	-I$(top_srcdir)/src/util/stringSupport			\
#	-I$(top_srcdir)/src/roseSupport				\
#	-I$(top_srcdir)/src/util/commandlineProcessing		\
#	-I$(top_srcdir)/src/util				\
#	-I$(top_srcdir)/src					\
#	-I$(top_srcdir)/src/frontend/SageIII/astPostProcessing	\
#	-I$(top_srcdir)/src/frontend/SageIII/astFixup		\
#	-I$(top_srcdir)/src/frontend/SageIII/sageInterface	\
#	-I$(top_srcdir)/src/midend/astDiagnostics		\
#	-I$(top_srcdir)/src/backend/unparser			\
#	-I$(top_srcdir)/src/midend/astRewriteMechanism		\
#	-I$(top_srcdir)/src/midend/astQuery			\
#	-I$(top_srcdir)/src/frontend/SageIII/astMerge		\
#	-I$(top_builddir)/src/frontend/SageIII/astFileIO	\
#	-I$(top_srcdir)/src/midend/astInlining			\
#	-I$(top_srcdir)/src/midend/programTransformation/partialRedundancyElimination	\
#	-I$(top_srcdir)/src/midend/programTransformation/constantFolding	\
#	-I$(top_srcdir)/src/midend/programAnalysis/CFG		\
#	-I$(top_srcdir)/src/midend/astUtil/astInterface		\
#	-I$(top_srcdir)/src/util/support			

# This is just to get the environment for Java.
dummy_JAVA=
dummydir=$(datadir)/java

# This programs just make sure that no symbol are missing.
noinst_PROGRAMS = testlink
testlink_SOURCES = testlink.cc
testlink_LDADD = $(LIBS_WITH_RPATH) $(ROSE_SEPARATE_LIBS) libsage3_wrap.la

# testlink.o: libsage3_wrap.la

bin_PROGRAMS = frontendAndDump
frontendAndDump_SOURCES = frontendAndDump.cc
frontendAndDump_LDADD = $(LIBS_WITH_RPATH) $(ROSE_SEPARATE_LIBS)

javalib_DATA	= sage3.jar
javalibdir	= $(datadir)/java

lib_LTLIBRARIES	= libsage3_wrap.la 
nodist_libsage3_wrap_la_SOURCES	= sage3_wrap.cxx
#libsage3_wrap_la_SOURCES	= sage3_wrap.cxx
libsage3_wrap_la_CPPFLAGS	= $(INCLUDES)
libsage3_wrap_la_LIBADD		= -lrose $(RT_LIBS) -lCompass -lCompassCheckers
libsage3_wrap_la_LDFLAGS	= -L../../src -L$(compass_build_tooldir)/compass -L$(compass_build_srcdir)/compassSupport

sage3_wrap.cxx:	sage3.i sgname.i wchart.i casts.i Cxx_Grammar.i
#		$(top_builddir)/src/frontend/SageIII/Cxx_Grammar.h 
	mkdir -p gov/llnl/casc/rose
	$(SWIG) `echo $(INCLUDES) | sed 's/ -pthread / /g' | sed 's/ -Winvalid-pch / /g'  | sed 's/ -fPIC / /g'` \
	  -c++ -java -Wall -v -I$(srcdir) \
	  -package gov.llnl.casc.rose -outdir gov/llnl/casc/rose \
	  -o sage3_wrap.cxx \
	  $(srcdir)/sage3.i

casts.i: $(srcdir)/generate_casts.sh
	$(srcdir)/generate_casts.sh

# gov/llnl/casc/rose/GenericVector.java: GenericVector.java
# 	mkdir -p gov/llnl/casc/rose
# 	ln -sf ../../../../$< $@
# 
# gov/llnl/casc/rose/Ptr.java: Ptr.java
# 	cp $< $@
# 
# gov/llnl/casc/rose/Log.java: Log.java
# 	cp $< $@
# 
# gov/llnl/casc/rose/MakeSgNode.java: MakeSgNode.java
# 	cp $< $@

compile_java_files: sage3_wrap.cxx \
                    GenericVector.java \
                    Ptr.java \
                    Log.java \
                    MakeSgNode.java
	mkdir -p class/gov/llnl/casc/rose
	rm -f class/gov/llnl/casc/rose/*.class
	$(CLASSPATH_ENV) $(JAVAC) $(AM_JAVACFLAGS) $(JAVACFLAGS) \
	  gov/llnl/casc/rose/*.java \
	  $(srcdir)/GenericVector.java \
	  $(srcdir)/Log.java \
	  $(srcdir)/MakeSgNode.java \
	  $(srcdir)/Ptr.java \
	  -d class
	touch compile_java_files

sage3.jar: compile_java_files
	$(JAR) cf $@ -C class .

# TESTS=testjava.sh

check-local: test

test: test.class sage3.jar .libs/libsage3_wrap.so input.cc
	env LD_LIBRARY_PATH="../.libs:.libs:${LD_LIBRARY_PATH}" \
	  $(JAVA) -classpath sage3.jar:. test $(srcdir)/input.cc -o /dev/null

test.class: test.java sage3.jar
	$(JAVAC) -classpath sage3.jar:. $(srcdir)/test.java -d .

Cxx_Grammar.i: ../../src/frontend/SageIII/Cxx_Grammar.h
	echo '// vim: syn=cpp:' > Cxx_Grammar.i
	cat ../../src/frontend/SageIII/Cxx_Grammar.h | \
	sed '/StorageClass/d' | \
	sed '/InMemoryPool/d' | \
	sed '/static void traverseMemoryPool/d' | \
	sed '/executeVisitorMemberFunction/d' | \
	sed '/postdeclarations for SgAsmNode/,$$s!^[ 	]*virtual [^b][^;0]*;$$!//&!' | \
	sed '/postdeclarations for SgAsmNode/,$$s!virtual RTIReturnType roseRTI!//&!' | \
	sed '/postdeclarations for SgAsmNode/,$$s!virtual VariantT variantT!//&!' | \
	sed '/^#line/d' | \
	sed '/^ *$$/d' | \
	sed '/SgInitializedName($$/,/)/d' | \
	sed '/ROSE_DEPRECATED/d' | \
	sed '/visitRepresentativeNode/d' | \
	sed '/numberOfNodes/d' | \
	sed '/memoryUsage/d' | \
	sed '/ friend /d' | \
	sed '/operator new/d' | \
	sed '/ operator delete/d' | \
	sed '/post_construction_initialization/d' | \
	sed '/Cxx_GrammarTerminalNames/d' | \
	sed '/isSg/d' | \
	sed '/p_[_A-Za-z0-9]*;$$/d' | \
	sed '/<semaphore.h>/,/};/d' | \
	sed '/namespace AST_FileIO/,/}/d' | \
	sed 's/ class / struct /g' | \
	sed 's/__attribute__(([^)]*))//g' | \
	cat >> Cxx_Grammar.i

CLEANFILES = sage3_wrap.cxx sage3_wrap.h compile_java_files sage3.jar \
	     rose_input.cc casts.i Cxx_Grammar.i

clean-local:
	rm -rf gov class

else

# Still export source files.
EXTRA_DIST += testlink.cc frontendAndDump.cc

endif
