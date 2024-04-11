#ifndef ROSE_CommandLine_License_H
#define ROSE_CommandLine_License_H
#include <RoseFirst.h>

#include <Sawyer/CommandLine.h>

namespace Rose {
namespace CommandLine {

/** Text of the ROSE software license.
 *
 *  This text comes directly from the LicenseInformation/ROSE_BSD_License.txt file in the source code. */
extern const char *licenseText;

/** Show license text and exit. */
class ShowLicenseAndExit: public Sawyer::CommandLine::SwitchAction {
protected:
    ShowLicenseAndExit() {}

public:
    /** Shared ownership pointer. */
    using Ptr = Sawyer::SharedPointer<ShowLicenseAndExit>;

    /** Allocating constructor. */
    static Ptr instance();

protected:
    void operator()(const Sawyer::CommandLine::ParserResult&);
};

} // namespace
} // namespace

#endif
