#include <featureTests.h>
#ifdef ROSE_ENABLE_SARIF
#include <Rose/Sarif/Rule.h>

#include <Rose/Sarif/Exception.h>
#include <Rose/StringUtility/Escape.h>
#include <Combinatorics.h>                              // rose

#include <boost/uuid/uuid_io.hpp>

namespace Rose {
namespace Sarif {

bool
Rule::emit(std::ostream &out) {
    out <<"{"
        <<"\"id\":\"" <<StringUtility::jsonEscape(id()) <<"\""
        <<",\"guid\":\"" <<uuid() <<"\"";

    if (!name().empty() && name() != id())
        out <<",\"name\":\"" <<StringUtility::jsonEscape(name()) <<"\"";

    if (!helpUri().empty())
        out <<",\"helpUri\":\"" <<StringUtility::jsonEscape(helpUri()) <<"\"";

    if (!description().empty()) {
        out <<",\"fullDescription\":{"
            <<"\"text\":\"" <<StringUtility::jsonEscape(description()) <<"\""
            <<"}";
    }

    Node::emit(out);

    out <<"}";
    return true;
}

} // namespace
} // namespace

#endif
