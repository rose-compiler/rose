#ifndef MATLAB_TO_CPP
#define MATLAB_TO_CPP

class SgProject;

/**
 * MatlabToCpp represents all the transformations that need to be done to
 * transform Matlab AST to C++ AST.
 */
namespace MatlabToCpp
{
  /**
   * takes a project and runs the transformations/traversals to transform Matlab
   * based AST to C++ AST.
   */
  void transform(SgProject* project);
}


#endif /* MATLAB_TO_CPP */
