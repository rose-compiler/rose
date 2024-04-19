#ifndef ROSE_BinaryAnalysis_BasicTypes_H
#define ROSE_BinaryAnalysis_BasicTypes_H
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <RoseFirst.h>

// #include's for subdirectories and sub-namespaces are at the end of this header.
#include <Rose/BinaryAnalysis/RegisterDescriptor.h>
#include <Sawyer/SharedPointer.h>
#include <memory>

namespace Rose {
namespace BinaryAnalysis {

// Document definitions, but not forwared declarations.

class AbstractLocation;
class AsmFunctionIndex;
class AstHasher;
class BestMapAddress;
class BinaryLoader;
using BinaryLoaderPtr = Sawyer::SharedPointer<BinaryLoader>; /**< Reference counting pointer. */
class BinaryLoaderElf;
using BinaryLoaderElfPtr = Sawyer::SharedPointer<BinaryLoaderElf>; /**< Reference counting pointer. */
class BinaryLoaderElfObj;
using BinaryLoaderElfObjPtr = Sawyer::SharedPointer<BinaryLoaderElfObj>; /**< Reference counting pointer. */
class BinaryLoaderPe;
using BinaryLoaderPePtr = Sawyer::SharedPointer<BinaryLoaderPe>; /**< Refernce counting pointer. */
class BinaryToSource;
class CodeInserter;
class ConcreteLocation;
class DataFlow;
class Demangler;
class FeasiblePath;
class FunctionCall;
class FunctionSimilarity;
class HotPatch;
class InstructionCache;
class InstructionPtr;
class LibraryIdentification;
class LockedInstruction;
class MagicNumber;
class ManagedInstruction;
class MemoryMap;
using MemoryMapPtr = Sawyer::SharedPointer<MemoryMap>;  /**< Reference counting pointer. */
class NoOperation;
class Reachability;
using RegisterDescriptors = std::vector<RegisterDescriptor>; /**< List of register descriptors in dictionary. */
class RegisterDictionary;
using RegisterDictionaryPtr = Sawyer::SharedPointer<RegisterDictionary>; /**< Reference counting pointer. */
class RegisterNames;
class RegisterParts;
class SerialInput;
using SerialInputPtr = Sawyer::SharedPointer<SerialInput>; /**< Reference counting pointer. */
class SerialIo;
using SerialIoPtr = Sawyer::SharedPointer<SerialIo>;    /**< Reference counting pointer. */
class SerialOutput;
using SerialOutputPtr = Sawyer::SharedPointer<SerialOutput>; /**< Reference counting pointer. */
class SmtlibSolver;
class SmtSolver;
using SmtSolverPtr = std::shared_ptr<SmtSolver>;  /**< Reference counting pointer. */
class SourceLocations;
class SRecord;
class SymbolicExpresssionParser;
class SystemCall;
class TaintedFlow;
class Z3Solver;

namespace CallingConvention {
class Analysis;
class Declaration;
using DeclarationPtr = Sawyer::SharedPointer<Declaration>; /**< Reference counting pointer. */
class Definition;
using DefinitionPtr = Sawyer::SharedPointer<Definition>; /**< Reference counting pointer. */
} // namespace

namespace PointerDetection {
class Analysis;
class PointerDescriptor;
class Settings;
} // namespace

namespace ReturnValueUsed {
class Analysis;
class CallSiteResults;
} // namespace

namespace StackDelta {
class Analysis;
} // namespace

namespace Strings {
class AnyCodePoint;
class BasicCharacterEncodingScheme;
class BasicLengthEncodingScheme;
class CharacterEncodingForm;
class CharacterEncodingScheme;
class CodePointPredicate;
class EncodedString;
class Exception;
class LengthEncodedString;
class LengthEncodingScheme;
class NoopCharacterEncodingForm;
class PrintableAscii;
class StringEncodingScheme;
class StringFinder;
class TerminatedString;
class Utf8CharacterEncodingForm;
class Utf16CharacterEncodingForm;
} // namespace

namespace SymbolicExpression {
class Exception;
class ExprExprHashMap;
class Formatter;
class Interior;
using InteriorPtr = Sawyer::SharedPointer<Interior>;    /**< Reference counting pointer. */
class Leaf;
using LeafPtr = Sawyer::SharedPointer<Leaf>;            /**< Reference counting pointer. */
class Node;
using Ptr = Sawyer::SharedPointer<Node>;                /**< Reference counting pointer. */
class Simplifier;
class Type;
class Visitor;
} // namespace
using SymbolicExpressionPtr = SymbolicExpression::Ptr;

namespace Variables {
class BaseVariable;
class GlobalVariable;
class StackFrame;
class StackVariable;
class VariableFinder;
using VariableFinderPtr = Sawyer::SharedPointer<VariableFinder>; /**< Reference counting pointer. */
} // namespace

} // namespace
} // namespace

#endif

// #include's for sub-namespaces and sub-directories
#include <Rose/BinaryAnalysis/Architecture/BasicTypes.h>
#include <Rose/BinaryAnalysis/Concolic/BasicTypes.h>
#include <Rose/BinaryAnalysis/Debugger/BasicTypes.h>
#include <Rose/BinaryAnalysis/Disassembler/BasicTypes.h>
#include <Rose/BinaryAnalysis/Dwarf/BasicTypes.h>
#include <Rose/BinaryAnalysis/InstructionSemantics/BaseSemantics/BasicTypes.h>
#include <Rose/BinaryAnalysis/ModelChecker/BasicTypes.h>
#include <Rose/BinaryAnalysis/Partitioner2/BasicTypes.h>

#endif
