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
// Factory factory;
  Compass::Parameters params(Compass::findParameterFile());
  Compass::PrintingOutputObject output(std::cerr);
// Compass::TraversalBase* trav = factory.create(params, &output);
  Compass::TraversalBase* trav = new Checker(params, &output);

  if (trav) {
    trav->run(sageProject);
  } else {
    std::cerr << trav->getName() << " failed to initialize\n";
    std::cerr << "Short Description:\n";
    std::cerr << trav->getShortDescription();
    return 1;
  }
  return 0;
}
