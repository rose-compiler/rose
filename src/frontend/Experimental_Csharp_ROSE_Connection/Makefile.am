include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

AM_CPPFLAGS = -std=c++11 $(ROSE_INCLUDES) -I$(MONO_INCLUDE) -I$(top_srcdir)/src/frontend/Experimental_Csharp_ROSE_Connection

noinst_LTLIBRARIES = libexperimentalRoseCsharp.la
pkglib_LTLIBRARIES = libcsharpBuilder.la

# DQ (8/23/2017): I'm not clear if we need makefile conditionals, since the directory is already conditionally processed in the parent directory.

if ROSE_EXPERIMENTAL_CSHARP_ROSE_CONNECTION
# libexperimentalRoseCsharp_la_SOURCES = csharp_support.C csharp_support_lib.C
  libexperimentalRoseCsharp_la_SOURCES = csharp_support.C

# DQ (9/12/2017): These dependences cause librose to always be rebuilt, which should not be required and is annoying.
# libexperimentalRoseCsharp_la_DEPENDENCIES = build_dynamic_library build_csharp_entry_point
# libexperimentalRoseCsharp_la_DEPENDENCIES = csharp_support_lib.so build_csharp_entry_point
#~   libexperimentalRoseCsharp_la_DEPENDENCIES = 
  libexperimentalRoseCsharp_la_DEPENDENCIES = build_csharp_entry_point

  libcsharpBuilder_la_SOURCES = \
    csharpBuilder.C \
    csharpBuilder_wrap.C \
    csharp_support_lib.C \
    ast2dot.C

  libcsharpBuilder_la_LDFLAGS = -module -avoid-version -shared $(BOOST_LDFLAGS) $(BOOST_THREAD_LIB) -L$(MONO_LIBS) -lmono-2.0 -lpthread -ldl -lrt `pkg-config --cflags --libs mono-2`
#~   libroseCsharpBuilder_la_DEPENDENCIES = csharpBuilder.so
else
# Compile using a function that represents the API of the Csharp support.
  libexperimentalRoseCsharp_la_SOURCES = dummy_csharp_main.C
endif

pkginclude_HEADERS = csharp_support.h

MONO_HOME   = @mono_home@
ROSLYN_HOME = @roslyn_home@

MONO_LIBS   = $(MONO_HOME)/lib
MONO_INCLUDE = $(MONO_HOME)/include/mono-2.0 

print:
	echo "MONO_HOME    = $(MONO_HOME)"
	echo "MONO_INCLUDE = $(MONO_INCLUDE)"
	echo "MONO_LIBS    = $(MONO_LIBS)"
	echo "ROSLYN_HOME  = $(ROSLYN_HOME)"

# DQ (8/24/2017): Does this require C++11?
# all:
#	$(CXX) -std=c++11 -Wall -fPIC -O2 -I$(MONO_INCLUDE) -c $(srcdir)/csharp_support_lib.C `pkg-config -cflags -libs mono-2`
#	$(CXX) -std=c++11 -Wl,--no-undefined -shared -o csharp_support_lib.so csharp_support_lib.o -L$(MONO_LIBS) -lmono-2.0 -lpthread -ldl -lrt `pkg-config -cflags -libs mono-2`
#	$(CXX) -std=c++11 -I$(MONO_INCLUDE) $(ROSE_INCLUDES) $(srcdir)/csharp_support.C -L$(MONO_LIBS) -lmono-2.0 -lpthread -ldl -lrt
#	$(CXX) -std=c++11 -I$(MONO_INCLUDE) -I$(top_srcdir)/src/util -I$(top_srcdir)/src/frontend/SageIII $(srcdir)/csharp_support.C -L$(MONO_LIBS) -lmono-2.0 -lpthread -ldl -lrt
#	$(CXX) -std=c++11 -I$(MONO_INCLUDE) $(srcdir)/csharp_support.C -L$(MONO_LIBS) -lmono-2.0 -lpthread -ldl -lrt
#	$(CXX) -std=c++11 -I$(MONO_INCLUDE) -I$(top_srcdir)/src/util -I$(top_srcdir)/src/frontend/SageIII $(srcdir)/csharp_support.C -L$(MONO_LIBS) -lmono-2.0 -lpthread -ldl -lrt
#	mcs /target:library $(srcdir)/csharp_main.cs /reference:$(srcdir)/RGiesecke.DllExport.Metadata.dll
#	mcs /target:library $(srcdir)/csharp_main.cs /reference:/nfs/casc/overture/ROSE/opt/rhel7/x86_64/csharp/libs/RGiesecke.DllExport.Metadata.dll /reference:/nfs/casc/overture/ROSE/opt/rhel7/x86_64/csharp/libs/Microsoft.CodeAnalysis.Analyzers.dll /reference:/nfs/casc/overture/ROSE/opt/rhel7/x86_64/csharp/libs/Microsoft.CodeAnalysis.CSharp.Analyzers.dll /reference:/nfs/casc/overture/ROSE/opt/rhel7/x86_64/csharp/libs/Microsoft.CodeAnalysis.CSharp.Workspaces.dll /reference:/nfs/casc/overture/ROSE/opt/rhel7/x86_64/csharp/libs/Microsoft.CodeAnalysis.CSharp.dll /reference:/nfs/casc/overture/ROSE/opt/rhel7/x86_64/csharp/libs/Microsoft.CodeAnalysis.VisualBasic.Workspaces.dll /reference:/nfs/casc/overture/ROSE/opt/rhel7/x86_64/csharp/libs/Microsoft.CodeAnalysis.VisualBasic.dll /reference:/nfs/casc/overture/ROSE/opt/rhel7/x86_64/csharp/libs/Microsoft.CodeAnalysis.Workspaces.dll /reference:/nfs/casc/overture/ROSE/opt/rhel7/x86_64/csharp/libs/Microsoft.CodeAnalysis.dll


#~ csharpB%.o: $(srcdir)/csharpB%.C
#~ 	$(CXX) -std=c++11 -Wall -fPIC -O2 $(ROSE_INCLUDES) -c $< -o $@

#~ -I$(top_srcdir)/src/util -I$(top_srcdir)/src/frontend/SageIII 	
# build_dynamic_library: $(srcdir)/csharp_support_lib.C
# csharp_support_lib.so: $(srcdir)/csharp_support_lib.C $(srcdir)/csharp_support.h $(srcdir)/src/util/rose_paths.C
#~ csharpB%.so: csharpBuilder.o csharpBuilder_wrap.o
#~ 	$(CXX) -std=c++11 -Wl,--no-undefined -shared -o $@ $^ -lpthread -ldl -lrt 


#~ csharp_support_lib.so: $(srcdir)/csharp_support_lib.C $(srcdir)/csharp_support.h
#	$(CXX) -std=c++11 -Wall -fPIC -O2 -I$(MONO_INCLUDE) -I$(top_srcdir)/src/util -I$(top_srcdir)/src/frontend/SageIII -c $(srcdir)/csharp_support_lib.C `pkg-config -cflags -libs mono-2`
#	$(CXX) -std=c++11 -Wl,--no-undefined -shared -o csharp_support_lib.so csharp_support_lib.o -L$(MONO_LIBS) -lmono-2.0 -lpthread -ldl -lrt `pkg-config -cflags -libs mono-2`
#~ 	$(CXX) -std=c++11 -Wall -fPIC -O2 -I$(MONO_INCLUDE) -I$(top_srcdir)/src/util -I$(top_srcdir)/src/frontend/SageIII -c $(srcdir)/csharp_support_lib.C `pkg-config --cflags --libs mono-2`
#	$(CXX) -std=c++11 -Wl,--no-undefined -shared -o csharp_support_lib.so csharp_support_lib.o -L$(MONO_LIBS) -lmono-2.0 -lpthread -ldl -lrt `pkg-config --cflags --libs mono-2`
#	$(CXX) -std=c++11 -Wall -fPIC -I$(top_srcdir)/src/util -I$(top_srcdir)/src/frontend/SageIII -c $(builddir)/../../util/rose_paths.C
#	$(CXX) -std=c++11 -Wl,--no-undefined -shared -o csharp_support_lib.so csharp_support_lib.o rose_paths.o -L$(MONO_LIBS) -lmono-2.0 -lpthread -ldl -lrt `pkg-config --cflags --libs mono-2`
#~ 	$(CXX) -std=c++11 -Wl,--no-undefined -shared -o csharp_support_lib.so csharp_support_lib.o -L$(MONO_LIBS) -lmono-2.0 -lpthread -ldl -lrt `pkg-config --cflags --libs mono-2`

build_csharp_entry_point: $(srcdir)/csharp_main.cs
#	mcs /target:library $(srcdir)/csharp_main.cs -o$(buildir)/csharp_main.dll /reference:$(srcdir)/RGiesecke.DllExport.Metadata.dll
#	mcs /target:library $(srcdir)/csharp_main.cs /reference:$(srcdir)/RGiesecke.DllExport.Metadata.dll
#	mcs /target:library $(srcdir)/csharp_main.cs /reference:/nfs/casc/overture/ROSE/opt/rhel7/x86_64/csharp/libs/RGiesecke.DllExport.Metadata.dll /reference:/nfs/casc/overture/ROSE/opt/rhel7/x86_64/csharp/libs/Microsoft.CodeAnalysis.Analyzers.dll /reference:/nfs/casc/overture/ROSE/opt/rhel7/x86_64/csharp/libs/Microsoft.CodeAnalysis.CSharp.Analyzers.dll /reference:/nfs/casc/overture/ROSE/opt/rhel7/x86_64/csharp/libs/Microsoft.CodeAnalysis.CSharp.Workspaces.dll /reference:/nfs/casc/overture/ROSE/opt/rhel7/x86_64/csharp/libs/Microsoft.CodeAnalysis.CSharp.dll /reference:/nfs/casc/overture/ROSE/opt/rhel7/x86_64/csharp/libs/Microsoft.CodeAnalysis.VisualBasic.Workspaces.dll /reference:/nfs/casc/overture/ROSE/opt/rhel7/x86_64/csharp/libs/Microsoft.CodeAnalysis.VisualBasic.dll /reference:/nfs/casc/overture/ROSE/opt/rhel7/x86_64/csharp/libs/Microsoft.CodeAnalysis.Workspaces.dll /reference:/nfs/casc/overture/ROSE/opt/rhel7/x86_64/csharp/libs/Microsoft.CodeAnalysis.dll
#	/reference:$(srcdir)/RGiesecke.DllExport.Metadata.dll
#	mcs /target:library $(srcdir)/csharp_main.cs /reference:/nfs/casc/overture/ROSE/opt/rhel7/x86_64/csharp/libs/RGiesecke.DllExport.Metadata.dll /reference:/nfs/casc/overture/ROSE/opt/rhel7/x86_64/csharp/libs/Microsoft.CodeAnalysis.Analyzers.dll /reference:/nfs/casc/overture/ROSE/opt/rhel7/x86_64/csharp/libs/Microsoft.CodeAnalysis.CSharp.Analyzers.dll /reference:/nfs/casc/overture/ROSE/opt/rhel7/x86_64/csharp/libs/Microsoft.CodeAnalysis.CSharp.Workspaces.dll /reference:/nfs/casc/overture/ROSE/opt/rhel7/x86_64/csharp/libs/Microsoft.CodeAnalysis.CSharp.dll /reference:/nfs/casc/overture/ROSE/opt/rhel7/x86_64/csharp/libs/Microsoft.CodeAnalysis.VisualBasic.Workspaces.dll /reference:/nfs/casc/overture/ROSE/opt/rhel7/x86_64/csharp/libs/Microsoft.CodeAnalysis.VisualBasic.dll /reference:/nfs/casc/overture/ROSE/opt/rhel7/x86_64/csharp/libs/Microsoft.CodeAnalysis.Workspaces.dll /reference:/nfs/casc/overture/ROSE/opt/rhel7/x86_64/csharp/libs/Microsoft.CodeAnalysis.dll
	mcs /target:library /nowarn:CS1685 /reference:Threading=/nfs/casc/overture/ROSE/opt/rhel7/x86_64/csharp/libs/System.Threading.Tasks.dll  /reference:/nfs/casc/overture/ROSE/opt/rhel7/x86_64/csharp/libs/Microsoft.CodeAnalysis.Analyzers.dll /reference:/nfs/casc/overture/ROSE/opt/rhel7/x86_64/csharp/libs/Microsoft.CodeAnalysis.CSharp.Analyzers.dll /reference:/nfs/casc/overture/ROSE/opt/rhel7/x86_64/csharp/libs/Microsoft.CodeAnalysis.CSharp.Workspaces.dll /reference:/nfs/casc/overture/ROSE/opt/rhel7/x86_64/csharp/libs/Microsoft.CodeAnalysis.CSharp.dll /reference:/nfs/casc/overture/ROSE/opt/rhel7/x86_64/csharp/libs/Microsoft.CodeAnalysis.VisualBasic.Workspaces.dll /reference:/nfs/casc/overture/ROSE/opt/rhel7/x86_64/csharp/libs/Microsoft.CodeAnalysis.VisualBasic.dll /reference:/nfs/casc/overture/ROSE/opt/rhel7/x86_64/csharp/libs/Microsoft.CodeAnalysis.Workspaces.dll /reference:/nfs/casc/overture/ROSE/opt/rhel7/x86_64/csharp/libs/Microsoft.CodeAnalysis.dll /reference:/nfs/casc/overture/ROSE/opt/rhel7/x86_64/csharp/libs/RGiesecke.DllExport.Metadata.dll /reference:/nfs/casc/overture/ROSE/opt/rhel7/x86_64/csharp/libs/System.Reflection.dll /reference:Runtime=/nfs/casc/overture/ROSE/opt/rhel7/x86_64/csharp/libs/System.Runtime.dll /reference:/nfs/casc/overture/ROSE/opt/rhel7/x86_64/csharp/libs/System.Reflection.dll /reference:/nfs/casc/overture/ROSE/opt/rhel7/x86_64/csharp/libs/System.Collections.Immutable.dll /reference:/nfs/casc/overture/ROSE/opt/rhel7/x86_64/csharp/libs/System.Text.Encoding.dll $(srcdir)/csharp_main.cs $(srcdir)/SeqKind.cs $(srcdir)/csharpBuilderPINVOKE.cs $(srcdir)/csharpBuilder.cs $(srcdir)/astBuilder.cs $(srcdir)/SWIGTYPE_p_namespace.cs
	echo "Copy $(srcdir)/csharp_main.dll to $(builddir)"
	cp $(srcdir)/csharp_main.dll $(builddir)

## $(srcdir)/parseTreeTraversal.cs $(srcdir)/abstractSyntaxTreeTraversal.cs $(srcdir)/buildDotGraph.cs 

# build_executable: build_dynamic_library build_csharp_entry_point
build_executable: build_csharp_entry_point $(srcdir)/csharp_support.C $(srcdir)/ast2dot.C
#	$(CXX) -std=c++11 -DBUILD_EXECUTABLE -I$(MONO_INCLUDE) $(srcdir)/csharp_support.C -L$(MONO_LIBS) -lmono-2.0 -lpthread -ldl -lrt
#	$(CXX) -std=c++11 -DBUILD_EXECUTABLE -I$(MONO_INCLUDE) -I$(top_srcdir)/src/util -I$(top_srcdir)/src/frontend/SageIII $(srcdir)/csharp_support.C -L$(MONO_LIBS) -lmono-2.0 -lpthread -ldl -lrt
	$(CXX) -std=c++11 -g -DBUILD_EXECUTABLE -I$(MONO_INCLUDE) -I$(top_srcdir)/src/util -I$(top_srcdir)/src/frontend/SageIII $(srcdir)/csharp_support.C -L$(MONO_LIBS) -lmono-2.0 -lpthread -ldl -lrt

# DQ (9/22/2017): This is the compilation command that works for the example using roslyn from sandbox/roslyn/llnl
roslyn:
	/nfs/casc/overture/ROSE/opt/rhel7/x86_64/mono/62458f4/installation/bin/csc /noconfig /unsafe- /checked- /nowarn:1701,1702,1705 /nostdlib+ /errorreport:prompt /warn:4 "/define:TRACE;DEBUG;NETCOREAPP1_1" `roslyn-config libs` /debug+ /debug:portable /filealign:512 /nologo /optimize- /out:bin/main.exe /target:exe /warnaserror- /utf8output /deterministic+ `roslyn-config analyzers` src/Program.cs

OLD_all:
	g++ -std=c++11 -Wall -fPIC -O2 -I$(MONO_HOME) -c Kazelib.cpp Kazelib.h `pkg-config -cflags -libs mono-2`
	g++ -std=c++11 -Wl,--no-undefined -shared -o Kazelib.so Kazelib.o -L$(MONO_LIBS) -lmono-2.0 -lpthread -ldl -lrt `pkg-config -cflags -libs mono-2`
	g++ -std=c++11 -Wl,--no-undefined -I$(MONO_HOME) main.cpp -L$(MONO_LIBS) -lmono-2.0 -lpthread -ldl -lrt
	mcs /target:library test.cs /reference:RGiesecke.DllExport.Metadata.dll

test:
	./a.out


clean-local:
	rm -f *.so a.out *.dll
