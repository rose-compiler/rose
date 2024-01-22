#include <featureTests.h>
#ifdef ROSE_ENABLE_SARIF
#include <sage3basic.h>
#include <Rose/Sarif/Result.h>

#include <Rose/Sarif/Analysis.h>
#include <Rose/Sarif/Artifact.h>
#include <Rose/Sarif/Exception.h>
#include <Rose/Sarif/Location.h>
#include <Rose/Sarif/Rule.h>

#include <boost/bind/bind.hpp>

using namespace boost::placeholders;

namespace Rose {
namespace Sarif {

Result::~Result() {}

Result::Result(Kind kind, Severity severity, const std::string &mesg)
    : kind_(kind), severity_(severity), message_(mesg), locations(*this) {
    ASSERT_require((kind == Kind::FAIL && severity != Severity::NONE) ||
                   (kind != Kind::FAIL && severity == Severity::NONE));
    locations.beforeResize(boost::bind(&Result::checkLocationsResize, this, _1, _2));
    locations.afterResize(boost::bind(&Result::handleLocationsResize, this, _1, _2));
}

Result::Ptr
Result::instance(Kind kind, const std::string &mesg) {
    ASSERT_forbid(kind == Kind::FAIL);
    return Ptr(new Result(kind, Severity::NONE, mesg));
}

Result::Ptr
Result::instance(Kind kind, const std::string &mesg, const LocationPtr &location) {
    ASSERT_not_null(location);
    auto result = instance(kind, mesg);
    result->locations.push_back(location);
    return result;
}

Result::Ptr
Result::instance(Severity severity, const std::string &mesg) {
    ASSERT_forbid(severity == Severity::NONE);
    return Ptr(new Result(Kind::FAIL, severity, mesg));
}

Result::Ptr
Result::instance(Severity severity, const std::string &mesg, const LocationPtr &location) {
    ASSERT_not_null(location);
    auto result = instance(severity, mesg);
    result->locations.push_back(location);
    return result;
}

Kind
Result::kind() const {
    return kind_;
}

Severity
Result::severity() const {
    return severity_;
}

const std::string&
Result::message() const {
    return message_;
}

void
Result::message(const std::string &s) {
    if (s == message_)
        return;
    checkPropertyChange("Result", "message", message_.empty(), {{"locations", locations.empty()}});
    message_ = s;
    if (isIncremental())
        emitMessage(incrementalStream(), emissionPrefix());
}

void
Result::emitMessage(std::ostream &out, const std::string &firstPrefix) {
    if (!message_.empty()) {
        out <<firstPrefix <<"message:\n";
        const std::string pp = makeObjectPrefix(firstPrefix);
        out <<pp <<"text: " <<StringUtility::yamlEscape(message_) <<"\n";
    }
}

const std::string&
Result::id() const {
    return id_;
}

void
Result::id(const std::string &s) {
    if (s == id_)
        return;
    checkPropertyChange("Result", "id", id_.empty(), {{"locations", locations.empty()}});
    id_ = s;
    if (isIncremental())
        emitId(incrementalStream(), emissionPrefix());
}

void
Result::emitId(std::ostream &out, const std::string &firstPrefix) {
    if (!id_.empty())
        out <<firstPrefix <<"id: " <<StringUtility::yamlEscape(id_) <<"\n";
}

Rule::Ptr
Result::rule() const {
    return rule_;
}

void
Result::rule(const Rule::Ptr &r) {
    if (r == rule_)
        return;
    checkPropertyChange("Result", "rule", !rule_, {{"locations", locations.empty()}});
    if (isIncremental() && !findRuleIndex(r))
        throw IncrementalError::notAttached("Result::rule");
    rule_ = r;
    if (isIncremental())
        emitRule(incrementalStream(), emissionPrefix());
}

void
Result::emitRule(std::ostream &out, const std::string &firstPrefix) {
    if (rule_) {
        out <<firstPrefix <<"ruleId: " <<StringUtility::yamlEscape(rule_->id()) <<"\n";
        const std::string p = makeNextPrefix(firstPrefix);
        if (auto idx = findRuleIndex(rule_)) {
            out <<p <<"ruleIndex: " <<*idx <<"\n";
        } else {
            throw Sarif::Exception("Result::rule must be attached to the log before the result is emitted");
        }
    }
}

Artifact::Ptr
Result::analysisTarget() const {
    return analysisTarget_;
}

void
Result::analysisTarget(const Artifact::Ptr &artifact) {
    if (artifact == analysisTarget_)
        return;
    checkPropertyChange("Result", "analysisTarget", !analysisTarget_, {{"locations", locations.empty()}});
    if (isIncremental() && !findArtifactIndex(artifact))
        throw IncrementalError::notAttached("Result::analysisTarget");
    analysisTarget_ = artifact;
    if (isIncremental())
        emitAnalysisTarget(incrementalStream(), emissionPrefix());
}

void
Result::emitAnalysisTarget(std::ostream &out, const std::string &firstPrefix) {
    if (analysisTarget_) {
        if (!findArtifactIndex(analysisTarget_))
            throw Sarif::Exception("Result::analysisTarget must be attached to the log before the result is emitted");
        out <<firstPrefix <<"analysisTarget:\n";
        const std::string pp = makeObjectPrefix(firstPrefix);
        out <<pp <<"uri: " <<StringUtility::yamlEscape(analysisTarget_->uri()) <<"\n";
    }
}

void
Result::checkLocationsResize(int delta, const Location::Ptr &location) {
    if (!location)
        throw Sarif::Exception("cannot add null location to a result");
    if (isIncremental() && delta < 0)
        throw IncrementalError("location cannot be removed from a result");
}

void
Result::handleLocationsResize(int delta, const Location::Ptr &location) {
    if (isIncremental()) {
        ASSERT_require(1 == delta);
        ASSERT_forbid(locations.empty());

        // Make sure we can't change this pointer in the future
        lock(locations.back(), "locations");

        // Prior location can no longer be modified
        if (locations.size() >= 2)
            locations[locations.size() - 2]->freeze();

        // Emit this new location
        std::ostream &out = incrementalStream();
        const std::string p = emissionPrefix();
        if (1 == locations.size())
            out <<p <<"locations:\n";
        location->emitYaml(out, makeListPrefix(p));
    }
}

void
Result::emitYaml(std::ostream &out, const std::string &firstPrefix) {
    switch (kind_) {
        case Kind::PASS:
            out <<firstPrefix <<"kind: pass\n";
            break;
        case Kind::OPEN:
            out <<firstPrefix <<"kind: open\n";
            break;
        case Kind::INFORMATIONAL:
            out <<firstPrefix <<"kind: informational\n";
            break;
        case Kind::NOT_APPLICABLE:
            out <<firstPrefix <<"kind: notApplicable\n";
            break;
        case Kind::REVIEW:
            out <<firstPrefix <<"kind: review\n";
            break;
        case Kind::FAIL:
            out <<firstPrefix <<"kind: fail\n";
            break;
    }

    const std::string p = makeNextPrefix(firstPrefix);
    const std::string pp = makeObjectPrefix(p);

    switch (severity_) {
        case Severity::NONE:
            out <<p <<"level: none\n";
            break;
        case Severity::NOTE:
            out <<p <<"level: note\n";
            break;
        case Severity::WARNING:
            out <<p <<"level: warning\n";
            break;
        case Severity::ERROR:
            out <<p <<"level: error\n";
            break;
    }

    emitMessage(out, p);
    emitId(out, p);
    emitRule(out, p);
    emitAnalysisTarget(out, p);

    if (!locations.empty()) {
        out <<p <<"locations:\n";
        for (auto &location: locations) {
            location->emitYaml(out, makeListPrefix(p));
            if (isIncremental()) {
                lock(location, "locations");
                if (location != locations.back())
                    location->freeze();
            }
        }
    }
}

std::string
Result::emissionPrefix() {
    return makeObjectPrefix(makeObjectPrefix(parent->emissionPrefix()));
}

Sawyer::Optional<size_t>
Result::findRuleIndex(const Rule::Ptr &rule) {
    if (rule) {
        if (auto ana = findFirstAncestor<Analysis>()) {
            for (size_t i = 0; i < ana->rules.size(); ++i) {
                if (ana->rules[i] == rule)
                    return i;
            }
        }
    }
    return Sawyer::Nothing();
}

Sawyer::Optional<size_t>
Result::findArtifactIndex(const Artifact::Ptr &artifact) {
    if (artifact) {
        if (auto ana = findFirstAncestor<Analysis>()) {
            for (size_t i = 0; i < ana->artifacts.size(); ++i) {
                if (ana->artifacts[i] == artifact)
                    return i;
            }
        }
    }
    return Sawyer::Nothing();
}

} // namespace
} // namespace

#endif
