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


namespace RAJA_Checker 
{
  // TODO: move some functions to SageInterface if needed.
  //! Find and warn if there are data member accesses within a scope
  // This is useful in the context of RAJA programming: 
  //   Accessing data members of big data structures within RAJA kernels may be bad for performance. 
  void warnDataMemberAccess (SgStatement* topnode)
  {
    ROSE_ASSERT (topnode != NULL);
    // Two cases 
    //1. Dot Expression: a.b
    //2. Arrow Expression: a-> b; 
    //
    //But ROSE AST makes implicit this-> explicit: a becomes this->a, we must exclude this case since it is just to access its own data member. 
    //
    // Similarly 
    //    a->b becomes (this->a)->b
    // We only look at SgDotExp and SgArrowExp where are not compiler generated, 
    //   i.e. those dot and arrow expressions showing up in the original source code.
    // Then we further check if the left hand operand is this-> if it is user-written this->a. 

    VariantVector vv;
    vv.push_back(V_SgArrowExp);
    vv.push_back(V_SgDotExp);
    Rose_STL_Container<SgNode*> mem_access_set = NodeQuery::querySubTree(topnode ,vv);

    for (size_t i=0; i< mem_access_set.size(); i++)
    {
      SgExpression* exp = isSgExpression(mem_access_set[i]);
      Sg_File_Info* finfo = exp->get_file_info();
      if (finfo)
      {
        if (!finfo->isCompilerGenerated())
        {
          // Additionally, we exclude user added explicit this->
          SgBinaryOp* bop = isSgBinaryOp(exp);
          if (!isSgThisExp(bop->get_lhs_operand()))
             cout<<"Found data member access at:"<< finfo->get_filename() <<" " << finfo->get_line() <<":"<< finfo->get_col()  <<endl;
        }
      } 
    }
  } //end warnDataMemberAccess 


  // some helper functions to detect RAJA constructs
  //! Check if a namespace 
  bool isRAJANamespaceDeclaration (SgNode* namespace_decl)
  {
    bool retval = false;
    if (SgNamespaceDeclarationStatement* ndecl = isSgNamespaceDeclarationStatement (namespace_decl))
    {
      if (ndecl->get_qualified_name().getString() =="::RAJA")
        retval = true;
    }
    else
    {
      cerr<<"isRAJANamespaceDeclaration() encounters a parameter which is not SgNamespaceDeclarationStatement:"<< namespace_decl->class_name()<<endl;
      ROSE_ASSERT (false);
    }
    return retval; 
  }

  //! Check if a function call expression is a call to a RAJA function, like RAJA::forall ...
  // The associated function declaration of the call should be within a RAJA namespace. 
  // The AST looks like:   SgNamespaceDeclaration
  //                       * SgNamespaceDefinitionStatement
  //                       ** SgFunctionDeclaration
  bool isCallToRAJAFunction (SgNode* callexp)
  {
    bool retval= false;
    if (SgFunctionCallExp* call_exp = isSgFunctionCallExp (callexp) )
    {
      SgFunctionDeclaration* func_decl = call_exp->getAssociatedFunctionDeclaration();
      ROSE_ASSERT (func_decl!=NULL);
      
      SgScopeStatement* scope = func_decl->get_scope();
      if (SgNamespaceDefinitionStatement * ns_def = isSgNamespaceDefinitionStatement(scope) )
      {
        retval = isRAJANamespaceDeclaration ( ns_def->get_parent() ); 
      }
    }
    else
    {
      cerr<<"isCallToRAJAFunction () encounters a parameter which is not SgFunctionCallExp :"<< callexp->class_name()<<endl;
      ROSE_ASSERT (false);
    }

    return retval; 
  }

  //! Check if a lambda function is inside a call to a RAJA template function. Return the raja function decl if it is. 
  // This is a key interface function. Developers can first find a lambda expression , 
  // then check if it is a RAJA template function's parameter.
  // The AST should look like  SgFunctionCallExp
  //                           * SgExprListExp
  //                           ** SgLambdaExp 
  bool isRAJATemplateFunctionCallParameter(SgLocatedNode* n, SgFunctionDeclaration** raja_func_decl = NULL)
  {
    bool retval = false; 
    ROSE_ASSERT (n!= NULL);
    if (SgLambdaExp* le = isSgLambdaExp (n))
    {
      SgNode* parent = le->get_parent();
      ROSE_ASSERT(parent!=NULL);
      parent = parent->get_parent();
      ROSE_ASSERT(parent!=NULL);
      if (SgFunctionCallExp* call_exp = isSgFunctionCallExp (parent)) 
      {
        retval = isCallToRAJAFunction (call_exp);
        if (raja_func_decl != NULL)
          * raja_func_decl = call_exp-> getAssociatedFunctionDeclaration();
      }
    }
    else
    {
      cerr<<"isRAJATemplateFunctionCallParameter () encounters a parameter which is not SgLambdaExp:"<< n->class_name()<<endl;
      ROSE_ASSERT (false);
    }
    return retval;
  }

} // end RAJA_Checker namespace


using namespace RAJA_Checker;

void RoseVisitor::visit ( SgNode* n)
{
  // Only watch for Located nodes from input user source files.
  if (SgLocatedNode* lnode = isSgLocatedNode(n))
  {
    // skip compiler generated codes, mostly from template headers
    if (lnode->get_file_info()->isCompilerGenerated())
      return;

    // catch lambda expressions  
   if (n->variantT() == V_SgLambdaExp)
    {
      if ( SgLambdaExp* le = isSgLambdaExp(n))
      {
        SgFunctionDeclaration* raja_func = NULL; 
        // Check if the lambda expression is used as a parameter of RAJA function call
        if (isRAJATemplateFunctionCallParameter (le, & raja_func))
        {
         cout<<"Found a lambda exp within RAJA func call ..."<<endl; 
          //le->get_file_info()->display();
          cout<<"RAJA func name is:"<<
          raja_func->get_name()<<endl;
          // scan the function body for data member accesses. 
          warnDataMemberAccess (le->get_lambda_function()->get_definition());
        }
      }
    }
  }
}

int
main ( int argc, char* argv[])
{
  SgProject* project = frontend(argc,argv);
  ROSE_ASSERT (project != NULL);

  // ROSE Traversal
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

