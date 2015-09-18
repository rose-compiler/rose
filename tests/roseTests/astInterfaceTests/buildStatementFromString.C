// Liao         5/5/2011
//     revised 9/18/2015
// test build AST from string
#include <rose.h>
#include <stdio.h>

using namespace std;
using namespace SageInterface;
using namespace SageBuilder;

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

  // int j;
  s= buildStatementFromString ("int j;", f_body); 
  appendStatement(s, f_body);

  // for () 
  s= buildStatementFromString ("for (i=0; i<100; i++) x+=i;", f_body); 
  appendStatement(s, f_body);


  // build simplest expression stmt
  s= buildStatementFromString ("1+sizeof (int);", f_body);
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

  // while
  s= buildStatementFromString ("while (x != 0) \n x-- ;", f_body); 
  appendStatement(s, f_body);

  // do stmt while (exp);
  s= buildStatementFromString ("do x == 0; \n while (1) ;", f_body); 
  appendStatement(s, f_body);

  //TEST if else
  s= buildStatementFromString ("if (i!=0) x++; else x-- ;", f_body); 
  appendStatement(s, f_body);

  // return 
  s= buildStatementFromString ("return ;", f_body); 
  appendStatement(s, f_body);

  AstTests::runAllTests(project);
  backend(project);   
  return 0;
}

