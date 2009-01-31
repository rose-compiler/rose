include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

INCLUDES = $(ROSE_INCLUDES) -I$(srcdir)/graph -I$(srcdir)/libraryIdentification

noinst_LTLIBRARIES = libbinaryMidend.la

# Isolated part of library that is independent of the mysql database
libbinaryMidend_la_DEPENDENCIES =
libbinaryMidend_la_SOURCES =  RoseBin_CompareAnalysis.cpp RoseBin_ControlFlowAnalysis.cpp \
     RoseBin_printInstructions.cpp RoseBin_DataFlowAnalysis.cpp \
     RoseBin_CallGraphAnalysis.cpp RoseBin_FlowAnalysis.cpp \
     dataflowanalyses/RoseBin_DefUseAnalysis.cpp \
     dataflowanalyses/RoseBin_VariableAnalysis.cpp \
     dataflowanalyses/InterruptAnalysis.cpp \
     dataflowanalyses/RoseBin_Emulate.cpp \
     dataflowanalyses/RoseBin_DataFlowAbstract.cpp \
     graph/RoseBin_DotGraph.cpp graph/RoseBin_Graph.cpp graph/RoseBin_GmlGraph.cpp \
     libraryIdentification/libraryIdentification_reader.C libraryIdentification/libraryIdentification_writer.C

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
   graph/RoseBin_DotGraph.h graph/RoseBin_Graph.h \
   graph/RoseBin_GmlGraph.h \
   libraryIdentification/libraryIdentification.h

include_HEADERS = $(BINARY_DISASSEMBLY_HEADER_FILES)

EXTRA_DIST = 


clean-local:
