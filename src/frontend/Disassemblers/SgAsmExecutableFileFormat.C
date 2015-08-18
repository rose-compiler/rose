#include "sage3basic.h"

#include "Diagnostics.h"

using namespace rose;

Sawyer::Message::Facility SgAsmExecutableFileFormat::mlog;

void
SgAsmExecutableFileFormat::initDiagnostics() {
    static bool initialized = false;
    if (!initialized) {
        initialized = true;
        mlog = Sawyer::Message::Facility("rose::BinaryAnalysis::Ast", Diagnostics::destination);
        Diagnostics::mfacilities.insertAndAdjust(mlog);
    }
}
