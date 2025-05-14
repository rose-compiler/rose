#include <featureTests.h>
#ifdef ROSE_ENABLE_SARIF
#include <Rose/Sarif/Node.h>

#include <Rose/Sarif/Log.h>
#include <fstream>

#include <Rose/StringUtility.h>

namespace Rose {
namespace Sarif {

bool
validate_hierarchical_string(const std::string &str) {

    std::vector<std::string> elems;

    Rose::StringUtility::splitStringIntoStrings(str, '/', elems);

    for (const auto &e : elems) {
        if (e.size() == 0)
            return false;
    }

    return true;
}

bool
Node::addProperty(const std::string &name, const nlohmann::json &property) {

    if (!validate_hierarchical_string(name))
        return false;

    properties()[name] = property;

    return true;
}

} // namespace
} // namespace

#endif
