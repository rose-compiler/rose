// testing generating function side effects

#include "rose.h"
#include "RoseAst.h"

#include "CommandOptions.h"
    
//OpenMP attribute for OpenMP 3.0
#include "OmpAttribute.h"
    
//Array Annotation headers
#include <CPPAstInterface.h>
#include <ArrayAnnot.h>
#include <ArrayRewrite.h>
      
//Dependence graph headers
#include <AstInterface_ROSE.h>
#include <LoopTransformInterface.h>
#include <AnnotCollect.h>
#include <OperatorAnnotation.h>
#include <LoopTreeDepComp.h>

#include <iostream>
#include <set>
using namespace std;

#if 1
//testing a new version with better conversion, obtain the current level granularity, not always converting to top level objects!!
SgInitializedName* convertRefToInitializedName2(SgNode* current)
{
  SgInitializedName* name = NULL;
  ROSE_ASSERT(current != NULL);
  if (isSgInitializedName(current))
  {
    name = isSgInitializedName(current);
  }
  else if (isSgPntrArrRefExp(current)) // arrays: convert to parent level
  {
    bool suc=false;
    SgExpression* nameExp = NULL;
    suc= SageInterface::isArrayReference(isSgExpression(current),&nameExp);
    ROSE_ASSERT(suc == true);
     // has to resolve this recursively
    return convertRefToInitializedName2(nameExp);
  }
  else if (isSgVarRefExp(current)) // variable reference expression: direct convert to initName
  {
#if 0
    SgNode* parent = current->get_parent();
    if (isSgDotExp(parent))
    {
       if (isSgDotExp(parent)->get_rhs_operand() == current)
        return convertRefToInitializedName2(parent);
    }
    else if(isSgArrowExp(parent))
    {
      if (isSgArrowExp(parent)->get_rhs_operand() == current)
        return convertRefToInitializedName2(parent);
    }
#endif
    name = isSgVarRefExp(current)->get_symbol()->get_declaration();
  }
  else if (isSgDotExp(current)) // obtain the rhs
  {
    SgExpression* rhs = isSgDotExp(current)->get_rhs_operand();
    ROSE_ASSERT(rhs);
     // has to resolve this recursively
    return convertRefToInitializedName2(rhs);
  }
   else if (isSgArrowExp(current)) // obtain the rhs
  {
    SgExpression* rhs = isSgArrowExp(current)->get_rhs_operand();
    ROSE_ASSERT(rhs);
     // has to resolve this recursively
    return convertRefToInitializedName2(rhs);
  } // The following expression types are usually introduced by left hand operands of DotExp, ArrowExp
#if 0
  else if (isSgPointerDerefExp(current)) // *p access is different from p: how to differentiate them??
  {
    return convertRefToInitializedName2(isSgPointerDerefExp(current)->get_operand());
  }
#endif
  else if (isSgCastExp(current))
  {
    return convertRefToInitializedName2(isSgCastExp(current)->get_operand());
  }
  else if (isSgThisExp(current)) // this pointer cannot be converted to  SgInitializedName 
  {
    return NULL; 
  }
  else
  {
    cerr<<"In convertRefToInitializedName2(): unhandled reference type:"<<current->class_name()<<endl;
    ROSE_ASSERT(false);
  }
  ROSE_ASSERT(name != NULL);
  return name;
}

#endif

int main(int argc, char * argv[])
{
  SgProject *project = frontend (argc, argv);

  vector<string> remainingArgs (argv, argv+argc);
  // work with the parser of the ArrayAbstraction module
  //Read in annotation files after -annot 
  CmdOptions::GetInstance()->SetOptions(remainingArgs);
  ArrayAnnotation* annot = ArrayAnnotation::get_inst();
  annot->register_annot();
  ReadAnnotation::get_inst()->read();

  //Strip off custom options and their values to enable backend compiler 
  CommandlineProcessing::removeArgsWithParameters(remainingArgs,"-annot");

  RoseAst ast(project);
  for (RoseAst::iterator i=ast.begin();i!=ast.end();++i)
  {
    SgFunctionDeclaration* func = isSgFunctionDeclaration (*i);
    if (!func) continue;  
    if (func->get_definition() == NULL) continue; // skip prototype declaration
    cout<<"--------------------------------------------------"<<endl;
    cout<<"Function name:"<<func->get_qualified_name()<<endl;

    vector<SgNode*> readRefs, writeRefs;
    // focus on definition only
    if (!SageInterface::collectReadWriteRefs(func->get_definition(),readRefs, writeRefs))
    {
      cerr<<"Warning: SageInterface::collectReadWriteRefs() returns false."<<endl;
      continue; 
    }

    // i, j, b , k, argc, argv
    vector<SgNode*>::iterator iter;
    for (iter=readRefs.begin();iter!=readRefs.end();iter++)
    {
      if (iter== readRefs.begin())
        cout<<"read references:"<<endl; 

      if (*iter)
      {
        cout<<"\t"<<(*iter)->class_name()<<"\t@"<<(*iter)<< "  ";
        if (convertRefToInitializedName2(*iter))
           cout<<convertRefToInitializedName2(*iter)->get_qualified_name()<<endl;
        else
          cout<<"NULL SgInitializedName"<<endl;
          
      }
      else  
        cout<<"NULL ref"<<endl;
    }
    /* i, j, a, b
    */
    for (iter=writeRefs.begin();iter!=writeRefs.end();iter++)
    {
      if (iter== writeRefs.begin())
        cout<<"write references:"<<endl; 
      cout<<"\t"<<(*iter)->class_name()<<"\t@"<<(*iter)<< "  ";
      if ((*iter) )
      {  
        if (convertRefToInitializedName2(*iter))
          cout<<convertRefToInitializedName2(*iter)->get_qualified_name()<<endl;
        else 
          cout<<"NULL SgInitializedName" <<endl;
      }
      else  
        cout<<"NULL ref"<<endl;
    }
    
    //-------------------------------------------------------------------
    set<SgInitializedName*> readNames, writeNames; 
    if (!SageInterface::collectReadWriteVariables(func->get_definition(),readNames,writeNames))
    {
      cerr<<"Warning: SageInterface::collectReadWriteVariables() returns false."<<endl;
      continue; 
    }

    set<SgInitializedName*>::iterator iter2;
    for (iter2=readNames.begin();iter2!=readNames.end();iter2++)
    {
      if (iter2== readNames.begin())
        cout<<"read var names:"<<endl; 
      cout<<"\t"<<(*iter2)->get_qualified_name()<<endl;
    }

    for (iter2=writeNames.begin();iter2!=writeNames.end();iter2++)
    {
      if (iter2== writeNames.begin())
        cout<<"write var names:"<<endl; 
      cout<<"\t"<<(*iter2)->get_qualified_name()<<endl;
    }

#if 0
    // read only variables  // TODO: why THIS ???
    set<SgInitializedName*> readOnlyVars;
    SageInterface::collectReadOnlyVariables(func,readOnlyVars);
    for (iter=readOnlyVars.begin();iter!=readOnlyVars.end();iter++)
    {
      if (iter== readNames.begin())
        cout<<"read-only variable:"<<endl; 
      if (*iter)
        cout<<(*iter)->get_qualified_name()<<endl;
      else  
        cout<<"NULL"<<endl;
    }
#endif
  }
  // Generate source code from AST and call the vendor's compiler
  return backend(project);
}

