#include <featureTests.h>
#ifdef ROSE_ENABLE_SARIF
#include <Rose/Sarif/Analysis.h>

#include <Rose/Sarif/Artifact.h>
#include <Rose/Sarif/Exception.h>
#include <Rose/Sarif/Result.h>
#include <Rose/Sarif/Rule.h>
#include <Rose/StringUtility/Escape.h>
#include <ROSE_UNUSED.h>

namespace Rose {
namespace Sarif {

std::string
Analysis::makeShellCommand(const std::vector<std::string> &args) {
    std::string retval;
    for (const std::string &arg: args)
        retval += (retval.empty() ? "" : " ") + StringUtility::bourneEscape(arg);
    return retval;
}

Rule::Ptr
Analysis::findRuleById(const std::string &id) {
    for (auto &rule: rules()) {
        if (rule->id() == id)
            return rule();
    }
    return nullptr;
}

Rule::Ptr
Analysis::findRuleByName(const std::string &name) {
    for (auto &rule: rules()) {
        if (rule->name() == name)
            return rule();
    }
    return nullptr;
}

bool
Analysis::emit(std::ostream &out) {
    out <<"{";

    {
        out <<"\"tool\":{"
            <<"\"driver\":{"
            <<"\"name\":\"" <<StringUtility::jsonEscape(name()) <<"\"";
        if (!version().empty())
            out <<",\"version\":\"" <<StringUtility::jsonEscape(version()) <<"\"";

        if (!informationUri().empty())
            out <<",\"informationUri\":\"" <<StringUtility::jsonEscape(informationUri()) <<"\"";

        if (!rules().empty()) {
            out <<",\"rules\":[";
            std::string sep;
            for (auto &rule: rules()) {
                out <<sep;
                rule->emit(out);
                sep = ",";
            }
            out <<"]";
        }
        out <<"}}";
    }

    if (!commandLine().empty() || exitStatus()) {
        out <<",\"invocations\":[{";

        if (!commandLine().empty()) {
            out <<"\"commandLine\":\"" <<StringUtility::jsonEscape(makeShellCommand(commandLine())) <<"\"";
            if (commandLine().size() > 1) {
                out <<",\"arguments\":[";
                for (size_t i = 1; i < commandLine().size(); ++i)
                    out <<(1==i?"":",") <<"\"" <<StringUtility::jsonEscape(commandLine()[i]) <<"\"";
                out <<"]";
            }
        }

        if (exitStatus()) {
            out <<",\"exitCode\": " <<*exitStatus()
                <<",\"executionSuccessful\":" <<(*exitStatus() == 0 ? "true" : "false");
        }

        out <<"}]";
    }

    if (!artifacts().empty()) {
        out <<",\"artifacts\":[";
        std::string sep;
        for (auto &artifact: artifacts()) {
            out <<sep;
            artifact->emit(out);
            sep = ",";
        }
        out <<"]";
    }

    if (!results().empty()) {
        out <<",\"results\":[";
        std::string sep;
        for (auto &result: results()) {
            out <<sep;
            result->emit(out);
            sep = ",";
        }
        out <<"]";
    }

    out <<"}";
    return true;
}

} // namespace
} // namespace

#endif
