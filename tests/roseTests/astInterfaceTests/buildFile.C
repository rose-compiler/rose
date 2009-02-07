// Liao, 4/8/2008
// Demonstrate how to build a SgFile
//
// SageBuilder contains the AST nodes/subtrees builders
// SageInterface contains any other AST utility tools 
//-------------------------------------------------------------------
#include "rose.h"
#include <stdio.h>

using namespace SageBuilder;
using namespace SageInterface;
using namespace std;

int main (int argc, char *argv[])
{
  //absolute filename should also works
 // std::string fileName="/home/liao6/buildrose/tests/roseTests/astInterfaceTests/testfile525.cpp";
  std::string fileName="testfile525.cpp";
  std::string fileName2="testfile626.cpp";
  std::string fileName3="rose_rose_testfile626.cpp";
  std::string fileName4="rose_rose_testfile525.cpp";

  SgProject *project = frontend (argc, argv);
  SgStatement * stmt1 = getFirstStatement(getFirstGlobalScope(project)); 

#if 1
//case 1. Calling it with project available, but input file does not exist 
  remove (fileName.c_str());
  SgSourceFile *sgfile = buildFile(fileName,fileName,project);

  // try to add something into the new file
  SgGlobal* global = sgfile->get_globalScope();
  SgVariableDeclaration *varDecl = buildVariableDeclaration("i", buildIntType());
  appendStatement (varDecl,isSgScopeStatement(global));

  //test copy across SgFile
  if (stmt1 != NULL)
  {
    cout<<"Found a source stmt:"<<stmt1->unparseToString()<<endl;
    SgStatement* copy = deepCopy<SgStatement>(stmt1);
    appendStatement (copy,isSgScopeStatement(global));
  }
#endif

#if 1
// case 2. Calling it without the project parameter and without input file 
  remove (fileName2.c_str());
  SgSourceFile *sgfile2 = buildFile(fileName2,fileName2);

  // try to add something into the new file
  SgGlobal* global2 = sgfile2->get_globalScope();
  SgVariableDeclaration *varDecl2 = buildVariableDeclaration("j", buildIntType());
  appendStatement (varDecl2,isSgScopeStatement(global2));

  // test AST copy across SgFile
  SgStatement* varDecl3_cp = deepCopy<SgStatement> (stmt1);
  SgStatement* varDecl2_cp = deepCopy<SgStatement> (varDecl);
  appendStatement(varDecl2_cp, isSgScopeStatement(global2));
  appendStatement(varDecl3_cp, isSgScopeStatement(global2));

  SgProject * project2= sgfile2->get_project();
  AstTests::runAllTests(project2);
  project2->unparse();
  //backend(project2);
#endif 

#if 1
// case 3. Input File already exists, load it and do some transformation, project exists  
  SgSourceFile * sgfile3 = buildFile(fileName2,fileName3,project);   
  SgGlobal* global3 = sgfile3->get_globalScope();
  SgVariableDeclaration *varDecl3 = buildVariableDeclaration("y", buildIntType());
  appendStatement (varDecl3,isSgScopeStatement(global3));
  //test AST copy across SgSourceFile
  SgStatement* varDecl4_cp = deepCopy<SgStatement> (stmt1);
  appendStatement(varDecl4_cp, isSgScopeStatement(global3));

  AstTests::runAllTests(project);
  project->unparse();
  //backend(project);
#endif 

#if 1
// case 4. Input File already exists, but project does not.  
   SgSourceFile *sgfile4 = buildFile(fileName,fileName4);

  // try to add something into the new file
  SgGlobal* global4 = sgfile4->get_globalScope();
  SgVariableDeclaration *varDecl4 = buildVariableDeclaration("z", buildIntType());
  appendStatement (varDecl4,isSgScopeStatement(global4));

  SgProject * project4= sgfile4->get_project();
  AstTests::runAllTests(project4);
  project4->unparse();
 //backend(project); 
#endif  

  return 0;
}

