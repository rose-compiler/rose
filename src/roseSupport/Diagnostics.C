#include "sage3basic.h"                                 // only because some header files need it

#include "sawyer/Assert.h"
#include "AsmUnparser.h"                                // rose::AsmUnparser
#include "BinaryDataFlow.h"                             // BinaryAnalysis::DataFlow
#include "BinaryLoader.h"                               // rose::BinaryLoader
#include "BinaryTaintedFlow.h"                          // BinaryAnalysis::TaintedFlow
#include "Diagnostics.h"                                // rose::Diagnostics
#include "Disassembler.h"                               // rose::Disassembler
#include "Partitioner.h"                                // rose::Partitioner

namespace rose {
namespace Diagnostics {

Sawyer::Message::DestinationPtr destination;
Sawyer::Message::Facility mlog("rose");
Sawyer::Message::Facilities facilities;

void initialize() {
    if (!isInitialized()) {
        // Allow libsawyer to initialize itself if necessary.  Among other things, this makes Saywer::Message::merr actually
        // point to something.  This is also the place where one might want to assign some other message plumbing to
        // rose::Diagnostics::destination (such as sending messages to additional locations).
        Sawyer::initializeLibrary();
        if (destination==NULL)
            destination = Sawyer::Message::merr;
        mlog.initStreams(destination);
        facilities.insert(mlog);

        // Where should failed assertions go for the Sawyer::Assert macros like ASSERT_require()?
        Sawyer::Message::assertionStream = mlog[FATAL];

        // Register logging facilities from other software layers.  These facilities should already be in a usable, but
        // default, state. They probably have all streams enabled (debug through fatal) and are emitting to standard error
        // using the POSIX unbuffered output functions.  Calling these initializers should make all the streams point to the
        // rose::Diagnostics::destination that we set above.
        BinaryLoader::initDiagnostics();
        Disassembler::initDiagnostics();
        Partitioner::initDiagnostics();
        AsmUnparser::initDiagnostics();
        BinaryAnalysis::DataFlow::initDiagnostics();
        BinaryAnalysis::TaintedFlow::initDiagnostics();

        // By default, only messages of informational importance and above are dispalyed.
        facilities.control("none, >=info");
    }
}

bool isInitialized() {
    return destination!=NULL;
}

} // namespace
} // namespace
