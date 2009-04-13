// Example ROSE Translator: used within ROSE/tutorial

#include "rose.h"
#include <functional>
#include <ostream>
using namespace std;

bool VERBOSE_MESSAGES_OF_WAVE = true;
bool do_db_optimization = false;
#include "macroRewrapper.h"


static const char* OPTION_NAMESTYLEFILE = "*imd:db";

//! Default command-line prefix for ROSE options
static const char* OPTION_PREFIX_ROSE = "-rose:";


//! Default command-option separator tag
static const char* OPTION_VALUE_SEPARATOR = "$^";


int main( int argc, char * argv[] )
   {

     {
       std::vector<std::string> newArgv(argv,argv+argc);
       newArgv.push_back("-rose:skip_rose");
       SgProject* project = frontend(newArgv);
       backend(project);

     }
     std::vector<std::string> newArgv(argv,argv+argc);
     newArgv.push_back("-rose:wave");
     newArgv.push_back("-rose:excludePath");
     newArgv.push_back("/include/g++_HEADERS/");
     newArgv.push_back("-rose:excludePath");
     newArgv.push_back("/include/gcc_HEADERS/");
     newArgv.push_back("-rose:excludePath");
     newArgv.push_back("/include-staging/g++_HEADERS/");
     newArgv.push_back("-rose:excludePath");
     newArgv.push_back("/include-staging/gcc_HEADERS/");
     newArgv.push_back("-rose:excludePath");
     newArgv.push_back("/usr/include/");
     newArgv.push_back("-rose:excludePath");
     newArgv.push_back("/tests/CompileTest/");
     newArgv.push_back("-rose:excludePath");
     newArgv.push_back("<builtin>");
     newArgv.push_back("-rose:excludePath");
     newArgv.push_back("<built-in>");
     newArgv.push_back("-rose:excludePath");
     newArgv.push_back("<builltin>");



  // Build the AST used by ROSE
     SgProject* project = frontend(newArgv);
     ROSE_ASSERT(project != NULL);

  // Build a list of functions within the AST
     AnalyzeMacroCalls* macroCalls = new AnalyzeMacroCalls(project, false, std::cerr);

  // Assume that there is only one file
     std::string filename;
     for(int i = 0 ; i < project->numberOfFiles(); i++)
     {
       SgSourceFile* file = isSgSourceFile(&project->get_file(i));
       if( file != NULL)
         filename = file->getFileName();

     };

     ROSE_ASSERT(filename != "");

     filename+=".out";
     filename = StringUtility::stripPathFromFileName(filename);

     ofstream myfile;
     myfile.open (filename.c_str());

     ROSE_ASSERT(myfile.is_open());
     std::cout << "Outputing to the file " << filename << std::endl;


     macroCalls->print_out_all_macros(myfile);
     //     return backend(project);
     myfile.close();

   }

