// Example ROSE Translator: used within ROSE/tutorial

#include "rose.h"
#include <functional>
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
                                                  &raw_values[0]);

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
  // Build the AST used by ROSE
     SgProject* project = frontend(argc,argv);
     ROSE_ASSERT(project != NULL);

     std::string imd_db = getRoseOptionValues (&argc, argv, OPTION_NAMESTYLEFILE);

     if( imd_db == "" ){
       cout << "Usage: -rose:imd:db <name-of-database> <rest-of-compilation-line>" << std::endl;
       exit(1);
     }

     //init_macdb will return 0 if success
     int open_db = init_macdb(imd_db.c_str());
     if(open_db != 0){
         cout << "Error: The database file " << imd_db << " can not be opened." << std::endl; 
         exit(1);
     } 
  // Build a list of functions within the AST
     AnalyzeMacroCalls* macroCalls = new AnalyzeMacroCalls(project);
     macroCalls->add_all_macro_calls_to_db();
     //checkMacrosMapOnlyTokensAtThatPosition(project);
     close_macdb();
  // Note: Show composition of AST queries
//     return backend(project);

   }

