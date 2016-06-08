#include <rose.h>

int main(int argc, char **argv)
{
    // Initialize and check compatibility. See rose::initialize
    ROSE_INITIALIZE;

    SgProject *project = frontend(argc, argv);

    std::string fileName = project->get_outputFileName();
    AST_FILE_IO::startUp(project);
    AST_FILE_IO::writeASTToFile(fileName += ".binary");

    std::cout << "done writing AST to " << fileName << "!" << std::endl;
}
