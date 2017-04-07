// An analyzer for RAJA code, trying to report a set of properties
//
//
// Liao, 4/6/2017
#include "rose.h"
#include <iostream>
using namespace std;
using namespace SageInterface;

class RoseVisitor : public AstSimpleProcessing
{
  protected:
    void virtual visit ( SgNode* node);
};

void RoseVisitor::visit ( SgNode* n)
{
  //      if (isSgFunctionDeclaration(n)!=NULL)
  //      This will match SgTemplateInstantiationFunctionDecl, which is not wanted.
  //if (n->variantT() == V_SgFunctionDeclaration || n->variantT() == V_SgMemberFunctionDeclaration)
  // Only catch lambda function, which is a SgMemberFunctionDeclaration
  if (n->variantT() == V_SgMemberFunctionDeclaration)
  {
    ROSE_ASSERT (n->variantT() != V_SgTemplateInstantiationFunctionDecl);
    SgFunctionDeclaration* func = isSgFunctionDeclaration(n);
    ROSE_ASSERT(func != NULL);
    if (func->get_definition() == NULL) return;

    // TODO: skip things from headers. 
    // skip compiler generated codes, mostly from template headers
    if (func->get_file_info()->isCompilerGenerated())
    {
      return;
    }

    SgBasicBlock* body = func->get_definition()->get_body();
    if (body == NULL) return;
    // Prepare the function body: ensure body basic block for while, for, if, etc.
    // SageInterface::changeAllBodiesToBlocks (body, false);

    if ( SgLambdaExp* le = isSgLambdaExp(n->get_parent()))
    {
      cout<<"Found a lambda function body..."<<endl; 
      body->get_file_info()->display();
    }
  }
}

int
main ( int argc, char* argv[])
{
  SgProject* project = frontend(argc,argv);
  ROSE_ASSERT (project != NULL);

  // ROSE memory traversal to catch all sorts of nodes, not just those on visible AST
  RoseVisitor visitor;

  SgFilePtrList file_ptr_list = project->get_fileList();
  for (size_t i = 0; i<file_ptr_list.size(); i++)                                                                                          
  {                                                                                                                                        
    SgFile* cur_file = file_ptr_list[i];                                                                                                   
    SgSourceFile* s_file = isSgSourceFile(cur_file);                                                                                       
    if (s_file != NULL)                                                                                                                    
    {                                                                                                                                      
      visitor.traverseWithinFile(s_file, preorder);                                                                               
    }                                                                                                                                      
  }            




  return backend(project);
}

