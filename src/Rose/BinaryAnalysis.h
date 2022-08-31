#ifndef ROSE_BinaryAnalysis_H
#define ROSE_BinaryAnalysis_H

#include <Rose/BinaryAnalysis/AbstractLocation.h>
#include <Rose/BinaryAnalysis/AsmFunctionIndex.h>
#include <Rose/BinaryAnalysis/AstHash.h>
#include <Rose/BinaryAnalysis/BasicTypes.h>
#include <Rose/BinaryAnalysis/BestMapAddress.h>
#include <Rose/BinaryAnalysis/BinaryLoaderElf.h>
#include <Rose/BinaryAnalysis/BinaryLoaderElfObj.h>
#include <Rose/BinaryAnalysis/BinaryLoader.h>
#include <Rose/BinaryAnalysis/BinaryLoaderPe.h>
#include <Rose/BinaryAnalysis/CallingConvention.h>
#include <Rose/BinaryAnalysis/CodeInserter.h>
#include <Rose/BinaryAnalysis/Concolic.h>
#include <Rose/BinaryAnalysis/ConcreteLocation.h>
#include <Rose/BinaryAnalysis/ControlFlow.h>
#include <Rose/BinaryAnalysis/DataFlow.h>
#include <Rose/BinaryAnalysis/Debugger.h>
#include <Rose/BinaryAnalysis/Demangler.h>
#include <Rose/BinaryAnalysis/Disassembler.h>
#include <Rose/BinaryAnalysis/DisassemblerCil.h>
#include <Rose/BinaryAnalysis/DisassemblerJvm.h>
#include <Rose/BinaryAnalysis/Dwarf.h>
#include <Rose/BinaryAnalysis/FeasiblePath.h>
#include <Rose/BinaryAnalysis/FunctionCall.h>
#include <Rose/BinaryAnalysis/FunctionSimilarity.h>
#include <Rose/BinaryAnalysis/HotPatch.h>
#include <Rose/BinaryAnalysis/InstructionCache.h>
#include <Rose/BinaryAnalysis/InstructionEnumsAarch32.h>
#include <Rose/BinaryAnalysis/InstructionEnumsAarch64.h>
#include <Rose/BinaryAnalysis/InstructionEnumsCil.h>
#include <Rose/BinaryAnalysis/InstructionEnumsJvm.h>
#include <Rose/BinaryAnalysis/InstructionEnumsM68k.h>
#include <Rose/BinaryAnalysis/InstructionEnumsMips.h>
#include <Rose/BinaryAnalysis/InstructionEnumsPowerpc.h>
#include <Rose/BinaryAnalysis/InstructionEnumsX86.h>
#include <Rose/BinaryAnalysis/InstructionSemantics.h>
#include <Rose/BinaryAnalysis/LibraryIdentification.h>
#include <Rose/BinaryAnalysis/MagicNumber.h>
#include <Rose/BinaryAnalysis/Matrix.h>
#include <Rose/BinaryAnalysis/MemoryMap.h>
#include <Rose/BinaryAnalysis/ModelChecker.h>
#include <Rose/BinaryAnalysis/NoOperation.h>
#include <Rose/BinaryAnalysis/Partitioner2.h>
#include <Rose/BinaryAnalysis/PointerDetection.h>
#include <Rose/BinaryAnalysis/Reachability.h>
#include <Rose/BinaryAnalysis/RegisterDescriptor.h>
#include <Rose/BinaryAnalysis/RegisterDictionary.h>
#include <Rose/BinaryAnalysis/RegisterNames.h>
#include <Rose/BinaryAnalysis/RegisterParts.h>
#include <Rose/BinaryAnalysis/ReturnValueUsed.h>
#include <Rose/BinaryAnalysis/SerialIo.h>
#include <Rose/BinaryAnalysis/SmtCommandLine.h>
#include <Rose/BinaryAnalysis/SmtlibSolver.h>
#include <Rose/BinaryAnalysis/SmtSolver.h>
#include <Rose/BinaryAnalysis/SourceLocations.h>
#include <Rose/BinaryAnalysis/SRecord.h>
#include <Rose/BinaryAnalysis/StackDelta.h>
#include <Rose/BinaryAnalysis/String.h>
#include <Rose/BinaryAnalysis/SymbolicExpr.h>
#include <Rose/BinaryAnalysis/SymbolicExprParser.h>
#include <Rose/BinaryAnalysis/SystemCall.h>
#include <Rose/BinaryAnalysis/TaintedFlow.h>
#include <Rose/BinaryAnalysis/ToSource.h>
#include <Rose/BinaryAnalysis/Unparser.h>
#include <Rose/BinaryAnalysis/Utils.h>
#include <Rose/BinaryAnalysis/Variables.h>
#include <Rose/BinaryAnalysis/Z3Solver.h>

/** Binary analysis.
 *
 *  ROSE supports both source code analysis and binary analysis. This namespace contains most of the binary analysis
 *  features. The @ref binary_tutorial is a good place to get started, and most of its examples link to the API
 *  documentation. Follow those links to the enclosing class and/or namespace to obtain an overview of how those classes and
 *  namespaces work. */
namespace BinaryAnalysis {}

#endif
