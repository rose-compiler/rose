/*
Winnie:
test code for loopCollapsing
*/
#include "rose.h"
#include <string>
#include <iostream>
#include "commandline_processing.h"

using namespace std;
using namespace AbstractHandle;
vector<SgForStatement*> loopList;

bool isDirectiveAttached(SgStatement* stmt)
{
  SgPragmaDeclaration* pragmaStmt = isSgPragmaDeclaration(SageInterface::getPreviousStatement(stmt));
  if(pragmaStmt != NULL)
  {
    SgPragma* pragma = isSgPragma(pragmaStmt->get_pragma());
    ROSE_ASSERT(pragma); 
    if (SgProject::get_verbose() > 2)
      cout << "pragma: " << pragma->get_pragma() << endl;

    if(pragma->get_pragma().find("collapse") != string::npos)
      return true;
  }
  return false;
}
class loopTraversal : public ROSE_VisitorPattern
{
  public:
    void visit(SgForStatement* loop);
};

void loopTraversal::visit(SgForStatement* loop)
{
  if(isDirectiveAttached(loop))
    loopList.push_back(loop);
}
int main(int argc, char * argv[])

{
  std::string handle;
  int factor =2;
  // command line processing
  //--------------------------------------------------
  vector<std::string> argvList (argv, argv+argc);
  if (!CommandlineProcessing::isOptionWithParameter (argvList,"-rose:loopcollapse:","abstract_handle",handle, true)
     || !CommandlineProcessing::isOptionWithParameter (argvList,"-rose:loopcollapse:","factor",factor, true))
   {
     cout<<"Usage: loopCollapsing inputFile.c -rose:loopcollapse:abstract_handle <handle_string> -rose:loopcollapse:factor N"<<endl;
     return 0;
   }

  // Retrieve corresponding SgNode from abstract handle
  //--------------------------------------------------
  SgProject *project = frontend (argvList);
  SgStatement* stmt = NULL;
  ROSE_ASSERT(project != NULL);
  loopTraversal translateLoop;
  traverseMemoryPoolVisitorPattern(translateLoop);
  for(vector<SgForStatement*>::iterator i=loopList.begin(); i!=loopList.end(); ++i)
  {
    SgForStatement* forStatement = isSgForStatement(*i);
    bool result = SageInterface::loopCollapsing(forStatement, factor);
    ROSE_ASSERT(result != false);
  }
  //--------------------------------------------------

  // Generate source code from AST and call the vendor's compiler
  return backend(project);
}

