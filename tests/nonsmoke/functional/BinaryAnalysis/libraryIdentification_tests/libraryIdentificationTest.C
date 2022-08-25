static const char *purpose = "test creation of a library attribution database";
static const char *description =
    "The purpose of libraryIdentificationTest is to test the creation of "
    "a library attribution database, and then using the ROSE backend.\n\n"

    "This only really makes sense for statically compiled executables "
    "because dynamically linked executables already identify all the "
    "functions they call from dynamically loaded libraries.";


#include <rose.h>                                       // must be first
#include <Rose/BinaryAnalysis/LibraryIdentification.h>

#include <Rose/BinaryAnalysis/Disassembler/Base.h>
#include <Rose/BinaryAnalysis/Partitioner2/Engine.h>
#include <Rose/BinaryAnalysis/Partitioner2/Partitioner.h>
#include <Rose/CommandLine.h>

// For mmap
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>

using namespace Rose::BinaryAnalysis;
namespace P2 = Rose::BinaryAnalysis::Partitioner2;
using Flir = Rose::BinaryAnalysis::LibraryIdentification;

//---------------------------------------------------------------------------------
// Settings adjusted from the command line. Defuault c'tor initializes the defaults
struct Settings {
    std::string databaseName = "testLibraryIdentification.db";  //Name of the database file to read from
};

// Describe and parse the command-line
static std::vector<std::string>
parseCommandLine(int argc, char *argv[], P2::Engine &engine, Settings &settings) {
    using namespace Sawyer::CommandLine;
    Parser p = engine.commandLineParser(purpose, description);
    p.doc("Synopsis", "@prop{programName} [@v{switches}] @v{file_names}...");
    
    // Create a group of switches specific to this tool
    SwitchGroup flirtLibraryMatch("FlirtLibraryMatch-specific switches");
    flirtLibraryMatch.name("flirtLibraryMatch");
    
    flirtLibraryMatch.insert(Switch("database-filename", 'd')
                             .argument("string", anyParser(settings.databaseName))
                             .doc("Filename of the FLIRT database.  Default is \"" + settings.databaseName + "\""));
    
    // Parse the command-line and get the non-switch, positional arguments at the end
    return p.with(flirtLibraryMatch).parse(argc, argv).apply().unreachedArgs();
}

int
main(int argc, char** argv) {
    ROSE_INITIALIZE;
    TimingPerformance timer ("AST Library Identification checker (main): time (sec) = ",true);
    P2::Engine engine;
    engine.doingPostFunctionStackDelta(false); // We don't need StackDelta analysis

    //----------------------------------------------------------
    // Parse Command line args
    Settings settings;
    std::vector<std::string> specimenNames = parseCommandLine(argc, argv, engine, settings);
     
    if (specimenNames.empty())
        throw std::runtime_error("no specimen specified; see --help");
     
    //----------------------------------------------------------
    // Make library hash
    Rose::Combinatorics::HasherFnv fnv;
    Rose::Combinatorics::Hasher& hasher = dynamic_cast<Rose::Combinatorics::Hasher&>(fnv);
    for (std::vector<std::string>::iterator fileIt = specimenNames.begin(); fileIt != specimenNames.end(); ++fileIt) {
        const char* filename = (*fileIt).c_str();
        struct stat st;
        int fhand = ::open(filename, O_RDONLY);
        ::fstat(fhand, &st);
        size_t file_size = st.st_size;
             
        uint8_t* file_buf = (uint8_t*)mmap(0, file_size, PROT_READ, MAP_FILE|MAP_PRIVATE, fhand, 0);
        hasher.append(file_buf, file_size);
        munmap(file_buf, file_size);
    }
    std::string libHash = fnv.toString();

    engine.loadSpecimens(specimenNames);

    // Some analyses need to know what part of the address space is being disassembled.
    ASSERT_not_null(engine.memoryMap());
    AddressIntervalSet executableSpace;
    for (const MemoryMap::Node &node: engine.memoryMap()->nodes()) {
        if ((node.value().accessibility() & MemoryMap::EXECUTABLE) != 0)
            executableSpace.insert(node.key());
    }
     
    // Create a partitioner that's tuned for a certain architecture, and then tune it even more depending on our command-line.
    P2::Partitioner partitioner = engine.createPartitioner();

    // Build a Library Identification database (in the current directory).
    Flir flir;
    flir.createDatabase(settings.databaseName);
    auto lib = Flir::Library::instance(libHash, "foo", "0.0", partitioner.instructionProvider().disassembler()->name());
    flir.insertLibrary(lib, partitioner);

#if 0
    // Match functions in AST against Library Identification database.
    matchAgainstLibraryIdentificationDataBase("testLibraryIdentification.db", project);
#else
    printf ("SKIPPING TEST OF BINARY AGAINST GENERATED DATABASE! \n");
#endif
}
