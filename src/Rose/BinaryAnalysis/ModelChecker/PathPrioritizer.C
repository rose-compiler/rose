#include <featureTests.h>
#ifdef ROSE_ENABLE_MODEL_CHECKER
#include <sage3basic.h>
#include <Rose/BinaryAnalysis/ModelChecker/PathPrioritizer.h>

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

} // namespace
} // namespace
} // namespace

#endif
