/// Test the MPI Determinism analysis against a single test case.
///
/// Run this program like any other ROSE-based tool, passing it a
/// source file or files to work on, and relevant compiler frontend
/// options. Specifically, the mpi.h distrubuted alongside this must
/// proceed any other in the #include paths.
///
/// For this code to determine if the analysis was correct, the target
/// code must define three int variables:
/// - SOURCE_DETERMINISM: whether the program always tells MPI exactly
///   where messages are coming from
/// - TAG_DETERMINISM: whether the program always tells MPI exactly
///   what tags to expect
/// - FUNCTION_DETERMINISM: whether the program avoids use of
///   non-determinism via checking for a subset of outstanding
///   requests (e.g. MPI_Waitany)
/// If the code is deterministic in a given respect, the variable
/// should be set to 1. Otherwise, it should be set to 0.

#include "MpiDeterminismAnalysis.h"
#include <iostream>
#include <Sawyer/Assert.h>

using std::cerr;
using std::cout;
using std::endl;

DeterminismState getExpectation(SgNode *ast, const char *varName)
{
  SgName name(varName);

  Rose_STL_Container<SgNode*> sdNodes = NodeQuery::querySubTree(ast, &name, NodeQuery::VariableDeclarationFromName);
  if (sdNodes.size() != 1) {
    cerr << "Didn't find target variable " << varName << " in list of size " << sdNodes.size() << endl;

    for (Rose_STL_Container<SgNode*>::iterator i = sdNodes.begin(); i != sdNodes.end(); ++i)
      cerr << "\t" << (*(isSgVariableDeclaration(*i)->get_variables().begin()))->get_name().str() << endl;

    return QUESTIONABLE;
  }

  SgNode *nSd = *(sdNodes.begin());
  SgVariableDeclaration *vdSd = dynamic_cast<SgVariableDeclaration *>(nSd);
  if (!vdSd) {
    cerr << "Node wasn't a variable declaration" << endl;
    return QUESTIONABLE;
  }

  SgInitializedName *inSd = vdSd->get_decl_item(name);
  SgAssignInitializer *aiSd = dynamic_cast<SgAssignInitializer*>(inSd->get_initializer());
  if (!aiSd) {
    cerr << "Couldn't pull an assignment initializer out" << endl;
    return QUESTIONABLE;
  }

  SgIntVal *ivSd = dynamic_cast<SgIntVal*>(aiSd->get_operand());
  if (!ivSd) {
    cerr << "Assignment wasn't an intval" << endl;
    return QUESTIONABLE;
  }

  int value = ivSd->get_value();
  return value ? DETERMINISTIC : NONDETERMINISTIC;
}

const char* strFromDet(DeterminismState d)
{
  switch (d) {
  case DETERMINISTIC: return "deterministic";
  case QUESTIONABLE: return "unclear";
  case NONDETERMINISTIC: return "NONdeterminisitic";
  }
  ASSERT_not_reachable("unhandled DeterminismState");
}

void report(DeterminismState actual, DeterminismState expected, const char *kind, int &incorrect, int &imprecise)
{
  cout << "\t" << strFromDet(actual) << " (expected " << strFromDet(expected) << ") with respect to receive " << kind << endl;
  
  if (QUESTIONABLE == actual && QUESTIONABLE != expected)
    imprecise++;
  else if (actual != expected)
    incorrect++;
}

int main(int argc, char **argv)
{
  SgProject *project = frontend(argc, argv);

  // Check that the mpi.h included by the program is the hacked
  // version that defines all the constants as int variables, and not
  // as macros
  SgName hacked("MPI_HACKED_HEADER_INCLUDED");
  Rose_STL_Container<SgNode*> vars = NodeQuery::querySubTree(project, &hacked, NodeQuery::VariableDeclarationFromName);
  if (vars.size() != 1) {
    cerr << "You must be using the hacked mpi.h that defines things nicely for this to work!" << endl;
    return 10;
  }

  //  ConstantPropagationAnalysis cp;

  MpiDeterminismAnalysis a;
  MpiDeterminism d = a.traverse(project);

  DeterminismState sourceExpectation = getExpectation(project, "SOURCE_DETERMINISM");
  DeterminismState tagExpectation = getExpectation(project, "TAG_DETERMINISM");
  DeterminismState functionExpectation = getExpectation(project, "FUNCTION_DETERMINISM");

  int incorrect = 0, imprecise = 0;

  cout << "Analysis finds that this program is" << endl;
  report(d.source, sourceExpectation, "sources", incorrect, imprecise);
  report(d.tag, tagExpectation, "tags", incorrect, imprecise);
  report(d.functions, functionExpectation, "functions", incorrect, imprecise);

  cout << "Analysis was precise in " << 3 - incorrect - imprecise 
       << " cases, imprecise in " << imprecise 
       << " cases, and WRONG in " << incorrect << " cases." << endl;

  delete project;

  return incorrect;
}
