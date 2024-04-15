#include <featureTests.h>
#ifdef ROSE_ENABLE_MODEL_CHECKER
#include <Rose/BinaryAnalysis/ModelChecker/Tag.h>

#include <Rose/Sarif/Analysis.h>
#include <Rose/Sarif/Result.h>
#include <Rose/StringUtility/Escape.h>

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
NameTag::name() const {
    // No lock necessary since the name is never modified after construction.
    return name_;
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

void
NameTag::toYaml(std::ostream &out, const std::string &prefix) const {
    out <<prefix <<StringUtility::yamlEscape(name_) <<"\n";
}

Sarif::Result::Ptr
NameTag::toSarif(const Sarif::Analysis::Ptr&) const {
    return Sarif::Result::instance(Sarif::Severity::ERROR, name_);
}

} // namespace
} // namespace
} // namespace

#endif
