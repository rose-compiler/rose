// Liao, 12/8/2014
#include "ai_measurement.h"

//Array Annotation headers
#include <CPPAstInterface.h>
#include <ArrayAnnot.h>
#include <ArrayRewrite.h>

using namespace std;
using namespace SageInterface;
using namespace SageBuilder;
using namespace ArithemeticIntensityMeasurement; 

int local_loop_id=0; // unique loop id, at least for the context of each function
bool considerLambda = false; // if lambda function bodies should be included for estimation. 

//TODO: refactor into some RAJA handling namespace
//! Check if a statement is from RAJA headers, like installraja/include/RAJA/IndexSet.hxx 
bool insideRAJAHeader(SgLocatedNode* node)
{
  bool rtval = false;
  ROSE_ASSERT (node != NULL);
  Sg_File_Info* finfo = node->get_file_info();
  if (finfo!=NULL)
  {
    string fname = finfo->get_filenameString();
    string header_str1 = string("include/RAJA");
    // if the file name has a sys header path of either source or build tree
    if ((fname.find (header_str1, 0) != string::npos)
       )
      rtval = true;
  }
  return rtval;
}

//! Check if a node is a call statement to RAJA::forall (...), return bool and save function ref exp
bool isRAJAforallStmt(SgNode* n, SgFunctionRefExp** ref_exp_out, SgLambdaExp** lambda_exp_out)
{
  bool ret = false; 
  if (SgExprStatement* estmt = isSgExprStatement(n))
  {   
    if (SgFunctionCallExp* exp = isSgFunctionCallExp(estmt->get_expression()))
    {   
      if (SgFunctionRefExp* ref_exp = isSgFunctionRefExp (exp->get_function()))
      {   
        SgFunctionSymbol* fs = ref_exp->get_symbol_i();
        string func_name =fs->get_name().getString();
        bool hasRightName = (func_name.find("forall",0) ==0);
        SgScopeStatement* f_scope = fs->get_declaration()->get_scope();
        bool hasRightScope = false;
        if (SgNamespaceDefinitionStatement* def = isSgNamespaceDefinitionStatement(f_scope))
        {   
          SgNamespaceDeclarationStatement* decl = def->get_namespaceDeclaration();
          if (decl->get_name().getString() == "RAJA")
            hasRightScope = true;
        }   

        if (hasRightName && hasRightScope)
        {   
     //     cout<<"Debugging: Found a RAJA::forall function call!"<<endl;
     //     estmt->get_file_info()->display();
          if (ref_exp_out != NULL)
            *ref_exp_out = ref_exp;
          if (lambda_exp_out != NULL)
          { 
            SgExprListExp* parameters = exp -> get_args();
            SgExpressionPtrList explist = parameters->get_expressions();
            SgExpression* last_exp = explist[explist.size()-1];
            // Some weired code have a constructor in between. we have to use query to find lambdaExp
            //*lambda_exp_out = isSgLambdaExp(last_exp); // last parameter must to the lambda function
            vector <SgLambdaExp*> lvec = querySubTree<SgLambdaExp>(last_exp); //, V_SgLambdaExp);
            if (lvec.size()!= 1)
            {
              cerr<<"error: last expression of RAJA::forall() does not contain lambda exp SgLambdaExp. It is "<< last_exp->unparseToString() <<endl;
              exp->get_file_info()->display();
              ROSE_ASSERT (lvec.size() ==1);
            }
            *lambda_exp_out= lvec[0];
          }
          ret = true;
        }   
      }   
    }   
  } 
  return ret; 
}

bool processStatements(SgNode* n)
{
  ROSE_ASSERT (n!=NULL);
  // Skip compiler generated code, system headers, etc.
  if (isSgLocatedNode(n))
  {
    if (isSgLocatedNode(n)->get_file_info()->isCompilerGenerated())
      return false;
  }

  if (insideSystemHeader(isSgLocatedNode(n)) ||insideRAJAHeader(isSgLocatedNode(n)) )
    return false;

  // consider RAJA::forall loops, lambda functions only, estimate arithmetic intensity
  if (considerLambda)
  {
     // RAJA::forall template function calls
     SgFunctionRefExp* ref_exp = NULL;   
     SgLambdaExp* lambda_exp = NULL;
     if (isRAJAforallStmt (n, &ref_exp, &lambda_exp))
     {
       SgBasicBlock* body = lambda_exp->get_lambda_function()->get_definition()->get_body();
       ROSE_ASSERT (body != NULL);
       cout<<"Debugging: a RAJA::forall loop:"<< body->get_file_info()->get_line();
       FPCounters* fp_counters = calculateArithmeticIntensity(body);
       cout<<fp_counters->toString()<<endl;
     }
  }  
  else  // all other regular C/C++ loop cases 
  {
    // For C/C++ loops 
    if (isSgForStatement(n)!=NULL){
      SgForStatement* loop = isSgForStatement(n);
      SgScopeStatement* scope = loop->get_scope();
      ROSE_ASSERT(scope != NULL);

      if (running_mode == e_analysis_and_instrument)
        instrumentLoopForCounting (loop);
      else if (running_mode == e_static_counting)
      {  
        SgStatement* lbody = loop->get_loop_body();
//        CountFPOperations (lbody);
//        CountMemOperations (lbody , false, true); // bool includeScalars /*= true*/, bool includeIntType /*= true*/
//        FPCounters* fp_counters = getFPCounters (lbody);
        FPCounters* fp_counters = calculateArithmeticIntensity(lbody);

        ofstream reportFile(report_filename.c_str(), ios::app);
        //     cout<<"Writing counter results to "<< report_filename <<endl;
        reportFile<< fp_counters->toString();

        // verify the counting results are consistent with reference results from pragmas	 
        if (SgStatement* prev_stmt = getPreviousStatement(loop))
        {
          if (isSgPragmaDeclaration(prev_stmt))
          {
            FPCounters* ref_result = getFPCounters (prev_stmt);
            FPCounters* current_result = getFPCounters (lbody);
            if (ref_result != NULL)
            {
              if (!current_result->consistentWithReference (ref_result))
              {
                cerr<<"Error. Calculated FP operation counts differ from reference counts parsed from pragma!"<<endl;
                ref_result->printInfo("Reference counts are ....");
                current_result->printInfo("Calculated counts are ....");
              }
              assert (current_result->consistentWithReference (ref_result)); 
            }
            else
            {
              // I believe ref_result should be available at this point
              assert (false);
            }  
          }
        } // end verification
      } 
    }
    //TODO: merge this into the previous branch: parsing and verifying the same time.
    // Get reference FP operation counting values from pragma, if available.
    // This is no longer useful since we use bottomup traversal!!
    // We should split this into another phase!!
    else if (isSgPragmaDeclaration(n))
    {
      FPCounters* result = parse_aitool_pragma(isSgPragmaDeclaration(n));
      if (result != NULL)
      {
        isSgPragmaDeclaration(n) -> setAttribute("FPCounters", result);
        if (debug)
        {
          FPCounters* result2 = getFPCounters (isSgLocatedNode(n));
          result2->printInfo("After set and getFPCounters");
        }
      }
    }
    // Now Fortran support
    else if (SgFortranDo* doloop = isSgFortranDo(n))
    {
      instrumentLoopForCounting (doloop);
    }

  }
  return true;
}

//==============================================================================
int main (int argc, char** argv)
{
  // Build the AST used by ROSE
  vector <string> argvList (argv, argv + argc);
  // The command option to accept report file name
  string report_option="-report-file";

  if (CommandlineProcessing::isOption(argvList,"-help","", false))
  {
    cout<<"---------------------Tool-Specific Help-----------------------------------"<<endl;
    cout<<"This is a source analysis to estimate FLOPS and Load/store bytes for loops in your C/C++ or Fortran code."<<endl;
    cout<<"Usage: "<<argvList[0]<<" -c ["<<report_option<<" result.txt] "<< "input.c"<<endl;
    cout<<endl;
    cout<<"The optional "<<report_option<<" option is provided for users to specify where to save the results"<<endl;
    cout<<"By default, the results will be saved into a file named report.txt"<<endl;
    cout<<"  -static-counting-only  enable a static estimation of arithmetic intensity of loop bodies"<<endl;
    cout<<"  -debug                 enable a verbose debugging mode "<<endl;
    cout<<"  -support_raja_loops    support RAJA loops"<<endl;
    cout<<"  -report-file           specify a customized output file name, instead of the default file ai_tool_report.txt"<<endl;
    cout<<"  -annot  annot_file     accept user specified function side effect annotations"<<endl;
    cout<<"  -use-algorithm-v2      enable flow-sensitive counting of FP operations for static counting, still a new feature under development."<<endl;
    cout<<"Detailed instructions: https://en.wikibooks.org/wiki/ROSE_Compiler_Framework/Arithmetic_intensity_measuring_tool "<<endl;
    cout<<"----------------------Generic Help for ROSE tools--------------------------"<<endl;
  }

  if (CommandlineProcessing::isOption(argvList,"-static-counting-only","", true))
  {
    running_mode = e_static_counting; 
  }

  if (CommandlineProcessing::isOption(argvList,"-support_raja_loops","", true))
  {
    considerLambda = true;  // RAJA::forall () loops
    running_mode = e_static_counting; // additionally make sure we enter the static counting mode
    argvList.push_back ("-rose:Cxx11_only"); // ensure C++ 11 mode is turned on
  }


  if (CommandlineProcessing::isOption(argvList,"-use-algorithm-v2","", true))
  {
    algorithm_version = 2;
  }


  if (CommandlineProcessing::isOption(argvList,"-debug","", true))
  {
    debug = true; 
  }
  else 
    debug = false;

  if (CommandlineProcessing::isOptionWithParameter (argvList, report_option,"", report_filename,true))
  {
    if (debug)
      cout<<"Using user specified file: "<<report_filename<<" for storing results."<<endl;
  }
  else
  {
    //report_filename="ai_tool_report.txt"; // this is set in src/ai_measurement.cpp already
    if (debug)
      cout<<"Using the default file:"<<report_filename<<" for storing results."<<endl;
  }

  //Save -debugdep, -annot file .. etc, 
  // used internally in ReadAnnotation and Loop transformation
  CmdOptions::GetInstance()->SetOptions(argvList);
  bool dumpAnnot = CommandlineProcessing::isOption(argvList,"","-dumpannot",true);

  //Read in annotation files after -annot 
  ArrayAnnotation* annot = ArrayAnnotation::get_inst();
  annot->register_annot();
  ReadAnnotation::get_inst()->read();
  if (dumpAnnot)
    annot->Dump();
  //Strip off custom options and their values to enable backend compiler 
  CommandlineProcessing::removeArgsWithParameters(argvList,"-annot");

  SgProject* project = frontend(argvList);

  // Insert your own manipulations of the AST here...
  SgFilePtrList file_ptr_list = project->get_fileList();
  //visitorTraversal exampleTraversal;
  for (size_t i = 0; i<file_ptr_list.size(); i++)
  {
    SgFile* cur_file = file_ptr_list[i];
    SgSourceFile* s_file = isSgSourceFile(cur_file);
    if (s_file != NULL)
    {

      if ((running_mode ==  e_static_counting) && algorithm_version == 2)
      {
        OperationCountingTraversal oct;
        FPCounters returnAttribute = oct.traverseWithinFile(s_file);
        continue; // skip the rest loop iteration
      }

      //  Original V1 algorithm 
      // Preorder is not friendly for transformation
      //exampleTraversal.traverseWithinFile(s_file, postorder);
      Rose_STL_Container<SgNode*> nodeList = NodeQuery::querySubTree(s_file,V_SgStatement);
      if (running_mode == e_analysis_and_instrument) // reverse of pre-order for transformation mode
      {
        for (Rose_STL_Container<SgNode *>::reverse_iterator i = nodeList.rbegin(); i != nodeList.rend(); i++)
        {
          SgStatement *stmt= isSgStatement(*i);
          processStatements (stmt);
        }
      }
      else if (running_mode ==  e_static_counting) // pre-order traverse for analysis only mode 
      {
        for (Rose_STL_Container<SgNode *>::iterator i = nodeList.begin(); i != nodeList.end(); i++)
        {
          SgStatement *stmt= isSgStatement(*i);
          processStatements (stmt);
        }
      }
      else
      {
        cerr<<"Error. unrecognized execution mode:"<< running_mode<<endl;
        ROSE_ASSERT (false);
      }
    } // endif 
  } // end for

  // Generate source code from AST and invoke your
  // desired backend compiler
  return backend(project);
}

