// Example ROSE Translator: used within ROSE/tutorial

#include "rose.h"
#include <functional>
using namespace std;

bool VERBOSE_MESSAGES_OF_WAVE = false;
#include "macroRewrapper.h"
#include <vector>





void test_iterate_over_all_macro_calls(AnalyzeMacroCalls* macroCalls, SgProject* project, macro_def_call_type& macro_def){

}




int main( int argc, char * argv[] )
   {
  // Build the AST used by ROSE

      SgProject* project = frontend(argc,argv);
      AnalyzeMacroCalls* macroCalls = new AnalyzeMacroCalls(project);
	 acro_def_call_type macrosCallsMappedToDefs = macroCalls->getMapOfDefsToCalls();

	test_iterate_over_all_macro_calls(macroCalls,project,macrosCallsMappedToDefs);
//     return backend(project);

   };

