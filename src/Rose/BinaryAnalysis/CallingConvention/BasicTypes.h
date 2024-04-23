#ifndef ROSE_BinaryAnalysis_CallingConvention_BasicTypes_H
#define ROSE_BinaryAnalysis_CallingConvention_BasicTypes_H
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS

#include <Rose/BinaryAnalysis/InstructionSemantics/BaseSemantics/BasicTypes.h>

#include <Sawyer/Message.h>

#include <vector>

namespace Rose {
namespace BinaryAnalysis {
namespace CallingConvention {

/** Initialize diagnostics.
 *
 *  This is normally called as part of ROSE's diagnostics initialization, but it doesn't hurt to call it often. */
void initDiagnostics();

/** Facility for diagnostic output.
 *
 *  The facility can be controlled directly or via ROSE's command-line. */
extern Sawyer::Message::Facility mlog;

/** The order that arguments are pushed onto the stack. */
enum class StackParameterOrder {
    LEFT_TO_RIGHT,                                      /**< Stack parameters pushed left to right (Pascal order). */
    RIGHT_TO_LEFT,                                      /**< Stack parameters pushed right to left (C order). */
    UNSPECIFIED,                                        /**< Stack parameter order is unknown or unspecified. */
};

/** The direction in which the stack grows. */
enum class StackDirection {
    GROWS_UP,                                           /**< A push increments the stack pointer. */
    GROWS_DOWN,                                         /**< A push decrements the stack pointer. */
};

/** Who is responsible for popping stack parameters. */
enum class StackCleanup {
    BY_CALLER,                                          /**< The caller pops all stack parameters. */
    BY_CALLEE,                                          /**< The called function pops all stack parameters. */
    UNSPECIFIED,                                        /**< Stack parameter cleanup is unknown or unspecified. */
};

class Analysis;
/** Shared-ownership pointer to @ref Analysis. */
using AnalysisPtr = Sawyer::SharedPointer<Analysis>;

class Declaration;
/** Shared-ownership pointer to @ref Declaration. */
using DeclarationPtr = Sawyer::SharedPointer<Declaration>;

class Definition;
/** Shared-ownership pointer for @ref Definition. */
using DefinitionPtr = Sawyer::SharedPointer<Definition>;

/** An ordered collection of calling convention definitions. */
using Dictionary = std::vector<DefinitionPtr>;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Free functions
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** Read a function argument from a semantic state. */
InstructionSemantics::BaseSemantics::SValuePtr
readArgument(const InstructionSemantics::BaseSemantics::RiscOperatorsPtr&, const DefinitionPtr&, size_t argNumber);

/** Write a function argument to a semantic state. */
void writeArgument(const InstructionSemantics::BaseSemantics::RiscOperatorsPtr&, const DefinitionPtr&,
                   size_t argNumber, const InstructionSemantics::BaseSemantics::SValuePtr &value);

/** Read the return value that a function is returning. */
InstructionSemantics::BaseSemantics::SValuePtr
readReturnValue(const InstructionSemantics::BaseSemantics::RiscOperatorsPtr&, const DefinitionPtr&);

/** Write a value to a function return semantic state. */
void writeReturnValue(const InstructionSemantics::BaseSemantics::RiscOperatorsPtr&, const DefinitionPtr&,
                      const InstructionSemantics::BaseSemantics::SValuePtr &returnValue);

/** Simulate a function return.
 *
 *  The RISC operator's current state is adjusted as if a function with the specified calling convention returned. */
void simulateFunctionReturn(const InstructionSemantics::BaseSemantics::RiscOperatorsPtr&, const DefinitionPtr&);

/** Print a definition. */
std::ostream& operator<<(std::ostream&, const Definition&);

/** Print analysis information. */
std::ostream& operator<<(std::ostream&, const Analysis&);

} // namespace
} // namespace
} // namespace

#endif
#endif
