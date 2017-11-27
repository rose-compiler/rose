static const char *purpose = "dump XML for various ROSE entities";
static const char *description =
    "Parse, disassemble, partition, and analyze the specified binary specimen and then produce XML that describes various "
    "parts of it.";

// To generate JSON, pipe the output from this command through an XML-to-JSON filter such as "xml2json.py --strip_text", which
// you can find (here)[https://github.com/hay/xml2json]. If you want the JSON to be pretty printed, send it also through "jq ."

#include <rose.h>
#include <rosePublicConfig.h>

#ifdef ROSE_HAVE_BOOST_SERIALIZATION_LIB

#include <boost/archive/xml_oarchive.hpp>
#include <Partitioner2/Engine.h>
#include <Sawyer/Stopwatch.h>

using namespace Rose;
using namespace Rose::BinaryAnalysis;
using namespace Sawyer::Message::Common;
namespace P2 = Rose::BinaryAnalysis::Partitioner2;

enum SerializableEntity {
    SERIALIZE_PARTITIONER,                              // serialize the entire partitioner
    SERIALIZE_MMAP,                                     // serialize the MemoryMap
    SERIALIZE_AST                                       // serialize the AST
};

Sawyer::Message::Facility mlog;
SerializableEntity whatToSerialize = SERIALIZE_PARTITIONER;
boost::filesystem::path outputFileName;

// Parse command-line and return the positional arguments describing the binary specimen.
std::vector<std::string>
parseCommandLine(int argc, char *argv[], P2::Engine &engine) {
    using namespace Sawyer::CommandLine;

    SwitchGroup tool("Tool-specific switches");

    tool.insert(Switch("output", 'o')
                .argument("name", anyParser(outputFileName))
                .doc("Name of file into which the XML is written. The default is to send it to standard output."));

    tool.insert(Switch("entity")
                .argument("name", enumParser(whatToSerialize)
                          ->with("ast", SERIALIZE_AST)
                          ->with("mmap", SERIALIZE_MMAP)
                          ->with("partitioner", SERIALIZE_PARTITIONER))
                .doc("Specifies what entities of the ROSE internal representation should be serialized to XML."

                     "@named{ast}{Prints all information stored in the AST subtrees rooted at SgAsmGenericFile nodes. "
                     "This is the data structure used by analses that are written in the style of ROSE source code "
                     "analyses and includes information organized hierarchally.}"

                     "@named{mmap}{Prints information about the memory map. The memory map describes how the specimen "
                     "is mapped into virtual memory.}"

                     "@named{partitioner}{All information from Rose::BinaryAnalysis::Partitioner2. This is the "
                     "data structure used by most binary analysis and includes things such as the address map, "
                     "control flow graph, functions, basic blocks, instructions, static data blocks, memory map "
                     "and possibly stack delta analysis, may-return analysis, calling convention analysis, etc. "
                     "depending on what post-partitioning analyses were enabled.}"));


    Parser p = engine.commandLineParser(purpose, description);
    p.errorStream(mlog[FATAL]);
    return p.with(tool).parse(argc, argv).apply().unreachedArgs();
}

int
main(int argc, char *argv[]) {
    ROSE_INITIALIZE;
    Diagnostics::initAndRegister(&mlog, "tool");

    P2::Engine engine;
    std::vector<std::string> specimen = parseCommandLine(argc, argv, engine);
    P2::Partitioner partitioner = engine.partition(specimen);

    mlog[INFO] <<"generating XML";
    Sawyer::Stopwatch timer;

    std::ofstream tmpOutputFile(outputFileName.string().c_str());
    std::ostream &outputFile = outputFileName.empty() ? std::cout : tmpOutputFile;
    boost::archive::xml_oarchive xml(outputFile);

    switch (whatToSerialize) {
        case SERIALIZE_MMAP: {
            MemoryMap::Ptr mmap = partitioner.memoryMap();
            xml <<BOOST_SERIALIZATION_NVP(mmap);
            break;
        }
            
        case SERIALIZE_PARTITIONER:
            xml <<BOOST_SERIALIZATION_NVP(partitioner);
            break;

        case SERIALIZE_AST: {
            SgAsmBlock *gblock = P2::Modules::buildAst(partitioner, engine.interpretation(), engine.settings().astConstruction);
            if (SgProject *project = SageInterface::getEnclosingNode<SgProject>(gblock)) {
                std::vector<SgAsmGenericFile*> files = SageInterface::querySubTree<SgAsmGenericFile>(project);
                BOOST_FOREACH (SgAsmGenericFile *file, files) {
                    SgNode *parent = file->get_parent();
                    file->set_parent(NULL);
                    xml <<BOOST_SERIALIZATION_NVP(file);
                    file->set_parent(parent);
                }
                std::vector<SgAsmInterpretation*> interps = SageInterface::querySubTree<SgAsmInterpretation>(project);
                BOOST_FOREACH (SgAsmInterpretation *interp, interps) {
                    SgNode *parent = interp->get_parent();
                    interp->set_parent(NULL);
                    xml <<BOOST_SERIALIZATION_NVP(interp);
                    interp->set_parent(parent);
                }
            } else {
                SgNode *parent = gblock->get_parent();
                gblock->set_parent(NULL);
                xml <<BOOST_SERIALIZATION_NVP(gblock);
                gblock->set_parent(parent);
            }
            break;
        }
    }
    mlog[INFO] <<"; took " <<timer <<" seconds\n";
}

#else

int
main() {
    std::cerr <<"error: this configuration of ROSE does not support XML serialization\n";
    exit(1);
}

#endif
