/*
by Liao, 1/8/2009
*/
#include "rose.h"
#include <iostream>
#include <string>
using namespace std;
//using namespace OmpSupport;

// simple (no information propagation) preorder traverse an AST from 'root', call a function upon each of the visited node
void simplePreorderTraversal(SgNode* root, void (*func_ptr)(SgNode* n))
{
  class visitorTraversal : public AstSimpleProcessing
  {
    public:
      visitorTraversal( void (*func_ptr)(SgNode* n) ):fptr(func_ptr)
       { ROSE_ASSERT (fptr != NULL); };
    private:
     //disallow default constructor, enforce the function pointer parameter
      visitorTraversal(){};
      void (*fptr)(SgNode* n);
    protected:
      virtual void visit(SgNode* node)
      {
        ROSE_ASSERT(fptr != NULL);
        fptr(node);
      }
  };

  visitorTraversal myvisitor(func_ptr);
  // traverse full AST
  myvisitor.traverse(root,preorder);
  // traverse subtree which represents the files specified on the command line
  //myvisitor.traverseInputFiles(root,preorder);
  //  traverse only nodes which represents code of the same file as the root node
  //myvisitor.traverseInputFiles(root,preorder);
}

void private_lock_visitor2(SgNode* node)
{
  SgFunctionCallExp * func_call = isSgFunctionCallExp(node);
  if (!func_call) return;
  std::string func_call_name = func_call->getAssociatedFunctionSymbol()->get_name().getString();
  if (func_call_name != "omp_unset_lock" && func_call_name != "omp_set_lock" && func_call_name != "omp_test_lock")
    return;
  std::vector<SgVarRefExp*> exp_vec = SageInterface::querySubTree<SgVarRefExp>(func_call, V_SgVarRefExp);
//  cout<<exp_vec[0]->unparseToString()<<endl;
//  if (exp_vec.size() >1)
//  {
//    cout<<exp_vec[1]->unparseToString()<<endl;
//    ROSE_ASSERT (exp_vec.size() ==1);
//  }
  // only consider calls set/unset/test locks
  // get the lock declaration statement
  SgVariableDeclaration* lock_decl = isSgVariableDeclaration((exp_vec[0])->get_symbol()->get_declaration()->get_declaration());
  ROSE_ASSERT(lock_decl);
  //get the enclosing parallel region statement
  SgOmpParallelStatement* lock_region = SageInterface::getEnclosingNode<SgOmpParallelStatement>(func_call);
  // orphaned case
  // local lock could be private, send warning
  if (lock_region == NULL)
  {
    if (!isSgGlobal(lock_decl->get_scope()))
    {
      cerr<<"Found a possible private lock for a parallel region "<<endl;
      isSgLocatedNode(node)->get_file_info()->display("aaa"); 
    }
  }
  else // the lock reference cite is lexically within a parallel region
    // if the lock declaration is inside the region, send error message
  {
    if (SageInterface::isAncestor(lock_region, lock_decl))
    {
      cerr<<"Found a reference to a private lock within a parallel region"<<endl;
      isSgLocatedNode(node)->get_file_info()->display("bbb"); 
    }
  }
}


void private_lock_visitor(SgNode* node)
{
  SgVarRefExp* n = isSgVarRefExp(node);
  if (n)
  {
    std::string type_name = n->get_type()->unparseToString();
    if (type_name=="omp_lock_t") // TODO there should be a better way for this
    {
      if (isSgAddressOfOp(n->get_parent()) && isSgFunctionCallExp(n->get_parent()->get_parent()->get_parent()))
      {
        SgFunctionSymbol *func_symbol=  isSgFunctionCallExp(n->get_parent()->get_parent()->get_parent())->getAssociatedFunctionSymbol();
        ROSE_ASSERT(func_symbol != NULL);
        std::string func_call_name = func_symbol->get_name().getString();
        if (func_call_name == "omp_unset_lock" ||func_call_name == "omp_set_lock" ||func_call_name == "omp_test_lock")
        {
          // only consider calls set/unset/test locks
          // get the lock declaration statement
          SgVariableDeclaration* lock_decl = isSgVariableDeclaration(n->get_symbol()->get_declaration()->get_declaration());
          ROSE_ASSERT(lock_decl);
          //get the enclosing parallel region statement
          SgOmpParallelStatement* lock_region = SageInterface::getEnclosingNode<SgOmpParallelStatement>(n);
          // orphaned case
          // local lock could be private, send warning
          if (lock_region == NULL)
          {
            if (!isSgGlobal(lock_decl->get_scope()))
            {
              cerr<<"Found a possible private lock for a parallel region "<<endl;
              isSgLocatedNode(node)->get_file_info()->display("aaa"); 
            }
          }
          else // the lock reference cite is lexically within a parallel region
            // if the lock declaration is inside the region, send error message
          {
            if (SageInterface::isAncestor(lock_region, lock_decl))
            {
              cerr<<"Found a reference to a private lock within a parallel region"<<endl;
              isSgLocatedNode(node)->get_file_info()->display("bbb"); 
            }
          }
        }
      } // function call
    } // type
  }// var Ref
}


int main(int argc, char * argv[])

{
  SgProject *project = frontend (argc, argv);
  AstTests::runAllTests(project);

  simplePreorderTraversal(project, &private_lock_visitor2);

  return backend(project);
}


