// Liao 5/5/2011
// test build AST from string
#include <rose.h>
#include <stdio.h>
#include "AstFromString.h"

using namespace std;
using namespace SageInterface;
using namespace SageBuilder;
using namespace AstFromString;

//! Build a statement from its string format, under the specified scope. Additional SageInterface function calls are needed to insert the built statement into a desired point under the scope.
SgStatement* buildStatementFromString(const string& s, SgScopeStatement * scope)
{
  SgStatement* result = NULL;
  assert (scope != NULL);
  // set input and context for the parser
  c_char = s.c_str();
#if 0
  char* s_char = new char [s.size() +1];
  strcpy (s_char, s.c_str());
  c_char = s_char;
#endif
  assert (c_char== s.c_str());
  c_sgnode = scope;
  if (afs_match_statement())
  {
    result = isSgStatement(c_parsed_node); // grab the result
    assert (result != NULL);
  }
  else
  {
    cerr<<"Error. buildStatementFromString() cannot parse input string:"<<s
        <<"\n\t under the given scope:"<<scope->class_name() <<endl;
    assert (0);
  }

  return result;
}
int main(int argc, char** argv)
{
  SgProject* project = frontend(argc, argv);
  AstTests::runAllTests(project);

  // Locate the target scope
  SgFunctionDeclaration* func= findDeclarationStatement<SgFunctionDeclaration> (project, "bar", NULL, true);
  ROSE_ASSERT (func != NULL);

  SgBasicBlock* f_body = func->get_definition()->get_body();
  assert (f_body != NULL);
  SgStatement* s = NULL;

  // build simplest expression stmt
  s= buildStatementFromString ("1;", f_body);
  appendStatement(s, f_body);

  // function call statement
  s= buildStatementFromString ("foo();", f_body); 
  appendStatement(s, f_body);

  // labeled statement
  s= buildStatementFromString ("mylabel:;", f_body); 
  appendStatement(s, f_body);

  // goto after the label statement
  s = buildStatementFromString ("goto mylabel;", f_body);
  appendStatement(s, f_body);

  // goto statement is before the label statement
  s = buildStatementFromString ("goto mylabel2;", f_body);
  appendStatement(s, f_body);

  // labeled statement
  s= buildStatementFromString ("mylabel2:;", f_body); 
  appendStatement(s, f_body);



  AstTests::runAllTests(project);
  backend(project);   
  return 0;
}
