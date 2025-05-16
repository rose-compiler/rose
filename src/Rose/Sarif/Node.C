#include <featureTests.h>
#ifdef ROSE_ENABLE_SARIF
#include <Rose/Sarif/Node.h>

#include <Rose/Sarif/Log.h>
#include <fstream>

#include <Rose/StringUtility.h>

namespace Rose {
namespace Sarif {

bool
Node::emit(std::ostream &out) {
    emitProperties(out);
    return true;
}

bool
Node::emitProperties(std::ostream &out) {
    const auto &props = properties();

    if (props.is_null())
        return false;

    ASSERT_always_require2(props.is_object(), "properties set to unexepected (non-object) type in Sarif!!");

    out.width(0);
    out << ",\"properties\":" << props;
    return true;
}

bool
validateHierarchicalString(const std::string &str) {

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

    if (!validateHierarchicalString(name))
        return false;

    properties()[name] = property;

    return true;
}

} // namespace
} // namespace

#endif
