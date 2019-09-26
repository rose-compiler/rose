// An analyzer for RAJA code, trying to report a set of properties
//
//
// Liao, 4/6/2017
#include "rose.h"
#include <CommandLine.h>
#include <iostream>
#include "keep_going.h"
#include <string>
#include "AstMatching.h"
#include "AstTerm.h"
#include "RoseAst.h"
#include "DefUseAnalysis.h"

#include <Sawyer/CommandLine.h>
static const char* purpose = "This tool detects various patterns in input source files.";
static const char* description =
        "This tool detects several patterns in C++ source files. Currently supported patterns are: "
        " Pattern 1: the Nodal Accumulation Pattern in C++ for loops or LLNL/RAJA loops.";

//        " Pattern 1: the use of class or structure data members in RAJA loops."
using namespace std;
using namespace SageInterface;

// used to store reference correct results into a file.
// It will be used for diff-based correctness checking. So don't output varying debugging  info into this file.
ofstream ofile; 

// A helper function to match simple regular expression patterns
// .  match a single character
// *  match zero or more of the preceding element
bool isMatch(string s, string p) {
  /**
   * f[i][j]: if s[0..i-1] matches p[0..j-1]

   * if p[j - 1] != '*'
   *      f[i][j] = f[i - 1][j - 1] && (s[i - 1] == p[j - 1] || p[i-1=='.'])
   * if p[j - 1] == '*', denote p[j - 2] with x
   *      f[i][j] is true iff any of the following is true
   *      1) "x*" repeats 0 time and matches empty: = f[i][j - 2] // eliminate the last two chars of p [j-2]
   *      2) "x*" repeats >= 1 times and matches "x*x": s[i - 1] == x && f[i - 1][j]  
   * '.' matches any single character
   */
  int m = s.size(), n = p.size();

  // (m+1) x (n+1) matrix 
  vector < vector <bool> > f(m + 1, vector<bool>(n + 1, false));


  f[0][0] = true; // bootstrap point: from true
  // if p size is 0: no match is possible, set to false
  for (int i = 1; i <= m; i++)
    f[i][0] = false;
  // p[0.., j - 3, j - 2, j - 1] matches empty iff p[j - 1] is '*' and p[0..j - 3] matches empty
  for (int j = 1; j <= n; j++)
    f[0][j] = j > 1 && '*' == p[j - 1] && f[0][j - 2];

  // go through all i, j values , both start from 1
  for (int i = 1; i <= m; i++)
    for (int j = 1; j <= n; j++)
    {
      if (p[j - 1] != '*') // straightforward match
        f[i][j] = f[i - 1][j - 1] && (s[i - 1] == p[j - 1] || '.' == p[j - 1]);
      else
        // p[0] cannot be '*' so no need to check "j > 1" here
        f[i][j] = (f[i][j - 2] || ((s[i - 1] == p[j - 2] || '.' == p[j - 2]) && f[i - 1][j]));
    }

  return f[m][n];
} // end isMatch



namespace RAJA_Checker 
{
  bool enable_debug = false;
  bool keep_going = false;

  // Not in use right now
  bool checkDataMember = false; 
  bool checkNodalAccumulationPattern = true;

  // A helper function to stringify a located node, often for debugging purposes.
  std::string toString (SgNode* snode);

  //! Processing command line options
  std::vector<std::string> commandline_processing(std::vector< std::string > & argvList);

  //! Retrieve def use info. for a node's enclosing function definition. run for entire project for now
  DFAnalysis* obtainDFAnalysis ();

  //! If a for loop is a nodal accumulation loop , return the recognized first accumulation statement
  bool isNodalAccumulationLoop(SgForStatement* forloop, SgExprStatement*& fstmt);

  //! Check if a lambda function is a nodal accumulation function, embedded within a RAJA function call
  bool isEmbeddedNodalAccumulationLambda(SgLambdaExp* exp, SgExprStatement*& fstmt);

  //! Check if a lambda function is a nodal accumulation function and referenced as a function parameter of a RAJA function.
  bool isIndirectNodalAccumulationLambda(SgLambdaExp* exp, SgExprStatement*& fstmt, SgExprStatement*& callStmt);

  //! Check if a lambda function has the nodal accumulation pattern
  bool hasNodalAccumulationBody( SgLambdaExp* exp, SgExprStatement*& fstmt);

  //! Check if a variable is only initialized once since its declaration, at the entry point of a loop.
  // Internally reaching definition analysis is used to retrieve such information. 
  bool initializedOnceWithBasePlusOffset (SgInitializedName* iname, SgStatement* loopStatement, bool*);

  //! Check if an expression is used as a function call parameter to a RAJA function. 
  bool isRAJATemplateFunctionCallParameter(SgLocatedNode* n,  // n: SgLambdaExp
                         SgExprStatement* & callStmt, // return  the call statement
                         SgFunctionDeclaration*& raja_func_decl);

  //! Check if an expression is used as a function call parameter to a template function with a given name 
  bool isWrapperTemplateFunctionCallParameter(SgLocatedNode* n,  // n: SgLambdaExp
                         const vector<string>& func_names, // the matching names of wrapper functions
                         SgExprStatement* & callStmt, // return  the call statement
                         SgFunctionDeclaration*& template_func_decl);

  //! Check if a block of statement has the nodal accumulation pattern, with a known loop index variable and a loop stmt
  bool isNodalAccumulationBodyAtomic(SgBasicBlock* bb, SgInitializedName* lvar, SgExprStatement*& fstmt, SgStatement* loopStmt);

  //! Check if a block of statement, including its eligible nested basic blocks, has the nodal accumulation pattern, with a known loop index variable and a loop stmt
  bool isNodalAccumulationBody(SgBasicBlock* bb, SgInitializedName* lvar, SgExprStatement*& fstmt, SgStatement* loopStmt);

  //! Check if a statement is an accumulation statement , lhsInitPatternMatch is set to true if the lhs is declared/initialized using a form of base+offset
  bool isNodalAccumulationStmt (SgStatement* s, SgInitializedName* lvar, bool * lhsInitPatternMatch, SgStatement* loopStmt, bool* lhsUniqueDef); 

  //!  if there is a form like: int varRef1 = indexSet[loopIndex]; 
  bool connectedViaIndexSet (SgVarRefExp* varRef1, SgInitializedName* loopIndex);

  //!  if there is a form like: int varRef1 = loopIndex; 
  bool connectedViaAssignment (SgVarRefExp* varRef1, SgInitializedName* loopIndex);

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
          {
           if (RAJA_Checker::enable_debug)
            ofile<<"Found data member access at:"<< finfo->get_filename() <<" " << finfo->get_line() <<":"<< finfo->get_col()  <<endl;
          }
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
  //
  //
  // The AST should look like  SgFunctionCallExp
  //                           * SgExprListExp
  //                           ** SgLambdaExp 
  bool isRAJATemplateFunctionCallParameter(SgLocatedNode* n,  // n: SgLambdaExp
                       SgExprStatement* & callStmt, 
                       SgFunctionDeclaration*& raja_func_decl)
  {
    bool retval = false; 
    ROSE_ASSERT (n!= NULL);
    // Add another case: the call to the lambda exp is indirectly through a variable reference.
    SgExpression* le = isSgExpression(n);
    if (isSgLambdaExp (le) || isSgVarRefExp(le))
    {
      SgNode* parent = le->get_parent();
      ROSE_ASSERT(parent!=NULL);
      parent = parent->get_parent();
      ROSE_ASSERT(parent!=NULL);
      if (SgFunctionCallExp* call_exp = isSgFunctionCallExp (parent)) 
      {
        retval = isCallToRAJAFunction (call_exp);
        //if (raja_func_decl != NULL)
        raja_func_decl = call_exp-> getAssociatedFunctionDeclaration();
        callStmt = isSgExprStatement(call_exp->get_parent());
        ROSE_ASSERT(callStmt);
      }
    }
#if 0
    else
    {
      cerr<<"isRAJATemplateFunctionCallParameter () encounters a parameter which is not SgLambdaExp or SgVarRef:"<< n->class_name()<<endl;
      ROSE_ASSERT (false);
    }
#endif    
    return retval;
  }

  //! Check if a lambda function is inside a call to a template function matching a given name. Return the function decl if it is. 
  // This is a key interface function. Developers can first find a lambda expression , 
  // then check if it is a RAJA template function's parameter.
  //
  //
  // The AST should look like  SgFunctionCallExp
  //                           * SgExprListExp
  //                           ** SgLambdaExp 
  bool isWrapperTemplateFunctionCallParameter(SgLocatedNode* n,  // n: SgLambdaExp
                       const vector<string>& func_names, // the matching names of wrapper functions
                       SgExprStatement* & callStmt, 
                       SgFunctionDeclaration*& raja_func_decl)
  {
    bool retval = false; 
    ROSE_ASSERT (n!= NULL);
    // Add another case: the call to the lambda exp is indirectly through a variable reference.
    SgExpression* le = isSgExpression(n);
    if (isSgLambdaExp (le) || isSgVarRefExp(le))
    {
      SgNode* parent = le->get_parent();
      ROSE_ASSERT(parent!=NULL);
      parent = parent->get_parent();
      ROSE_ASSERT(parent!=NULL);
      if (SgFunctionCallExp* call_exp = isSgFunctionCallExp (parent)) 
      {
        // check if the function name matches a given name
        retval = isCallToRAJAFunction (call_exp);

        SgFunctionDeclaration* func_decl = call_exp->getAssociatedFunctionDeclaration();
        string obtained_name = (func_decl->get_qualified_name()).getString(); 
       
        for (size_t i=0; i< func_names.size(); i++)
        {
          string func_name = func_names[i];
          //  if (func_name == obtained_name) // the full qualified name is ::for_all < seq_exec ,  >, we only match the first portion for now
          //if (obtained_name.find (func_name,0) !=string::npos)
          if (isMatch(obtained_name, func_name)) 
          {
            retval = true; 
            break; 
          }
        }

        if (enable_debug)
        {
          if (!retval)
          {
            cout<<"\t obtained func name is: "<< obtained_name <<" no matching given names: "<<endl;
            for (size_t i=0; i< func_names.size(); i++)
              cout<<func_names[i]<<endl;
          }
        }

        //if (raja_func_decl != NULL)
        raja_func_decl = call_exp-> getAssociatedFunctionDeclaration();
        callStmt = isSgExprStatement(call_exp->get_parent());
        ROSE_ASSERT(callStmt);
      }
    }

    return retval;
  }

} // end RAJA_Checker namespace

//! Initialize the switch group and its switches.
Sawyer::CommandLine::SwitchGroup commandLineSwitches() 
{
  using namespace Sawyer::CommandLine;

  // Default log files for keep_going option
 // Using home may be a better choice, no scattered log files in every subdirectories.  
  //  report_filename__fail(boost::filesystem::path(getenv("HOME")).native()+"/rajaChecker-failed_files.txt");
  //  report_filename__pass(boost::filesystem::path(getenv("HOME")).native()+"/rajaChecker-passed_files.txt");

  Rose::KeepGoing::report_filename__fail = boost::filesystem::path(getenv("HOME")).native()+"/rajaChecker-failed_files.txt";
  Rose::KeepGoing::report_filename__pass = boost::filesystem::path(getenv("HOME")).native()+"/rajaChecker-passed_files.txt";

  SwitchGroup switches("RAJA Checker's switches");                                                                         
  switches.doc("These switches control the RAJA Checker tool. ");                                                          
  switches.name("");                                                                                      
                                                                                                                      
  switches.insert(Switch("debug")                                                                              
      .intrinsicValue(true, RAJA_Checker::enable_debug)                                                        
      .doc("Enable the debugging mode."));                                                                            

  switches.insert(Switch("report")                                                                            
      .argument("string", anyParser(Rose::KeepGoing::report_filename__pass))                                          
      .doc("Specify the report file for storing results, default is HOME/rajaChecker-passed-files.txt"));

  // Keep going option, false by default
  switches.insert(Switch("keep_going")                                                                                
      .intrinsicValue(true, RAJA_Checker::keep_going)                                                          
      .doc("Allow the tool to keep going even if errors happen"));                                             

  switches.insert(Switch("failure_report")                                                                            
      .argument("string", anyParser(Rose::KeepGoing::report_filename__fail))                                          
      .doc("Only used when keep_going is turned on. Specify the report file for storing files the tool cannot process, default is HOME/rajaChecker-failed-files.txt"));

#if 0                                                                                                                      
  switches.insert(Switch("dumpannot")                                                                                 
      .intrinsicValue(true, AutoParallelization::dump_annot_file)                                                     
      .doc("Dump annotation file content for debugging purposes."));                                                  
#endif                                                                                                                      
  return switches;                                                                                                    
} 

std::string RAJA_Checker::toString (SgNode* node)
{
  ROSE_ASSERT (node);
  ostringstream ss; 

  ss<<node->class_name() <<" @";
  ss<<node<<" :";

  SgLocatedNode* snode = isSgLocatedNode(node);
  if (!snode)
  {
    return ss.str();
  }
  
  if (SgInitializedName* iname = isSgInitializedName(snode))
    ss<<iname->get_name()<<" :";

  ss<<snode->get_file_info()->get_line(); 

  return ss.str();
}

std::vector<std::string> RAJA_Checker::commandline_processing(std::vector< std::string > & argvList)
{
  using namespace Sawyer::CommandLine;                                                                                
  Parser p = Rose::CommandLine::createEmptyParserStage(purpose, description);                                     
  p.doc("Synopsis", "@prop{programName} @v{switches} @v{files}...");                                                  
  p.longPrefix("-");                                                                                                  

// initialize generic Sawyer switches: assertion, logging, threads, etc.                                              
  p.with(Rose::CommandLine::genericSwitches());                                                                   
                                                                                                                      
// initialize this tool's switches                                                                                    
  p.with(commandLineSwitches());                                                                                      
                                                                                                                      
// --rose:help for more ROSE switches                                                                                 
  SwitchGroup tool("ROSE builtin switches");                                                                          
  bool showRoseHelp = false;                                                                                          
  tool.insert(Switch("rose:help")                                                                                     
             .longPrefix("-")                                                                                         
             .intrinsicValue(true, showRoseHelp)                                                                      
             .doc("Show the old-style ROSE help."));                                                                  
  p.with(tool);                                                                                                       

  std::vector<std::string> remainingArgs = p.parse(argvList).apply().unparsedArgs(true);                              

  if (RAJA_Checker::keep_going)                                                                                
    remainingArgs.push_back("-rose:keep_going");                                                                      
                                                                                                                      
// AFTER parse the command-line, you can do this:                                                                     
 if (showRoseHelp)                                                                                                    
    SgFile::usage(0);

  return remainingArgs;                  
}

using namespace RAJA_Checker;
using namespace SageInterface;


//! We avoid calling def-use analysis for input codes with unsupported constructs
// TODO: refine the granularity to be function level later
bool hasUnsupportedLanguageFeatures( VariantT* unsupportedConstruct)
{
  std::set<VariantT> blackListDict; 
  blackListDict.insert(V_SgLambdaExp);

  // build a dictionary of language constructs shown up in the loop, then query it
  RoseAst ast ( getProject());
  for(RoseAst::iterator i=ast.begin();i!=ast.end();++i) {
    if (blackListDict.find( (*i)->variantT()) != blackListDict.end())
    {
      *unsupportedConstruct= (*i)->variantT(); 
      return true; 
    }
  }
  return false;
}


//! Retrieve def use info. for a node's enclosing function definition
// Three possible cases: 
// 1. first time calling of the analysis
// 2. already called, analysis fails.  return NULL
// 3. already called, analysis succeeds.  return defuse value
// We use integer values to indicate them
// TODO: optimize it later , per function, not for the entire project, too expensive
DFAnalysis* RAJA_Checker::obtainDFAnalysis ()
{
  // run for entire project for now, later optimize for function level run only.
  static std::map <SgFunctionDefinition*, int> callingStatus; 
  static std::map <SgFunctionDefinition* , DFAnalysis*> cachedResults;

  static int called =0; 
  // must be static to persist across calls
  static DFAnalysis* defuse = NULL; 
  
  if (called ==0)
  {
    called = 1; 
    VariantT badConstruct; 
    if (hasUnsupportedLanguageFeatures (&badConstruct))
    {
     if (enable_debug)
          cout<<"Warning, defuse analysis skipped due to unsupported construct ..."<< badConstruct <<endl;
       return NULL; 
    }
    else
    {
      defuse = new DefUseAnalysis(getProject());
      int val = defuse->run(false); // debug is false
      if (val == 1)  // failed analysis? reset defuse to be NULL
      {
        if (enable_debug)
          cout<<"Warning, dfuse analysis fails..."<<endl;
        defuse = NULL;
      }
    }
  }
  
#if 0
  ROSE_ASSERT (node);
  SgFunctionDefinition* fd = getEnclosingFunctionDefinition (node);

  // def-use analysis never been called upon this function def?
  if (callingStatus[fd]==0)
  {

  }
#endif
  return defuse;
}
/*

Algorithm:
for a for loop

the body has a sequence of accumulation operation statements, in groups of 4. 
  lhs1 accum-op rhs;
  lhs: array element access using loop index  xm[loop_index], xm is a pointer to a double type
  accum-op:  +=, -=, *=, /=, MIN (), MAX, ..
  rhs:  double type scalar

Example:
   double* xa4, * ya4, ....; 
   xa1 = xa4 + 1
   for (i= ...)
   {
     xa4[i] += ax ; ya4[i] += ay ; za4[i] += az ; ;
     xa3[i] += ax ; ya3[i] += ay ; za3[i] += az ; ;
   }

In AST, these accumulation statements are 
SgExprStatement   
* SgPlusAssignOp
** SgPntrArrRefExp
*** SgVarRefExp: xa4, pointer to double
*** SgVarRefExp: i,   double scalar
** SgVarRefExp:  ax

Note that there might be empty statements in the sequence 

SgExprStatement
* SgNullExpression 

 * */


// Check if two integer variables are connected through an indirect array access: int i1 = IndexSet[i2]; 
// Useful to find of i is derived from loop index i2 for example stmt like:
//  a[i] = ..; 
// single level of indexSet?? int i1 = IndexSet[i2];
// TODO: multiple level of indexSet?? int i1 = IndexSet[iS2[i2]];
bool RAJA_Checker::connectedViaIndexSet (SgVarRefExp* varRef1, SgInitializedName* loopIndex)
{
  AstMatching m;
  ROSE_ASSERT (varRef1!=NULL);
  ROSE_ASSERT (loopIndex!=NULL);
  if (enable_debug)
  {
    cout<<"\t\t Entering connectedViaIndexSet () ..."<<endl;
  }  

  SgVariableSymbol* sym = varRef1->get_symbol();
  SgInitializedName* iname = sym->get_declaration();
  SgDeclarationStatement* decl = iname->get_declaration();
  
  if (enable_debug)
  {
    cout<<"\t\t the ast term for var decl:"<<endl;
    string p("$rhs=SgVariableDeclaration");
    MatchResult res=m.performMatching(p, decl);
    for(MatchResult::iterator i=res.begin();i!=res.end();++i) {
      SgVariableDeclaration* i1 = isSgVariableDeclaration((*i)["$rhs"]);
      cout<< AstTerm::astTermWithNullValuesToString(i1)<<endl;
    }
  }

  //        SgVariableDeclaration(null,SgInitializedName(SgAssignInitializer(SgPntrArrRefExp(SgVarRefExp,SgVarRefExp))))
  //string p("SgVariableDeclaration(null,SgInitializedName(SgAssignInitializer(SgPntrArrRefExp(SgVarRefExp,$rhs=SgVarRefExp))))");
  // there may be this-> pointer for indeSet variable reference!!
  string p("SgVariableDeclaration(null,SgInitializedName(SgAssignInitializer(SgPntrArrRefExp(_,$rhs=SgVarRefExp))))");
  MatchResult res=m.performMatching(p, decl);
  if (enable_debug)
    cout<<"\t\t matched result size():"<<res.size()<<endl;
#if 1  
  for(MatchResult::iterator i=res.begin();i!=res.end();++i) {
     SgVarRefExp* rhs = isSgVarRefExp((*i)["$rhs"]);

     //SgVariableDeclaration* i1 = isSgVariableDeclaration((*i)["$rhs"]);
     //cout<< AstTerm::astTermWithNullValuesToString(i1)<<endl;

     ROSE_ASSERT (rhs);
     SgVariableSymbol* sym2= rhs->get_symbol();
     ROSE_ASSERT (sym2);
     SgInitializedName* i2 = sym2->get_declaration();
     if (loopIndex==i2 ) 
       return true; 
  }
#endif  
  return false;
}

// Check if varRef1 is defined by loopIndex 
// We accept both direct assignment and deriving value of lhs from loopIndex
// e.g 1: int varRef1 = loopIndex;
// e.g 2: int varRef1 = (loopIndex < nnalls )? loopIndex: loopIndex - nnalls;
// This helps to find loop index buried behind another variable. 
bool RAJA_Checker::connectedViaAssignment(SgVarRefExp* varRef1, SgInitializedName* loopIndex)
{
  AstMatching m;
  ROSE_ASSERT (varRef1!=NULL);
  ROSE_ASSERT (loopIndex!=NULL);
  if (enable_debug)
  {
    cout<<"\t\t Entering connectedViaAssignment() ..."<<endl;
  }  

  // symbol and decl for varRef1  
  SgVariableSymbol* sym = varRef1->get_symbol();
  SgInitializedName* iname = sym->get_declaration();
  SgDeclarationStatement* decl = iname->get_declaration();
  
  // check if loopIndex appears within rhs
  RoseAst ast(decl);

  SgSymbol* idxSym = loopIndex->search_for_symbol_from_symbol_table(); 
  for(RoseAst::iterator i=ast.begin();i!=ast.end();++i) {
    if (SgVarRefExp * varRef = isSgVarRefExp(*i))
    {
      if (varRef->get_symbol() == idxSym)
        return true; 
    }
  }

#if 0  
  if (enable_debug)
  {
    cout<<"\t\t the ast term for var decl:"<<endl;
    string p("$rhs=SgVariableDeclaration");
    MatchResult res=m.performMatching(p, decl);
    for(MatchResult::iterator i=res.begin();i!=res.end();++i) {
      SgVariableDeclaration* i1 = isSgVariableDeclaration((*i)["$rhs"]);
      cout<< AstTerm::astTermWithNullValuesToString(i1)<<endl;
    }
  }

  //SgVariableDeclaration(null,SgInitializedName(SgAssignInitializer(SgVarRefExp:zoneIdx)))
  //string p("SgVariableDeclaration(null,SgInitializedName(SgAssignInitializer(SgPntrArrRefExp(SgVarRefExp,$rhs=SgVarRefExp))))");
  // there may be this-> pointer for indeSet variable reference!!
  //string p("SgVariableDeclaration(null,SgInitializedName(SgAssignInitializer(SgPntrArrRefExp(_,$rhs=SgVarRefExp))))");
  string p("SgVariableDeclaration(null,SgInitializedName(SgAssignInitializer($rhs=SgVarRefExp)))");
  MatchResult res=m.performMatching(p, decl);
  if (enable_debug)
    cout<<"\t\t matched result size():"<<res.size()<<endl;
  for(MatchResult::iterator i=res.begin();i!=res.end();++i) {
     SgVarRefExp* rhs = isSgVarRefExp((*i)["$rhs"]);

     //SgVariableDeclaration* i1 = isSgVariableDeclaration((*i)["$rhs"]);
     //cout<< AstTerm::astTermWithNullValuesToString(i1)<<endl;

     ROSE_ASSERT (rhs);
     SgVariableSymbol* sym2= rhs->get_symbol();
     ROSE_ASSERT (sym2);
     SgInitializedName* i2 = sym2->get_declaration();
     if (loopIndex==i2 ) 
       return true; 
  }
#endif  
  return false;
}


// Check if an operand is a form of x[index], x is a pointer to a double, index is a loop index
bool isDoubleArrayAccess (SgExpression* exp, SgInitializedName * lvar, SgVarRefExp** varRefOut)
{
  ROSE_ASSERT (lvar != NULL);
  if (exp == NULL) 
  {
    if (RAJA_Checker::enable_debug) cout<<"\t\t\t NULL exp" <<endl;
    return false;
  }
  SgPntrArrRefExp* arr = isSgPntrArrRefExp (exp);
  if (arr == NULL)
  {
    if (RAJA_Checker::enable_debug) cout<<"\t\t\t not SgPntrArrRefExp, but" << exp->class_name() <<endl;
    return false;
  }

  SgExpression* lhs, *rhs;
  lhs = arr->get_lhs_operand();
  if (lhs == NULL) 
  {
    if (RAJA_Checker::enable_debug) cout<<"\t\t\t lhs of a[i] is NULL " <<endl;
    return false;
  }
  // The lhs should be a form of simple double pointer used as array, not complex data member like obj->array[i]. 
  SgVarRefExp* ref = isSgVarRefExp(lhs);
 if (!ref)
  {
    bool isThis = false; 
    SgArrowExp * arrow = isSgArrowExp (lhs);
    if (arrow)
    {
      SgThisExp * thisexp = isSgThisExp(arrow->get_lhs_operand());
      if (thisexp)
        isThis = true; 
    }

    // we allow this->data[i], but not a->data[i] as lhs
    if (!isThis)
    {
      if (RAJA_Checker::enable_debug) cout<<"\t\t\t array var is not a simple variable reference type, but " << lhs->class_name() <<endl;
      return false;
    }
    else
    {
      // store rhs of this->rhs as the varRefOut
      // would it always be SgVarRefExp ?
      *varRefOut = isSgVarRefExp(arrow->get_rhs_operand());
    }
  }
  else
  {
    // store ref if not NULL
    *varRefOut = ref; 
  }
 
  ROSE_ASSERT (*varRefOut != NULL);

  rhs = arr->get_rhs_operand();
  if (rhs == NULL) 
  {
    if (RAJA_Checker::enable_debug) cout<<"\t\t\t rhs of a[i] is NULL " <<endl;
    return false;
  }

  // lhs is a pointer type
  SgType* ltype = lhs->get_type()->stripType(SgType::STRIP_TYPEDEF_TYPE|SgType::STRIP_REFERENCE_TYPE);
  SgPointerType* ptype = isSgPointerType(ltype);
  if (!ptype)
  {
    if (RAJA_Checker::enable_debug) cout<<"\t\t\t array var's type is not a pointer type, but " << lhs->get_type()->class_name() <<endl;
    return false;
  }
  // lhs is a pointer to double, float or integer 
   SgType* bst = ptype->get_base_type();
   if (! isSgTypeDouble( bst) && ! isSgTypeInt ( bst )  && ! isSgTypeFloat ( bst) )
   {
    if (RAJA_Checker::enable_debug) cout<<"\t\t\t ptype of array 's base type not a double, int or float type, but " << ptype->get_base_type()->class_name()<<endl;
    return false;
   }
  
  // rhs is a loop index
 
 // strip off one or more levels of index arrays
  while (isSgPntrArrRefExp(rhs))
    rhs = isSgPntrArrRefExp(rhs)->get_rhs_operand();

  SgVarRefExp* varRef = isSgVarRefExp(rhs) ;
  
  if (varRef == NULL) 
  {
    if (RAJA_Checker::enable_debug) cout<<"\t\t\t rhs of a[i] is not SgVarRefExp, but " << rhs->class_name() <<endl;
    return false;
  }

  SgSymbol * s1 = varRef->get_symbol();
  SgSymbol * s2 = lvar->get_symbol_from_symbol_table ();
  if ( s1 != s2 && !connectedViaIndexSet(varRef, lvar) && ! connectedViaAssignment(varRef, lvar))
  {
    if (RAJA_Checker::enable_debug) 
    {
      cout<<"\t\t\t symbol is not equal to loop index symbol" <<endl;
      cout<< "\t\t\t\t rhs of a[i] var Ref:" << varRef->unparseToString() <<" : "<<s1->unparseToString() <<endl;
      cout<< "\t\t\t\t loop index var:" << lvar->unparseToString() <<" : "<<s2->unparseToString() <<endl;
    }
    return false;
  }
  
  return true; 
}

//  accum-op:  +=, -=, *=, /=, MIN (), MAX, ..
bool isNodalAccumulationOp (SgExpression* op, bool& isAssign )
{
  if (op == NULL) return false;

  if (isSgPlusAssignOp(op) ||
      isSgMinusAssignOp(op) ||
      isSgMultAssignOp(op) ||
      isSgDivAssignOp(op)  || 
       isSgAssignOp(op)  // MIN() and MAX () expanded to  xm[i]= xm[i]<op2 ? xm[i]: op2 or xm[i]= xm[i]>op2 ? xm[i]: op2
     )
  {
    isAssign = isSgAssignOp (op);
    return true;
  }

  return false;
}

// only check expression statement: single line statement
bool isMatchedAssignment (SgExprStatement * input, SgVariableDeclaration* decl)
{
  ROSE_ASSERT (input !=NULL);
  SgExpression* lhs=NULL, *rhs=NULL;
  
  if (isAssignmentStatement (input, &lhs, &rhs))
  {
    if (SgVarRefExp* var = isSgVarRefExp (lhs))
    {
      // match left hand side
      if (var->get_symbol() == getFirstVarSym(decl))
      {
        // right hand is a + operation
        SgAddOp * addop = isSgAddOp(rhs); 
        if (addop)
          return true; 
        else
        {
          if (RAJA_Checker::enable_debug)
            cout <<"\t\t\t\t\t rhs is not an add op, but "<< rhs->class_name()<< endl;
        }
      }
      else
      {
        if (RAJA_Checker::enable_debug)
          cout <<"\t\t\t\t\t lhs symbol does not match decl's symbol"<< endl;
      }
    }
    else
    {
      if (RAJA_Checker::enable_debug)
        cout <<"\t\t\t\t\t lhs is not VarRefExp, but "<< lhs->class_name() <<endl;
    }
  }
//  else if (isSgScopeStatement (input)) 
//  {
//  }
  return false;
}

// Check a scope statement for any matching assignment for decl
//TODO: This is really a workaround due to lack of def-use analysis for RAJA code
bool hasMatchedAssignmentInside (SgScopeStatement* sinput, SgVariableDeclaration* decl)
{
  RoseAst ast(sinput);
  for(RoseAst::iterator i=ast.begin();i!=ast.end();++i) {
    SgExprStatement* estmt = isSgExprStatement(*i);
    if (estmt)
    {
      if (isMatchedAssignment (estmt, decl)) 
        return true;
    }
  }
  return false;   
}


// For a variable declaration without initialization, find its followed unique assignment
// within the same scope right after the declaration, if there is a unique assignment 
// with a format like: var = base +offset; 
// This has to recognize the assignment statement buried within if-statement.
bool findUniqueNextAssignment (SgVariableDeclaration* decl, SgStatement* loopStmt)
{
  ROSE_ASSERT (decl);

  int counter = 0; 
  SgStatement* current = decl;
  while ((current = getNextStatement(current)))
  {
    if (RAJA_Checker::enable_debug)
      cout <<"\t\t\t\t\t checking next stmt at line "<< current->get_file_info()->get_line() <<endl;
    if (SgExprStatement* estmt =  isSgExprStatement(current))
    {
      if (isMatchedAssignment(estmt, decl))
        counter++;
    } // end if assignment
    else if (SgScopeStatement* scopestmt=  isSgScopeStatement(current))
    {
      if (hasMatchedAssignmentInside(scopestmt, decl))
        counter++;
    }
    else
    {
      if (RAJA_Checker::enable_debug)
        cout <<"\t\t\t\t\t Not an assignment stmt "<< current->get_file_info()->get_line() <<endl;
    }
  } // end while

  if (counter>=1)
  {
    if (RAJA_Checker::enable_debug)
      cout <<"\t\t\t\t\t Met condition: findUniqueNextAssignment() finds  "<< counter << " matched assignment "<<endl;
  }
  else
  {
    if (RAJA_Checker::enable_debug)
      cout <<"\t\t\t\t\t Failed condition: findUniqueNextAssignment() finds  "<< counter << " matched assignment "<<endl;
  }

  // TODO need users confirmation about this new pattern test case: nodalAccPattern5-multipleAssign.cpp
  //what will happen if we only check if >=1? 
  return (counter >= 1); 
  //return (counter == 1); 
}

// check if it is an initialization with a form of base+offset
bool conformingAssignInitializer(SgAssignInitializer * assign_init)
{
  ROSE_ASSERT (assign_init);
  SgAddOp * addop = isSgAddOp(assign_init->get_operand_i());
  if (addop)
    return true;
  return false;
}

// Check if an assign op is the only def place for a variable iname
// its previous statement should have 0 or 1 def place as OUT DEF set
bool uniqueAssignOp (SgAssignOp* assign_op, SgInitializedName* iname)
{
  ROSE_ASSERT (assign_op);
  // unique value assignment, right after variable declaration without initialization
  // This is a unique assignment , its IN set is its declaration
  SgStatement* prevStmt = getPreviousStatement (getEnclosingStatement(assign_op));
  ROSE_ASSERT (prevStmt);

  if (RAJA_Checker::enable_debug)
    cout<<"previous stmt is "<< toString(prevStmt) <<endl;

  DFAnalysis* dfa = obtainDFAnalysis();
  if (dfa)
  { 
    // OUT_def_set (prevStmt): only one def node, which can be either assignInitializer (with assign initializer) or but a naked variable decl
    // which is SgInitializedName
    vector <SgNode*> defs = dfa->getDefFor(prevStmt, iname);
    if (defs.size() ==0) //no previous definition. This may be caused by wrong def-use analysis results. It should have at least one def (declared)
    {

      if (RAJA_Checker::enable_debug)
        cout<<"cond 1 is met: no previous definition within OUT set of prev statement "<<endl;
      return true;    
    }
    else
      if (defs.size() ==1)
      {
        if (isSgInitializedName(defs[0]) || isSgAssignInitializer (defs[0]) )
        {
          if (RAJA_Checker::enable_debug)
            cout<<"cond 1 is met: no previous definition within OUT set of prev statement "<<endl;
          return true;
        }
        else
        {
          if (RAJA_Checker::enable_debug)
            cout<<"in uniqueAssignOp(): the DEF OUT set of prevstmt is size 1, but the DEF node is not initialized name or assign initializer. It is "<< toString(defs[0])<<endl;
        }
      }
      else
      {
        if (RAJA_Checker::enable_debug)
        {
          cout<<"Not uniquely assigned before reaching the loop!  OUT DEF of prev statement is not size 1, but "<<defs.size() <<endl;
          for (size_t i =0; i< defs.size(); i++)
          {
            cout<<toString(defs[i])<<endl;
          }
        }
      }
  }

  // if dfa fails, assuming the worst. Cannot decide
  return false;
}

// unique assign operation in the path of variable declaration to the entry of loop
bool conformingAssignOp(SgAssignOp* assign_op)
{
  ROSE_ASSERT (assign_op);
  // rhs is a form of base+ offset
  SgAddOp * addop = isSgAddOp(assign_op->get_rhs_operand());
  if (addop)
    return true;
  else
  {
    if (RAJA_Checker::enable_debug)
       cout<<"rhs op is not a conforming plus operation, but "<< assign_op->get_rhs_operand()->class_name()<<endl;
    
  }

  return false;
}


// Check if a variable is declared within a declaration with a form of 
// 1. double var = base + offset;  Or
// 2. double var; ...; var = base + offset;  
// Up to then IN entry to a loopstatement
// TODO: all variables should be consistent about the choice of two ???
bool initializedWithBasePlusOffset (SgInitializedName* iname, SgStatement* loopStatement)
{
  ROSE_ASSERT (iname != NULL);
  if (RAJA_Checker::enable_debug)
    cout<<"\t\t\t Checking if initName is previously initialized with base+offset: "<< iname->get_name() <<endl;
  
//  SgVariableSymbol* sym = isSgVariableSymbol(iname->get_symbol_from_symbol_table()); 
//  ROSE_ASSERT (sym != NULL);
  SgVariableDeclaration* decl = isSgVariableDeclaration(iname->get_declaration()); 

  if (decl==NULL) 
  {
    if (RAJA_Checker::enable_debug)
      cout<<"\t\t left side iname variable's declaration is not a SgVariableDeclaration, but a "<< iname->get_declaration()->class_name()<<endl;
    return false;
  }

  // form 1: initialized as part of declaration
  bool form1=false, form2=false;
  SgAssignInitializer * assign_init = isSgAssignInitializer (iname->get_initptr());
  if (assign_init)
  {
    form1 = conformingAssignInitializer (assign_init); 
  }
  else // the variable is initialized later, search and find it! TODO
  {
    if (findUniqueNextAssignment (decl, loopStatement))
      form2 = true; 
    else
    {
      if (RAJA_Checker::enable_debug)
        cout <<"No unique next assignment stmt of a format of: var = base + offset;"<<endl;
    } 
  }

  return (form1 || form2); 
}


//! Check if a variable is only initialized once since its declaration, at the entry point of a loop.
// Internally reaching definition analysis is used to retrieve such information. 
bool RAJA_Checker::initializedOnceWithBasePlusOffset (SgInitializedName* iname, SgStatement* loopStatement, bool * defOnce)
{
  ROSE_ASSERT (iname);
  //TODO: support lambda function call later, not a for loop anymore
  ROSE_ASSERT (loopStatement);
  // Initialize it to be true
  * defOnce = true; 

  // This analysis only stores OUT set of def or use variables for each CFG node. 
  // We use the OUT_definition set of SgForInitStatement as an approximation of IN-definition for the loop. 
  DFAnalysis* dfa = obtainDFAnalysis(); 
  if(dfa)
  {
    if (RAJA_Checker::enable_debug)
      cout<<"DFA analyis successful .."<<endl;
    if(SgForStatement* fs = isSgForStatement(loopStatement))
    {
      SgForInitStatement* finit = fs->get_for_init_stmt();
      vector <SgNode*> defs = dfa->getDefFor(finit, iname); 
      // TODO: There should be only one reaching definition
      // we should check if more than two, they are from mutual exclusive paths, like true-false branches.  
      // get their previous DEF nodes, must be AssignInitialzer
      bool hasAssign= false; 
      // Check all reaching definitions one by one
      // at least one of them is base+offset
      for (size_t i=0; i< defs.size(); i++)
      {
        // if (RAJA_Checker::enable_debug)
        SgLocatedNode * lnode = isSgLocatedNode (defs[i]);
        ROSE_ASSERT (lnode);
        if (RAJA_Checker::enable_debug)
          cout<< toString(iname) << " Defined at "  << toString(lnode) <<endl;
        // assign initializer?  
        if (SgAssignInitializer * init = isSgAssignInitializer(lnode))
        {
          if (conformingAssignInitializer(init))
          {
            // for an assign intializer, there is no other previous definition possible. So it is defOnce on this CFG path
            // *defOnce = true;  // no effect for defOnce
            hasAssign=true;
          }
        } // assignment operation within an assignment statement
        else if (SgAssignOp * assign = isSgAssignOp(lnode))
        {
           if (conformingAssignOp(assign))
           {
             hasAssign = true; // instead of returning true, we just keep the value, let the iteration finish with checking all
             // it is possible a later def operation is a conforming assign initializer but with defs.size() !=1
              if (RAJA_Checker::enable_debug)
                cout<<"this assign op is conforming"<<endl;
           }
           else
           {
              if (RAJA_Checker::enable_debug)
                cout<<"this assign op is not conforming"<<endl;
           }
           // If any of the assignment is not unique reaching definition, defOnce is set to be false
           if (!uniqueAssignOp (assign, iname))
           {
             *defOnce = false;
              if (RAJA_Checker::enable_debug)
                cout<<"this assign op is not unique reaching definition, defOnce set to be false"<<endl;
           }
        }
        else
        {
        if (RAJA_Checker::enable_debug)
          cout<<"warning, RAJA_Checker::initializedOnceWithBasePlusOffset() encounters unhandled def node type: "<<endl;
        }
      } // end for

      // Now at least one path is initialized with base+offset
      if (hasAssign) 
        return true; 
    } // end if for statement
  }
  else
  {
    if (RAJA_Checker::enable_debug)
      cout<<"\t\t\t DFA analysis returns NULL ..."<<endl;
  }
  return false;  
}


/* lvar is the enclosing loop's loop index variable. 
 * xa4[i] += ax; within a for-loop, i is loop index
 *  lhs: array element access using loop index  xm[loop_index], xm is a pointer to a double type
 *  accum-op:  +=, -=, *=, /=, MIN (), MAX, ..
 *
 *  More conditions for the variable on the left side
 *   1. Must declared within the enclosing function body, not as function parameter
 *   2. must be assigned between the declaration and the entry of the enclosing loop body
 *   3. the assignment must have a form of base + offset for the rhs // at least one of the statement!!
 *   4. must be declared/initialized in a straight line code block. 
 *
 * */
bool RAJA_Checker::isNodalAccumulationStmt (SgStatement* s, SgInitializedName* lvar, bool * lhsInitPatternMatch, SgStatement* loopStatement, bool* lhsUniqueDef)
{
  if (RAJA_Checker::enable_debug)  // ofile is used for diffing,   cout is for checking traces
    cout<<"\t Checking isNodalAccumulationStmt() for stmt at line:"<<s->get_file_info()->get_line()<<endl;

  ROSE_ASSERT (lvar != NULL);
  if (s==NULL) return false;
  SgExprStatement* es = isSgExprStatement (s);
  if (es==NULL) 
  {
    if (RAJA_Checker::enable_debug)
      cout<<"\t\t Failed: is SgExprStatement, returning false"<<endl;
    return false;
  }
  else
  {
    if (RAJA_Checker::enable_debug)
      cout<<"\t\t Met condition: Is SgExprStatement, continue next check"<<endl;
  }  

  SgExpression* exp = es->get_expression();
  if (exp==NULL)
  {
    if (RAJA_Checker::enable_debug)
      cout<<"\t\t Failed: is SgExpression, return false"<<endl;
    return false;
  }
  else
  {
    if (RAJA_Checker::enable_debug)
      cout<<"\t\t Met condition: is SgExpression, continue next check "<<endl;
  }  

  bool isAssign = false; // is direct assignment statement or not: differentiate MIN/MAX pattern
  if (! isNodalAccumulationOp (exp, isAssign) )
  {  
    if (RAJA_Checker::enable_debug)
      cout<<"\t\t Failed  isNodalAccumulationOp, returning false"<<endl;
    return false;
  }
  else
  {
    if (RAJA_Checker::enable_debug)
      cout<<"\t\t Met condition: isNodalAccumulationOp, continue next check "<<endl;
  }  

  SgBinaryOp* bop = isSgBinaryOp (exp);
  ROSE_ASSERT (bop != NULL);

  // check properties of lhs = ..  ----------------------------
  // lhs is x[i]
  SgVarRefExp* varRef=NULL; 
  if (!isDoubleArrayAccess(bop->get_lhs_operand(), lvar, &varRef))
  {
    if (RAJA_Checker::enable_debug)
      cout<<"\t\t Failed: is DoubleArrayAccess like array[index] for lhs, returning false"<<endl;
    return false;
  }
  else
  {
    if (RAJA_Checker::enable_debug)
      cout<<"\t\t Met condition: lhs has a form of array[index], continue next check "<<endl;
  }  

  // check lhs should be declared within the enclosing function's body
  //
  ROSE_ASSERT (varRef!=NULL); 
  SgVariableSymbol* sym= varRef->get_symbol();
  ROSE_ASSERT (sym !=NULL); 
  SgInitializedName* iname= sym->get_declaration();
  ROSE_ASSERT (iname!=NULL); 
  if (isSgFunctionParameterList(iname->get_parent()))
  {
    if (RAJA_Checker::enable_debug)
      cout<<"\t\t Failed: declared within the enclsoing function's body: variable is a function parameter, returning false"<<endl;
    return false;
  }
  else
  {
    if (RAJA_Checker::enable_debug)
      cout<<"\t\t Met condition: lhs is declared within the enclosing function's body, continue next check"<<endl;
  }  

  // lhs should be initialized in advance using a form of  lhs = base+offset; 

  // for variables captured within a labmda expression,a new variable is created.
  // We have to find the original variable SgInitializedName. 
  SgLambdaExp* le = NULL; 
  le = getEnclosingNode<SgLambdaExp> (s);
  if (le)
  {
    SgStatement* call_stmt = getEnclosingStatement (le);
    SgVariableSymbol* orig_sym =  lookupVariableSymbolInParentScopes (iname->get_name(), call_stmt->get_scope());
    ROSE_ASSERT (orig_sym);
    iname = orig_sym->get_declaration();
  }

  // Check the patter of initialized once with base+offset, like: xa1 = base +offset.
  // We use two methods together: 
  //      1 . structural analysis: initializedWithBasePlusOffset ()
  //      2.  we then further use data flow analysis to tighten the screw for for-loop: only initialized once
  // TODO: only use one of initializedWithBasePlusOffset () and initializedOnceWithBasePlusOffset()
  // old structure based checking, not accurate. But handles both for-loop and lambda exps.
  if (initializedWithBasePlusOffset (iname, loopStatement) )
  {
    *lhsInitPatternMatch = true;
    if (RAJA_Checker::enable_debug)
      cout<<"\t\t Met condition: initialized-with-base+offset using structural analysis , continue next check"<<endl;
  }
  else
  {
    *lhsInitPatternMatch = false;
    if (RAJA_Checker::enable_debug)
      cout<<"\t\t Failed: not Found initialized-with-base+offset using structural analysis, continue next check using DFA analysis"<<endl;
  }


  // What will happen if no uniqueness check at all ?? 
#if 0
  // new version using def-use analysis , tighten the screw further for supported for-loops 
  // TODO: DFA may report wrong reaching definition number for nodalAccPattern5-multipleAssign.cpp
  if ( loopStatement) //TODO handle lambda body!!
  { // obtain two conditions: 
    // Condition 1. has at least one reaching def as base + offset
    // condition 2: all reaching definitions are unique
    if( initializedOnceWithBasePlusOffset (iname, loopStatement, lhsUniqueDef))
    {
      *lhsInitPatternMatch = true; 
      if (RAJA_Checker::enable_debug)
        cout<<"\t\t Met condition: initialized once with base+offset according to def-use analysis, continue next check "<<endl;
    }
    else
    {
      if (RAJA_Checker::enable_debug)
        cout<<"\t\t Failed: Initialized once with base+offset according to def-use analysis or analysis fails "<<endl;
    }
  }
  else // for lambda function , we assuming conditions are always met for now TODO, def-use analysis for lambda??
  {
    *lhsUniqueDef= true; 
  }
#else
   *lhsUniqueDef= true;

#endif

  if (RAJA_Checker::enable_debug)
    cout<<"\t\t Failed: initialized once with base+offset according to both structual or def-use analysis"<<endl;


  // check properties of rhs ------------------------   
  if (isAssign) // match rhs pattern for MIN/MAX
  {
    SgExpression* rhs = bop->get_rhs_operand(); 
    if (!isSgConditionalExp(rhs))
    { // TODO : more restrictive pattern match here: 
      // xm[i] = xm[i]<op2 ? xm[i]: op2
      // xm[i] = xm[i]>op2 ? xm[i]: op2
      if (RAJA_Checker::enable_debug)
        cout<<"\t\t not conditional operation for rhs within lhs = rhs, but "<< rhs->class_name() << "  returning false.." <<endl;
      return false;
    }  
    else
    {
      if (RAJA_Checker::enable_debug)
        cout<<"\t\t Met condition: rhs has a pattern of MIN/MAX, continue next check"<<endl;
    }
  }
  else // match rhs pattern for += rhs, -= rhs, etc. 
  {
    // rhs is a scalar type
    SgType* rhs_type = bop->get_rhs_operand()->get_type();
    rhs_type = rhs_type->stripType(SgType::STRIP_REFERENCE_TYPE);
    if (!SageInterface::isScalarType (rhs_type)) 
    {
      if (RAJA_Checker::enable_debug)
        cout<<"\t\t Failed : scalar type for rhs, but "<< rhs_type->class_name()<< "  returning false.." << endl;
      return false;
    }
    else
    {
      if (RAJA_Checker::enable_debug)
        cout<<"\t\t Met condition: rhs has a scalar type, continue check"<<endl;
    }

    // skip const or typedef chain
    rhs_type =rhs_type->stripTypedefsAndModifiers();

    // rhs is a double, float or integer type, both integer and float arrays are allowed
    if (!isSgTypeDouble(rhs_type) && !isSgTypeFloat(rhs_type) && !isSgTypeInt(rhs_type)) 
    {
      if (RAJA_Checker::enable_debug)
        cout<<"\t\t Failed: not double, float or integer type for rhs, but a type of "<< rhs_type->class_name() <<endl;
      return false;
    }
    else
    {
      if (RAJA_Checker::enable_debug)
        cout<<"\t\t Met condition: rhs type is double, float or int, continue check"<<endl;
    }
  }

  if (RAJA_Checker::enable_debug)
      cout<< "\t\t Met all conditions for isNodalAccumulationStmt(), returning true ..."<<endl;
  // meet all conditions above
  return true;
}

bool isNullStatement(SgStatement* s)
{
  if (s==NULL) return false;
  SgExprStatement* es = isSgExprStatement(s);
  if (es == NULL) return false;

  if (es->get_expression())
  {
    return isSgNullExpression(es->get_expression());
  }
  return false;
}

// find the next non-null statement from the current s
// skip empty statement like ;
SgStatement* getNextNonNullStatement(SgStatement* s)
{
  if (s == NULL) return NULL;

  SgStatement* r = NULL;
  r = SageInterface::getNextStatement (s);
  if (r== NULL) return r; 
  //skip one or more ; 
  while (isNullStatement(r))
    r = SageInterface::getNextStatement (r);
  //now r is NULL, or not a NULL statement
  return r; 
}
// With a known loop variable lvar, check if a basic block contains the 4-statement pattern
// This is the finest level of checking. We call it atomic. 
bool RAJA_Checker::isNodalAccumulationBodyAtomic(SgBasicBlock* bb, SgInitializedName* lvar, SgExprStatement*& fstmt, SgStatement* loopStmt)
{
  ROSE_ASSERT (bb != NULL);
  ROSE_ASSERT (lvar != NULL);
  if (enable_debug)
    cout<<"\tEntering isNodalAccumulationBodyAtomic() for basic block at line:"<<bb->get_file_info()->get_line()<<endl;
  //if the body contains at least 4 nodal accumulation statement in a row, then it is a matched loop
  // Find all expression statements. if there is one Nodal Accumulation Statement (NAS) , and it is followed by 3 other NAS.
  // then there is a match. 
  Rose_STL_Container<SgNode*> stmtList = NodeQuery::querySubTree(bb, V_SgExprStatement);
  for (Rose_STL_Container<SgNode*>::iterator iter = stmtList.begin(); iter != stmtList.end(); iter++)
  {
    bool init1=false, init2=false, init3=false, init4=false;
    bool unique1=false, unique2=false, unique3=false, unique4=false;
    SgExprStatement* s = isSgExprStatement(*iter);
    if (isNodalAccumulationStmt (s, lvar, &init1, loopStmt, &unique1))
    {
      SgStatement* s2, *s3, *s4; 
      s2= getNextNonNullStatement(s);
      if (enable_debug)
        cout<<"\t\t "<<toString(s)<<" base + offset "<< init1 << " unique reaching def "<< unique1<<endl;
      if (s2!=NULL && isNodalAccumulationStmt (s2, lvar, &init2, loopStmt, &unique2))
      {
        if (enable_debug)
          cout<<"\t\t "<<toString(s2)<<" base + offset "<< init2 << " unique reaching def "<< unique2<<endl;
        s3 = getNextNonNullStatement (s2);
        if (s3!=NULL && isNodalAccumulationStmt (s3, lvar, &init3, loopStmt, &unique3))
        {
          if (enable_debug)
            cout<<"\t\t "<<toString(s3)<<" base + offset "<< init3 << " unique reaching def "<< unique3<<endl;
          s4 = getNextNonNullStatement (s3);
          if (s4 != NULL && isNodalAccumulationStmt (s4, lvar, &init4, loopStmt, &unique4))
          { 
            if (enable_debug)
              cout<<"\t\t "<<toString(s4)<<" base + offset "<< init4 << " unique reaching def "<< unique4<<endl;
            fstmt = s; 
            // why or here?  should be all matching?
            // already 4 levels of if conditions, already && of 4 statements with accumulation pattern
            // at least one variable is initialized with base+ offset
            // Add another condition: all variables are assigned/initialized once before enter this point
            if ((init1 || init2 || init3 ||init4)
                && (unique1 && unique2 && unique3 && unique4))
              return true;
            else
            {
              if (enable_debug)
              {
                cout<<"\t\t Failed to matched 4 accumulation statement pattern, but none of the accumulation statements have lhs initialized with base + offset "<<endl;
                cout << "\t\t sub cond1: (init1|| init2 || init3 ||init4): " << (init1 || init2 || init3 ||init4) << " init1 ="<< init1 <<" init2 ="<< init2 << " init3 ="<< init3 << " init4 ="<< init4   <<endl;
                cout << "\t\t sub cond2: (unique1 && unique2 && unique3 && unique4): " << (unique1 && unique2 && unique3 && unique4) << " unique1="<< unique1 <<" unique2 ="<< unique2 << " unique3 ="<< unique3 << " unique4 ="<< unique4 <<endl;
              }

            }
          } // end s4
        } //end s3
      } // end s2
    } // end s1 
  } // end for loop  

  if (enable_debug)
    cout<<"\t\t Failed isNodalAccumulationBodyAtomic () return false.. "<<endl;
  return false;
}

// With a known loop variable lvar, check if a basic block contains the 4-statement pattern
// We have to find all eligible basic blocks within this bb (self included). 
// The reason is that the pattern may be buried inside if-stmt's true of false bodies.
/*
The algorithm : iterate over for-loop's body: start with for_loop->get_loop_body()
* or in the case of RAJA function call: body of lambda function expression.

for all statement in the iterator
  if is another for loop, skip its entire subtree 
  if while loop, skip its entire subtree

 if (SgBasic block) // find all basic blocks, do the match
  {
    call the atomic check for the found BB  
    if found , match , if not?
    iterator ++;  
   }
 */
bool RAJA_Checker::isNodalAccumulationBody(SgBasicBlock* bb, SgInitializedName* lvar, SgExprStatement*& fstmt, SgStatement* loopStmt)
{
  if (enable_debug)
    cout<<"\t\t Entering isNodalAccumulationBody ()..."<<endl;

  RoseAst ast(bb); 
  for(RoseAst::iterator i=ast.begin();i!=ast.end();++i) {
    SgLocatedNode* lnode = isSgLocatedNode(*i); 
    if (lnode == NULL) continue; 
#if 0
    // skip things in system header
    if (SageInterface::insideSystemHeader (lnode)) 
    { 
      i.skipChildrenOnForward(); 
      continue; 
    }
#endif 
    SgForStatement* forstmt= isSgForStatement(*i);
    // TODO: do users want to skip while or do-while loops?
    //  We should not see nested raja function calls. Nested raja patterns have their own versions. 
    if (forstmt)
    { 
      i.skipChildrenOnForward();  
      if (enable_debug)
        cout<<"\t\t\t Skipping a nested loop at line "<<forstmt->get_file_info()->get_line() <<endl;
    }
    else if (SgBasicBlock* cur_bb = isSgBasicBlock(*i))
    {
      SgExprStatement* firststmt= NULL; 
      if (enable_debug)
        cout<<"\t\t\t Search a basic block at line..."<<cur_bb->get_file_info()->get_line() <<endl;
      if (isNodalAccumulationBodyAtomic (cur_bb, lvar, firststmt, loopStmt) )
      {
        fstmt = firststmt; 
        if (enable_debug)
          cout<<"\t\t\t Met condition: isNodalAccumulationBodyAtomic () for a basic block at line..."<<cur_bb->get_file_info()->get_line() <<endl;
        return true; 
      }
      else
      {
        if (enable_debug)
          cout<<"\t\t\t Failed condition: isNodalAccumulationBodyAtomic () for a basic block at line..."<<cur_bb->get_file_info()->get_line() <<endl;
      }
    }
  }
  return false;
}

bool RAJA_Checker::hasNodalAccumulationBody( SgLambdaExp* exp, SgExprStatement*& fstmt)
{
  ROSE_ASSERT(exp);
   if (enable_debug)
     cout<<"\t\t entering hasNodalAccumulationBody ..."<<endl;
   // ROSE uses a anonymous class declaration for lambda expression. Its function is a member function. 
   SgMemberFunctionDeclaration* lfunc = isSgMemberFunctionDeclaration( exp->get_lambda_function());
   if (lfunc ==NULL) return false;

   // loop variable is modeled as the first parameter of the lambda function's parameter list
   SgFunctionParameterList* plist = lfunc->get_parameterList();
   if (plist ==NULL) return false;
   
   // TODO handle more complex RAJA pattern using loop index variables. 
   SgInitializedName* lvar= (plist->get_args())[0];
   if (lvar ==NULL) return false;

   SgFunctionDefinition* def = lfunc->get_definition();
   if (def==NULL) return false;
   SgBasicBlock* bb = def->get_body();
   if (bb ==NULL) return false;

  return isNodalAccumulationBody (bb, lvar, fstmt, NULL); // TODO: handle lambda call statement 
  
}
bool RAJA_Checker::isNodalAccumulationLoop(SgForStatement* forloop, SgExprStatement*& fstmt)
{
  SgStatement* body = forloop->get_loop_body();
  if (body == NULL) 
  {
    if (RAJA_Checker::enable_debug)
      cout<< "\t Failed condition: NULL body, return false;"<<endl;
    return false;
  }

  SgBasicBlock* bb = isSgBasicBlock(body);
  if (bb == NULL) 
  {
    if (RAJA_Checker::enable_debug)
      cout<< "\t Failed condition: NULL Basic Block as body, return false;"<<endl;
    return false;
  }

  SgInitializedName* lvar = SageInterface::getLoopIndexVariable (forloop);
  if (lvar ==NULL)
  {
#if 0    
    AstMatching m;
    
    MatchResult res=m.performMatching("SgForStatement(_,_,SgPlusPlusOp($I=SgVarRefExp)|SgMinusMinusOp($I=SgVarRefExp),..)",forloop);

    ofile<<res.size()<<endl;
    for(MatchResult::iterator i=res.begin();i!=res.end();++i) {
       SgVarRefExp* ivar = isSgVarRefExp( (*i)["$I"]);
       ofile<<"var:"<< ivar->unparseToString()<<endl;
    }
#endif    
    if (RAJA_Checker::enable_debug)
      cout<<"Failed condition: SageInterface::getLoopIndexVariable() returns NULL for loop:"<<forloop->get_file_info()->displayString()<<endl;
    return false;
  }

  if (RAJA_Checker::enable_debug)
    cout<<"\t Met conditions: for loop body, basic block body, valid loop index variable for loop:"<<forloop->get_file_info()->displayString()<<endl;

  return isNodalAccumulationBody (bb, lvar, fstmt, forloop);
}
/*
RAJA::forall< class RAJA::seq_exec  > (0,n, [=] (int i)
 {  .... });

The expected AST is 
SgExprStatement
* SgFunctionCallExp
*** SgExprListExp
**** SgIntVal: 0
**** SgVarRefExp: n
**** SgLambdaExp: 
***** SgLambdaCaptureList  
***** SgMemberFunctionDeclaration 
****** SgFunctionParameterList:  (int i) // loop index variable
****** SgFunctionDefinition
******* SgBasicBlock  // the lambda function body

 * */
bool RAJA_Checker::isEmbeddedNodalAccumulationLambda(SgLambdaExp* exp, SgExprStatement* & fstmt)
{
  ROSE_ASSERT (exp!=NULL);
  // this is the raja template function declaration!!
  SgFunctionDeclaration* raja_func = NULL;
  SgExprStatement* call_stmt = NULL;
  vector<string> wrappers; 
  wrappers.push_back("::for_all <.*");
  // use regular expression to describe all patterns
  wrappers.push_back("::for_all_.* <.*");
/*
  wrappers.push_back("::for_all_zones <");
  wrappers.push_back("::for_all_region_zones <");
  wrappers.push_back("::for_all_zones_tiled <");
  wrappers.push_back("::for_all_mixed_slots <");
  wrappers.push_back("::for_all_mixed_zones <");
*/
  if (!isRAJATemplateFunctionCallParameter (exp, call_stmt, raja_func)  && 
      !isWrapperTemplateFunctionCallParameter (exp, wrappers,  call_stmt, raja_func))
  {
    if (RAJA_Checker::enable_debug)
       cout<<"\tLambda exp is not a raja template function call parameter "<<endl;   
    return false;
  }
   
   if (raja_func ==NULL) 
   {
     if (RAJA_Checker::enable_debug)
       cout<<"\t raja template function declaration is NULL"<<endl;   
     return false;
   }

  return hasNodalAccumulationBody (exp, fstmt);
}

// In this pattern: the lambda function is declared first, then called by the RAJA function in the same scope
/*
Example simplest code:

auto kernel = [=](int i)
{
  x1[i] += rh1;
} ; 

forall(kernel);

The AST: 
  auto kernel ..;  is a variable declaration; rhs is a lambda expression.
  forall(kernel): is a function call with a parameter referencing kernel. 

The algorithm:
 
*/
bool RAJA_Checker::isIndirectNodalAccumulationLambda(SgLambdaExp* exp, SgExprStatement*& fstmt, SgExprStatement*& callStmt)
{ 
  if (enable_debug)
    cout<<"\t Entering isIndirectNodalAccumulationLambda()."<<endl;
  ROSE_ASSERT (exp);

  //1.  check if this is within auto kernel = ...; 
  SgStatement* stmt = SageInterface::getEnclosingStatement(exp);
  AstMatching m; 
  MatchResult r =m.performMatching ("$L=SgVariableDeclaration(null,SgInitializedName(SgAssignInitializer($R=SgLambdaExp)))", stmt);
  // must match exactly 1.
  if (r.size()!=1 ) 
  {
    if (enable_debug)
      cout<<"\t\t Match more than one variable decl within isIndirectNodalAccumulationLambda."<<endl;
    return false;
  }

  SgVariableDeclaration* decl = NULL;
  SgLambdaExp*  exp2 = NULL;
  for(MatchResult::iterator i=r.begin();i!=r.end();++i) {
    decl = isSgVariableDeclaration((*i)["$L"]);
    //cout<< AstTerm::astTermWithNullValuesToString(i1)<<endl;
    exp2 = isSgLambdaExp((*i)["$R"]);
  }

  if (exp !=exp2) 
  {
    if (enable_debug)
      cout<<"\t\t Cannot find auto kernel=lambda_exp; for lambda exp at line:"<< exp->get_file_info()->get_line()<<endl;
    return false;
  }
  // now we have found: auto kernel = lambda_exp; 

  //2. if kernel is called within RAJA::forall ()
  // find all next statements within the same scope
  SgStatement* nstmt= getNextNonNullStatement (decl);
  bool found = false; 
  while (nstmt != NULL)
  {
    RoseAst ast (nstmt); 
    for(RoseAst::iterator i=ast.begin();i!=ast.end();++i) {
      SgNode* n= (*i);
      if (SgVarRefExp* varRef = isSgVarRefExp(n))
      {
        if (varRef->get_symbol() == getFirstVarSym(decl))
        {
          // if this is a raja call's parameter ..
          SgFunctionDeclaration* raja_func = NULL;
          if (isRAJATemplateFunctionCallParameter (varRef, callStmt, raja_func))
          {
            found = true;
            break; 
          }
        } // symbol match
      }
    } // end for 

    if (found)
      break; 
    nstmt = getNextNonNullStatement (nstmt);
  } // end while

  if(!found)
  {
    if (enable_debug)
      cout<<"Cannot find RAJA::forall <> () using indirect reference to lambda exp at line:"<< exp->get_file_info()->get_line()<<endl;
    return false;
  }

  //3. if the kernel has the pattern
  return hasNodalAccumulationBody(exp, fstmt);
}

class RoseVisitor : public AstSimpleProcessing
{
  protected:
    void virtual visit ( SgNode* node);
};

void RoseVisitor::visit ( SgNode* n)
{
  // Only watch for Located nodes from input user source files.
  if (SgLocatedNode* lnode = isSgLocatedNode(n))
  {
    // skip compiler generated codes, mostly from template headers
    if (lnode->get_file_info()->isCompilerGenerated())
      return;

    
//-------------------  Nodal accumulation loop detection ----------------
    if (SgForStatement* forloop = isSgForStatement(lnode))
    {
      if (checkNodalAccumulationPattern)
      {
        SgExprStatement* fstmt = NULL; 

        if (RAJA_Checker::enable_debug)
            cout<<"Entering checkNodalAccumulationPattern for loop at line:" << forloop->get_file_info()->get_line()<<endl;
        if ( isNodalAccumulationLoop (forloop, fstmt))
        {
            ostringstream oss;
            oss<<"Found a nodal accumulation loop at line:"<< forloop->get_file_info()->get_line();
            oss<<" 1st acc. stmt at line:"<< fstmt->get_file_info()->get_line()<<endl;

            SgSourceFile* file = getEnclosingSourceFile(forloop);
            string s(":");
            string entry= forloop->get_file_info()->get_filename()+s+oss.str(); // add full filename to each log entries
            Rose::KeepGoing::File2StringMap[file]+= entry;
          if (RAJA_Checker::enable_debug)
          {
            ofile<<oss.str(); // don't use absolute file name for the output file, which is used for correctness checking
            cout<<forloop->get_file_info()->get_filename()+s+oss.str(); // also output to std out

          }
        }
        else // we also report negative findings for loops, to calculate true/false negatives
        {
          ostringstream oss;
          oss<<"Not a nodal accumulation loop at line:"<< forloop->get_file_info()->get_line()<<endl;

          SgSourceFile* file = getEnclosingSourceFile(forloop);
          string s(":");
          string entry= forloop->get_file_info()->get_filename()+s+oss.str(); // add full filename to each log entries
          Rose::KeepGoing::File2StringMap[file]+= entry;
          if (RAJA_Checker::enable_debug)
          {
            ofile<<oss.str(); // don't use absolute file name for the output file, which is used for correctness checking
            cout<<forloop->get_file_info()->get_filename()+s+oss.str(); // also output to std out

          }
        }
      }
    } // end if for loop

    // catch lambda expressions  
    if (n->variantT() == V_SgLambdaExp)
    {
      if ( SgLambdaExp* le = isSgLambdaExp(n))
      {
//----------- Check if the lambda expression is a parameter of RAJA function call with nodal accumulation pattern
        if (checkNodalAccumulationPattern)
        {
          if (RAJA_Checker::enable_debug)
             cout<<"Entering checking for Lambda Exp at line: "<<le->get_file_info()->get_line() <<endl;
          SgExprStatement* fstmt = NULL; 
          SgExprStatement* callstmt = NULL; 

          if ( isEmbeddedNodalAccumulationLambda(le, fstmt) || isIndirectNodalAccumulationLambda (le, fstmt, callstmt) )
          {
              ostringstream oss; 
              oss<<"Found a nodal accumulation lambda function at line:"<< le->get_file_info()->get_line();
              oss<<" 1st acc. stmt at line:"<< fstmt->get_file_info()->get_line()<<endl;
              if (callstmt)
                oss<<"\t This labmda function is used as a function parameter in a RAJA function is at line:"<< callstmt->get_file_info()->get_line()<<endl;

              SgSourceFile* file = getEnclosingSourceFile(le);
              string s(":");
              string entry= le->get_file_info()->get_filename()+s+oss.str(); // add full filename to each log entries.
              Rose::KeepGoing::File2StringMap[file]+= entry;
            if (RAJA_Checker::enable_debug)
            {
              ofile<<oss.str(); // don't use absolute file name for the output file, which is used for correctness checking
              cout<<le->get_file_info()->get_filename()+s+oss.str(); // also output to std out
            }
          }
          else // collect negative cases, calculating true/false negatives
          {
            ostringstream oss; 
            oss<<"Not a nodal accumulation lambda function at line:"<< le->get_file_info()->get_line()<<endl;
            SgSourceFile* file = getEnclosingSourceFile(le);
            string s(":");
            string entry= le->get_file_info()->get_filename()+s+oss.str(); // add full filename to each log entries.
            Rose::KeepGoing::File2StringMap[file]+= entry;
            if (RAJA_Checker::enable_debug)
            {
              ofile<<oss.str(); // don't use absolute file name for the output file, which is used for correctness checking
              cout<<le->get_file_info()->get_filename()+s+oss.str(); // also output to std out
            }
          }
        }

//----------- Check if the lambda expression is used as a parameter of RAJA function call
        SgFunctionDeclaration* raja_func = NULL; 
        SgExprStatement* call_stmt = NULL; 
        if (isRAJATemplateFunctionCallParameter (le, call_stmt, raja_func))
        {
          //cout<<"Found a lambda exp within RAJA func call ..."<<endl; 
          //le->get_file_info()->display();
          //cout<<"RAJA func name is:"<<
          //raja_func->get_name()<<endl;

          // scan the function body for data member accesses. 
          if (checkDataMember)
            warnDataMemberAccess (le->get_lambda_function()->get_definition());
        }
      }
    } // end if lambda exp

  } // end if located node
}



static void initDebugOutputFile(SgProject* project)
{
  SgFilePtrList fl = project->get_files();
  SgFile* firstfile = fl[0];
  ROSE_ASSERT (firstfile!=NULL);

  string filename = Rose::StringUtility::stripPathFromFileName (firstfile->getFileName());
  string ofilename = filename+".output";
  ofile.open(ofilename.c_str());
}


//---------------------------------------------------------------------------
int
main ( int argc, char* argv[])
{
  vector<string> argvList(argv, argv+argc);
  argvList = commandline_processing (argvList);

// check if the translator is running in -E mode, if yes, skip the work
  bool preprocessingOnly = false; 

   if (CommandlineProcessing::isOption (argvList,"-E","",false))
   {
     preprocessingOnly = true; 
     // we should not put debugging info here. Otherwise polluting the generated preprocessed file!!
   }

  SgProject* project = frontend(argvList);
  ROSE_ASSERT (project != NULL);

  if (preprocessingOnly)
     return backend(project);

  // register signal handling function: mark the label as midend. 
  // But it does not really differentiate when the signal happens! 
  if (KEEP_GOING_CAUGHT_MIDEND_SIGNAL)
  {
    std::cout
      << "[WARN] "
      << "Configured to keep going after catching a signal in rajaChecker"
      << std::endl; 
    Rose::KeepGoing::setMidendErrorCode (project, 100);                                                               
  }
  else
  {
    if (RAJA_Checker::enable_debug)
      initDebugOutputFile (project);
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

    if (RAJA_Checker::enable_debug)
      ofile.close();
  }
  int status =0; 
  // backend() may also cause trouble. we have to catch signals for it also.
  if (KEEP_GOING_CAUGHT_BACKEND_UNPARSER_SIGNAL)
  {
    std::cout
      << "[WARN] "
      << "Configured to keep going after catching a backend unparser's signal in rajaChecker"
      << std::endl; 
  }
  else
  { 
    // Report errors
    // For this analysis-only tool. 
    // Can we turn off backend unparsing and compilation. 
    // So the tool can process more files and generate more complete reports.
    // We cannot do this. Some build processes need *.o files. 
    status = backend(project);
    // important: MUST call backend() first, then generate reports.
    // otherwise, backend errors will not be caught by keep-going feature!!
  }

 // One problem: some files fail backend , but the analysis generates useful info.
  // How to output analysis info for them?
  // the report of failed files will contain the analysis results. 
  //TODO: would a single report file easier for users?

// We want the reports are generated with or without keep_going option
//  if (RAJA_Checker::keep_going)
  {
    std::vector<std::string> orig_rose_cmdline(argv, argv+argc);
    Rose::KeepGoing::generate_reports (project, orig_rose_cmdline);
  }  

  //return backend(project);
  return status;
}

