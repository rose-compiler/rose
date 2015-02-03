#include <rose.h>
#include <FileSystem.h>

#include <sawyer/CommandLine.h>
#include <sawyer/Message.h>
#include <string>
#include <vector>

using namespace rose;
using namespace Sawyer::Message::Common;

Sawyer::Message::Facility mlog;

struct Settings {};

static Sawyer::CommandLine::ParserResult
parseCommandLine(int argc, char *argv[], Settings &settings /*in,out*/) {
    using namespace Sawyer::CommandLine;
    Parser parser;
    parser
        .purpose("compares ROSE-generated files from two directories")
        .version(std::string(ROSE_SCM_VERSION_ID).substr(0, 8), ROSE_CONFIGURE_DATE)
        .chapter(1, "ROSE Command-line Tools")
        .doc("synopsis",
             "@prop{programName} [@v{switches}] @v{dir1} @v{dir2}")
        .doc("description",
             "Scans the two specified directories and creates a recursive, unified diff of the ROSE-generated files "
             "in those directories.")
        .with(CommandlineProcessing::genericSwitches());

    return parser.parse(argc, argv).apply();
}

static FileSystem::Path
copyRoseFiles(const FileSystem::Path &root) {
    FileSystem::Path destination = FileSystem::createTemporaryDirectory();
    boost::regex re("rose_.*");
    FileSystem::copyFilesRecursively(root, destination, FileSystem::baseNameMatches(re), FileSystem::isDirectory);
    return destination;
}

int
main(int argc, char *argv[]) {
    // Initialize libraries
    Diagnostics::initialize();
    mlog = Diagnostics::Facility("tool", Diagnostics::destination);
    Diagnostics::mfacilities.insertAndAdjust(mlog);

    // Parse command line
    Settings settings;
    std::vector<std::string> args = parseCommandLine(argc, argv, settings).unreachedArgs();
    if (args.size()!=2) {
        mlog[FATAL] <<"incorrect usage; see --help\n";
        exit(1);
    }
    FileSystem::Path dir1 = args[0];
    FileSystem::Path dir2 = args[1];
    if (!FileSystem::isDirectory(dir1)) {
        mlog[FATAL] <<"not a directory: " <<dir1 <<"\n";
        exit(1);
    }
    if (!FileSystem::isDirectory(dir2)) {
        mlog[FATAL] <<"not a directory: " <<dir2 <<"\n";
        exit(1);
    }

    // Copy all the rose_* files to temporary directories so that "diff" doesn't compare the non-rose files.
    // The diff command has command-line switches to exclude certain file name patterns, but no switches that cause it to only
    // include certain patterns.
    FileSystem::Path tmpDir1 = copyRoseFiles(dir1);
    FileSystem::Path tmpDir2 = copyRoseFiles(dir2);

    // Compare the temporary directories
    std::string cmd = "diff -baur "
                      " --label='" + dir1.string() + "' '" + tmpDir1.string() + "'"
                      " --label='" + dir2.string() + "' '" + tmpDir2.string() + "'";
    if (system(cmd.c_str()))
        exit(1);
}
