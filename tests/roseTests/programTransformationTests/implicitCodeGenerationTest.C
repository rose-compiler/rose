#include "rose.h"
#include <defaultFunctionGenerator.h>
#include <shortCircuitingTransformation.h>
#include <destructorCallAnnotator.h>

#include <iostream>
using namespace std;

// This tool shall be used to test the three transformations
int main(int argc, char **argv)
   {
     assert (argv);
     vector<string> argvList(argv, argv + argc);

     bool doDefaultFunctionGenerator      = CommandlineProcessing::isOption(argvList, "-icg:", "defaultFunctionGenerator", true);
     bool doShortCircuitingTransformation = CommandlineProcessing::isOption(argvList, "-icg:", "shortCircuitingTransformation", true);
     bool doDestructorCallAnnotator       = CommandlineProcessing::isOption(argvList, "-icg:", "destructorCallAnnotator", true);

     cout << "doDefaultFunctionGenerator      = " << doDefaultFunctionGenerator      << endl;
     cout << "doShortCircuitingTransformation = " << doShortCircuitingTransformation << endl;
     cout << "doDestructorCallAnnotator       = " << doDestructorCallAnnotator       << endl;

     SgProject *prj = frontend(argvList);

     if (doDefaultFunctionGenerator)
        {
          defaultFunctionGenerator(prj);
        }

     if (doShortCircuitingTransformation)
        {
          shortCircuitingTransformation(prj);
        }

     if (doDestructorCallAnnotator)
        {
          destructorCallAnnotator(prj);
        }

     prj->get_file(0).set_unparse_includes(true);

     prj->unparse();
   }
