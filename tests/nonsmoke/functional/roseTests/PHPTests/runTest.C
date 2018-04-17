#include <rose.h>
#include <string>

using namespace std;

int
main(int argc, char** argv)
{
    if (argc != 2)
    {
        fprintf(stderr, "Usage: %s phpfile\n", argv[0]);
        return 1;
    }
    string scriptName = argv[1];

    SgProject* project = frontend(argc, argv);
    ROSE_ASSERT(project != NULL);

    SgFile* file = (*project)[0];
    ROSE_ASSERT(file);
    ROSE_ASSERT(file->get_PHP_only());

    ROSE_ASSERT(project->get_PHP_only());

    cout << "--- Unparsing --- " << endl;

    // probably best to only test the parsing until the PHP
    // specific unparser is implemented
    // project->unparse();

    cout << "--- All Done --- " << endl;

    return 0;
}
