#include "sage3basic.h"
#include "Diagnostics.h"
#include <EditDistance/EditDistance.h>

namespace rose {
namespace EditDistance {

Sawyer::Message::Facility mlog;

void
initDiagnostics() {
    static bool initialized = false;
    if (!initialized) {
        initialized = true;
        Diagnostics::initAndRegister(&mlog, "rose::EditDistance");
    }
}

} // namespace
} // namespace
