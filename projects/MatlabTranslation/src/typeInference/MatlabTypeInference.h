
#ifndef _MATLAB_TYPE_INFERENCE_H
#define _MATLAB_TYPE_INFERENCE_H

#include <map>
#include <utility>

// \todo remove
#include <iostream>

class SgProject;

namespace MatlabAnalysis
{
  /**
   * The way we built our SgMatlabForStatement is wrong.
   * It should have had two children, initialization ( i = expression) and body.
   * Right now for i = 1:10 is represented as i, 1:10, body
   * This is preventing the type inference system from finding out that i is assigned to 1:10
   *
   * \todo does not seem to be called at this point...
   *
   * Two ways to fix this:
   * 1) update for loop design. (this is the most easiest.)
   *    However we don't have EDG license so we cannot modify this.
   * 2) Insert a i = 1:10 statement before the for loop
   *    so that type inference engine can recognize that.
   */
  // void addAssignOpBeforeMatlabForStatement(SgProject *project);

  /**
   * loads type definitions for supported Matlab builtin functions.
   */
  void loadMatlabBuiltins(std::string basedir);


  /**
   * analyses the types of Matlab expressions.
   */
  void typeAnalysis(SgProject* project);

  /**
   * Changes identifiers in Matlab programs that should be treated
   * as function calls.
   *
   * Examples inlcude tic, tac which we want to transform to tic(), tac()
   */
  void makeFunFromUnrealVars(SgProject* project);
}

#endif /* _MATLAB_TYPE_INFERENCE_H */
