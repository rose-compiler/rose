// WARNING: Changes to this file must be contributed back to Sawyer or else they will
//          be clobbered by the next update from Sawyer.  The Sawyer repository is at
//          https://gitlab.com/charger7534/sawyer.git.




#include <Sawyer/Assert.h>
#include <Sawyer/Message.h>
#include <Sawyer/Sawyer.h>

// Possibly emit stack traces?
#if BOOST_VERSION >= 106500 && !defined(__APPLE__)
#define SAWYER_ENABLE_STACK_TRACE
#endif

#ifdef SAWYER_ENABLE_STACK_TRACE
    #define BOOST_STACKTRACE_USE_ADDR2LINE
    #include <boost/stacktrace.hpp>
#endif

#include <cstdlib>

namespace Sawyer {
namespace Assert {

SAWYER_EXPORT AssertFailureHandler assertFailureHandler;

SAWYER_EXPORT void
fail(const char *mesg, const char *expr, const std::string &note, const char *filename, unsigned linenum, const char *funcname)
{
    initializeLibrary();

    if (!Message::assertionStream)
        Message::assertionStream = Message::mlog[Message::FATAL];

    *Message::assertionStream <<mesg <<":\n";
    if (filename && *filename)
        *Message::assertionStream <<"  " <<filename <<":" <<linenum <<"\n";
    if (funcname && *funcname)
        *Message::assertionStream <<"  " <<funcname <<"\n";
    if (expr && *expr)
        *Message::assertionStream <<"  " <<expr <<"\n";
    if (!note.empty())
        *Message::assertionStream <<"  " <<note <<"\n";

#ifdef SAWYER_ENABLE_STACK_TRACE
    // Stack traces are enabled if the SAWYER_BACKTRACE environment variable is set to anything but an empty string or the string
    // "0".
    const bool showBacktrace = []() {
        if (const char *env = getenv("SAWYER_BACKTRACE")) {
            if (*env == '\0' || (*env == '0' && env[1] == '\0')) {
                return false;
            } else {
                return true;
            }
        } else {
            return false;
        }
    }();

    if (showBacktrace) {
        auto stack = boost::stacktrace::stacktrace();
        static const size_t nSkip = 1;                      // number of frames to skip
        static const size_t maxShow = 15;                   // maximum number of frames to show
        if (stack.size() > nSkip) {
            size_t frameIdx = 0;
            *Message::assertionStream <<"  stack:\n";
            for (boost::stacktrace::frame frame: stack) {
                if (++frameIdx <= nSkip) {
                    // skip the first frames because they're not interesting. E.g., the first frame is this very function, which is part
                    // of the mechanism handling the failure and thus not of interest to a normal user.
                } else if (frameIdx - nSkip > maxShow) {
                    *Message::assertionStream <<"    additional stack frames suppressed\n";
                    break;
                } else {
                    *Message::assertionStream <<"    #" <<(frameIdx - nSkip) <<" at " <<frame.address() <<"\n";

                    const std::string name = frame.name();
                    if (!name.empty())
                        *Message::assertionStream <<"      " <<name <<"\n";

                    const std::string file = frame.source_file();
                    if (!file.empty())
                        *Message::assertionStream <<"      " <<frame.source_file() <<":" <<frame.source_line() <<"\n";

                    if ("main" == name)
                        break;                              // normal users think "main" is the start of their program
                }
            }
        }
    } else {
        *Message::assertionStream <<"  run with `SAWYER_BACKTRACE=1` environment variable to display a backtrace\n";
    }
#endif



    if (assertFailureHandler)
        assertFailureHandler(mesg, expr, note, filename, linenum, funcname);
    abort();
}

} // namespace
} // namespace
