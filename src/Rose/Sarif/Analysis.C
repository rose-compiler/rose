#include <sage3basic.h>
#include <Rose/Sarif/Analysis.h>

#include <Rose/Sarif/Exception.h>
#include <Rose/Sarif/Result.h>

#include <boost/bind/bind.hpp>

using namespace boost::placeholders;

namespace Rose {
namespace Sarif {

Analysis::~Analysis() {}

Analysis::Analysis(const std::string &name)
    : name_(name), results(*this) {
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

const std::vector<std::string>&
Analysis::commandLine() const {
    return commandLine_;
}

void
Analysis::commandLine(const std::vector<std::string> &cmd) {
    if (std::equal(commandLine_.begin(), commandLine_.end(), cmd.begin(), cmd.end()))
        return;
    if (isFrozen())
        throw IncrementalError::frozenObject("Analysis");
    if (isIncremental()) {
        if (!commandLine_.empty())
            throw IncrementalError::cannotChangeValue("Analysis::commandLine");
        if (exitStatus_)
            throw IncrementalError::cannotSetAfter("Analysis::commandLine", "Analysis::exitStatus");
        if (!results.empty())
            throw IncrementalError::cannotSetAfter("Analysis::commandLine", "Analysis::results");
    }

    // Modify
    commandLine_ = cmd;

    // Emit new yaml
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
    if (isFrozen())
        throw IncrementalError::frozenObject("Analysis");
    if (isIncremental()) {
        if (exitStatus_)
            throw IncrementalError::cannotChangeValue("Analysis::exitStatus");
        if (!results.empty())
            throw IncrementalError::cannotSetAfter("Analysis::exitStatus", "Analysis::results");
    }

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
        if (commandLine_.empty()) {
            out <<firstPrefix <<"invocations:\n";
            out <<makeListPrefix(firstPrefix) <<"exitCode: " <<*exitStatus_ <<"\n";

        } else {
            out <<makeNextPrefix(makeListPrefix(firstPrefix)) <<"exitCode: " <<*exitStatus_ <<"\n";
        }
    }
}

void
Analysis::checkResultsResize(int delta, const Result::Ptr &result) {
    if (!result)
        throw Sarif::Exception("cannot add null result to an anaysis");
    if (isIncremental() && delta < 0)
        throw IncrementalError("result cannot be removed from an analysis");
}

void
Analysis::handleResultsResize(int delta, const Result::Ptr &result) {
    if (isIncremental()) {
        ASSERT_require(1 == delta);
        ASSERT_forbid(results.empty());

        // Make sure we can't change this pointer in the future
        results.back().beforeChange([](const Node::Ptr&, const Node::Ptr&) {
            throw IncrementalError("result cannot be reassigned");
        });

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

void
Analysis::emitYaml(std::ostream &out, const std::string &firstPrefix) {
    const std::string p = makeNextPrefix(firstPrefix);
    const std::string pp = makeObjectPrefix(p);
    const std::string ppp = makeObjectPrefix(pp);

    out <<firstPrefix <<"tool:\n";
    out <<pp <<"driver:\n";
    out <<ppp <<"name: " <<StringUtility::yamlEscape(name_) <<"\n";

    emitCommandLine(out, p);
    emitExitStatus(out, p);

    if (!results.empty()) {
        out <<p <<"results:\n";
        const std::string pList = makeListPrefix(p);
        for (const auto &result: results) {
            result->emitYaml(out, pList);
            if (isIncremental() && result != results.back())
                result->freeze();
        }
    }
}

std::string
Analysis::emissionPrefix() {
    return makeObjectPrefix(makeObjectPrefix(parent->emissionPrefix()));
}

} // namespace
} // namespace
