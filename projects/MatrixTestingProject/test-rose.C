static const char *purpose = "simple program to test whether a ROSE installation is usable";
static const char *description =
    "This program's only purpose is to parse the command-line in order to determine that ROSE has been installed "
    "correctly and is usable.";

#ifdef INTERNAL_BUILD
    #include <rose.h>
    #include <rosePublicConfig.h>
    #include <CommandLine.h>
#else
    #include <rose/rose.h>                              // must be first!
    #include <rose/rosePublicConfig.h>
    #include <rose/CommandLine.h>
#endif

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

} // namespace
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int
main(int argc, char *argv[]) {
    ROSE_INITIALIZE;
    Rose::Diagnostics::initAndRegister(&mlog, "tool");

    parseCommandLine(argc, argv);
}
