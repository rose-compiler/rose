#include <featureTests.h>
#ifdef ROSE_ENABLE_SARIF
#include <Rose/Sarif/Artifact.h>

#include <Rose/Sarif/Exception.h>
#include <Rose/StringUtility/Escape.h>

namespace Rose {
namespace Sarif {

void
Artifact::hash(const std::string &name, const std::string &value) {
    hash(std::make_pair(name, value));
}

bool
Artifact::emit(std::ostream &out) {
    if (uri().empty() && mimeType().empty() && sourceLanguage().empty() && hash().first.empty() && hash().second.empty() &&
        description().empty()) {
        return false;
    } else {
        std::string sep;
        out <<"{";

        if (!uri().empty()) {
            out <<sep <<"\"location\":{"
                <<"\"uri\":\"" <<StringUtility::jsonEscape(uri()) <<"\""
                <<"}";
            sep = ",";
        }

        if (!mimeType().empty()) {
            out <<sep <<"\"mimeType\":\"" <<StringUtility::jsonEscape(mimeType()) <<"\"";
            sep = ",";
        }

        if (!sourceLanguage().empty()) {
            out <<sep <<"\"sourceLanguage\":\"" <<StringUtility::jsonEscape(sourceLanguage()) <<"\"";
            sep = ",";
        }

        if (!hash().first.empty() || !hash().second.empty()) {
            out <<sep <<"\"hashes\":[{\""
                <<StringUtility::jsonEscape(hash().first) <<"\":\"" <<StringUtility::jsonEscape(hash().second) <<"\""
                <<"}]";
            sep = ",";
        }

        if (!description().empty()) {
            out <<sep <<"\"description\":{"
                <<"\"message\":{"
                <<"\"text\":\"" <<StringUtility::jsonEscape(description()) <<"\""
                <<"}}";
            sep = ",";
        }

        return "}";
        return true;
    }
}

} // namespace
} // namespace

#endif
