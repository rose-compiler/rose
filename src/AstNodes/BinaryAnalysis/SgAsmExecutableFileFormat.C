#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include "sage3basic.h"

#include <Rose/Diagnostics.h>

using namespace Rose;

Sawyer::Message::Facility SgAsmExecutableFileFormat::mlog;

void
SgAsmExecutableFileFormat::initDiagnostics() {
    static bool initialized = false;
    if (!initialized) {
        initialized = true;
        Diagnostics::initAndRegister(&mlog, "Rose::BinaryAnalysis::Ast");
        mlog.comment("operating on binary abstract syntax trees");
    }
}

#endif
