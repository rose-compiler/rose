//
// Please Do Not Modify this file!!!
//
//
// This file should be included from your main file
// The variable myChecker should be defined to your checker's 
// checker object.  This file is provided to support testing
// of the detector in isolation.

#include "compass.h"
//#include <rose.h>
#include <iostream>

#if PERFLOG
#include <string>
#include <map>
//#include <rosehpct/util/general.hh>
//#include <rosehpct/xml2profir/xml2profir.hh>
#include <rosehpct/sage/sage.hh>
//#include <rosehpct/profir2sage/profir2sage.hh>
//#include <rosehpct/rosehpct.hh>

using namespace std;
//using namespace GenUtil;
using namespace RoseHPCT;
#endif

int main(int argc, char** argv) {
#if PERFLOG
  REGISTER_ATTRIBUTE_FOR_FILE_IO(MetricAttr);
  AST_FILE_IO::clearAllMemoryPools();
  SgProject * sageProject = (SgProject*) (AST_FILE_IO::readASTFromFile("input.perf.bin"));
  cout<<"Dumping to PDF..."<<endl;
  AstPDFGeneration pdf;
  pdf.generateInputFiles(sageProject);
#else
  Rose_STL_Container<std::string> commandLineArray = CommandlineProcessing::generateArgListFromArgcArgv (argc,argv);

  Compass::commandLineProcessing(commandLineArray);

  SgProject* sageProject = frontend(commandLineArray);

//  SgProject* sageProject = frontend(argc,argv);
#endif

// Factory factory;
  Compass::Parameters params(Compass::findParameterFile());
  Compass::PrintingOutputObject output(std::cerr);
// Compass::TraversalBase* trav = factory.create(params, &output);
  Compass::runCheckerAndPrereqs(myChecker, sageProject, params, &output);
  return 0;
}
