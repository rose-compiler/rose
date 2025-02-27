#include <featureTests.h>
#ifdef ROSE_ENABLE_MODEL_CHECKER
#include <Rose/BinaryAnalysis/ModelChecker/PathNode.h>

#include <Rose/BinaryAnalysis/InstructionSemantics/BaseSemantics/RiscOperators.h>
#include <Rose/BinaryAnalysis/InstructionSemantics/BaseSemantics/State.h>
#include <Rose/BinaryAnalysis/ModelChecker/ExecutionUnit.h>
#include <Rose/BinaryAnalysis/ModelChecker/SemanticCallbacks.h>
#include <Rose/BinaryAnalysis/ModelChecker/Settings.h>
#include <Rose/BinaryAnalysis/ModelChecker/Tag.h>
#include <Rose/BinaryAnalysis/SymbolicExpression.h>
#include <Rose/Sarif/Location.h>

#include <rose_isnan.h>

#include <boost/scope_exit.hpp>

using namespace Sawyer::Message::Common;
namespace BS = Rose::BinaryAnalysis::InstructionSemantics::BaseSemantics;

namespace Rose {
namespace BinaryAnalysis {
namespace ModelChecker {

PathNode::PathNode(const ExecutionUnit::Ptr &unit)
    : executionUnit_(unit), id_(Sawyer::fastRandomIndex(UINT64_MAX)) {
    ASSERT_not_null(unit);
}

PathNode::PathNode(const Ptr &parent, const ExecutionUnit::Ptr &unit, const SymbolicExpression::Ptr &assertion,
                   const SmtSolver::Evidence &evidence, const BS::State::Ptr &parentOutgoingState)
    : parent_(parent), executionUnit_(unit),  incomingState_(parentOutgoingState), assertions_{assertion},
      evidence_(evidence), id_(Sawyer::fastRandomIndex(UINT32_MAX)) {
    ASSERT_not_null(unit);
    ASSERT_not_null(parent);
    ASSERT_not_null(assertion);
}

PathNode::~PathNode() {}

PathNode::Ptr
PathNode::instance(const ExecutionUnit::Ptr &unit) {
    ASSERT_not_null(unit);
    return Ptr(new PathNode(unit));
}

PathNode::Ptr
PathNode::instance(const Ptr &parent, const ExecutionUnit::Ptr &unit, const SymbolicExpression::Ptr &assertion,
                   const SmtSolver::Evidence &evidence, const BS::State::Ptr &parentOutgoingState) {
    ASSERT_not_null(unit);
    ASSERT_not_null(parent);
    ASSERT_not_null(assertion);
    return Ptr(new PathNode(parent, unit, assertion, evidence, parentOutgoingState));
}

uint32_t
PathNode::id() const {
    SAWYER_THREAD_TRAITS::LockGuard lock(mutex_);
    return id_;
}

double
PathNode::sortKey() const {
    SAWYER_THREAD_TRAITS::LockGuard lock(mutex_);
    return sortKey_;
}

void
PathNode::sortKey(double d) {
    SAWYER_THREAD_TRAITS::LockGuard lock(mutex_);
    sortKey_ = d;
}

PathNode::Ptr
PathNode::parent() const {
    // No lock necessary since this cannot change after object construction
    return parent_;
}

ExecutionUnit::Ptr
PathNode::executionUnit() const {
    // No lock necessary since this cannot change after object construction
    ASSERT_not_null(executionUnit_);
    return executionUnit_;
}

size_t
PathNode::nSteps() const {
    // No lock necessary since execution unit pointer can't change after object construction
    ASSERT_not_null(executionUnit_);
    return executionUnit_->nSteps();
}

void
PathNode::assertion(const SymbolicExpression::Ptr &expr) {
    ASSERT_not_null(expr);
    SAWYER_THREAD_TRAITS::LockGuard lock(mutex_);
    assertions_.push_back(expr);
}

std::vector<SymbolicExpression::Ptr>
PathNode::assertions() const {
    SAWYER_THREAD_TRAITS::LockGuard lock(mutex_);
    return assertions_;                                 // must be a copy for thread safety
}

SmtSolver::Evidence
PathNode::evidence() const {
    // this member is read-only, initialized in the constructor, so no need to lock.
    return evidence_;
}

std::pair<BS::State::Ptr, bool>
PathNode::incomingState_NS(const Settings::Ptr &settings, const SemanticCallbacks::Ptr &semantics,
                           const BS::RiscOperators::Ptr &ops, const SmtSolver::Ptr &solver) {
    ASSERT_not_null(settings);
    ASSERT_not_null(semantics);
    ASSERT_not_null(ops);

    BS::State::Ptr state;
    bool needsInitialization = false;

    if (incomingState_) {
        state = incomingState_->clone();
    } else if (parent_) {
        parent_->execute(settings, semantics, ops, solver);
        state = parent_->copyOutgoingState();
    } else {
        state = semantics->createInitialState();
        needsInitialization = true;
    }

    return {state, needsInitialization};
}

void
PathNode::execute(const Settings::Ptr &settings, const SemanticCallbacks::Ptr &semantics, const BS::RiscOperators::Ptr &ops,
                  const SmtSolver::Ptr &solver) {
    ASSERT_not_null(settings);
    ASSERT_not_null(semantics);
    ASSERT_not_null(ops);
    SAWYER_THREAD_TRAITS::LockGuard lock(mutex_);

    // Do we actually need to execute anything?
    if (outgoingState_ || executionFailed_) {
        return;                                         // already executed
    } else if (settings->rejectUnknownInsns && executionUnit_->containsUnknownInsn()) {
        SAWYER_MESG(mlog[DEBUG]) <<"  contains not-allowed \"unknown\" instruction(s)\n";
        executionFailed_ = true;
        return;
    }

    // Get the incoming state, which might require recursively executing the parent.
    BS::State::Ptr state;
    bool needsInitialization = false;
    std::tie(state, needsInitialization) = incomingState_NS(settings, semantics, ops, solver);

    // Prepare the RISC operators and maybe initialize the path initial state
    ASSERT_forbid(ops->currentState());                 // safety net
    ops->currentState(state);                           // ops is thread local, references state
    BOOST_SCOPE_EXIT(&ops) {
        ops->currentState(nullptr);
    } BOOST_SCOPE_EXIT_END;
    if (needsInitialization)
        semantics->initializeState(ops);

    // Allow the semantics layer to access the model checker's SMT solver if necessary.
    SmtSolver::Transaction tx(solver);
    semantics->attachModelCheckerSolver(ops, solver);
    BOOST_SCOPE_EXIT(&semantics, &ops) {
        semantics->attachModelCheckerSolver(ops, SmtSolver::Ptr());
    } BOOST_SCOPE_EXIT_END;

    // Execute the current node. Note that we're still holding the lock on this node so if other threads are also needing to
    // execute this node, they'll block and when they finally make progress they'll return fast (the "already executed"
    // condition above).
    if (ops->currentState()) {                          // null if parent execution failed
        SAWYER_MESG(mlog[DEBUG]) <<"  running pre-execution steps\n";
        std::vector<Tag::Ptr> tags = semantics->preExecute(shared_from_this(), ops);
        tags_.insert(tags_.end(), tags.begin(), tags.end());
    }
    if (ops->currentState()) {
        std::vector<Tag::Ptr> tags = executionUnit_->execute(settings, semantics, ops);  // state is now updated
        tags_.insert(tags_.end(), tags.begin(), tags.end());
    }
    if (ops->currentState()) {
        SAWYER_MESG(mlog[DEBUG]) <<"  running post-execution steps\n";
        std::vector<Tag::Ptr> tags = semantics->postExecute(shared_from_this(), ops);
        tags_.insert(tags_.end(), tags.begin(), tags.end());
    }

    // Execution has either succeeded or failed depending on whether the RISC operators has a current state.
    if (ops->currentState()) {
        outgoingState_ = state;
        executionFailed_ = false;
    } else {
        outgoingState_ = nullptr;
        executionFailed_ = true;
    }

    // We no longer need the incoming state
    incomingState_ = BS::State::Ptr();

    // ops->currentState is reset to null on scope exit
}

void
PathNode::doNotExecute() {
    SAWYER_THREAD_TRAITS::LockGuard lock(mutex_);
    outgoingState_ = nullptr;
    executionFailed_ = true;
}

bool
PathNode::executionFailed() const {
    SAWYER_THREAD_TRAITS::LockGuard lock(mutex_);
    return executionFailed_;
}

BS::State::Ptr
PathNode::copyOutgoingState() const {
    SAWYER_THREAD_TRAITS::LockGuard lock(mutex_);
    return outgoingState_ ? outgoingState_->clone() : BS::State::Ptr();
}

PathNode::BorrowedOutgoingState
PathNode::borrowOutgoingState() {
    SAWYER_THREAD_TRAITS::LockGuard lock(mutex_);
    ASSERT_not_null(outgoingState_);
    BorrowedOutgoingState borrowed(this, outgoingState_);
    outgoingState_ = BS::State::Ptr();
    return borrowed;
}

void
PathNode::restoreOutgoingState(const BS::State::Ptr &state) {
    SAWYER_THREAD_TRAITS::LockGuard lock(mutex_);
    ASSERT_forbid2(outgoingState_, "something gave this node a state while its state was borrowed");
    outgoingState_ = state;
}

PathNode::BorrowedOutgoingState::BorrowedOutgoingState(PathNode *node, const BS::State::Ptr &state)
    : node(node), state(state) {
    ASSERT_not_null(node);
    ASSERT_not_null(state);
}

PathNode::BorrowedOutgoingState::~BorrowedOutgoingState() {
    ASSERT_not_null(node);
    ASSERT_not_null(state);
    node->restoreOutgoingState(state);
}

void
PathNode::releaseOutgoingState() {
    SAWYER_THREAD_TRAITS::LockGuard lock(mutex_);
    ASSERT_require(outgoingState_);
    outgoingState_ = BS::State::Ptr();
}

void
PathNode::appendTag(const Tag::Ptr &tag) {
    ASSERT_not_null(tag);
    SAWYER_THREAD_TRAITS::LockGuard lock(mutex_);
    tags_.push_back(tag);
}

void
PathNode::appendTags(const std::vector<Tag::Ptr> &tags) {
    if (!tags.empty()) {
        ASSERT_require(std::find(tags.begin(), tags.end(), Tag::Ptr()) == tags.end());
        SAWYER_THREAD_TRAITS::LockGuard lock(mutex_);
        tags_.insert(tags_.end(), tags.begin(), tags.end());
    }
}

size_t
PathNode::nTags() const {
    SAWYER_THREAD_TRAITS::LockGuard lock(mutex_);
    return tags_.size();
}

std::vector<Tag::Ptr>
PathNode::tags() const {
    SAWYER_THREAD_TRAITS::LockGuard lock(mutex_);
    return tags_;                                       // must be a copy for thread safety
}

Sawyer::Optional<rose_addr_t>
PathNode::address() const {
    // No lock necessary since the execution unit pointer cannot change after construction
    ASSERT_not_null(executionUnit_);
    return executionUnit_->address();
}

double
PathNode::processingTime() const {
    SAWYER_THREAD_TRAITS::LockGuard lock(mutex_);
    return processingTime_;
}

void
PathNode::incrementProcessingTime(double seconds) {
    if (!rose_isnan(seconds)) {
        SAWYER_THREAD_TRAITS::LockGuard lock(mutex_);
        if (rose_isnan(processingTime_)) {
            processingTime_ = seconds;
        } else {
            processingTime_ += seconds;
        }
    }
}

std::string
PathNode::printableName() const {
    ASSERT_not_null(executionUnit_);
    return executionUnit_->printableName();
}

void
PathNode::toYamlHeader(const Settings::Ptr &settings, std::ostream &out, const std::string &prefix1) const {
    ASSERT_not_null(executionUnit_);
    executionUnit_->toYamlHeader(settings, out, prefix1);
    std::string prefix(prefix1.size(), ' ');

#if 0 // [Robb Matzke 2021-08-13]: I'm not sure it really makes sense to output this information yet
      // since we're not showing the states. The evidence variable names don't mean too much if you can't
      // look them up in the states to see where they came from. Even worse, the states for the non-final
      // nodes have probably been deleted by now to save memory.
    SmtSolver::Evidence e = evidence();
    if (!e.isEmpty()) {
        out <<prefix <<"evidence:\n";
        for (const SmtSolver::Evidence::Node &node: e.nodes()) {
            out <<prefix <<"  - name: " <<node.key() <<"\n";
            out <<prefix <<"    value: " <<*node.value() <<"\n";
        }
    }
#endif
}

void
PathNode::toYamlSteps(const Settings::Ptr &settings, std::ostream &out, const std::string &prefix1,
                      size_t stepOrigin, size_t maxSteps) const {
    executionUnit()->toYamlSteps(settings, out, prefix1, stepOrigin, maxSteps);
}

std::vector<Sarif::Location::Ptr>
PathNode::toSarif(const size_t maxSteps) const {
    return executionUnit()->toSarif(maxSteps);
}

} // namespace
} // namespace
} // namespace

#endif
