#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <sage3basic.h>
#include <Rose/BinaryAnalysis/ModelChecker/Tag.h>

namespace Rose {
namespace BinaryAnalysis {
namespace ModelChecker {

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Base class
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Tag::Tag(size_t nodeStep)
    : nodeStep_(nodeStep) {}

Tag::~Tag() {}

size_t
Tag::nodeStep() const {
    // no lock necessary since the node step is read-only
    return nodeStep_;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// NameTag
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

NameTag::NameTag(size_t nodeStep, const std::string &name)
    : Tag(nodeStep), name_(name) {}

NameTag::Ptr
NameTag::instance(size_t nodeStep, const std::string &name) {
    ASSERT_forbid(name.empty());
    return Ptr(new NameTag(nodeStep, name));
}

std::string
NameTag::printableName() const {
    ASSERT_forbid(name_.empty());
    return "tag " + name_;
}

void
NameTag::print(std::ostream &out, const std::string &prefix) const {
    out <<prefix <<printableName() <<"\n";
}

} // namespace
} // namespace
} // namespace

#endif
