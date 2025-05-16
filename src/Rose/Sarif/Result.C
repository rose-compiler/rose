#include <featureTests.h>
#ifdef ROSE_ENABLE_SARIF
#include <Rose/Sarif/Result.h>

#include <Rose/Sarif/Analysis.h>
#include <Rose/Sarif/Artifact.h>
#include <Rose/Sarif/CodeFlow.h>
#include <Rose/Sarif/Exception.h>
#include <Rose/Sarif/Location.h>
#include <Rose/Sarif/Rule.h>
#include <Rose/StringUtility/Escape.h>
#include <ROSE_UNUSED.h>

namespace Rose {
namespace Sarif {


Result::Ptr
Result::instance(Kind kind, const std::string &mesg) {
    ASSERT_forbid(kind == Kind::FAIL);
    auto self = Ptr(new Result);
    self->kind(kind);
    self->severity(Severity::NONE);
    self->message(mesg);
    return self;
}

Result::Ptr
Result::instance(Kind kind, const std::string &mesg, const LocationPtr &location) {
    ASSERT_not_null(location);
    auto self = instance(kind, mesg);
    self->locations().push_back(location);
    return self;
}

Result::Ptr
Result::instance(Severity severity, const std::string &mesg) {
    ASSERT_forbid(severity == Severity::NONE);
    auto self = Ptr(new Result);
    self->kind(Kind::FAIL);
    self->severity(severity);
    self->message(mesg);
    return self;
}

Result::Ptr
Result::instance(Severity severity, const std::string &mesg, const LocationPtr &location) {
    ASSERT_not_null(location);
    auto self = instance(severity, mesg);
    self->locations().push_back(location);
    return self;
}

bool
Result::emit(std::ostream &out) {
    out <<"{";

    switch (kind()) {
        case Kind::PASS:
            out <<"\"kind\":\"pass\"";
            break;
        case Kind::OPEN:
            out <<"\"kind\":\"open\"";
            break;
        case Kind::INFORMATIONAL:
            out <<"\"kind\":\"informational\"";
            break;
        case Kind::NOT_APPLICABLE:
            out <<"\"kind\":\"notApplicable\"";
            break;
        case Kind::REVIEW:
            out <<"\"kind\":\"review\"";
            break;
        case Kind::FAIL:
            out <<"\"kind\":\"fail\"";
            break;
    }

    switch (severity()) {
        case Severity::NONE:
            out <<",\"level\":\"none\"";
            break;
        case Severity::NOTE:
            out <<",\"level\":\"note\"";
            break;
        case Severity::WARNING:
            out <<",\"level\":\"warning\"";
            break;
        case Severity::ERROR:
            out <<",\"level\":\"error\"";
            break;
    }

    if (!message().empty()) {
        out <<",\"message\":{"
            <<"\"text\":\"" <<StringUtility::jsonEscape(message()) <<"\""
            <<"}";
    }

    if (!id().empty())
        out <<",\"id\":\"" <<StringUtility::jsonEscape(id()) <<"\"";

    if (rule()) {
        out <<",\"ruleId\":\"" <<StringUtility::jsonEscape(rule()->id()) <<"\"";
        if (auto idx = findRuleIndex(rule())) {
            out <<",\"ruleIndex\":" <<*idx;
        } else {
            throw Sarif::Exception("Result::rule must be attached to the log before the result is emitted");
        }
    }

    if (analysisTarget()) {
        if (!findArtifactIndex(analysisTarget()))
            throw Sarif::Exception("Result::analysisTarget must be attached to the log before the result is emitted");
        out <<",\"analysisTarget\":{"
            <<"\"uri\":\"" <<StringUtility::jsonEscape(analysisTarget()->uri()) <<"\""
            <<"}";
    }

    if (!locations().empty()) {
        out <<",\"locations\":[";
        std::string sep;
        for (auto &location: locations()) {
            out <<sep;
            location->emit(out);
            sep = ",";
        }
        out <<"]";
    }

    if (!codeFlows().empty()) {
        out <<",\"codeFlows\":[";
        std::string sep;
        for (auto &codeFlow: codeFlows()) {
            out <<sep;
            codeFlow->emit(out);
            sep = ",";
        }
        out <<"]";
    }

    Node::emit(out);

    out <<"}";
    return true;
}

Sawyer::Optional<size_t>
Result::findRuleIndex(const Rule::Ptr &rule) {
    if (rule) {
        if (auto ana = findFirstAncestor<Analysis>()) {
            for (size_t i = 0; i < ana->rules().size(); ++i) {
                if (ana->rules()[i] == rule)
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
            for (size_t i = 0; i < ana->artifacts().size(); ++i) {
                if (ana->artifacts()[i] == artifact)
                    return i;
            }
        }
    }
    return Sawyer::Nothing();
}

} // namespace
} // namespace

#endif
