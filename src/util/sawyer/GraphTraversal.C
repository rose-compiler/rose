#include <sawyer/GraphTraversal.h>
#include <sstream>
#include <string>

namespace Sawyer {
namespace Container {
namespace Algorithm {

SAWYER_EXPORT std::string
traversalEventName(TraversalEvent event) {
    switch (event) {
        case NO_EVENT:        return "NO_EVENT";
        case ENTER_VERTEX:    return "ENTER_VERTEX";
        case DISCOVER_VERTEX: return "DISCOVER_VERTEX";
        case ENTER_EDGE:      return "ENTER_EDGE";
        case LEAVE_VERTEX:    return "LEAVE_VERTEX";
        case LEAVE_EDGE:      return "LEAVE_EDGE";
        case FOLLOW_EDGE:     return "FOLLOW_EDGE";
    }
    std::ostringstream ss;
    ss <<"TraversalEvent(0x" <<std::hex <<event <<")";
    return ss.str();
}

} // namespace
} // namespace
} // namespace
