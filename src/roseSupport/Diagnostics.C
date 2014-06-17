#include "sage3basic.h"                                 // only because some header files need it

#include "Assert.h"                                     // Sawyer::Assert
#include "Diagnostics.h"                                // rose::Diagnostics
#include "Disassembler.h"                               // rose::Disassembler
#include "BinaryLoader.h"                               // rose::BinaryLoader

namespace rose {
namespace Diagnostics {

Sawyer::Message::DestinationPtr destination;
Sawyer::Message::Facility log;
Sawyer::Message::Facilities facilities;

void initialize() {
    if (!isInitialized()) {
        // Basic diagnostic messages initialization
        Sawyer::initializeLibrary();
        destination = Sawyer::Message::merr;
        log = Sawyer::Message::Facility("rose", destination);
        facilities.insert(log);

        // Where should failed assertions go for the Sawyer::Assert macros like ASSERT_require()?
        Sawyer::Assert::assertionStream = log[Sawyer::Message::FATAL];

        // Register logging facilities from other software layers
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
