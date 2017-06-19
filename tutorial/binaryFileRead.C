#include <rose.h>

int main(int argc, char **argv)
{
    // Initialize and check compatibility. See Rose::initialize
    ROSE_INITIALIZE;

    if (argc != 2)
    {
        std::cerr << "must be called with exactly one binary AST file argument"
            << std::endl;
    }
    ROSE_ASSERT(argc == 2);

    AST_FILE_IO::readASTFromFile(argv[1]);
}
