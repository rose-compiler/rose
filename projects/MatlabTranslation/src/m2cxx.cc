#include <iostream>
#include <iterator>
#include <string>
#include <boost/filesystem.hpp>

#include "rose.h"

#include "MatlabParser.h"
#include "MatlabUnparser.h"
#include "transformations/MatlabToCpp.h"
#include "typeInference/MatlabTypeInference.h"
#include "transformations/armadillo/ArmaOptimizer.h"

#define WITH_ARMA 1
#define UNPARSE_TO_MATLAB 0

int main(int argc, char* argv[])
{
  ROSE_ASSERT(argc > 0);

  for (int z = 0; z < argc; ++z)
    std::cout << argv[z] << std::endl;

  boost::filesystem::path xcute(argv[0]);
  SgProject*              p = MatlabParser::frontend(argc, argv);

  if (UNPARSE_TO_MATLAB)
    MatlabUnparser::backend(p);
  // convert IR to SSA form
  //  MatlabAnalysis::convertToSSA(p);

  // preprocess and normalize (to some extent) the intermediate
  //   Matlab AST.
  MatlabAnalysis::loadMatlabBuiltins(xcute.remove_filename().string());
  MatlabAnalysis::makeFunFromUnrealVars(p);

  // deduce types
  MatlabAnalysis::typeAnalysis(p);

  // convert IR from SSA to "normal" form
  // MatlabAnalysis::convertToSSA(p);

  // transform code to C++
  MatlabToCpp::transform(p);

  // Armadillo library specific optimizations
  if (WITH_ARMA)
    ArmaOpt::optimize(p);

  // print C++ code
  backend(p);

  //AstTests::runAllTests(p);
  return 0;
}
