#include <rose.h>

int main(int argc, char **argv)
{
    // Initialize and check compatibility. See rose::initialize
    ROSE_INITIALIZE;

    if (argc != 2)
    {
        std::cerr << "must be called with exactly one binary AST file argument"
            << std::endl;
    }
    ROSE_ASSERT(argc == 2);

    SgProject *project = AST_FILE_IO::readASTFromFile(argv[1]);

    // your processing goes here...
}
