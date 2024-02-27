#ifndef ROSE_BinaryAnalysis_ModelChecker_BasicTypes_H
#define ROSE_BinaryAnalysis_ModelChecker_BasicTypes_H
#include <featureTests.h>
#ifdef ROSE_ENABLE_MODEL_CHECKER

#include <Rose/Sarif/BasicTypes.h>
#include <Sawyer/Message.h>
#include <memory>
#include <boost/shared_ptr.hpp>

namespace Rose {
namespace BinaryAnalysis {

/** Model checking framework.
 *
 *  Model checking is a method for checking whether a model of a system meeds a given specification. This analysis models the
 *  execution of machine instructions on a state consisting of CPU registers and globally addressable memory. It also models
 *  some source-level concepts such as local and global variables and function call frames. The main class for this API is the
 *  @ref Rose::BinaryAnalysis::ModelChecker::Engine "Engine", which configures and then drives the analysis. */
namespace ModelChecker {

class AlwaysTrue;
/** Shared-ownership pointer to @ref AlwaysTrue. */
using AlwaysTruePtr = std::shared_ptr<AlwaysTrue>;

class BasicBlockUnit;
/** Shared-ownership pointer to @ref BasicBlockUnit. */
using BasicBlockUnitPtr = std::shared_ptr<BasicBlockUnit>;

class BestCoverageFirst;
/** Shared-ownership pointer to @ref BestCoverageFirst. */
using BestCoverageFirstPtr = std::shared_ptr<BestCoverageFirst>;

class Engine;
/** Shared-ownership pointer to @ref Engine. */
using EnginePtr = std::shared_ptr<Engine>;

class ErrorTag;
/** Shared-ownership pointer to @ref ErrorTag. */
using ErrorTagPtr = std::shared_ptr<ErrorTag>;

class Exception;

class ExecutionUnit;
/** Shared-ownership pointer to @ref ExecutionUnit. */
using ExecutionUnitPtr = std::shared_ptr<ExecutionUnit>;

class ExternalFunctionUnit;
/** Shared-ownership pointer to @ref ExternalFunctionUnit. */
using ExternalFunctionUnitPtr = std::shared_ptr<ExternalFunctionUnit>;

class FailureUnit;
/** Shared-ownership pointer to @ref FailureUnit. */
using FailureUnitPtr = std::shared_ptr<FailureUnit>;

class FastestPathFirst;
/** Shared-ownership pointer to @ref FastestPathFirst. */
using FastestPathFirstPtr = std::shared_ptr<FastestPathFirst>;

class FoundVariable;

class HasFinalTags;
/** Shared-ownership pointer to @ref HasFinalTags. */
using HasFinalTagsPtr = std::shared_ptr<HasFinalTags>;

class InstructionUnit;
/** Shared-ownership pointer to @ref InstructionUnit. */
using InstructionUnitPtr = std::shared_ptr<InstructionUnit>;

class LongestPathFirst;
/** Shared-ownership pointer to @ref LongestPathFirst. */
using LongestPathFirstPtr = std::shared_ptr<LongestPathFirst>;

class NameTag;
/** Shared-ownership pointer to @ref NameTag. */
using NameTagPtr = std::shared_ptr<NameTag>;

class NullDereferenceTag;
/** Shared-ownership pointer to @ref NullDereferenceTag. */
using NullDereferenceTagPtr = std::shared_ptr<NullDereferenceTag>;

class OutOfBoundsTag;
/** Shared-ownership pointer to @ref OutOfBoundsTag. */
using OutOfBoundsTagPtr = std::shared_ptr<OutOfBoundsTag>;

class ParseError;

class Path;
/** Shared-ownership pointer to @ref Path. */
using PathPtr = std::shared_ptr<Path>;

class PathNode;
/** Shared-ownership pointer to @ref PathNode. */
using PathNodePtr = std::shared_ptr<PathNode>;

class PathPredicate;
/** Shared-ownership pointer to @ref PathPredicate. */
using PathPredicatePtr = std::shared_ptr<PathPredicate>;

class PathPrioritizer;
/** Shared-ownership pointer to @ref PathPrioritizer. */
using PathPrioritizerPtr = std::shared_ptr<PathPrioritizer>;

class PathQueue;
/** Shared-ownership pointer to @ref PathQueue. */
using PathQueuePtr = std::shared_ptr<PathQueue>;

class Periodic;
/** Shared-ownership pointer to @ref Periodic. */
using PeriodicPtr = std::shared_ptr<Periodic>;

class RandomPathFirst;
/** Shared-ownership pointer to @ref RandomPathFirst. */
using RandomPathFirstPtr = std::shared_ptr<RandomPathFirst>;

class SemanticCallbacks;
/** Shared-ownership pointer to @ref SemanticCallbacks. */
using SemanticCallbacksPtr = std::shared_ptr<SemanticCallbacks>;

class Settings;
/** Shared-ownership pointer to @ref Settings. */
using SettingsPtr = std::shared_ptr<Settings>;

class ShortestPathFirst;
/** Shared-ownership pointer to @ref ShortestPathFirst. */
using ShortestPathFirstPtr = std::shared_ptr<ShortestPathFirst>;

class SourceLister;
/** Shared-ownership pointer to @ref SourceLister. */
using SourceListerPtr = std::shared_ptr<SourceLister>;

class Tag;
/** Shared-ownership pointer to @ref Tag. */
using TagPtr = std::shared_ptr<Tag>;

class UninitializedVariableTag;
/** Shared-ownership pointer to @ref UninitializedVariableTag. */
using UninitializedVariableTagPtr = std::shared_ptr<UninitializedVariableTag>;

class WorkerStatus;
/** Shared-ownership pointer to @ref WorkerStatus. */
using WorkerStatusPtr = std::shared_ptr<WorkerStatus>;

class WorkPredicate;
/** Shared-ownership pointer to @ref WorkPredicate. */
using WorkPredicatePtr = std::shared_ptr<WorkPredicate>;

extern Sawyer::Message::Facility mlog;
void initDiagnostics();

namespace PartitionerModel {
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

/** Create a SARIF log for model checker results. */
Rose::Sarif::LogPtr makeSarifLog();

/** Add model checker rules to a SARIF analysis. */
void insertSarifRules(const Rose::Sarif::AnalysisPtr&);

} // namespace
} // namespace
} // namespace

#endif
#endif
