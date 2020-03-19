#include <rose.h>
#include <BinaryConcolic.h>                             // rose
#if defined(ROSE_ENABLE_CONCOLIC_TESTING) && defined(ROSE_HAVE_SQLITE3)

static const char *purpose = "tests concolic testing";
static const char *description =
    "To be written. This program is currently only for testing some of the concolic testing framework and is not useful "
    "to users at this time.";

#include <CommandLine.h>                                // rose
#include <Partitioner2/Engine.h>                        // rose
#include <Sawyer/CommandLine.h>
#include <Sawyer/Message.h>

using namespace Rose::BinaryAnalysis;
using namespace Sawyer::Message::Common;
namespace P2 = Rose::BinaryAnalysis::Partitioner2;

static Sawyer::Message::Facility mlog;
static std::string databaseUrl = "testConcolicExecutor.db";

// Parse command-line and return name of specimen executable.
static std::string
parseCommandLine(int argc, char *argv[]) {
    using namespace Sawyer::CommandLine;

    SwitchGroup tool("Tool-specific switches");
    tool.insert(Switch("database", 'D')
                .argument("URL", anyParser(databaseUrl))
                .doc("Name of the database to create for this test."));


    Parser parser = Rose::CommandLine::createEmptyParser(purpose, description);
    parser.groupNameSeparator("-");
    parser.doc("Synopsis", "@prop{programName} [@v{switches}] @v{specimen_names}");
    parser.doc("Specimens",
               "The @v{specimen_name} must be the name of a Linux ELF executable file with an x86 or amd64 "
               "instruction set architecture.");
    parser.with(Rose::CommandLine::genericSwitches());
    parser.with(tool);

    std::vector<std::string> specimenArgs = parser.parse(argc, argv).apply().unreachedArgs();
    if (specimenArgs.empty()) {
        mlog[FATAL] <<"no binary specimen specified; see --help\n";
        exit(1);
    }
    return specimenArgs[0];
}

int
main(int argc, char *argv[]) {
    ROSE_INITIALIZE;
    Rose::Diagnostics::initAndRegister(&mlog, "tool");
    mlog.comment("concolic testing tool proper");
    std::string exeName = parseCommandLine(argc, argv);

#if !defined(__linux__)
    mlog[INFO] <<"this test supported only for Linux operating systems\n";
    exit(0); // test succeeds since it does nothing
#endif

    // Create the database and add a specimen to it.
    Concolic::Database::Ptr db = Concolic::Database::create(databaseUrl);
    Concolic::Specimen::Ptr specimen = Concolic::Specimen::instance(exeName);
    Concolic::TestCase::Ptr testCase = Concolic::TestCase::instance(specimen);
#if 1
    std::vector<std::string> args;
    args.push_back("1");
    args.push_back("2");
    testCase->args(args);
#endif
    Concolic::TestCaseId testCaseId = db->id(testCase);

    // Run the concolic executor on the test case
    Concolic::ConcolicExecutor::Ptr executor = Concolic::ConcolicExecutor::instance();
    executor->execute(db, testCase);
}

#else

#include <iostream>
int main() {
    std::cerr <<"concolic testing is not enabled\n";
}

#endif
