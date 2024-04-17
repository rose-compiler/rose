#include <rose.h>                                       // must be first
#ifdef ROSE_ENABLE_LIBRARY_IDENTIFICATION

static const char *gPurpose = "detect library functions in executables";
static const char *gDescription =
    "The specimen's functions are hashed and looked up in one or more databases, and the matching database entries are shown in "
    "the output. The first positional command-line argument is the name of a ROSE Binary Analysis and the remaining arguments "
    "are the URLs for the databases.";

#include <Rose/BinaryAnalysis/LibraryIdentification.h>
#include <Rose/BinaryAnalysis/Partitioner2/Partitioner.h>
#include <Rose/CommandLine.h>
#include <Rose/Diagnostics.h>

#include <batSupport.h>
#include <boost/filesystem.hpp>
#include <Sawyer/ThreadWorkers.h>

using namespace Sawyer::Message::Common;
using namespace Rose;
using namespace Rose::BinaryAnalysis;
namespace P2 = Rose::BinaryAnalysis::Partitioner2;
using Flir = Rose::BinaryAnalysis::LibraryIdentification;
using DbFunctionPair = std::pair<std::string /*dbname*/, Flir::Function::Ptr>;
using Functions = Sawyer::Container::Map<rose_addr_t /*func_addr*/, std::vector<DbFunctionPair>>;
using LibraryCountPair = std::pair<Flir::Library::Ptr, size_t>;
using Libraries = Sawyer::Container::Map<std::string /*libhash*/, LibraryCountPair>;

static Sawyer::Message::Facility mlog;

struct Settings {
    SerialIo::Format stateFormat = SerialIo::BINARY;    // format for the RBA file
    bool showDetails = false;
    bool showSummary = true;
    Flir::Settings flir;
};

static std::vector<std::string>
parseCommandLine(int argc, char *argv[], Settings &settings) {
    using namespace Sawyer::CommandLine;

    // Generic switches
    SwitchGroup generic = Rose::CommandLine::genericSwitches();
    generic.insert(Bat::stateFileFormatSwitch(settings.stateFormat));

    // Tool switches
    SwitchGroup tool("Tool specific switches");
    tool.name("tool");

    Rose::CommandLine::insertBooleanSwitch(tool, "show-details", settings.showDetails,
                                           "Show details about how each specimen function matches functions in the list "
                                           "of databases.");

    Rose::CommandLine::insertBooleanSwitch(tool, "show-summary", settings.showSummary,
                                           "Show summary information about how many times each library was matched.");

    // Parsing
    Parser parser = Rose::CommandLine::createEmptyParser(gPurpose, gDescription);
    parser.errorStream(mlog[FATAL]);
    parser.with(tool);
    parser.with(Flir::commandLineSwitches(settings.flir));
    parser.with(generic);
    parser.doc("Synopsis", "@prop{programName} [@v{switches}] @v{RBA} @v{databases}...");
    parser.doc("Databases", Sawyer::Database::Connection::uriDocString());

    std::vector<std::string> args = parser.parse(argc, argv).apply().unreachedArgs();
    if (args.size() < 2) {
        mlog[FATAL] <<"incorrect usage; see --help\n";
        exit(1);
    }

    return args;
}

static size_t
nInsns(const P2::Partitioner::ConstPtr &partitioner, const P2::Function::Ptr &function) {
    return Flir::nInsns(partitioner, function);
}

class Worker {
    Flir::Settings settings_;
    const std::string &dbName_;
    P2::Partitioner::ConstPtr partitioner_;             // not null
    std::shared_ptr<Flir> flir_;
    Sawyer::ProgressBar<size_t> &progress_;

    // The following fields are protected by a mutex
    SAWYER_THREAD_TRAITS::Mutex &mutex_;
    Functions &functions_;
    Libraries &libraries_;

public:
    Worker(const Flir::Settings &settings, const std::string &dbName, const P2::Partitioner::ConstPtr &partitioner,
           Sawyer::ProgressBar<size_t> &progress, SAWYER_THREAD_TRAITS::Mutex &mutex, Functions &functions, Libraries &libraries)
        : settings_(settings), dbName_(dbName), partitioner_(partitioner), progress_(progress), mutex_(mutex),
          functions_(functions), libraries_(libraries) {
        ASSERT_not_null(partitioner);
    }

    void operator()(size_t, const P2::Function::Ptr &function) {
        if (!flir_) {
            flir_ = std::make_shared<Flir>();
            flir_->settings(settings_);
            flir_->connect(dbName_);
        }

        for (const Flir::Function::Ptr &found: flir_->search(partitioner_, function)) {
            SAWYER_THREAD_TRAITS::LockGuard lock(mutex_);
            functions_.insertMaybeDefault(function->address()).push_back(DbFunctionPair(dbName_, found));
            ++libraries_.insertMaybe(found->library()->hash(), LibraryCountPair(found->library(), 0)).second;
        }

        ++progress_;
    }
};


int
main(int argc, char *argv[]) {
    // Initialization
    ROSE_INITIALIZE;
    Diagnostics::initAndRegister(&mlog, "tool");
    mlog.comment("fast library identification and recognition");
    Bat::checkRoseVersionNumber(MINIMUM_ROSE_LIBRARY_VERSION, mlog[FATAL]);
    Bat::registerSelfTests();

    // Command-line parsing
    Settings settings;
    std::vector<std::string> args = parseCommandLine(argc, argv, settings);
    ASSERT_require(args.size() >= 2);
    std::string rbaFileName = args.front();
    args.erase(args.begin(), args.begin()+1);
    P2::Partitioner::Ptr partitioner;
    try {
        partitioner = P2::Partitioner::instanceFromRbaFile(rbaFileName, settings.stateFormat);
    } catch (const std::exception &e) {
        mlog[FATAL] <<"cannot load partitioner from \"" <<StringUtility::cEscape(rbaFileName) <<"\": " <<e.what() <<"\n";
        exit(1);
    }

    // Match each function against the databases. It's fastest to open each database just once.
    SAWYER_THREAD_TRAITS::Mutex resultMutex;            // guards 'functions' and 'libraries' as they're being initialized
    Functions functions;                                // specimen functions and the corresponding database functions
    Libraries libraries;

    Sawyer::ProgressBar<size_t> progress(args.size() * partitioner->nFunctions(), mlog[MARCH]);
    for (const std::string &dbName: args) {
        Sawyer::Container::Graph<P2::Function::Ptr> work;
        for (const P2::Function::Ptr &function: partitioner->functions())
            work.insertVertex(function);
        Sawyer::workInParallel(work, Rose::CommandLine::genericSwitchArgs.threads,
                               Worker(settings.flir, dbName, partitioner, progress, resultMutex, functions, libraries));
    }

    // Print information about matched functions
    if (settings.showDetails) {
        for (const Functions::Node &node: functions.nodes()) {
            P2::Function::Ptr function = partitioner->functionExists(node.key());
            ASSERT_not_null(function);                      // because we found it earlier
            std::cout <<function->printableName()
                      <<" with " <<StringUtility::plural(nInsns(partitioner, function), "instructions") <<" matches:\n";
            for (const DbFunctionPair &dbFunctionPair: node.value()) {
                const Flir::Function::Ptr &found = dbFunctionPair.second;
                std::cout <<"  function " <<StringUtility::addrToString(found->address())
                          <<" \"" <<StringUtility::cEscape(found->name()) <<"\""
                          <<" with " <<StringUtility::plural(found->nInsns(), "instructions") <<"\n";
                std::cout <<"    library:       \"" <<StringUtility::cEscape(found->library()->name()) <<"\""
                          <<" version \"" <<StringUtility::cEscape(found->library()->version()) <<"\""
                          <<" arch \"" <<StringUtility::cEscape(found->library()->architecture()) <<"\"\n";
                std::cout <<"    database:      " <<dbFunctionPair.first <<":\n";
                std::cout <<"    function hash: " <<found->hash() <<"\n";
                std::cout <<"    library hash:  " <<found->library()->hash() <<"\n";
            }
        }
    }

    // How many matches from each library?
    if (settings.showSummary) {
        std::cout <<"specimen contains " <<StringUtility::plural(partitioner->nFunctions(), "functions") <<"\n";
        std::cout <<StringUtility::plural(functions.size(), "functions") <<" are matched to database entries\n";
        std::vector<LibraryCountPair> sorted(libraries.values().begin(), libraries.values().end());
        std::sort(sorted.begin(), sorted.end(), [](const LibraryCountPair &a, const LibraryCountPair &b) {
                return a.second > b.second;
            });
        for (const LibraryCountPair &pair: sorted) {
            std::cout <<StringUtility::plural(pair.second, "matches", "match") <<" for"
                      <<" library " <<pair.first->hash() <<" \"" <<StringUtility::cEscape(pair.first->name()) <<"\""
                      <<" version \"" <<StringUtility::cEscape(pair.first->version()) <<"\""
                      <<" arch \"" <<StringUtility::cEscape(pair.first->architecture()) <<"\"\n";
        }
    }
}

#else // LibraryIdentification is not enable...

#include <Rose/Diagnostics.h>
#include <cstring>

int main(int argc, char *argv[]) {
    ROSE_INITIALIZE;
    Sawyer::Message::Facility mlog;
    Rose::Diagnostics::initAndRegister(&mlog, "tool");

    int exitStatus = 1;
    for (int i = 1; i < argc; ++i) {
        if (!strcmp(argv[i], "--no-error-if-disabled"))
            exitStatus = 0;
    }

    mlog[Rose::Diagnostics::FATAL]
        <<"fast library identification and recognition (FLIR) is not enabled in this ROSE configuration\n";

    return exitStatus;
}

#endif
