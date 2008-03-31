include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

if ROSE_USE_BINARY_ANALYSIS_SUPPORT

INCLUDES = $(ROSE_INCLUDES) -I$(srcdir)/analyses -I$(srcdir)/graph #-I$(srcdir)/mysql_include

# binarylibdir = $(libdir)

# DQ (1/25/2008): I think we need to use DEV_LIB here
# binarylib_LTLIBRARIES = libbinaryDatabase.la libbinaryFrontend.la libbinarydisassembly.la
# TPS (03/13/08) added ROSE_USE_BINARY_SQL, so that binary analysis can be compiled without MySQL
if DEV_LIBS
lib_LTLIBRARIES = libbinaryDatabase.la libbinaryFrontend.la #libbinarydisassembly.la
else
# This will cause the dynamic library to be built (as libroseObjdumpDisassembler.so)
noinst_LTLIBRARIES = libbinaryFrontend.la #libbinarydisassembly.la
if ROSE_USE_BINARY_SQL
  lib_LTLIBRARIES = libbinaryDatabase.la 
endif
endif

if ROSE_USE_BINARY_SQL
# Isolated part of library that depends on the mysql database
libbinaryDatabase_la_DEPENDENCIES =
libbinaryDatabase_la_SOURCES = RoseBin.cpp  RoseBin_DB.cpp  RoseBin_DB_IDAPRO.cpp 
endif 

# Isolated part of library that is independent of the mysql database
libbinaryFrontend_la_DEPENDENCIES =
libbinaryFrontend_la_SOURCES = RoseBin_support.cpp  RoseBin_unparse.cpp \
     instructionDispatch.cpp \
     RoseBin_unparse_visitor.cpp analyses/RoseBin_CompareAnalysis.cpp analyses/RoseBin_ControlFlowAnalysis.cpp \
     analyses/RoseBin_printInstructions.cpp analyses/RoseBin_DataFlowAnalysis.cpp \
     graph/RoseBin_DotGraph.cpp graph/RoseBin_Graph.cpp graph/RoseBin_GmlGraph.cpp \
     analyses/RoseBin_CallGraphAnalysis.cpp analyses/RoseBin_FlowAnalysis.cpp \
     analyses/dataflowanalyses/RoseBin_DefUseAnalysis.cpp \
     analyses/dataflowanalyses/RoseBin_VariableAnalysis.cpp \
     analyses/dataflowanalyses/RoseBin_Emulate.cpp \
     analyses/dataflowanalyses/RoseBin_DataFlowAbstract.cpp readTicl.C RoseBin_file.cpp RoseFile.cpp \
     RoseBin_buildTree.cpp RoseBin_IDAPRO_buildTree.cpp


# Original combined library (as originally defined by Thomas)
#libbinarydisassembly_la_DEPENDENCIES =
#libbinarydisassembly_la_SOURCES = \
#     RoseBin_buildTree.cpp  RoseBin.cpp  RoseBin_DB.cpp  RoseBin_DB_IDAPRO.cpp \
#     RoseBin_IDAPRO_buildTree.cpp  RoseBin_support.cpp  RoseBin_unparse.cpp \
#     RoseBin_unparse_visitor.cpp analyses/RoseBin_CompareAnalysis.cpp analyses/RoseBin_ControlFlowAnalysis.cpp \
#     analyses/RoseBin_printInstructions.cpp analyses/RoseBin_DataFlowAnalysis.cpp \
#     graph/RoseBin_DotGraph.cpp graph/RoseBin_Graph.cpp graph/RoseBin_GmlGraph.cpp \
#     analyses/RoseBin_CallGraphAnalysis.cpp analyses/RoseBin_FlowAnalysis.cpp \
#     analyses/dataflowanalyses/RoseBin_DefUseAnalysis.cpp \
#     analyses/dataflowanalyses/RoseBin_VariableAnalysis.cpp \
#     analyses/dataflowanalyses/RoseBin_Emulate.cpp \
#     analyses/dataflowanalyses/RoseBin_DataFlowAbstract.cpp

endif

# Make sure these SAGE specific header files are placed into a distribution
if ROSE_USE_BINARY_SQL
BINARY_DISASSEMBLY_HEADER_FILES = \
   RoseBin_buildTree.h  RoseBin_DB.h  RoseBin_DB_IDAPRO.h \
   RoseBin.h  RoseBin_IDAPRO_branchGraph.h  RoseBin_IDAPRO_buildTree.h \
   RoseBin_IDAPRO_callGraph.h  RoseBin_IDAPRO_exprTree.h  RoseBin_IDAPRO_substTree.h \
   RoseBin_support.h  RoseBin_unparse.h  RoseBin_unparse_visitor.h MyAstAttribute.h \
   analyses/RoseBin_CompareAnalysis.h analyses/RoseBin_ControlFlowAnalysis.h analyses/RoseBin_printInstructions.h \
   analyses/RoseBin_DataFlowAnalysis.h graph/RoseBin_DotGraph.h graph/RoseBin_Graph.h \
   graph/RoseBin_GmlGraph.h analyses/RoseBin_FlowAnalysis.h analyses/RoseBin_CallGraphAnalysis.h \
   analyses/dataflowanalyses/RoseBin_DefUseAnalysis.h \
   analyses/dataflowanalyses/RoseBin_VariableAnalysis.h \
   analyses/dataflowanalyses/RoseBin_Emulate.h \
   analyses/dataflowanalyses/RoseBin_DataFlowAbstract.h AST_BIN_Traversal.h readTicl.h RoseBin_file.h RoseFile.h \
   RoseBin_abstract.h
else
BINARY_DISASSEMBLY_HEADER_FILES = \
   RoseBin_IDAPRO_branchGraph.h \
   RoseBin_IDAPRO_callGraph.h  RoseBin_IDAPRO_exprTree.h  RoseBin_IDAPRO_substTree.h \
   RoseBin_support.h  RoseBin_unparse.h  RoseBin_unparse_visitor.h MyAstAttribute.h \
   analyses/RoseBin_CompareAnalysis.h analyses/RoseBin_ControlFlowAnalysis.h analyses/RoseBin_printInstructions.h \
   analyses/RoseBin_DataFlowAnalysis.h graph/RoseBin_DotGraph.h graph/RoseBin_Graph.h \
   graph/RoseBin_GmlGraph.h analyses/RoseBin_FlowAnalysis.h analyses/RoseBin_CallGraphAnalysis.h \
   analyses/dataflowanalyses/RoseBin_DefUseAnalysis.h \
   analyses/dataflowanalyses/RoseBin_VariableAnalysis.h \
   analyses/dataflowanalyses/RoseBin_Emulate.h \
   analyses/dataflowanalyses/RoseBin_DataFlowAbstract.h AST_BIN_Traversal.h readTicl.h RoseBin_file.h RoseFile.h \
   RoseBin_abstract.h
endif
# DQ (1/19/2008): Removed this file from BINARY_DISASSEMBLY_HEADER_FILES since it is not available
# colorTraversal.h  

include_HEADERS = $(BINARY_DISASSEMBLY_HEADER_FILES)

EXTRA_DIST = mysql_include make-decision-tree.tcl RoseBin_instruction_table_arm RoseBin_instruction_table_x86

# Rule for maintainers (requires Tcl interpreter)
$(srcdir)/instructionDispatch.cpp: make-decision-tree.tcl RoseBin_instruction_table_arm RoseBin_instruction_table_x86
	cd $(srcdir); $(top_srcdir)/config/missing --run tclsh ./make-decision-tree.tcl x86 Arm


clean-local:
