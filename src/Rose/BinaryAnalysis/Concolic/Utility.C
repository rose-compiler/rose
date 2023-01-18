#include <featureTests.h>
#ifdef ROSE_ENABLE_CONCOLIC_TESTING
#include <sage3basic.h>
#include <Rose/BinaryAnalysis/Concolic.h>
#include <rose_getline.h>

#include <boost/algorithm/string/predicate.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <fstream>
#include <string>

namespace Rose {
namespace BinaryAnalysis {
namespace Concolic {

Sawyer::Message::Facility mlog;

// class method
void
initDiagnostics() {
    static bool initialized = false;
    if (!initialized) {
        initialized = true;
        Diagnostics::initAndRegister(&mlog, "Rose::BinaryAnalysis::Concolic");
        mlog.comment("concolic testing");
    }
}

bool
isRunningInContainer() {
    // There are many ways to determine if we're running inside a container. One easy way is to look at the name
    // of the root process.
    std::ifstream f("/proc/1/status");
    if (f) {
        while (true) {
            const std::string s = rose_getline(f);
            if (s.empty()) {
                break;
            } else if (boost::starts_with(s, "Name:\t")) {
                const std::string value = boost::trim_copy(s.substr(6));
                if ("systemd" == value || "init" == value) {
                    return false;
                } else {
                    return true;
                }
            }
        }
    }
    return false;
}

} // namespace
} // namespace
} // namespace

#endif
