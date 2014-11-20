#include <rose.h>

#include <FileSystem.h>
#include <sawyer/CommandLine.h>
#include <sawyer/Message.h>
#include <string>
#include <vector>

using namespace rose;
using namespace Sawyer::Message::Common;

Sawyer::Message::Facility mlog;

struct Settings {
    std::string identityDirName;                        // directory containing rose_* files produced by an identity translator
    std::string translatedDirName;                      // directory containing rose_* files produced with the real translator
    std::string topBuildDirName;                        // directory in which we are now building
};

static Sawyer::CommandLine::ParserResult
parseCommandLine(int argc, char *argv[], Settings &settings /*in,out*/) {
    using namespace Sawyer::CommandLine;
    Parser parser;
    parser
        .purpose("applies ROSE patches on the fly")
        .version(std::string(ROSE_SCM_VERSION_ID).substr(0, 8), ROSE_CONFIGURE_DATE)
        .chapter(1, "ROSE Command-line Tools")
        .with(CommandlineProcessing::genericSwitches())
        .doc("Synopsis",
             "@prop{programName} --identity=@v{directory} --translated=@v{directory} --top=@v{directory} "
             "-- @v{compiler_arguments}")
        .doc("Description",
             "Compares the @v{identity} and @v{translated} build directories to generate a collection of patches and then "
             "applies those patches to the output of this translator. The purpose of @prop{programName} is to enable the user "
             "to apply a previous translation to a new version of the input source code when the original translator is "
             "not available.  For instance, if the @v{translated} build directory contains output from some proprietary "
             "ROSE translator to which the user does not have access, then the user can make similar changes to a newer "
             "version of the input source code by comparing the @v{translated} output with the untranslated output stored "
             "in the @v{identity} directory.  The @v{translated} and @v{identity} directories must be generated from the "
             "same input and have the same structure.  The differences between those two directories will be applied to "
             "ROSE outputs as they are created and before the backend compiler is invoked.\n\n"

             "When @prop{programName} is run without the @s{identity}, @s{translated}, and @s{top} switches it becomes "
             "an identity translator that can be used to create the @v{identity} directory.\n\n"

             "Notice that the ROSE and compiler command-line arguments must be separated from this tool's arguments by "
             "a \"--\" switch. This is due to librose using a different argument parsing mechanism. If the double hyphen "
             "separator is missing then this tool will attempt to parse arguments intended for ROSE and will probably "
             "complain about unrecognized switches.\n\n")
        .doc("Example",
             "For instance, let us assume that we have a build directory, \"_build-translated\", that we configured to "
             "use a ROSE translator that adds instrumentation that performs run-time taint tracking on the shadow-4.1.4 "
             "package but a bug prevents it from running on the closely related shadow-4.1.5. Lets assume that we "
             "require 4.1.5 to be compiled immediately and we don't have time to fix the bug in the translator. We could "
             "use @prop{programName} to apply the same translations to the 4.1.5 version as we did to the 4.1.4 version.\n\n"

             "The first step is to compile the 4.1.4 version again, but with the identity translator in a \"_build-identity\" "
             "directory.  This will result in \"rose_*\" build artifacts that we can compare with the similar \"rose_*\" "
             "artifacts in the \"_build-translated\" directory.  Ideally, we want to use the same version of ROSE that we "
             "originally used to generate the \"_build-translated\" directory, otherwise the next step might try to apply "
             "changes that are only the result of different output formatting between different versions of ROSE.\n\n"

             "Finally, we configure shadow-4.1.5 to be compiled with @prop{programName}. The @s{identity} switch argument "
             "should be the \"_build-identity\" directory, @s{translated} should be \"_build-translated\", and we name "
             "the current build directory \"_build-patched\" and specify it with the @s{top} switch.  Since the shadow "
             "makefiles are recursive and change the current working directory, these three switches should be given "
             "absolute names.  When the @prop{programName} translator runs it will compare the @v{identity} output "
             "with the @v{translated} output and make those same changes under the \"_build-patched\" directory. Obviously, "
             "problems will result if shadow-4.1.5 differs too much from 4.1.4, but it might be better than nothing.");
             

    SwitchGroup tool("Tool-specific switches");
    tool.insert(Switch("identity")
                .argument("directory", anyParser(settings.identityDirName))
                .doc("Name of the root of the build directory that was created with a ROSE identity translator. This "
                     "directory should be produced from the same input source code as the @s{translated} directory.  Ideally, "
                     "it should also be created with the same version of ROSE otherwise the patch files may include "
                     "differences that are insignificant but due to differences in ROSE's backend unparser."));

    tool.insert(Switch("translated")
                .argument("directory", anyParser(settings.translatedDirName))
                .doc("Name of the root of the build directory that was created with a ROSE translator that modified "
                     "the source code in some way.  That translator might not be available to the user, otherwise instead "
                     "of using @prop{programName} he could just re-use that translator.  The directory should be different "
                     "than the @s{identity} directory but should be created from the same source code."));

    tool.insert(Switch("top")
                .argument("directory", anyParser(settings.topBuildDirName))
                .doc("Name of the root of the current build tree.  This is the top of the directory structure in which "
                     "@prop{programName} is generating build artifacts.  Its structure will be similar to the @v{identity} "
                     "and @v{translated} directories as far as the relative locations of build artifacts are concerned."));
    
    return parser.with(tool).parse(argc, argv).apply();
}

// Callback invoked immediately after backend produces a rose_* output file.
struct Patcher: UnparseFormatHelp::PostOutputCallback {
    FileSystem::Path identityRoot, translatedRoot, buildRoot;
    Patcher(const FileSystem::Path &identityRoot, const FileSystem::Path &translatedRoot, const FileSystem::Path &buildRoot)
        : identityRoot(identityRoot), translatedRoot(translatedRoot), buildRoot(buildRoot) {}
    bool operator()(bool chain, const Args &args) ROSE_OVERRIDE {
        if (chain) {
            FileSystem::Path identityName = identityRoot / FileSystem::makeRelative(args.outputName, buildRoot);
            FileSystem::Path translatedName = translatedRoot / FileSystem::makeRelative(args.outputName, buildRoot);
            FileSystem::Path targetName = args.outputName;
            mlog[DEBUG] <<"patching output:\n"
                        <<"  identity   = " <<identityName <<"\n"
                        <<"  translated = " <<translatedName <<"\n"
                        <<"  target     = " <<targetName <<"\n";
            if (FileSystem::isFile(identityName) && FileSystem::isFile(translatedName) && FileSystem::isFile(targetName)) {
                // generate a diff for identityName -> translatedName and apply it to targetName in place.
                std::string cmd = "diff -u '" + FileSystem::toString(identityName) + "'" +
                                  " '" + FileSystem::toString(translatedName) + "'" +
                                  "| patch --backup '" + FileSystem::toString(targetName) +"'";
                mlog[DEBUG] <<"patching with: " <<cmd <<"\n";
                if (0!=system(cmd.c_str()))
                    mlog[ERROR] <<"patch command failed: " <<cmd <<"\n";
            }
        }
        return chain;
    }
};

int
main(int argc, char *argv[]) {
    // Initialize libraries
    Diagnostics::initialize();
    mlog = Diagnostics::Facility("tool", Diagnostics::destination);
    Diagnostics::mfacilities.insertAndAdjust(mlog);

    // Parse command line
    Settings settings;
    std::vector<std::string> rose_args = parseCommandLine(argc, argv, settings).unreachedArgs();
    if (rose_args.empty()) {
        mlog[FATAL] <<"incorrect usage; see --help\n";
        exit(1);
    }
    bool isIdentityTranslator = settings.identityDirName.empty() || settings.translatedDirName.empty();
    if (isIdentityTranslator) {
        if (!settings.identityDirName.empty()) {
            mlog[FATAL] <<"presence of translated build-dir implies presence of identity build-dir but none specified\n";
            exit(1);
        } else if (!settings.translatedDirName.empty()) {
            mlog[FATAL] <<"presence of identity build-dir implies presence of translated build-dir but none specified\n";
            exit(1);
        }
    } else if (settings.topBuildDirName.empty()) {
        mlog[FATAL] <<"top of current build directory tree is required.\n";
        exit(1);
    }

    // Parse input file(s)
    rose_args.insert(rose_args.begin(), argv[0]);
    SgProject *project = frontend(rose_args);

    // Wire up the Patcher to backend() if necessary.
    UnparseFormatHelp *unparseHelper = new UnparseFormatHelp; // WARNING: gets deleted by backend(), so must be on the heap!
    Patcher *patcher = NULL;
    if (!isIdentityTranslator) {
        FileSystem::Path identityRoot = FileSystem::makeAbsolute(settings.identityDirName);
        FileSystem::Path translatedRoot = FileSystem::makeAbsolute(settings.translatedDirName);
        FileSystem::Path buildRoot = FileSystem::makeAbsolute(settings.topBuildDirName);
        mlog[DEBUG] <<"configuration:\n"
                    <<"  identity root   = " <<identityRoot <<"\n"
                    <<"  translated root = " <<translatedRoot <<"\n"
                    <<"  build root      = " <<buildRoot <<"\n";
        patcher = new Patcher(identityRoot, translatedRoot, buildRoot);
        unparseHelper->postOutputCallbacks.append(patcher);
    }

    // Generate output, invoke Patcher, and run backend compiler
    return backend(project, unparseHelper);
}
