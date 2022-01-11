#include <Rose/CommandLine/License.h>

#include <iostream>

namespace Rose {
namespace CommandLine {

const char *licenseText =
#include "license.h"
;

ShowLicenseAndExit::Ptr
ShowLicenseAndExit::instance() {
    return Ptr(new ShowLicenseAndExit);
}

void
ShowLicenseAndExit::operator()(const Sawyer::CommandLine::ParserResult&) {
    std::cout <<licenseText;
    exit(0);
}

} // namespace
} // namespace
