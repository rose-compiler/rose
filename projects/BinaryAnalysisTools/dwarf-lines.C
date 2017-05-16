#include <rose.h>
#include <DwarfLineMapper.h>
#include <Partitioner2/Engine.h>

using namespace rose::BinaryAnalysis;
namespace P2 = rose::BinaryAnalysis::Partitioner2;

int
main(int argc, char *argv[]) {
    ROSE_INITIALIZE;
#if 0 // [Robb P Matzke 2017-05-16]
    SgProject *project = ::frontend(argc, argv);
#else
    std::vector<std::string> specimens(argv+1, argv+argc);
    P2::Engine engine;
    engine.parseContainers(specimens);
    SgProject *project = SageInterface::getProject();
#endif
    DwarfLineMapper lineMapper(project);
    lineMapper.print(std::cout);
}
