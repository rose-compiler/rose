static const char *purpose = "test parsing and unparsing of JVM classes";
static const char *description = "Reads a JVM class, disassembles it, creates an AST, and then unparses it.";

#define ROSE_BINARY_TEST_DISABLED 0
#if ROSE_BINARY_TEST_DISABLED
#include <iostream>
int main() { std::cout << "ByteCode/Jvm/testParseUnparse disabled \n"; return 1; }
#else

#include <rose.h>
#include <Rose/BinaryAnalysis/Partitioner2/BasicBlock.h>
#include <Rose/BinaryAnalysis/Partitioner2/EngineJvm.h>
#include <Rose/BinaryAnalysis/Partitioner2/GraphViz.h>
#include <Rose/BinaryAnalysis/Partitioner2/Partitioner.h>

using namespace Rose;
using namespace Rose::BinaryAnalysis;
namespace P2 = Rose::BinaryAnalysis::Partitioner2;

int
main(int argc, char *argv[]) {
    // Get a Jvm engine instance
    P2::EngineJvm::Ptr engine = P2::EngineJvm::instance();

    // Parse the command line to find the specimen files
    std::vector<std::string> specimen = engine->parseCommandLine(argc, argv, purpose, description).unreachedArgs();

    // Parse the files and create the AST
    SgProject* project = engine->roseFrontendReplacement(specimen);
    
    // Unparse the files from the AST
    int status = backend(project);

    return status;
};

#endif
