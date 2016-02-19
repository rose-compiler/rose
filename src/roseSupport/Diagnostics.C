#include "sage3basic.h"                                 // only because some header files need it

#include "Sawyer/Assert.h"
#include "Sawyer/ProgressBar.h"
#include "AsmUnparser.h"                                // rose::BinaryAnalysis::AsmUnparser
#include "BaseSemantics2.h"                             // rose::BinaryAnalysis::InstructionSemantics2
#include "BinaryCallingConvention.h"                    // rose::BinaryAnalysis::CallingConvention
#include "BinaryDataFlow.h"                             // rose::BinaryAnalysis::DataFlow
#include "BinaryLoader.h"                               // BinaryLoader
#include "BinaryNoOperation.h"                          // rose::BinaryAnalysis::NoOperation
#include "BinaryPointerDetection.h"                     // rose::BinaryAnalysis::PointerDetection
#include "BinaryStackDelta.h"                           // rose::BinaryAnalysis::StackDelta
#include "BinaryString.h"                               // rose::BinaryAnalysis::String
#include "BinaryTaintedFlow.h"                          // rose::BinaryAnalysis::TaintedFlow
#include "Diagnostics.h"                                // rose::Diagnostics
#include "Disassembler.h"                               // rose::BinaryAnalysis::Disassembler
#include "Partitioner.h"                                // rose::BinaryAnalysis::Partitioner
#include <Partitioner2/Utility.h>                       // rose::BinaryAnalysis::Partitioner2
#include <EditDistance/EditDistance.h>                  // rose::EditDistance

#include <cstdarg>

namespace rose {
namespace Diagnostics {

ROSE_DLL_API Sawyer::Message::DestinationPtr destination;
ROSE_DLL_API Sawyer::Message::PrefixPtr mprefix;
ROSE_DLL_API Sawyer::Message::Facility mlog;
static bool isInitialized_ = false;

void initialize() {
    if (!isInitialized()) {
        isInitialized_ = true;

        // How do failed assertions behave?  Don't make any changes if this is already initialized by the user.
        if (!Sawyer::Assert::assertFailureHandler)
            failedAssertionBehavior(NULL);              // sets it to a default behavior based on configuration

        // Allow libsawyer to initialize itself if necessary.  Among other things, this makes Saywer::Message::merr actually
        // point to something.  This is also the place where one might want to assign some other message plumbing to
        // rose::Diagnostics::destination (such as sending messages to additional locations).
        Sawyer::initializeLibrary();
        if (mprefix==NULL)
            mprefix = Sawyer::Message::Prefix::instance();
        if (destination==NULL) {
            // use FileSink or FdSink because StreamSink can't tell whether output is a tty or not.
            destination = Sawyer::Message::FileSink::instance(stderr)->prefix(mprefix);
        }

        // Force certain facilities to be enabed or disabled. This might be different than the Sawyer default. If user wants
        // something else then use mfacilities.control("...") after we return. This doesn't affect any Facility object that's
        // already registered (such as any added already by the user or Sawyer's own, but we could use mfacilities.renable() if
        // we wanted that).
        mfacilities.impset(DEBUG, false);
        mfacilities.impset(TRACE, false);
        mfacilities.impset(WHERE, false);
        mfacilities.impset(MARCH, true);
        mfacilities.impset(INFO,  true);
        mfacilities.impset(WARN,  true);
        mfacilities.impset(ERROR, true);
        mfacilities.impset(FATAL, true);

        // (Re)construct the main librose Facility.  A Facility is constructed with all Stream objects enabled, but
        // insertAndAdjust will change that based on mfacilities' settings.
        mlog = Sawyer::Message::Facility("rose", destination);
        mfacilities.insertAndAdjust(mlog);

        // Where should failed assertions go for the Sawyer::Assert macros like ASSERT_require()?
        Sawyer::Message::assertionStream = mlog[FATAL];

        // Turn down the progress bar rates
        Sawyer::ProgressBarSettings::initialDelay(12.0);
        Sawyer::ProgressBarSettings::minimumUpdateInterval(2.5);

        // Register logging facilities from other software layers.  These facilities should already be in a usable, but
        // default, state. They probably have all streams enabled (debug through fatal) and are emitting to standard error
        // using the POSIX unbuffered output functions.  Calling these initializers should make all the streams point to the
        // rose::Diagnostics::destination that we set above.
        BinaryLoader::initDiagnostics();
        BinaryAnalysis::Disassembler::initDiagnostics();
        BinaryAnalysis::Partitioner::initDiagnostics();
        BinaryAnalysis::AsmUnparser::initDiagnostics();
        BinaryAnalysis::DataFlow::initDiagnostics();
        BinaryAnalysis::TaintedFlow::initDiagnostics();
        BinaryAnalysis::Partitioner2::initDiagnostics();
        BinaryAnalysis::PointerDetection::initDiagnostics();
        BinaryAnalysis::InstructionSemantics2::initDiagnostics();
        BinaryAnalysis::StackDelta::initDiagnostics();
        BinaryAnalysis::Strings::initDiagnostics();
        BinaryAnalysis::NoOperation::initDiagnostics();
        BinaryAnalysis::CallingConvention::initDiagnostics();
        EditDistance::initDiagnostics();
        SgAsmExecutableFileFormat::initDiagnostics();
    }
}

bool isInitialized() {
    return isInitialized_;
}

StreamPrintf mfprintf(std::ostream &stream) {
    return StreamPrintf(stream);
}

int StreamPrintf::operator()(const char *fmt, ...) {
    char buf_[1024];                                    // arbitrary size; most strings will be smaller
    char *buf = buf_;
    int bufsz = sizeof buf_, need = 0;

    while (1) {
        va_list ap;
        va_start(ap, fmt);
        need = vsnprintf(buf, bufsz, fmt, ap);          // "need" does not include NUL termination
        va_end(ap);
        if (need >= bufsz) {
            if (buf!=buf_)
                delete[] buf;
            bufsz = need + 1;                           // +1 for NUL termination
            buf = new char[bufsz];
        } else {
            break;
        }
    }
    stream <<buf;
    if (buf!=buf_)
        delete[] buf;

    return need;
}

        
    


} // namespace
} // namespace
