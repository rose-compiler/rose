#include <featureTests.h>
#ifdef ROSE_ENABLE_MODEL_CHECKER
#include <Rose/BinaryAnalysis/ModelChecker/PathPrioritizer.h>

#include <Rose/BinaryAnalysis/ModelChecker/ExecutionUnit.h>
#include <Rose/BinaryAnalysis/ModelChecker/Path.h>
#include <Rose/BinaryAnalysis/ModelChecker/PathNode.h>

namespace Rose {
namespace BinaryAnalysis {
namespace ModelChecker {

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// PathPrioritizer
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

PathPrioritizer::PathPrioritizer() {}

PathPrioritizer::~PathPrioritizer() {}

// This is not pure virtual because we need to use it in std::make_heap et al, which
// apparently needs to be able to instantiate these for its concept checks.
bool
PathPrioritizer::operator()(const Path::Ptr &/*worse*/, const Path::Ptr &/*better*/) const {
    ASSERT_not_reachable("you need to use a PathPrioritizer subclass");
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ShortestPathFirst
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

ShortestPathFirst::Ptr
ShortestPathFirst::instance() {
    return std::make_shared<ShortestPathFirst>();
}

bool
ShortestPathFirst::operator()(const Path::Ptr &worse, const Path::Ptr &better) const {
    ASSERT_not_null(worse);
    ASSERT_not_null(better);
    return better->nSteps() < worse->nSteps();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// LongestPathFirst
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

LongestPathFirst::Ptr
LongestPathFirst::instance() {
    return std::make_shared<LongestPathFirst>();
}

bool
LongestPathFirst::operator()(const Path::Ptr &worse, const Path::Ptr &better) const {
    ASSERT_not_null(worse);
    ASSERT_not_null(better);
    return better->nSteps() > worse->nSteps();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// FastestPathFirst
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

FastestPathFirst::Ptr
FastestPathFirst::instance() {
    return std::make_shared<FastestPathFirst>();
}

bool
FastestPathFirst::operator()(const Path::Ptr &worse, const Path::Ptr &better) const {
    ASSERT_not_null(worse);
    ASSERT_not_null(better);
    return better->processingTime() < worse->processingTime();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// RandomPathFirst
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

RandomPathFirst::Ptr
RandomPathFirst::instance() {
    return std::make_shared<RandomPathFirst>();
}

bool
RandomPathFirst::operator()(const Path::Ptr &worse, const Path::Ptr &better) const {
    ASSERT_not_null(worse);
    ASSERT_not_null(better);
    ASSERT_forbid(worse->isEmpty());
    ASSERT_forbid(better->isEmpty());
    return better->lastNode()->id() < worse->lastNode()->id();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// BestCoverageFirst
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

BestCoverageFirst::Ptr
BestCoverageFirst::instance() {
    return std::make_shared<BestCoverageFirst>();
}

double
BestCoverageFirst::coverageRatio(const Path::Ptr &path) {
    ASSERT_not_null(path);
    std::set<Address> nodes;
    size_t nNodes = 0;
    for (const PathNode::Ptr &pnode: path->nodes()) {
        if (const auto va = pnode->executionUnit()->address()) {
            nodes.insert(*va);
            ++nNodes;
        }
    }
    return nNodes > 0 ? (double)nodes.size() / nNodes : 0.0;
}

bool
BestCoverageFirst::operator()(const Path::Ptr &worse, const Path::Ptr &better) const {
    ASSERT_not_null(worse);
    ASSERT_not_null(better);
    ASSERT_forbid(worse->isEmpty());
    ASSERT_forbid(better->isEmpty());

    return coverageRatio(worse) < coverageRatio(better);
}

} // namespace
} // namespace
} // namespace

#endif
