static const char *purpose = "print some property of the RBA file";
static const char *description =
    "Given a BAT state for a binary specimen, print the specified property."

    "This tool reads the binary analysis state file provided as a command-line positional argument, or standard input if "
    "the name is \"-\" (a single hyphen) or not specified. The standard input mode works only on those operating systems "
    "whose standard input is opened in binary mode, such as Unix-like systems.";

#include <rose.h>
#include <CommandLine.h>                                // rose
#include <Partitioner2/Engine.h>                        // rose

#include <batSupport.h>

using namespace Rose;
using namespace Rose::BinaryAnalysis;
using namespace Sawyer::Message::Common;
namespace P2 = Rose::BinaryAnalysis::Partitioner2;

namespace {

Sawyer::Message::Facility mlog;
SerialIo::Format stateFormat = SerialIo::BINARY;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Property definitions
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class Property {
public:
    virtual ~Property() {}
    virtual std::string name() const = 0;
    virtual std::string doc() const = 0;
    virtual void eval(const P2::Partitioner&) const = 0;
};

class IsaProperty: public Property {
public:
    std::string name() const {
        return "isa";
    }

    std::string doc() const {
        return "Instruction set architecture.";
    }

    void eval(const P2::Partitioner &partitioner) const {
        if (Disassembler *d = partitioner.instructionProvider().disassembler()) {
            std::cout <<d->name() <<"\n";
        } else {
            std::cout <<"none\n";
        }
    }
};

class WordSizeProperty: public Property {
public:
    std::string name() const {
        return "wordsize";
    }

    std::string doc() const {
        return "Nominal size of data for architecture, measured in bits.";
    }

    void eval(const P2::Partitioner &partitioner) const {
        std::cout <<partitioner.instructionProvider().instructionPointerRegister().nBits() <<"\n";
    }
};

class FunctionCountProperty: public Property {
public:
    std::string name() const {
        return "nfunctions";
    }

    std::string doc() const {
        return "Number of functions.  Note that a \"function\" in binary analysis does not necessarily correspond to what "
            "might be defined as a \"function\" in the source code.";
    }

    void eval(const P2::Partitioner &partitioner) const {
        std::cout <<partitioner.nFunctions() <<"\n";
    }
};

class BasicBlockCountProperty: public Property {
public:
    std::string name() const {
        return "nbblocks";
    }

    std::string doc() const {
        return "Number of basic blocks.";
    }

    void eval(const P2::Partitioner &partitioner) const {
        std::cout <<partitioner.nBasicBlocks() <<"\n";
    }
};

class DataBlockCountProperty: public Property {
public:
    std::string name() const {
        return "ndblocks";
    }

    std::string doc() const {
        return "Number of static data blocks.";
    }

    void eval(const P2::Partitioner &partitioner) const {
        std::cout <<partitioner.nDataBlocks() <<"\n";
    }
};

class InsnCountProperty: public Property {
public:
    std::string name() const {
        return "ninsns";
    }

    std::string doc() const {
        return "Number of instructions appearing in the global control flow graph.";
    }

    void eval(const P2::Partitioner &partitioner) const {
        std::cout <<partitioner.nInstructions() <<"\n";
    }
};





////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Container for all known properties
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class Properties {
    Sawyer::Container::Map<std::string, Property*> properties_;
    size_t nErrors_;

public:
    Properties()
        : nErrors_(0) {}

    void define(Property *property) {
        ASSERT_not_null(property);
        properties_.insert(property->name(), property);
    }

    std::string doc() const {
        std::string s = "The following properties are defined:\n\n";
        BOOST_FOREACH (Property *property, properties_.values())
            s += "@named{" + property->name() + "}{" + property->doc() + "}\n";
        return s;
    }

    bool check(const std::vector<std::string> &names, std::ostream &errorStream) const {
        size_t status = true;
        BOOST_FOREACH (const std::string &name, names) {
            if (name != "all" && !properties_.exists(name)) {
                errorStream <<"unrecognized property: \"" <<StringUtility::cEscape(name) <<"\"\n";
                status = false;
            }
        }
        return status;
    }
                
    void eval(const std::string &name, const P2::Partitioner &partitioner) {
        Property *property = NULL;
        if (properties_.getOptional(name).assignTo(property)) {
            property->eval(partitioner);
        } else {
            mlog[ERROR] <<"property name not recognized: \"" <<StringUtility::cEscape(name) <<"\"\n";
            ++nErrors_;
        }
    }

    void evalAll(const P2::Partitioner &partitioner) {
        BOOST_FOREACH (Property *property, properties_.values()) {
            std::cout <<property->name() <<": ";
            property->eval(partitioner);
        }
    }

    size_t nErrors() const {
        return nErrors_;
    }
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Command-line parsing
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Parses the command-line and returns the positional arguments, the first of which is always present and is the name of the
// RBA file.
std::vector<std::string>
parseCommandLine(int argc, char *argv[], P2::Engine &engine, const Properties &properties) {
    using namespace Sawyer::CommandLine;

    //---------- Generic Switches ----------
    SwitchGroup generic = Rose::CommandLine::genericSwitches();
    generic.insert(Bat::stateFileFormatSwitch(stateFormat));

    //---------- Parsing -----------
    Parser parser = Rose::CommandLine::createEmptyParser(purpose, description);
    parser.errorStream(mlog[FATAL]);
    parser.with(generic);
    parser.doc("Synopsis", "@prop{programName} [@v{switches}] @v{rba-state} @v{properies}...");
    parser.doc("Properties", properties.doc());

    std::vector<std::string> args = parser.parse(argc, argv).apply().unreachedArgs();
    if (args.empty()) {
        mlog[FATAL] <<"incorrect usage; see --help\n";
        exit(1);
    } else if (args.size() == 1) {
        mlog[WARN] <<"no properties requested; only checking whether RBA loads\n";
    }
    
    return args;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
} // namespace

int
main(int argc, char *argv[]) {
    ROSE_INITIALIZE;
    Diagnostics::initAndRegister(&mlog, "tool");
    mlog.comment("querying basic properties of a specimen");
    Bat::checkRoseVersionNumber(MINIMUM_ROSE_LIBRARY_VERSION, mlog[FATAL]);
    Bat::registerSelfTests();

    Properties properties;
    properties.define(new IsaProperty);
    properties.define(new WordSizeProperty);
    properties.define(new FunctionCountProperty);
    properties.define(new BasicBlockCountProperty);
    properties.define(new DataBlockCountProperty);
    properties.define(new InsnCountProperty);

    P2::Engine engine;
    std::vector<std::string> args = parseCommandLine(argc, argv, engine, properties);
    boost::filesystem::path inputFileName = args[0];
    args.erase(args.begin());
    if (!properties.check(args, mlog[FATAL]))
        exit(1);
    size_t showAllProperties = std::count(args.begin(), args.end(), "all");

    P2::Partitioner partitioner = engine.loadPartitioner(inputFileName, stateFormat);

    if (showAllProperties) {
        properties.evalAll(partitioner);
    } else {
        BOOST_FOREACH (const std::string &property, args)
            properties.eval(property, partitioner);
    }

    return properties.nErrors() > 0;
}
