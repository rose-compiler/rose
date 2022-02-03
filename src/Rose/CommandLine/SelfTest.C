#include <Rose/CommandLine/SelfTest.h>

#include <Rose/Diagnostics.h>
#include <Rose/StringUtility/Diagnostics.h>
#include <Rose/StringUtility/Escape.h>
#include <vector>

namespace Rose {
namespace CommandLine {

std::vector<SelfTest::Ptr> selfTests;

ROSE_DLL_API void
runSelfTestsAndExit() {
    using namespace Rose::Diagnostics;

    // Run each test sequentially
    size_t npass=0, nfail=0;
    BOOST_FOREACH (const SelfTest::Ptr &test, selfTests) {
        if (test) {
            mlog[DEBUG] <<"running self test \"" <<StringUtility::cEscape(test->name()) <<"\"...\n";
            if ((*test)()) {
                mlog[INFO] <<"passed: self test \"" <<StringUtility::cEscape(test->name()) <<"\"\n";
                ++npass;
            } else {
                mlog[ERROR] <<"failed: self test \"" <<StringUtility::cEscape(test->name()) <<"\"\n";
                ++nfail;
            }
        }
    }

    // Report results and exit
    if (npass + nfail == 0) {
        mlog[INFO] <<"no self tests available for this tool\n";
        exit(0);
    } else if (nfail > 0) {
        mlog[FATAL] <<StringUtility::plural(nfail, "self tests") <<" failed; "
                    <<StringUtility::plural(npass, "self tests") <<" passed\n";
        exit(1);
    } else {
        mlog[INFO] <<"all self tests pass\n";
        exit(0);
    }
}

} // namespace
} // namespace
