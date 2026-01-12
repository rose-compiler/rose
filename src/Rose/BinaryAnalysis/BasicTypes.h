#ifndef ROSE_BinaryAnalysis_BasicTypes_H
#define ROSE_BinaryAnalysis_BasicTypes_H
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <RoseFirst.h>

// #include's for subdirectories and sub-namespaces are at the end of this header.
#include <Rose/Exception.h>
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
class LibraryIdentification;
class MagicNumber;
class MemoryMap;
using MemoryMapPtr = Sawyer::SharedPointer<MemoryMap>;  /**< Reference counting pointer. */
class NoOperation;
class Reachability;
class ReadWriteSets;
using ReadWriteSetsPtr = std::shared_ptr<ReadWriteSets>; /**< Reference counting pointer. */
using RegisterDescriptors = std::vector<RegisterDescriptor>; /**< List of register descriptors in dictionary. */
class RegisterDictionary;
using RegisterDictionaryPtr = Sawyer::SharedPointer<RegisterDictionary>; /**< Reference counting pointer. */
class RegisterNames;
class RegisterParts;
class SerialInput;
using SerialInputPtr = std::shared_ptr<SerialInput>; /**< Reference counting pointer. */
class SerialIo;
using SerialIoPtr = std::shared_ptr<SerialIo>; /**< Reference counting pointer. */
class SerialOutput;
using SerialOutputPtr = std::shared_ptr<SerialOutput>; /**< Reference counting pointer. */

namespace Serialization {

/** Format of the state file.
 *
 *  Regardless of format, the file begins with a magic header that identifies the container framing and allows
 *  multiple objects to be stored sequentially. The format selects how object payloads are encoded. */
enum Format {
    BINARY,     /**< Binary payloads are smaller and faster than the other formats, but are not portable across
                 *   architectures. */
    TEXT,       /**< Textual payloads store the data as ASCII text. They are larger and slower than binary payloads
                 *   but are portable across architectures. */
    XML,        /**< The payloads are stored as XML, which is a very verbose and slow format. Avoid using this if
                 *   possible. */
    FLATBUFFERS /**< FlatBuffers payloads. Requires ROSE to be configured with FlatBuffers support
                 *   (ENABLE_FLATBUFFERS_SERIALIZATION). */
};

/** Types of objects that can be saved. */
enum Savable {
    NO_OBJECT         = 0x00000000, /**< Object type for newly-initialized serializers. */
    PARTITIONER       = 0x00000001, /**< Rose::BinaryAnalysis::Partitioner2::Partitioner. */
    AST               = 0x00000002, /**< Abstract syntax tree. */
    END_OF_DATA       = 0x0000fffe, /**< Marks the end of the data stream. */
    ERROR             = 0x0000ffff, /**< Marks that the stream has encountered an error condition. */
    USER_DEFINED      = 0x00010000, /**< First user-defined object number. */
    USER_DEFINED_LAST = 0xffffffff  /**< Last user-defined object number. */
};

/** Errors thrown during serialization. */
class Exception: public Rose::Exception {
  public:
    /** Construct an exception with an error message. */
    explicit Exception(const std::string& s) : Rose::Exception(s) {}
    ~Exception() throw() {}
};

/** Progress callback function type. */
using ProgressCallback = std::function<void(size_t current, size_t total, const char* phase)>;
class SerialFrame;
class FrameRecord;

namespace FlatBuffers {
class Serializer;
class Deserializer;
} // namespace FlatBuffers

} // namespace Serialization
class SmtlibSolver;
class SmtSolver;
using SmtSolverPtr = std::shared_ptr<SmtSolver>;  /**< Reference counting pointer. */
class SourceLocations;
class SRecord;
class SymbolicExpresssionParser;
class SystemCall;
class TaintedFlow;
class VxworksTerminal;
using VxworksTerminalPtr = std::shared_ptr<VxworksTerminal>; /**< Reference counting pointer. */
class Z3Solver;

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
void initNamespace();
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

} // namespace
} // namespace

#endif

// #include's for sub-namespaces and sub-directories
#include <Rose/BinaryAnalysis/Architecture/BasicTypes.h>
#include <Rose/BinaryAnalysis/CallingConvention/BasicTypes.h>
#include <Rose/BinaryAnalysis/Concolic/BasicTypes.h>
#include <Rose/BinaryAnalysis/Debugger/BasicTypes.h>
#include <Rose/BinaryAnalysis/Disassembler/BasicTypes.h>
#include <Rose/BinaryAnalysis/Dwarf/BasicTypes.h>
#include <Rose/BinaryAnalysis/InstructionSemantics/BaseSemantics/BasicTypes.h>
#include <Rose/BinaryAnalysis/ModelChecker/BasicTypes.h>
#include <Rose/BinaryAnalysis/Partitioner2/BasicTypes.h>
#include <Rose/BinaryAnalysis/Variables/BasicTypes.h>

#endif
