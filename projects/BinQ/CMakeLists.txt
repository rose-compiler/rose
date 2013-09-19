
set(BINQ_MOC_SOURCES slide.h MyBars.h)
QT4_WRAP_CPP(BINQ_MOC_CPP ${BINQ_MOC_SOURCES})

set(BINQ_SOURCES
  BinQMain.C BinQSupport.C BinQGui.C slide.C LCS.C Clone.C FunctionDiff.C
  AlignFunctions.C BinCallGraph.C BinControlFlowAnalysis.C BinDataFlowAnalysis.C
  BufferOverflow.C InterruptAnalysis.C BinDynamicInfo.C MyBars.C BinQAbstract.C
  BinQbatch.C ForbiddenFunctionCall.C BinQinteractive.C NullAfterFree.C
  MallocAndFree.C InitPointerToNull.C ComplexityMetric.C DwarfFileInfo.C
)

add_executable(BinQ ${BINQ_SOURCES} ${BINQ_MOC_CPP})
target_link_librarieS(BinQ ROSE_DLL qrose ${Boost_LIBRARIES})
