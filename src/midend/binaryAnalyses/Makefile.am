include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

INCLUDES = -Winvalid-pch $(ROSE_INCLUDES) -I$(srcdir)/graph -I$(srcdir)/libraryIdentification 

noinst_LTLIBRARIES = libbinaryMidend.la

# Isolated part of library that is independent of the mysql database
libbinaryMidend_la_DEPENDENCIES =

sourse_files = \
     RoseBin_CompareAnalysis.cpp RoseBin_ControlFlowAnalysis.cpp \
     RoseBin_printInstructions.cpp RoseBin_DataFlowAnalysis.cpp \
     RoseBin_CallGraphAnalysis.cpp RoseBin_FlowAnalysis.cpp \
     dataflowanalyses/RoseBin_DefUseAnalysis.cpp \
     dataflowanalyses/RoseBin_VariableAnalysis.cpp \
     dataflowanalyses/InterruptAnalysis.cpp \
     dataflowanalyses/RoseBin_Emulate.cpp \
     dataflowanalyses/RoseBin_DataFlowAbstract.cpp GraphAlgorithms.C

if ROSE_USE_SQLITE_DATABASE
database_sourse_files = \
     libraryIdentification/libraryIdentification_reader.C \
     libraryIdentification/libraryIdentification_writer.C \
     libraryIdentification/functionIdentification.C 
else
database_sourse_files =
endif

# DQ (5/2/2009): Added support for configure options
if ROSE_USE_NEW_GRAPH_NODES
#if ROSE_USING_GRAPH_IR_NODES_FOR_BACKWARD_COMPATABILITY
# Using the new Graph IR nodes and the optional backward compatability API.
libbinaryMidend_la_SOURCES = $(sourse_files) $(database_sourse_files)
#else
# The current code will not compile with the new Graph IR node interface.
#libbinaryMidend_la_SOURCES =
#endif
else
# This is the original case (not using the new Graph IR nodes)
libbinaryMidend_la_SOURCES = $(sourse_files) $(database_sourse_files)
endif

BUILT_SOURCES = 

CLEANFILES = $(BUILT_SOURCES)

BINARY_DISASSEMBLY_HEADER_FILES = \
   RoseBin_CompareAnalysis.h RoseBin_ControlFlowAnalysis.h RoseBin_printInstructions.h \
   RoseBin_DataFlowAnalysis.h \
   RoseBin_FlowAnalysis.h RoseBin_CallGraphAnalysis.h \
   dataflowanalyses/RoseBin_DefUseAnalysis.h \
   dataflowanalyses/RoseBin_VariableAnalysis.h \
   dataflowanalyses/InterruptAnalysis.h \
   dataflowanalyses/RoseBin_Emulate.h \
   dataflowanalyses/RoseBin_DataFlowAbstract.h \
   libraryIdentification/libraryIdentification.h \
   libraryIdentification/functionIdentification.h \
   instructionSemantics/semanticsModule.h \
   instructionSemantics/x86InstructionSemantics.h GraphAlgorithms.h


include_HEADERS = $(BINARY_DISASSEMBLY_HEADER_FILES)

EXTRA_DIST = 


clean-local:

print:
	echo "libbinaryMidend_la_SOURCES = $(libbinaryMidend_la_SOURCES)"
