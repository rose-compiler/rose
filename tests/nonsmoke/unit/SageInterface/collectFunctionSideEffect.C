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

// using a log file to avoid new screen output from interfering with correctness checking
ofstream ofile;

#if 0
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


void dumpVectorNodes( vector<SgNode*>& Refs, const string & header, ofstream & ofile)
{
  vector<SgNode*>::iterator iter;
  for (iter=Refs.begin();iter!=Refs.end();iter++)
  {
    if (iter== Refs.begin())
      ofile<<header<<endl; 
    //ofile<<"read references:"<<endl; 

    if (*iter)
    {
      SgLocatedNode* lnode = isSgLocatedNode(*iter);
      ROSE_ASSERT (lnode);
      ofile<<"\t"<<(*iter)->class_name()<<"@" << lnode->get_file_info()->get_line()<<":"<<lnode->get_file_info()->get_col() <<  "\t";
      if (SgInitializedName* iname= SageInterface::convertRefToInitializedName(*iter, false))
        ofile<<iname->get_qualified_name()<<endl;
      else
        ofile<<"NULL SgInitializedName"<<endl;

    }
    else  
      ofile<<"NULL ref"<<endl;
  }
}

void dumpSetNames(set<SgInitializedName*>& Names,  const string & header, ofstream & ofile)
{
  set<SgInitializedName*>::iterator iter2;
  for (iter2=Names.begin();iter2!=Names.end();iter2++)
  {
    if (iter2== Names.begin())
      ofile<<header<<endl; 
    ofile<<"\t"<<(*iter2)->get_qualified_name()<<endl;
  }
}

int main(int argc, char * argv[])
{
  vector<string> remainingArgs (argv, argv+argc);

   //We must processing options first, before calling frontend!!
  // work with the parser of the ArrayAbstraction module
  //Read in annotation files after -annot 
  CmdOptions::GetInstance()->SetOptions(remainingArgs);
  bool dumpAnnot = CommandlineProcessing::isOption(remainingArgs,"","-dumpannot",true);
  ArrayAnnotation* annot = ArrayAnnotation::get_inst();
  annot->register_annot();
  ReadAnnotation::get_inst()->read();
  if (dumpAnnot)
    annot->Dump();
  //Strip off custom options and their values to enable backend compiler 
  CommandlineProcessing::removeArgsWithParameters(remainingArgs,"-annot");

  SgProject *project = frontend (remainingArgs);
  // our tests only use one single file for now.
  SgFilePtrList fl = project->get_files();
  SgFile* firstfile = fl[0];
  ROSE_ASSERT (firstfile!=NULL);

  string filename = Rose::StringUtility::stripPathFromFileName (firstfile->getFileName());
  string ofilename = filename+".collectFunctionSideEffect.output";
  ofile.open(ofilename.c_str());

  RoseAst ast(project);

  // function level side effect results
  for (RoseAst::iterator i=ast.begin();i!=ast.end();++i)
  {
    SgFunctionDeclaration* func = isSgFunctionDeclaration (*i);
    if (!func) continue;  
    if (func->get_definition() == NULL) continue; // skip prototype declaration
    ofile<<"--------------------------------------------------"<<endl;
    ofile<<"Function name:"<<func->get_qualified_name()<<endl;

    vector<SgNode*> readRefs, writeRefs;
    // focus on definition only
    if (!SageInterface::collectReadWriteRefs(func->get_definition(),readRefs, writeRefs))
    {
      ofile<<"Warning: SageInterface::collectReadWriteRefs() returns false."<<endl;
      continue; 
    }

    dumpVectorNodes(readRefs, "Read references:", ofile);
    dumpVectorNodes(writeRefs, "Write references:", ofile);
   
    //-------------------------------------------------------------------
    set<SgInitializedName*> readNames, writeNames; 
    if (!SageInterface::collectReadWriteVariables(func->get_definition(),readNames,writeNames, false))
    {
      ofile<<"Warning: SageInterface::collectReadWriteVariables() returns false."<<endl;
      continue; 
    }

   dumpSetNames(readNames, "Read var names:" , ofile);
   dumpSetNames(writeNames, "Write var names:" , ofile);

#if 0
    // read only variables  
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

  // loop level side effect results
  for (RoseAst::iterator i=ast.begin();i!=ast.end();++i)
  {
    SgForStatement* target= isSgForStatement(*i);
    if (!target) continue;  
    ofile<<"--------------------------------------------------"<<endl;
    ofile<<"Loop line:"<<target->get_file_info()->get_line()<<endl;

    vector<SgNode*> readRefs, writeRefs;
    // focus on definition only
    if (!SageInterface::collectReadWriteRefs(target,readRefs, writeRefs))
    {
      ofile<<"Warning: SageInterface::collectReadWriteRefs() returns false."<<endl;
      continue; 
    }

    dumpVectorNodes(readRefs, "Read references:", ofile);
    dumpVectorNodes(writeRefs, "Write references:", ofile);
   
    //-------------------------------------------------------------------
    set<SgInitializedName*> readNames, writeNames; 
    if (!SageInterface::collectReadWriteVariables(target,readNames,writeNames, false))
    {
      ofile<<"Warning: SageInterface::collectReadWriteVariables() returns false."<<endl;
      continue; 
    }

   dumpSetNames(readNames, "Read var names:" , ofile);
   dumpSetNames(writeNames, "Write var names:" , ofile);
  }

  ofile.close();
  // Generate source code from AST and call the vendor's compiler
  return backend(project);
}

