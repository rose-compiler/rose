include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

INCLUDES = -Winvalid-pch $(ROSE_INCLUDES) -I$(srcdir)/graph -I$(srcdir)/libraryIdentification 

noinst_LTLIBRARIES = libbinaryMidend.la
libbinaryMidend_la_DEPENDENCIES =

if ROSE_BUILD_BINARY_ANALYSIS_SUPPORT
libbinaryMidend_la_SOURCES = \
     binary_analysis.C \
     RoseBin_CompareAnalysis.cpp RoseBin_ControlFlowAnalysis.cpp \
     RoseBin_printInstructions.cpp RoseBin_DataFlowAnalysis.cpp \
     RoseBin_CallGraphAnalysis.cpp RoseBin_FlowAnalysis.cpp \
     dataflowanalyses/RoseBin_DefUseAnalysis.cpp \
     dataflowanalyses/RoseBin_VariableAnalysis.cpp \
     dataflowanalyses/InterruptAnalysis.cpp \
     dataflowanalyses/RoseBin_Emulate.cpp \
     dataflowanalyses/RoseBin_DataFlowAbstract.cpp \
     instructionSemantics/findConstants.C \
     instructionSemantics/InsnSemanticsExpr.C \
     instructionSemantics/SMTSolver.C \
     instructionSemantics/SymbolicSemantics.C \
     instructionSemantics/YicesSolver.C \
     instructionSemantics/VirtualMachineSemantics.C \
     GraphAlgorithms.C
else
libbinaryMidend_la_SOURCES = 
endif

if ROSE_USE_SQLITE_DATABASE
libbinaryMidend_la_SOURCES += \
     libraryIdentification/libraryIdentification_reader.C \
     libraryIdentification/libraryIdentification_writer.C \
     libraryIdentification/functionIdentification.C 
endif

include_HEADERS = \
   binary_analysis.h \
   GraphAlgorithms.h \
   RoseBin_CallGraphAnalysis.h \
   RoseBin_CompareAnalysis.h \
   RoseBin_ControlFlowAnalysis.h \
   RoseBin_DataFlowAnalysis.h \
   RoseBin_FlowAnalysis.h \
   RoseBin_printInstructions.h \
   dataflowanalyses/InterruptAnalysis.h \
   dataflowanalyses/RoseBin_DataFlowAbstract.h \
   dataflowanalyses/RoseBin_DefUseAnalysis.h \
   dataflowanalyses/RoseBin_Emulate.h \
   dataflowanalyses/RoseBin_VariableAnalysis.h \
   instructionSemantics/findConstants.h \
   instructionSemantics/InsnSemanticsExpr.h \
   instructionSemantics/SMTSolver.h \
   instructionSemantics/SymbolicSemantics.h \
   instructionSemantics/VirtualMachineSemantics.h \
   instructionSemantics/YicesSolver.h \
   instructionSemantics/flowEquations.h \
   instructionSemantics/semanticsModule.h \
   instructionSemantics/x86InstructionSemantics.h \
   libraryIdentification/functionIdentification.h \
   libraryIdentification/libraryIdentification.h \
   ether.h


EXTRA_DIST = CMakeLists.txt
