// Make sure diagnostics subsystem is working
#include <rose.h>
#include <Diagnostics.h>

#include <boost/algorithm/string/predicate.hpp>

using namespace Rose;
using namespace Rose::Diagnostics;

int
main() {
    // Force all ROSE diagnostics to go to our string stream instead of standard error. Normal programs would either only call
    // Diagnostics::initialize or frontend.
    std::ostringstream ss;
    Rose::Diagnostics::destination = Sawyer::Message::StreamSink::instance(ss);
    ROSE_INITIALIZE;

    // Emit some message and check that it appeared
    mlog[INFO] <<"-test 1-\n";
    if (!boost::contains(ss.str(), "-test 1-"))
        throw std::runtime_error("test 1 failed: got \"" + StringUtility::cEscape(ss.str()) + "\"\n");
}
