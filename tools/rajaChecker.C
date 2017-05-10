// An analyzer for RAJA code, trying to report a set of properties
//
//
// Liao, 4/6/2017
#include "rose.h"
#include <iostream>
#include "keep_going.h"
#include <string>

#include <Sawyer/CommandLine.h>
static const char* purpose = "This tool detects various patterns in input source files.";
static const char* description =
        "This tool detects several patterns in C++ source files. Currently supported patterns are: "
        " Pattern 1: the Nodal Accumulation Pattern in C++ for loops or LLNL/RAJA loops.";

//        " Pattern 1: the use of class or structure data members in RAJA loops."
using namespace std;
using namespace SageInterface;

// used to store debugging output into a file
ofstream ofile; 

namespace RAJA_Checker 
{
  bool enable_debug = false;
  bool keep_going = false;

  // Not in use right now
  bool checkDataMember = false; 
  bool checkNodalAccumulationPattern = true;

  //! Processing command line options
  std::vector<std::string> commandline_processing(std::vector< std::string > & argvList);

  //! If a for loop is a nodal accumulation loop , return the recognized first accumulation statement
  bool isNodalAccumulationLoop(SgForStatement* forloop, SgExprStatement*& fstmt);

  //! Check if a lambda function is a nodal accumulation function
  bool isNodalAccumulationLambdaExp(SgLambdaExp* exp, SgExprStatement*& fstmt);

  //! Check if a block of statement has the nodal accumulation pattern, with a known loop index variable 
  bool isNodalAccumulationBody(SgBasicBlock* bb, SgInitializedName* lvar, SgExprStatement*& fstmt);

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

std::vector<std::string> RAJA_Checker::commandline_processing(std::vector< std::string > & argvList)
{
  using namespace Sawyer::CommandLine;                                                                                
  Parser p = CommandlineProcessing::createEmptyParserStage(purpose, description);                                     
  p.doc("Synopsis", "@prop{programName} @v{switches} @v{files}...");                                                  
  p.longPrefix("-");                                                                                                  

// initialize generic Sawyer switches: assertion, logging, threads, etc.                                              
  p.with(CommandlineProcessing::genericSwitches());                                                                   
                                                                                                                      
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

// Check if an operand is a form of x[index], x is a pointer to a double, index is a loop index
bool isDoubleArrayAccess (SgExpression* exp, SgInitializedName * lvar)
{
  ROSE_ASSERT (lvar != NULL);
  if (exp == NULL) return false;
  SgPntrArrRefExp* arr = isSgPntrArrRefExp (exp);
  if (arr == NULL)
    return false;

  SgExpression* lhs, *rhs;
  lhs = arr->get_lhs_operand();
  if (lhs == NULL) return false;
  rhs = arr->get_rhs_operand();
  if (rhs == NULL) return false;

  // lhs is a pointer type
  SgPointerType* ptype = isSgPointerType(lhs->get_type());
  if (!ptype)
    return false;
  // lhs is a pointer to double  
   if (! isSgTypeDouble( ptype->get_base_type()) )
    return false;
  
  // rhs is a loop index
  SgVarRefExp* varRef = isSgVarRefExp(rhs) ;
  if (varRef == NULL) return false;

  if (varRef->get_symbol() != lvar->get_symbol_from_symbol_table ())
    return false;
  
  return true; 
}

//  accum-op:  +=, -=, *=, /=, MIN (), MAX, ..
bool isNodalAccumulationOp (SgExpression* op)
{
  if (op == NULL) return false;

  if (isSgPlusAssignOp(op) ||
      isSgMinusAssignOp(op) ||
      isSgMultAssignOp(op) ||
      isSgDivAssignOp(op)  // TODO: MIN() and MAX ()
     )
    return true;

  return false;
}


/*
 * xa4[i] += ax; within a for-loop, i is loop index
 *  lhs: array element access using loop index  xm[loop_index], xm is a pointer to a double type
 *  accum-op:  +=, -=, *=, /=, MIN (), MAX, ..
 *
 * */
bool isNodalAccumulationStmt (SgStatement* s, SgInitializedName* lvar)
{
  ROSE_ASSERT (lvar != NULL);
   if (s==NULL) return false;
   SgExprStatement* es = isSgExprStatement (s);
   if (es==NULL) return false;

   SgExpression* exp = es->get_expression();
   if (exp==NULL)
     return false;
   
   if (! isNodalAccumulationOp (exp) )
     return false;

   SgBinaryOp* bop = isSgBinaryOp (exp);
   ROSE_ASSERT (bop != NULL);

   // lhs is x[i]
   if (!isDoubleArrayAccess(bop->get_lhs_operand(), lvar))
     return false;
  
    // rhs is a scalar type
   if (!SageInterface::isScalarType (bop->get_rhs_operand()->get_type())) 
     return false;

   // rhs is a double type
   if (!isSgTypeDouble(bop->get_rhs_operand()->get_type())) 
     return false;
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
bool RAJA_Checker::isNodalAccumulationBody(SgBasicBlock* bb, SgInitializedName* lvar, SgExprStatement*& fstmt)
{
  ROSE_ASSERT (bb != NULL);
  ROSE_ASSERT (lvar != NULL);
  //if the body contains at least 4 nodal accumulation statement in a row, then it is a matched loop
  // Find all expression statements. if there is one Nodal Accumulation Statement (NAS) , and it is followed by 3 other NAS.
  // then there is a match. 
  Rose_STL_Container<SgNode*> stmtList = NodeQuery::querySubTree(bb, V_SgExprStatement);
  for (Rose_STL_Container<SgNode*>::iterator iter = stmtList.begin(); iter != stmtList.end(); iter++)
  {
    SgExprStatement* s = isSgExprStatement(*iter);
    if (isNodalAccumulationStmt (s, lvar))
    {
      SgStatement* s2, *s3, *s4; 
      s2= getNextNonNullStatement(s);
      if (s2!=NULL && isNodalAccumulationStmt (s2, lvar))
      {
        s3 = getNextNonNullStatement (s2);
        if (s3!=NULL && isNodalAccumulationStmt (s3, lvar))
        {
           s4 = getNextNonNullStatement (s3);
           if (s4 != NULL && isNodalAccumulationStmt (s4, lvar))
           { 
             fstmt = s; 
             return true;
           } // end s4
        } //end s3
      } // end s2
    } // end s1 
  } // end for loop  
  
  return false;
}

bool RAJA_Checker::isNodalAccumulationLoop(SgForStatement* forloop, SgExprStatement*& fstmt)
{
  SgStatement* body = forloop->get_loop_body();
  if (body == NULL) return false;

  SgBasicBlock* bb = isSgBasicBlock(body);
  if (bb == NULL) return false;

  SgInitializedName* lvar = SageInterface::getLoopIndexVariable (forloop);
  if (lvar ==NULL)
  {
    if (RAJA_Checker::enable_debug)
    {
      cerr<<"Warning: SageInterface::getLoopIndexVariable() returns NULL for loop:"<<forloop->get_file_info()->displayString()<<endl;
    }
    return false;
  }

  return isNodalAccumulationBody (bb, lvar, fstmt);
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
bool RAJA_Checker::isNodalAccumulationLambdaExp(SgLambdaExp* exp, SgExprStatement* & fstmt)
{
  ROSE_ASSERT (exp!=NULL);
  // this is the raja template function declaration!!
  SgFunctionDeclaration* raja_func = NULL;
  if (!isRAJATemplateFunctionCallParameter (exp, & raja_func))
    return false;
   
   if (raja_func ==NULL) return false;

   // ROSE uses a anonymous class declaration for lambda expression. Its function is a member function. 
   SgMemberFunctionDeclaration* lfunc = isSgMemberFunctionDeclaration( exp->get_lambda_function());
   if (lfunc ==NULL) return false;

   // loop variable is modeled as the first parameter of the lambda function's parameter list
   SgFunctionParameterList* plist = lfunc->get_parameterList();
   if (plist ==NULL) return false;

   SgInitializedName* lvar= (plist->get_args())[0];
   if (lvar ==NULL) return false;

   SgFunctionDefinition* def = lfunc->get_definition();
   if (def==NULL) return false;
   SgBasicBlock* bb = def->get_body();
   if (bb ==NULL) return false;

  return isNodalAccumulationBody (bb, lvar, fstmt);
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
        if ( isNodalAccumulationLoop (forloop, fstmt))
        {
            ostringstream oss;
            oss<<"Found a nodal accumulation loop at line:"<< forloop->get_file_info()->get_line()<<endl;
            oss<<"\t The first accumulation statement is at line:"<< fstmt->get_file_info()->get_line()<<endl;

            SgSourceFile* file = getEnclosingSourceFile(forloop);
            Rose::KeepGoing::File2StringMap[file]+=oss.str();
          if (RAJA_Checker::enable_debug)
          {
            ofile<<oss.str();
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
          SgExprStatement* fstmt = NULL; 
          if ( isNodalAccumulationLambdaExp(le, fstmt))
          {
              ostringstream oss; 
              oss<<"Found a nodal accumulation lambda function at line:"<< le->get_file_info()->get_line()<<endl;
              oss<<"\t The first accumulation statement is at line:"<< fstmt->get_file_info()->get_line()<<endl;

              SgSourceFile* file = getEnclosingSourceFile(le);
              Rose::KeepGoing::File2StringMap[file]+=oss.str();
            if (RAJA_Checker::enable_debug)
            {
              ofile<<oss.str();
            }
          }
        }


//----------- Check if the lambda expression is used as a parameter of RAJA function call
        SgFunctionDeclaration* raja_func = NULL; 
        if (isRAJATemplateFunctionCallParameter (le, & raja_func))
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

  string filename = rose::StringUtility::stripPathFromFileName (firstfile->getFileName());
  string ofilename = filename+".output";
  ofile.open(ofilename.c_str());
}


//---------------------------------------------------------------------------
int
main ( int argc, char* argv[])
{
  vector<string> argvList(argv, argv+argc);
  argvList = commandline_processing (argvList);

  SgProject* project = frontend(argvList);
  ROSE_ASSERT (project != NULL);

  // register midend signal handling function                                                                         
  if (KEEP_GOING_CAUGHT_MIDEND_SIGNAL)                                                                                
  {                                                                                                                   
    std::cout                                                                                                         
      << "[WARN] "                                                                                                    
      << "Configured to keep going after catching a "                                                                 
      << "signal in AutoPar"                                                                                          
      << std::endl;                                                                                                   
    Rose::KeepGoing::setMidendErrorCode (project, 100);                                                               
    goto label_end;                                                                                                   
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

label_end:
  // Report errors
  // For this analysis-only tool. 
  // Can we turn off backend unparsing and compilation. 
  // So the tool can process more files and generate more complete reports.
  // We cannot do this. Some build processes need *.o files. 
  int status = backend(project);
 // important: MUST call backend() first, then generate reports.
 // otherwise, backend errors will not be caught by keep-going feature!!

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

