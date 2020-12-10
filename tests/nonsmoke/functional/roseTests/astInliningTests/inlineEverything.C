// This test attempts to inline function calls until I cannot inline anything else or some limit is reached.
#include "rose.h"
#include <vector>
#include <string>
#include <iostream>

using namespace Rose;
using namespace std;

// Finds needle in haystack and returns true if found.  Needle is a single node (possibly an invalid pointer and will not be
// dereferenced) and haystack is the root of an abstract syntax (sub)tree.
static bool
isAstContaining(SgNode *haystack, SgNode *needle) {
    struct T1: AstSimpleProcessing {
        SgNode *needle;
        T1(SgNode *needle): needle(needle) {}
        void visit(SgNode *node) {
            if (node == needle)
                throw this;
        }
    } t1(needle);
    try {
        t1.traverse(haystack, preorder);
        return false;
    } catch (const T1*) {
        return true;
    }
}

// only call doInlinine(), without postprocessing or consistency checking. Useful to debugging things. 
static bool e_inline_only= false; 
static int e_inline_limit= 99999999; 
// a new driver: start from the main function, inlining only functions which are reachable from main().
static bool e_inline_main= false; 

// Given a function with a body, inlining recursively all functions called inside of the body.
// Return the number of call sites considered for inlining
//   and the call sites which are actuallyed inlined.
void inlineFromRoot (SgFunctionDeclaration* func_decl, int& callSitesConsidered,  std::vector <SgFunctionCallExp*>& inlined_calls)
{
  // sanity check
  ROSE_ASSERT (func_decl != NULL);
  func_decl =  isSgFunctionDeclaration(func_decl->get_definingDeclaration());
  ROSE_ASSERT (func_decl != NULL);

  bool changed; //if the AST of the function body has been changed due to inlining
  do {
    changed = false; 

    BOOST_FOREACH (SgFunctionCallExp *call, SageInterface::querySubTree<SgFunctionCallExp>(func_decl)) 
    {
      callSitesConsidered ++;
      if (callSitesConsidered >  e_inline_limit )
      { 
        if (Inliner::verbose)
          std::cout << "Reached the limit of number of functions:" << callSitesConsidered<< std::endl;
        break;
      }

      if (Inliner::verbose)
        std::cout << "Trying to inline the function id (starting from 1):" << callSitesConsidered << std::endl;

      if (doInline(call)) {
        ASSERT_always_forbid2(isAstContaining(func_decl, call),
            "Inliner says it inlined, but the call expression is still present in the AST.");
        inlined_calls.push_back(call);
        changed = true;
      }
    } //end boost_foreach() 
  } while (changed);
}

int
main (int argc, char* argv[]) {

  // Build the project object (AST) which we will fill up with multiple files and use as a
  // handle for all processing of the AST(s) associated with one or more source files.
  std::vector<std::string> argvList(argv, argv+argc);


  if (CommandlineProcessing::isOption(argvList,"(-h|-help|--help)","", false))
  {
    cout<<"---------------------Tool-Specific Help-----------------------------------"<<endl;
    cout<<"This is a program transformation tool to inline function calls in your C/C++ or Fortran code."<<endl;
    cout<<"Usage: "<<argvList[0]<<" -c [options] "<< "input.c"<<endl;
    cout<<endl;
    cout<<"The optional options include: "<<endl;
    cout<<" -skip-postprocessing: Skip postprocessing which cleanups code"<<endl;
    cout<<" -process-headers:     Process calls within header files"<<endl;
    cout<<" -verbose:            Printout debugging information"<<endl;
    cout<<" -limit N:            Inline up to N functions, then stop"<<endl;
    cout<<" -main-only:          Inline only functions reachable from main()"<<endl;
    cout<<"----------------------Generic Help for ROSE tools--------------------------"<<endl;
  }

  // inlining only, without any post processing of AST
  if (CommandlineProcessing::isOption (argvList,"-skip-postprocessing","", true))
  {
    cout<<"Skip postprocessing which cleans up the code...."<<endl;
    e_inline_only = true ;
  }
  else 
    e_inline_only = false;

  if (CommandlineProcessing::isOption (argvList,"-main-only","", true))
  {
    cout<<"Inlining only functions reachable from main()...."<<endl;
    e_inline_main = true ;
  }
  else 
    e_inline_main = false;


  if (CommandlineProcessing::isOptionWithParameter (argvList,"", "-limit", e_inline_limit, true))
  {
    cout<<"Limiting the number of functions to be inlined to be:" << e_inline_limit <<endl;
  }

  // skip calls within headers or not
  if (CommandlineProcessing::isOption (argvList,"-process-headers","", true))
  {
    Inliner::skipHeaders = false;
    cout<<"Processing calls within header files ...."<<endl;
  }
  else 
    Inliner::skipHeaders = true;

  if (CommandlineProcessing::isOption (argvList,"-verbose","",false))
  {
    Inliner::verbose= true;
    cout<<"Turning on verbose mode ...."<<endl;
  }
  else 
    Inliner::verbose= false;

  SgProject* sageProject = frontend(argvList);

  AstTests::runAllTests(sageProject);
  std::vector <SgFunctionCallExp*> inlined_calls; 

  // Inline one call at a time until all have been inlined.  Loops on recursive code.
// this is essentially recursion by default.
  int call_count =0; 
  size_t nInlined = 0;

  if (e_inline_main)
  {
      SgFunctionDeclaration * fdecl = SageInterface::findMain(sageProject);
      inlineFromRoot(fdecl, call_count, inlined_calls);
      nInlined = inlined_calls.size();
  }
  else // the original driver to inline everything, which is too aggressive.
    for (int count=0; count<10; ++count) 
    {
      bool changed = false;
      //    BOOST_FOREACH (SgFunctionCallExp *call, SageInterface::querySubTree<SgFunctionCallExp>(sageProject)) 
      // interesting user loops are often located in the end of the source file    
      BOOST_REVERSE_FOREACH (SgFunctionCallExp *call, SageInterface::querySubTree<SgFunctionCallExp>(sageProject)) 
      {
        call_count++; 
        if (call_count>  e_inline_limit )
        {
          if (Inliner::verbose)        
            std::cout << "Reached the limit of number of functions:" << call_count << std::endl;
          break; 
        }
        if (Inliner::verbose)        
          std::cout << "Trying to inline the function id (starting from 1):" << call_count << std::endl;
        if (doInline(call)) {
          ASSERT_always_forbid2(isAstContaining(sageProject, call),
              "Inliner says it inlined, but the call expression is still present in the AST.");
          ++nInlined;
          inlined_calls.push_back(call);
          changed = true;
          break; //TODO why is there a break???
        }
      }
      if (!changed)
        break;
    }

  std::cout <<"Test inlined " <<StringUtility::plural(nInlined, "function calls") << " out of "<< call_count<< " calls." <<"\n";
  for (size_t i=0; i< inlined_calls.size(); i++)
  {
    std::cout <<"call@line:col " <<inlined_calls[i]->get_file_info()->get_line() <<":" << inlined_calls[i]->get_file_info()->get_col() <<"\n";
  }

  // Post-inline AST normalizations

  // DQ (6/12/2015): These functions first renames all variable (a bit heavy handed for my tastes)
  // and then (second) removes the blocks that are otherwise added to support the inlining.  The removal
  // of the blocks is the motivation for renaming the variables, but the variable renaming is 
  // done evarywhere instead of just where the functions are inlined.  I think the addition of
  // the blocks is a better solution than the overly agressive renaming of variables in the whole
  // program.  So the best solution is to comment out both of these functions.  All test codes
  // pass (including the token-based unparsing tests).
  // renameVariables(sageProject);
  // flattenBlocks(sageProject);

  if (!e_inline_only)
  {
    // This can be problematic since it tries to modifies lots of things, including codes from headers which are not modified at all. 
    cleanupInlinedCode(sageProject);
    changeAllMembersToPublic(sageProject);
    AstTests::runAllTests(sageProject);
  }

  return backend(sageProject);
}
