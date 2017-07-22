#include "sage3basic.h"
#include "Diagnostics.h"
#include <EditDistance/EditDistance.h>

namespace Rose {
namespace EditDistance {

Sawyer::Message::Facility mlog;

void
initDiagnostics() {
    static bool initialized = false;
    if (!initialized) {
        initialized = true;
        Diagnostics::initAndRegister(&mlog, "Rose::EditDistance");
    }
}

} // namespace
} // namespace
