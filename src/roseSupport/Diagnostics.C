#include "sage3basic.h"                                 // only because some header files need it

#include "Assert.h"                                     // Sawyer::Assert
#include "Diagnostics.h"                                // rose::Diagnostics
#include "Disassembler.h"                               // rose::Disassembler
#include "BinaryLoader.h"                               // rose::BinaryLoader

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
        destination = Sawyer::Message::merr;
        mlog.initStreams(destination);
        facilities.insert(mlog);

        // Where should failed assertions go for the Sawyer::Assert macros like ASSERT_require()?
        Sawyer::Assert::assertionStream = mlog[FATAL];

        // Register logging facilities from other software layers.  These facilities should already be in a usable, but
        // default, state. They probably have all streams enabled (debug through fatal) and are emitting to standard error
        // using the POSIX unbuffered output functions.  Calling these initializers should make all the streams point to the
        // rose::Diagnostics::destination that we set above.
        BinaryLoader::initDiagnostics();
        Disassembler::initDiagnostics();

        // By default, only messages of informational importance and above are dispalyed.
        facilities.control("none, >=info");
    }
}

bool isInitialized() {
    return destination!=NULL;
}

} // namespace
} // namespace
