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
        mlog = Sawyer::Message::Facility("rose::EditDistance", Diagnostics::destination);
        Diagnostics::mfacilities.insertAndAdjust(mlog);
    }
}

} // namespace
} // namespace
