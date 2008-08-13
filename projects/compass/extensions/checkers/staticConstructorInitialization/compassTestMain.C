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
  Compass::Parameters params(Compass::findParameterFile());
  Compass::PrintingOutputObject output(std::cerr);
  const Compass::Checker* const checker = staticConstructorInitializationChecker;
 
  Compass::runCheckerWithPrereqs(checker, sageProject, params, output);
  return 0;
}
