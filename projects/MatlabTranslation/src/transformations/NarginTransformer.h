#ifndef _NARGIN_TRANSFORMER_H
#define _NARGIN_TRANSFORMER_H 1

class SgProject;

/**
 * MatlabToCpp represents all the transformations that need to be done to transform Matlab AST to C++ AST
 */
namespace MatlabToCpp
{
  /**
   * takes a project and runs the transformations/traversals to transform Matlab based AST to C++ AST
   */
  void transformNargin(SgProject* project);
}


#endif /* _NARGIN_TRANSFORMER_H */
