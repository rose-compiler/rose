#ifndef ROSE_BinaryAnalysis_ModelChecker_Types_H
#define ROSE_BinaryAnalysis_ModelChecker_Types_H
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS

#include <memory>

namespace Rose {
namespace BinaryAnalysis {

/** Model checking framework.
 *
 *  TODO: [Robb Matzke 2021-04-09] Top level model checker documentation. */
namespace ModelChecker {

class AlwaysTrue;
using AlwaysTruePtr = std::shared_ptr<AlwaysTrue>;

class BasicBlockUnit;
using BasicBlockUnitPtr = std::shared_ptr<BasicBlockUnit>;

class Engine;
using EnginePtr = std::shared_ptr<Engine>;

class ErrorTag;
using ErrorTagPtr = std::shared_ptr<ErrorTag>;

class Exception;

class ExecutionUnit;
using ExecutionUnitPtr = std::shared_ptr<ExecutionUnit>;

class ExternalFunctionUnit;
using ExternalFunctionUnitPtr = std::shared_ptr<ExternalFunctionUnit>;

class FailureUnit;
using FailureUnitPtr = std::shared_ptr<FailureUnit>;

class FastestPathFirst;
using FastestPathFirstPtr = std::shared_ptr<FastestPathFirst>;

class HasFinalTags;
using HasFinalTagsPtr = std::shared_ptr<HasFinalTags>;

class InstructionUnit;
using InstructionUnitPtr = std::shared_ptr<InstructionUnit>;

class LongestPathFirst;
using LongestPathFirstPtr = std::shared_ptr<LongestPathFirst>;

class NameTag;
using NameTagPtr = std::shared_ptr<NameTag>;

class NullDerefTag;
using NullDerefTagPtr = std::shared_ptr<NullDerefTag>;

class OobTag;
using OobTagPtr = std::shared_ptr<OobTag>;

class ParseError;

class Path;
using PathPtr = std::shared_ptr<Path>;

class PathNode;
using PathNodePtr = std::shared_ptr<PathNode>;

class PathPredicate;
using PathPredicatePtr = std::shared_ptr<PathPredicate>;

class PathPrioritizer;
using PathPrioritizerPtr = std::shared_ptr<PathPrioritizer>;

class PathQueue;
using PathQueuePtr = std::shared_ptr<PathQueue>;

class Periodic;
using PeriodicPtr = std::shared_ptr<Periodic>;

class RandomPathFirst;
using RandomPathFirstPtr = std::shared_ptr<RandomPathFirst>;

class SemanticCallbacks;
using SemanticCallbacksPtr = std::shared_ptr<SemanticCallbacks>;

class Settings;
using SettingsPtr = std::shared_ptr<Settings>;

class ShortestPathFirst;
using ShortestPathFirstPtr = std::shared_ptr<ShortestPathFirst>;

class SourceLister;
using SourceListerPtr = std::shared_ptr<SourceLister>;

class Tag;
using TagPtr = std::shared_ptr<Tag>;

class UninitVarTag;
using UninitVarTagPtr = std::shared_ptr<UninitVarTag>;

class WorkerStatus;
using WorkerStatusPtr = std::shared_ptr<WorkerStatus>;

class WorkPredicate;
using WorkPredicatePtr = std::shared_ptr<WorkPredicate>;

extern Sawyer::Message::Facility mlog;
void initDiagnostics();

namespace P2Model {
    class SValue;
    using SValuePtr = Sawyer::SharedPointer<SValue>;

    class RiscOperators;
    using RiscOperatorsPtr = boost::shared_ptr<RiscOperators>;

    class SemanticCallbacks;
    using SemanticCallbacksPtr = std::shared_ptr<SemanticCallbacks>;
}

/** Mode by which comparisons are made. */
enum class TestMode {
    OFF,                                                /**< Checking is disabled. */
    MAY,                                                /**< Detection is reported if it may occur. */
    MUST                                                /**< Detection is reported only if it must occur. */
};

/** Direction of data wrt storage. */
enum class IoMode {
    WRITE,                                              /**< Data is moving to storage. */
    READ                                                /**< Data is moving from storage. */
};


// Worker states. Used internally.
enum class WorkerState {
    STARTING,                                           // thread is initializing
    WAITING,                                            // thread is looking for new work
    WORKING,                                            // thread is actively working on a path
    FINISHED                                            // thread will never work again and is cleaning up
};

#define UNMANAGED_WORKER ((size_t)-1)                   // non-ID for an unmanaged worker; I.e., a user thread

} // namespace
} // namespace
} // namespace

#endif
#endif
