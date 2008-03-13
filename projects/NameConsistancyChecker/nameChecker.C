// Example ROSE Translator: used for testing ROSE infrastructure

#include "rose.h"

#include "checkNameImpl.h"

static const char* OPTION_NAMESTYLEFILE = "*name:file";

//! Default command-line prefix for ROSE options
static const char* OPTION_PREFIX_ROSE = "-rose:";


//! Default command-option separator tag
static const char* OPTION_VALUE_SEPARATOR = "$^";


//! Wrapper around the SLA string option processing routine.
#if 0
static	int
getRoseOptionValues (int* p_argc, char** argv, const char* opt_name,
		std::vector<std::string>& values)
   {
     int num_matches = sla_str (p_argc, argv,
		     OPTION_PREFIX_ROSE,
		     OPTION_VALUE_SEPARATOR,
		     opt_name,
		     (char **)NULL);
     if (num_matches > 0)
	{
	  char** raw_values = new char*[num_matches];
	  sla_str (p_argc, argv,
			  OPTION_PREFIX_ROSE, OPTION_VALUE_SEPARATOR, opt_name,
			  raw_values);
	  for (int i = 0; i < num_matches; i++)
	       values.push_back (std::string (raw_values[i]));
	}
     return num_matches;
   }
#endif

static	int
getRoseOptionValues (std::vector<std::string>& argv, const std::string& opt_name,
		std::vector<std::string>& values)
   {
     int num_matches = sla_str (argv,
		     OPTION_PREFIX_ROSE,
		     OPTION_VALUE_SEPARATOR,
		     opt_name,
		     (std::string*)NULL);
     if (num_matches > 0)
	{
	  std::vector<std::string> raw_values(num_matches);
	  sla_str (argv,
OPTION_PREFIX_ROSE, OPTION_VALUE_SEPARATOR, opt_name,
			  &raw_values[0]);
	  values.insert(values.end(), raw_values.begin(), raw_values.end());
	}
     return num_matches;
   }



int main( int argc, char * argv[] ) 
   {
     //std::cout << "GETTING PATHS" << std::endl;
     //getFilterPathMap(&argc,argv);
     std::vector<std::string> argvList(argv, argv + argc);
    
     //get file-names with input from commandline
     //The option is: -rose:name:file filename
     std::vector<std::string> raw_eqpaths;
     //getRoseOptionValues (&argc, argv, OPTION_NAMESTYLEFILE, raw_eqpaths);
     getRoseOptionValues (argvList, OPTION_NAMESTYLEFILE, raw_eqpaths);


  // #endifBuild the AST used by ROSE
     SgProject* project = frontend(argc,argv);

  // Run internal consistancy tests on AST
     //AstTests::runAllTests(project);

     for(std::vector<std::string>::iterator it = raw_eqpaths.begin();
		     it != raw_eqpaths.end(); ++it){
	  std::cout << "ENFORCING RULES FROM FILE: " << *it << std::endl;
	  NameEnforcer nm;
	  nm.readFile(*it);
	  nm.enforceRules(project);
	  std::cout << "DONE ENFORCING RULES FROM FILE: " << *it << std::endl;

     }
  // Insert your own manipulation of the AST here...

  // Generate source code from AST and call the vendor's compiler
     return backend(project);
   };
