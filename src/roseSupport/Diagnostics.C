#include "sage3basic.h"                                 // only because some header files need it

#include "sawyer/Assert.h"
#include "sawyer/ProgressBar.h"
#include "AsmUnparser.h"                                // rose::AsmUnparser
#include "BinaryDataFlow.h"                             // BinaryAnalysis::DataFlow
#include "BinaryLoader.h"                               // rose::BinaryLoader
#include "BinaryTaintedFlow.h"                          // BinaryAnalysis::TaintedFlow
#include "Diagnostics.h"                                // rose::Diagnostics
#include "Disassembler.h"                               // rose::Disassembler
#include "Partitioner.h"                                // rose::Partitioner
#include <Partitioner2/Utility.h>                       // rose::BinaryAnalysis::Partitioner2
#include <EditDistance/EditDistance.h>                  // rose::EditDistance

#include <cstdarg>

namespace rose {
namespace Diagnostics {

Sawyer::Message::DestinationPtr destination;
Sawyer::Message::PrefixPtr mprefix;
Sawyer::Message::Facility mlog;
static bool isInitialized_ = false;

void initialize() {
    if (!isInitialized()) {
        isInitialized_ = true;

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

        // (Re)construct the main librose Facility.  A Facility is constructed with all Stream objects enabled, so we'll
        // disable those that we deem are too noisy for most users.  However, the insertAndAdjust might make other choices if
        // mfacilities already has some stream inserted or the user has already called mfacilities.impset().
        mlog = Sawyer::Message::Facility("rose", destination);
        mlog[DEBUG].disable();
        mlog[TRACE].disable();
        mlog[WHERE].disable();
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
        EditDistance::initDiagnostics();
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
