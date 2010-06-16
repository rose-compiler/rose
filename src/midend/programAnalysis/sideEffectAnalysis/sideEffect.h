#ifndef SIDEEFFEECT_H
#define SIDEEFFEECT_H

#include "sage3.h"
#include <list>

// DQ (12/30/2005): This is a Bad Bad thing to do (I can explain)
// it hides names in the global namespace and causes errors in 
// otherwise valid and useful code. Where it is needed it should
// appear only in *.C files (and only ones not included for template 
// instantiation reasons) else they effect user who use ROSE unexpectedly.
// using namespace std;

// Abstract base class interface to the side effect implementation.
// Implements the algorithm described in:
// Interprocedural side-effect analysis in linear time
// Cooper and Kennedy
// PLDI 1988
// Pass command line arguments (as passed to compiler front end)
// to calcSideEffect in order to calculate side effects across the
// whole program.  Currently does not handle aliasing, though
// Cooper and Kennedy describe where to plug in an aliasing module.
// Also probably will not handle virtual method calls.   
class SideEffectAnalysis {

 public:

  //! "Constructor" to return a concrete instance of side effect implementation.
  static SideEffectAnalysis* create();

  //! Destructor.
  virtual ~SideEffectAnalysis() = 0;

  //! Perform the side effect analysis on the given project
  virtual int calcSideEffect(SgProject& project) = 0;
#if 1
  //! Perform the side effect analysis on a file
  virtual int calcSideEffect(SgFile& file) = 0;

  //! Perform the side effect analysis on a node
  virtual int calcSideEffect(SgNode& node) = 0;
#endif
  //! Return the list of invoked functions encountered during the analysis.
  virtual std::list<const char* > getCalledFunctions() = 0;

  //! Return a list of side effects for the given function.
  virtual std::list<const char* > getGMOD(const char* func) = 0;

  //! Return a list of side effects for the given statement.
  virtual std::list<const char* > getDMOD(const char* stmt) = 0;

  //! Return the identifier associated with this node and to be passed to getDMOD
  virtual std::string getNodeIdentifier(SgNode *node) = 0;

 protected:
  
  //! Disallow default constructor.
  SideEffectAnalysis();
  //! Disallow copy constructor.
  SideEffectAnalysis(const SideEffectAnalysis& rhs);

 private:
  //! Disallow assignment operator.
  SideEffectAnalysis& operator=(const SideEffectAnalysis& rhs);

};

//! Utility function to return the fully qualified name of a function given a function call expression AST node.
std::string getQualifiedFunctionName(SgFunctionCallExp *astNode);

//! Utility function to return the fully qualified name of a function given a fucntion declaration AST node.
std::string getQualifiedFunctionName(SgFunctionDeclaration *astNode);

#endif /* SIDEEFFECT_H */
