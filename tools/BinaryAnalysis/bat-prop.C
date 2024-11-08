static const char *purpose = "print some property of the RBA file";
static const char *description =
    "Given a BAT state for a binary specimen, print the specified property.";

#include <batSupport.h>

#include <Rose/BinaryAnalysis/Disassembler/Base.h>
#include <Rose/BinaryAnalysis/Partitioner2/Engine.h>
#include <Rose/BinaryAnalysis/Partitioner2/Partitioner.h>
#include <Rose/CommandLine.h>
#include <Rose/Initialize.h>

#include <boost/algorithm/string/predicate.hpp>

using namespace Rose;
using namespace Rose::BinaryAnalysis;
using namespace Sawyer::Message::Common;
namespace P2 = Rose::BinaryAnalysis::Partitioner2;

namespace {

Sawyer::Message::Facility mlog;

struct Settings {
    SerialIo::Format stateFormat = SerialIo::BINARY;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Property definitions
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class Property {
public:
    virtual ~Property() {}
    virtual std::string name() const = 0;
    virtual std::string doc() const = 0;
    virtual void eval(const P2::Partitioner::ConstPtr&) const = 0;
};

class IsaProperty: public Property {
public:
    std::string name() const {
        return "isa";
    }

    std::string doc() const {
        return "Instruction set architecture.";
    }

    void eval(const P2::Partitioner::ConstPtr &partitioner) const {
        if (Disassembler::Base::Ptr d = partitioner->instructionProvider().disassembler()) {
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

    void eval(const P2::Partitioner::ConstPtr &partitioner) const {
        std::cout <<partitioner->instructionProvider().instructionPointerRegister().nBits() <<"\n";
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

    void eval(const P2::Partitioner::ConstPtr &partitioner) const {
        std::cout <<partitioner->nFunctions() <<"\n";
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

    void eval(const P2::Partitioner::ConstPtr &partitioner) const {
        std::cout <<partitioner->nBasicBlocks() <<"\n";
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

    void eval(const P2::Partitioner::ConstPtr &partitioner) const {
        std::cout <<partitioner->nDataBlocks() <<"\n";
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

    void eval(const P2::Partitioner::ConstPtr &partitioner) const {
        std::cout <<partitioner->nInstructions() <<"\n";
    }
};

class IsaNameProperty: public Property {
public:
    std::string name() const {
        return "isa";
    }

    std::string doc() const {
        return "Name of instruction set architecture.";
    }

    void eval(const P2::Partitioner::ConstPtr &partitioner) const {
        std::cout <<partitioner->instructionProvider().disassembler()->name() <<"\n";
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
        for (Property *property: properties_.values())
            s += "@named{" + property->name() + "}{" + property->doc() + "}\n";
        return s;
    }

    bool check(const std::vector<std::string> &names, std::ostream &errorStream) const {
        size_t status = true;
        for (const std::string &name: names) {
            if (name != "all" && !properties_.exists(name)) {
                errorStream <<"unrecognized property: \"" <<StringUtility::cEscape(name) <<"\"\n";
                status = false;
            }
        }
        return status;
    }
                
    void eval(const std::string &name, const P2::Partitioner::ConstPtr &partitioner) {
        Property *property = NULL;
        if (properties_.getOptional(name).assignTo(property)) {
            property->eval(partitioner);
        } else {
            mlog[ERROR] <<"property name not recognized: \"" <<StringUtility::cEscape(name) <<"\"\n";
            ++nErrors_;
        }
    }

    void evalAll(const P2::Partitioner::ConstPtr &partitioner) {
        for (Property *property: properties_.values()) {
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

Sawyer::CommandLine::Parser
createSwitchParser(Settings &settings, Properties &properties) {
    using namespace Sawyer::CommandLine;

    //---------- Generic Switches ----------
    SwitchGroup generic = Rose::CommandLine::genericSwitches();
    generic.insert(Bat::stateFileFormatSwitch(settings.stateFormat));

    //---------- Parsing -----------
    Parser parser = Rose::CommandLine::createEmptyParser(purpose, description);
    parser.errorStream(mlog[FATAL]);
    parser.with(generic);
    parser.doc("Synopsis", "@prop{programName} [@v{switches}] @v{specimen} [--] @v{properies}...");
    parser.doc("Properties", properties.doc());
    return parser;
}

std::vector<std::vector<std::string>>
parseCommandLine(int argc, char *argv[], Sawyer::CommandLine::Parser &parser) {
    std::vector<std::vector<std::string>> groups = parser.regroupArgs(parser.parse(argc, argv).apply().unreachedArgs());

    // If one group, then the first arg is the specimen and the rest are property names
    if (groups.size() == 1) {
        groups.push_back(std::vector<std::string>());
        if (!groups[0].empty()) {
            std::swap(groups[0], groups[1]);
            groups[0].push_back(groups[1][0]);
            groups[1].erase(groups[1].begin());
        }
    }

    if (groups.size() != 2 || groups[0].empty()) {
        mlog[FATAL] <<"incorrect usage; see --help\n";
        exit(1);
    }

    if (groups[0].empty())
        groups[0].push_back("-");

    if (groups[1].empty())
        mlog[WARN] <<"no properties requested; only checking whether RBA loads\n";

    return groups;
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
    properties.define(new IsaNameProperty);

    // Parse command-line
    Settings settings;
    Sawyer::CommandLine::Parser switchParser = createSwitchParser(settings, properties);
    auto engine = P2::Engine::forge(argc, argv, switchParser /*in,out*/);
    const std::vector<std::vector<std::string>> groups = parseCommandLine(argc, argv, switchParser);
    ASSERT_require(groups.size() == 2);
    const std::vector<std::string> specimen = groups[0];
    ASSERT_forbid(specimen.empty());
    const std::vector<std::string> propNames = groups[1];

    // Ingest specimen
    P2::Partitioner::ConstPtr partitioner;
    if (specimen.size() == 1 && (specimen[0] == "-" || boost::ends_with(specimen[0], ".rba"))) {
        try {
            partitioner = P2::Partitioner::instanceFromRbaFile(specimen[0], settings.stateFormat);
        } catch (const std::exception &e) {
            mlog[FATAL] <<"cannot load partitioner from " <<specimen[0] <<": " <<e.what() <<"\n";
            exit(1);
        }
    } else {
        partitioner = engine->partition(specimen);
    }
    ASSERT_not_null(partitioner);

    if (!properties.check(propNames, mlog[FATAL]))
        exit(1);
    size_t showAllProperties = std::count(propNames.begin(), propNames.end(), "all");

    if (showAllProperties) {
        properties.evalAll(partitioner);
    } else {
        for (const std::string &property: propNames)
            properties.eval(property, partitioner);
    }

    return properties.nErrors() > 0;
}
