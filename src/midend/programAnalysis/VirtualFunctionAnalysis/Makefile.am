include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

INCLUDES =  $(ROSE_INCLUDES) -I$(srcdir)/.. -I$(srcdir)/../CFG -I$(top_srcdir)/src/util -I$(top_srcdir)/src/util/support -I$(top_srcdir)/src/util/graphs -I$(top_srcdir)/src/midend/astUtil -I$(top_srcdir)/src/midend/astUtil/astSupport -I$(top_srcdir)/src/midend/astUtil/astInterface -I$(top_srcdir)/src/util/support 

## The grammar generator (ROSETTA) should use its own template repository
CXX_TEMPLATE_REPOSITORY_PATH = .

libVirtualFunctionAnalysisSources = IntraProcDataFlowAnalysis.C InterProcDataFlowAnalysis.C  IntraProcAliasAnalysis.C PtrAliasAnalysis.C VirtualFunctionAnalysis.C


noinst_LTLIBRARIES = libVirtualFunctionAnalysis.la
libVirtualFunctionAnalysis_la_SOURCES = $(libVirtualFunctionAnalysisSources)

clean-local:
	rm -rf Templates.DB ii_files ti_files cxx_templates

distclean-local:
	rm -rf Templates.DB

include_HEADERS = IntraProcDataFlowAnalysis.h InterProcDataFlowAnalysis.h IntraProcAliasAnalysis.h PtrAliasAnalysis.h VirtualFunctionAnalysis.h 




EXTRA_DIST = CMakeLists.txt
