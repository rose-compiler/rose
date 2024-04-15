#include <featureTests.h>
#ifdef ROSE_ENABLE_MODEL_CHECKER
#include <Rose/BinaryAnalysis/ModelChecker/Path.h>

#include <Rose/BinaryAnalysis/InstructionSemantics/BaseSemantics/State.h>
#include <Rose/BinaryAnalysis/ModelChecker/ExecutionUnit.h>
#include <Rose/BinaryAnalysis/ModelChecker/PathNode.h>
#include <Rose/BinaryAnalysis/ModelChecker/Settings.h>
#include <Rose/BinaryAnalysis/SymbolicExpression.h>
#include <Rose/BitOps.h>
#include <Rose/Sarif/Location.h>
#include <Rose/StringUtility/Diagnostics.h>

#include <rose_isnan.h>

namespace BS = Rose::BinaryAnalysis::InstructionSemantics::BaseSemantics;
namespace IS = Rose::BinaryAnalysis::InstructionSemantics;

namespace Rose {
namespace BinaryAnalysis {
namespace ModelChecker {

Path::Path(const PathNode::Ptr &end)
    : end_(end) {}

Path::~Path() {}

Path::Ptr
Path::instance(const ExecutionUnit::Ptr &unit) {
    ASSERT_not_null(unit);
    auto node = PathNode::instance(unit);
    return Ptr(new Path(node));
}

Path::Ptr
Path::instance(const Path::Ptr &prefix, const ExecutionUnit::Ptr &unit, const SymbolicExpression::Ptr &assertion,
               const SmtSolver::Evidence &evidence, const BS::State::Ptr &parentOutgoingState) {
    ASSERT_not_null(prefix);
    ASSERT_not_null(unit);
    ASSERT_not_null(assertion);
    ASSERT_forbid(prefix->isEmpty());
    auto node = PathNode::instance(prefix->lastNode(), unit, assertion, evidence, parentOutgoingState);
    return Ptr(new Path(node));
}

bool
Path::isEmpty() const {
    // No lock necessary since end_ pointer cannot be changed after construction
    return end_ == nullptr;
}

size_t
Path::nNodes() const {
    // No lock necessary since the end_ pointer cannot change after the path is constructed, and the node parent pointers
    // cannot change once the nodes are created.
    size_t retval = 0;
    for (PathNode::Ptr node = end_; node; node = node->parent())
        ++retval;
    return retval;
}

size_t
Path::nSteps() const {
    // No lock necessary since the end_ pointer cannot change after the path is constructed, and the node parent pointers
    // cannot change once the nodes are created.
    size_t retval = 0;
    for (PathNode::Ptr node = end_; node; node = node->parent())
        retval += node->nSteps();
    return retval;
}

double
Path::processingTime() const {
    // No lock necessary since the end_ pointer cannot change after the path is constructed, and the node parent pointers
    // cannot change once the nodes are created.
    double retval = 0.0;
    for (PathNode::Ptr node = end_; node; node = node->parent()) {
        double nodeTime = node->processingTime();
        if (!rose_isnan(nodeTime))
            retval += nodeTime;
    }
    return retval;
}

uint64_t
Path::hash() const {
    Combinatorics::HasherSha256Builtin hasher;
    hash(hasher);
    uint64_t retval = 0;
    for (uint8_t byte: hasher.digest())
        retval = BitOps::rotateLeft(retval, 8) ^ uint64_t{byte};
    return retval;
}

void
Path::hash(Combinatorics::Hasher &hasher) const {
    // No lock necessary since the end_ pointer cannot change after the path is constructed, and the node parent pointers
    // cannot change once the nodes are created.
    for (PathNode::Ptr node = end_; node; node = node->parent()) {
        if (auto va = node->executionUnit()->address()) {
            hasher.insert(*va);
        } else {
            hasher.insert(1);                           // arbitrary, and unlikely to be an instruction address
        }
        hasher.insert(node->executionUnit()->nSteps());
    }
}

bool
Path::executionFailed() const {
    // No lock necessary since end_ pointer cannot be changed after construction
    return end_ ? end_->executionFailed() : false;
}

std::vector<SymbolicExpression::Ptr>
Path::assertions() const {
    // No lock necessary since the end_ pointer cannot change after the path is constructed, and the node parent pointers
    // cannot change once the nodes are created.
    std::vector<SymbolicExpression::Ptr> retval;
    for (PathNode::Ptr node = end_; node; node = node->parent()) {
        std::vector<SymbolicExpression::Ptr> nodeAssertions = node->assertions();
        retval.insert(retval.end(), nodeAssertions.begin(), nodeAssertions.end());
    }
    return retval;
}

PathNode::Ptr
Path::lastNode() const {
    // No lock necessary since the end_ pointer can't change after the path is constructed.
    return end_;
}

PathNode::Ptr
Path::firstNode() const {
    PathNode::Ptr retval;
    for (PathNode::Ptr node = end_; node; node = node->parent())
        retval = node;
    return retval;
}

std::vector<PathNode::Ptr>
Path::nodes() const {
    std::vector<PathNode::Ptr> retval;
    for (PathNode::Ptr node = end_; node; node = node->parent())
        retval.push_back(node);
    std::reverse(retval.begin(), retval.end());
    return retval;
}

std::string
Path::printableName() const {
    // FIXME[Robb Matzke 2021-03-30]: this name isn't unique to this path
    if (end_) {
        return "from " + firstNode()->executionUnit()->printableName() + " to " + end_->executionUnit()->printableName();
    } else {
        return "empty path";
    }
}

void
Path::print(const Settings::Ptr &settings, std::ostream &out, const std::string &prefix, size_t maxSteps) const {
    if (isEmpty()) {
        out <<prefix <<"empty path\n";
    } else {
        std::vector<PathNode::Ptr> vertices = nodes();
        maxSteps = std::min(maxSteps, nSteps());
        out <<prefix <<"path contains " <<StringUtility::plural(maxSteps, "steps") <<"\n";
        size_t step = 0;
        for (size_t i = 0; i < vertices.size() && maxSteps > 0; ++i) {
            ExecutionUnit::Ptr unit = vertices[i]->executionUnit();
            out <<prefix <<"  " <<vertices[i]->printableName() <<"\n";
            unit->printSteps(settings, out, prefix+"    ", step, maxSteps);
            step += unit->nSteps();
            maxSteps -= std::min(maxSteps, vertices[i]->nSteps());
        }
    }
}

void
Path::toYaml(const Settings::Ptr &settings, std::ostream &out, const std::string &prefix1, size_t maxSteps) const {
    if (!isEmpty()) {
        std::vector<PathNode::Ptr> vertices = nodes();
        maxSteps = std::min(maxSteps, nSteps());
        out <<prefix1 <<"path:\n";
        std::string prefix(prefix1.size(), ' ');
        size_t step = 0;
        for (size_t i = 0; i < vertices.size() && maxSteps > 0; ++i) {
            vertices[i]->toYamlHeader(settings, out, "    - ");
            vertices[i]->toYamlSteps(settings, out, "      ", step, maxSteps);
            step += vertices[i]->nSteps();
            maxSteps -= std::min(maxSteps, vertices[i]->nSteps());
        }
    }
}

std::vector<Sarif::Location::Ptr>
Path::toSarif(size_t maxSteps) const {
    std::vector<Sarif::Location::Ptr> retval;
    if (!isEmpty()) {
        maxSteps = std::min(maxSteps, nSteps());
        const std::vector<PathNode::Ptr> vertices = nodes();
        for (size_t i = 0; i < vertices.size() && maxSteps > 0; ++i) {
            const std::vector<Sarif::Location::Ptr> locations = vertices[i]->toSarif(maxSteps);
            retval.insert(retval.end(), locations.begin(), locations.end());
            maxSteps -= std::min(maxSteps, locations.size());
        }
    }
    return retval;
}

} // namespace
} // namespace
} // namespace

#endif
