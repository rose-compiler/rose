// Example ROSE Translator: used within ROSE/tutorial

#include "rose.h"

#include "macro.h"
#include <functional>
#include <ostream>
using namespace std;

bool VERBOSE_MESSAGES_OF_WAVE = true;
bool do_db_optimization = true;

#include "macroRewrapper.h"


static const char* OPTION_NAMESTYLEFILE = "*imd:db";

//! Default command-line prefix for ROSE options
static const char* OPTION_PREFIX_ROSE = "-rose:";


//! Default command-option separator tag
static const char* OPTION_VALUE_SEPARATOR = "$^";


//! Wrapper around the SLA string option processing routine.
std::string
getRoseOptionValues (int* p_argc, char** argv, const char* opt_name)
   {
     std::vector<std::string> newArgv(&argv[0],&argv[0]+*p_argc);
     int num_matches = sla_str (newArgv,
                     OPTION_PREFIX_ROSE,
                     OPTION_VALUE_SEPARATOR,
                     opt_name,
                     (std::string*)NULL);
     string value = "";
     if (num_matches > 0)
        {
          vector<string> raw_values(num_matches);
                    sla_str (newArgv,
                        OPTION_PREFIX_ROSE, OPTION_VALUE_SEPARATOR, opt_name,
                                                  &raw_values[0],1);

          if(num_matches>1){
             cout << "Error: More than one match to option -rose:imd:db " << endl;
             exit(1);
          }

          value = string(raw_values[0]);
        }
     return value;
   }



int main( int argc, char * argv[] )
   {

     //Get the database name and remove the option
     std::string imd_db = getRoseOptionValues (&argc, argv,OPTION_NAMESTYLEFILE);

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
     newArgv.push_back("<builtin>");
     newArgv.push_back("-rose:excludePath");
     newArgv.push_back("<built-in>");
     newArgv.push_back("-rose:excludePath");
     newArgv.push_back("<builltin>");

     newArgv.push_back("-rose:excludePath");
     newArgv.push_back("/usr/include/");
     newArgv.push_back("-rose:excludePath");
     newArgv.push_back("/tests/CompileTest/");



     imd_db="/home/saebjornsen1/tmp/compile-with-rose/testing.db";

     if( imd_db == "" ){
       cout << "Usage: -rose:imd:db <name-of-database> <rest-of-compilation-line>" << std::endl;
       exit(1);
     }
     CommandlineProcessing::removeArgsWithParameters(newArgv, string(string(OPTION_PREFIX_ROSE)+ string(OPTION_NAMESTYLEFILE)).c_str() );

     //init_macdb will return 0 if success
     int open_db = init_macdb(imd_db.c_str());
     if(open_db != 0){
       cout << "Error: The database file " << imd_db << " can not be opened." << std::endl; 
       exit(1);
     } 

     //Create the database tables if they don't exist
     create_database_tables();

  // Build the AST used by ROSE
     SgProject* project = frontend(newArgv);
     ROSE_ASSERT(project != NULL);


  // Build a list of functions within the AST
     AnalyzeMacroCalls* macroCalls = new AnalyzeMacroCalls(project,false, std::cerr);
std::cout << "Test" << std::endl;

     macroCalls->add_all_macro_calls_to_db();
std::cout << "Test" << std::endl;

     //checkMacrosMapOnlyTokensAtThatPosition(project);
     close_macdb();
  // Note: Show composition of AST queries
//     return backend(project);

   }
