#include <rose.h>

#include <BinarySourceLocations.h>
#include <Partitioner2/Engine.h>

using namespace Rose::BinaryAnalysis;
namespace P2 = Rose::BinaryAnalysis::Partitioner2;

int
main(int argc, char *argv[]) {
    // Initializations
    ROSE_INITIALIZE;
    std::vector<std::string> specimens(argv+1, argv+argc);

    // Parsing
    P2::Engine engine;
    engine.parseContainers(specimens);
    SgProject *project = SageInterface::getProject();

    // Output
    SourceLocations lineMapper;
    lineMapper.insertFromDebug(project);
    lineMapper.printSrcToAddr(std::cout);
    lineMapper.printAddrToSrc(std::cout);
}
