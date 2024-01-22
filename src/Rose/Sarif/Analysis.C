#include <featureTests.h>
#ifdef ROSE_ENABLE_SARIF
#include <sage3basic.h>
#include <Rose/Sarif/Analysis.h>

#include <Rose/Sarif/Artifact.h>
#include <Rose/Sarif/Exception.h>
#include <Rose/Sarif/Result.h>
#include <Rose/Sarif/Rule.h>

#include <boost/bind/bind.hpp>

using namespace boost::placeholders;

namespace Rose {
namespace Sarif {

Analysis::~Analysis() {}

Analysis::Analysis(const std::string &name)
    : name_(name), rules(*this), artifacts(*this), results(*this) {
    rules.beforeResize(boost::bind(&Analysis::checkRulesResize, this, _1, _2));
    rules.afterResize(boost::bind(&Analysis::handleRulesResize, this, _1, _2));

    artifacts.beforeResize(boost::bind(&Analysis::checkArtifactsResize, this, _1, _2));
    artifacts.afterResize(boost::bind(&Analysis::handleArtifactsResize, this, _1, _2));

    results.beforeResize(boost::bind(&Analysis::checkResultsResize, this, _1, _2));
    results.afterResize(boost::bind(&Analysis::handleResultsResize, this, _1, _2));
}

Analysis::Ptr
Analysis::instance(const std::string &name) {
    return Ptr(new Analysis(name));
}

const std::string&
Analysis::name() const {
    return name_;
}

const std::string&
Analysis::version() const {
    return version_;
}

void
Analysis::version(const std::string &s) {
    if (s == version_)
        return;

    checkPropertyChange("Anaylsis", "version", version_.empty(),
                        {
                            // Group 2
                            {"rules", rules.empty()},

                            // Group 3
                            {"commandLine", commandLine_.empty()},
                            {"exitStatus", !exitStatus_},

                            // Group 4
                            {"artifacts", artifacts.empty()},
                            {"results", results.empty()},
                        });

    version_ = s;

    if (isIncremental())
        emitVersion(incrementalStream(), emissionPrefix());
}

void
Analysis::emitVersion(std::ostream &out, const std::string &firstPrefix) {
    if (!version_.empty())
        out <<firstPrefix <<"version: " <<StringUtility::yamlEscape(version_) <<"\n";
}

const std::string&
Analysis::informationUri() const {
    return informationUri_;
}

void
Analysis::informationUri(const std::string &s) {
    if (s == informationUri_)
        return;
    checkPropertyChange("Anslysis", "informationUri", informationUri_.empty(),
                        {
                            // Group 2
                            {"rules", rules.empty()},

                            // Group 3
                            {"commandLine", commandLine_.empty()},
                            {"exitStatus", !exitStatus_},

                            // Group 4
                            {"artifacts", artifacts.empty()},
                            {"results", results.empty()}
                        });
    informationUri_ = s;
    if (isIncremental())
        emitInformationUri(incrementalStream(), emissionPrefix());
}

void
Analysis::emitInformationUri(std::ostream &out, const std::string &firstPrefix) {
    if (!informationUri_.empty())
        out <<firstPrefix <<"informationUri: " <<StringUtility::yamlEscape(informationUri_) <<"\n";
}

void
Analysis::checkRulesResize(int delta, const Rule::Ptr &rule) {
    if (!rule)
        throw Sarif::Exception("cannot add null rule to an analysis");
    if (isIncremental() && delta < 0)
        throw IncrementalError("rule cannot be removed from an analysis");
    checkPropertyChange("Analysis", "rules", true,
                        {
                            // Group 3
                            {"commandLine", commandLine_.empty()},
                            {"exitStatus", !exitStatus_},

                            // Group 4
                            {"artifacts", artifacts.empty()},
                            {"results", results.empty()}
                        });
}

void
Analysis::handleRulesResize(int delta, const Rule::Ptr &rule) {
    if (isIncremental()) {
        ASSERT_require(1 == delta);
        ASSERT_forbid(rules.empty());

        // Make sure we can't change this pointer in the future
        lock(rules.back(), "rules");

        // Prior rules can no longer be modified
        if (rules.size() >= 2)
            rules[rules.size() - 2]->freeze();

        // Emit this new rule
        std::ostream &out = incrementalStream();
        const std::string p = emissionPrefix();
        if (1 == rules.size())
            out <<p <<"rules:\n";
        rule->emitYaml(out, makeListPrefix(p));
    }
}

const std::vector<std::string>&
Analysis::commandLine() const {
    return commandLine_;
}

void
Analysis::commandLine(const std::vector<std::string> &cmd) {
    if (std::equal(commandLine_.begin(), commandLine_.end(), cmd.begin(), cmd.end()))
        return;
    checkPropertyChange("Anaylsis", "commandLine", commandLine_.empty(),
                        {
                            // Group 3
                            {"exitStatus", !exitStatus_},

                            // Group 4
                            {"artifacts", artifacts.empty()},
                            {"results", results.empty()}
                        });
    commandLine_ = cmd;
    if (isIncremental())
        emitCommandLine(incrementalStream(), emissionPrefix());
}

void
Analysis::emitCommandLine(std::ostream &out, const std::string &firstPrefix) {
    if (!commandLine_.empty()) {
        out <<firstPrefix <<"invocations:\n";

        std::string pList = makeListPrefix(firstPrefix);
        if (!commandLine_.empty()) {
            out <<pList <<"commandLine: " <<StringUtility::yamlEscape(makeShellCommand(commandLine_)) <<"\n";
            pList = makeNextPrefix(pList);
        }

        if (commandLine_.size() > 1) {
            out <<pList <<"arguments:\n";
            const std::string argPrefix = makeListPrefix(pList);
            for (size_t i = 1; i < commandLine_.size(); ++i)
                out <<argPrefix <<StringUtility::yamlEscape(commandLine_[i]) <<"\n";
            pList = makeNextPrefix(pList);
        }
    }
}

const Sawyer::Optional<int>&
Analysis::exitStatus() const {
    return exitStatus_;
}

void
Analysis::exitStatus(const Sawyer::Optional<int> &status) {
    if (status.isEqual(exitStatus_))
        return;
    checkPropertyChange("Anaylsis", "exitStatus", !exitStatus_,
                        {
                            // Group 4
                            {"artifacts", artifacts.empty()},
                            {"results", results.empty()}
                        });
    exitStatus_ = status;
    if (isIncremental())
        emitExitStatus(incrementalStream(), emissionPrefix());
}

void
Analysis::emitExitStatus(std::ostream &out, const std::string &firstPrefix) {
    if (exitStatus_) {
        // Exit status lives inside an object listed under "invocations". If we just emitted a command-line, then the object to
        // receive the exit status is still open, otherwise we have to create the "invocations" list and the object that holds the
        // exit status.
        const std::string listPrefix = makeListPrefix(firstPrefix);
        const std::string pp = makeNextPrefix(listPrefix);

        if (commandLine_.empty()) {
            out <<firstPrefix <<"invocations:\n";
            out <<listPrefix <<"exitCode: " <<*exitStatus_ <<"\n";
        } else {
            out <<pp <<"exitCode: " <<*exitStatus_ <<"\n";
        }

        if (*exitStatus_ == 0) {
            out <<pp <<"executionSuccessful: true\n";
        } else {
            out <<pp <<"executionSuccessful: false\n";
        }
    }
}

void
Analysis::checkArtifactsResize(int delta, const Artifact::Ptr &artifact) {
    if (!artifact)
        throw Sarif::Exception("cannot add null artifact to an analysis");
    if (isIncremental() && delta < 0)
        throw IncrementalError("artifact cannot be removed from an analysis");
    checkPropertyChange("Analysis", "artifacts", true,
                        {
                            // Group 4
                            {"results", results.empty()}
                        });
}

void
Analysis::handleArtifactsResize(int delta, const Artifact::Ptr &artifact) {
    if (isIncremental()) {
        ASSERT_require(1 == delta);
        ASSERT_forbid(artifacts.empty());

        // Make sure we can't change this pointer in the future
        lock(artifacts.back(), "rules");

        // Prior rules can no longer be modified. All but the last are already frozen
        if (!rules.empty())
            rules.back()->freeze();

        // Prior artifacts can no longer be modified
        if (artifacts.size() >= 2)
            artifacts[artifacts.size() - 2]->freeze();

        // Emit this new artifact
        std::ostream &out = incrementalStream();
        const std::string p = emissionPrefix();
        if (1 == artifacts.size())
            out <<p <<"artifacts:\n";
        artifact->emitYaml(out, makeListPrefix(p));
    }
}

void
Analysis::checkResultsResize(int delta, const Result::Ptr &result) {
    if (!result)
        throw Sarif::Exception("cannot add null result to an anaysis");
    if (isIncremental() && delta < 0)
        throw IncrementalError("result cannot be removed from an analysis");
    checkPropertyChange("Analysis", "results", true, {});
}

void
Analysis::handleResultsResize(int delta, const Result::Ptr &result) {
    if (isIncremental()) {
        ASSERT_require(1 == delta);
        ASSERT_forbid(results.empty());

        // Make sure we can't change this pointer in the future
        lock(results.back(), "results");

        // Prior rules and artifacts can no longer be modified. All but the last is already frozen.
        if (!artifacts.empty()) {
            artifacts.back()->freeze();
        } else if (!rules.empty()) {
            rules.back()->freeze();
        }

        // Prior result can no longer be modified
        if (results.size() >= 2)
            results[results.size() - 2]->freeze();

        // Emit this new result
        std::ostream &out = incrementalStream();
        const std::string p = emissionPrefix();
        if (1 == results.size())
            out <<p <<"results:\n";
        result->emitYaml(out, makeListPrefix(p));
    }
}

std::string
Analysis::makeShellCommand(const std::vector<std::string> &args) {
    std::string retval;
    for (const std::string &arg: args)
        retval += (retval.empty() ? "" : " ") + StringUtility::bourneEscape(arg);
    return retval;
}

Rule::Ptr
Analysis::findRuleById(const std::string &id) {
    for (auto &rule: rules) {
        if (rule->id() == id)
            return rule();
    }
    return nullptr;
}

Rule::Ptr
Analysis::findRuleByName(const std::string &name) {
    for (auto &rule: rules) {
        if (rule->name() == name)
            return rule();
    }
    return nullptr;
}

void
Analysis::emitYaml(std::ostream &out, const std::string &firstPrefix) {
    const std::string p = makeNextPrefix(firstPrefix);
    const std::string pp = makeObjectPrefix(p);
    const std::string ppp = makeObjectPrefix(pp);

    out <<firstPrefix <<"tool:\n";

    // Group 1: tool.driver
    out <<pp <<"driver:\n";
    out <<ppp <<"name: " <<StringUtility::yamlEscape(name_) <<"\n";
    emitVersion(out, ppp);
    emitInformationUri(out, ppp);

    // Group 2, tool.driver.rules
    if (!rules.empty()) {
        out <<ppp <<"rules:\n";
        const std::string pList = makeListPrefix(ppp);
        for (auto &rule: rules) {
            rule->emitYaml(out, pList);
            if (isIncremental()) {
                lock(rule, "rules");
                if (rule != rules.back() || !artifacts.empty() || !results.empty())
                    rule->freeze();
            }
        }
    }

    // Group 3: tool
    emitCommandLine(out, p);
    emitExitStatus(out, p);

    // Group 4: tool
    if (!artifacts.empty()) {
        out <<p <<"artifacts:\n";
        const std::string pList = makeListPrefix(p);
        for (auto &artifact: artifacts) {
            artifact->emitYaml(out, pList);
            if (isIncremental()) {
                lock(artifact, "artifacts");
                if (artifact != artifacts.back() || !results.empty())
                    artifact->freeze();
            }
        }
    }
    if (!results.empty()) {
        out <<p <<"results:\n";
        const std::string pList = makeListPrefix(p);
        for (auto &result: results) {
            result->emitYaml(out, pList);
            if (isIncremental()) {
                lock(result, "results");
                if (result != results.back())
                    result->freeze();
            }
        }
    }
}

std::string
Analysis::emissionPrefix() {
    return makeObjectPrefix(makeObjectPrefix(parent->emissionPrefix()));
}

} // namespace
} // namespace

#endif
