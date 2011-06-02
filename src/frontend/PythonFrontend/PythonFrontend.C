#include "sage3basic.h"
#include "rose_config.h"

#ifndef USE_ROSE_PYTHON_SUPPORT // If Python is disabled then compile a stub


#include <iostream>

int python_main(std::string, SgFile*)
{
    std::cerr << "python_main was called" << std::endl;
    return -1;
}

#else // Else if Python *is enabled*


#include <AST.h>
#include <AST_fold.h>
#include <parsing/parse.h>
#include <cmdline.h>
#include <embed/embed.h>
#include <pass_manager/Pass_manager.h>

using namespace AST;
using namespace SageBuilder;
using namespace SageInterface;

int python_main(string filename, SgFile *file) {    
    ROSE_ASSERT(file);

    //PHP::startup_php();

    //String sfilename(filename);
    //PHP_script* ir = parse(&sfilename, 0);
    //ROSE_ASSERT(ir);

    //AST_to_ROSE* trans = new AST_to_ROSE(file);
    //trans->fold_php_script(ir);

    cout << "python_main() not implemented" << endl;
    return 0;
}
#endif // else (ROSE_PYTHON is defined)
