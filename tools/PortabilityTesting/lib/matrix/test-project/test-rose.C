static const char *purpose = "simple program to test whether a ROSE installation is usable";
static const char *description =
    "This program's only purpose is to parse the command-line in order to determine that ROSE has been installed "
    "correctly and is usable.";

// We haven't yet fixed the fact that header files are named differently inside the ROSE library than when the library is installed.
#ifdef INTERNAL_BUILD
    #include <rose.h>
    #include <rosePublicConfig.h>
    #include <Rose/CommandLine.h>
#else
    #include <rose/rose.h>                              // must be first!
    #include <rose/rosePublicConfig.h>
    #include <rose/Rose/CommandLine.h>
#endif

using namespace Sawyer::Message::Common;                // for things like DEBUG, INFO, WARN, ERROR, FATAL, etc.

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
namespace {

Sawyer::Message::Facility mlog;

// Parse command-line and return the positional arguments which describe the specimen.
void
parseCommandLine(int argc, char *argv[]) {
    using namespace Sawyer::CommandLine;

    Parser parser = Rose::CommandLine::createEmptyParser(purpose, description);
    parser.doc("Synopsis", "@prop{programName} [@v{switches}] @v{ignored_args}");
    parser.errorStream(mlog[Sawyer::Message::FATAL]);
    parser.with(Rose::CommandLine::genericSwitches());
    parser.parse(argc, argv).apply();
}

// Run a self test
class SelfTest: public Rose::CommandLine::SelfTest {
public:
    std::string name() const override {
        return "test 1";
    }

    bool operator()() override {
        mlog[DEBUG].enable();
        mlog[DEBUG] <<"test passed\n";
        return true;
    }
};

} // namespace
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int
main(int argc, char *argv[]) {
    ROSE_INITIALIZE;
    Rose::Diagnostics::initAndRegister(&mlog, "tool");

    // Cause this tool's "--version" switch to say something like "1.2.3 using ROSE 0.11.145.133"
    Rose::CommandLine::versionString =
#include "VERSION"
        " using " + Rose::CommandLine::versionString;

    Rose::CommandLine::insertSelfTest<SelfTest>();
    parseCommandLine(argc, argv);
}
