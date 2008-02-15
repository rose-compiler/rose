//
// Please Do Not Modify this file!!!
//
//
// This file should be included from your main file
// The typedef Factory should be defined to your checker's 
// factory class.  This file is provided to support testing
// of the detector in isolation.

#include "compass.h"
#include <rose.h>
#include <iostream>

int main(int argc, char** argv) {
  SgProject* sageProject = frontend(argc,argv);
  Compass::Parameters params("compass_parameters");
  Compass::PrintingOutputObject output(std::cerr);
  Compass::TraversalBase* trav;
 
  try {
    trav = new Checker(params, &output);
  } catch (const Compass::ParameterNotFoundException& e) {
    std::cerr << e.what() << std::endl;
    trav = NULL;
  } catch (const Compass::ParseError& e) {
    std::cerr << e.what() << std::endl;
    trav = NULL;
  }

  if (trav) {
    trav->run(sageProject);
  } else {
    std::cerr << "error in compassTestMain: checker failed to initialize\n";
    return 1;
  }
  return 0;
}
