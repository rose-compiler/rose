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

bool processStatements(SgNode* n)
{
  ROSE_ASSERT (n!=NULL);
  // Skip compiler generated code, system headers, etc.
  if (isSgLocatedNode(n))
  {
    if (isSgLocatedNode(n)->get_file_info()->isCompilerGenerated())
      return false;
  }

  // For C/C++ loops 
  if (isSgForStatement(n)!=NULL){
    SgForStatement* loop = isSgForStatement(n);
    SgScopeStatement* scope = loop->get_scope();
    ROSE_ASSERT(scope != NULL);

   instrumentLoopForCounting (loop);
    // verify the counting results are consistent with reference results from pragmas	 
    if (SgStatement* prev_stmt = getPreviousStatement(loop))
    {
      if (isSgPragmaDeclaration(prev_stmt))
      {
        FPCounters* ref_result = getFPCounters (prev_stmt);
        FPCounters* current_result = getFPCounters (loop);
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
  return true;
}

//==============================================================================
int main (int argc, char** argv)
{
  // Build the AST used by ROSE
  vector <string> argvList (argv, argv + argc);

  debug = true; // debugging mode is on by default for now
  if (CommandlineProcessing::isOption(argvList,"--help","", false))
  {
    cout<<"---------------------Tool-Specific Help-----------------------------------"<<endl;
    cout<<"This is a source analysis to estimate FLOPS for loops in your C/C++ code."<<endl;
    cout<<"Usage: "<<argvList[0]<<" -c ["<<report_option<<" result.txt] "<< "input.c"<<endl;
    cout<<endl;
    cout<<"The optional "<<report_option<<" option is provided for users to specify where to save the results"<<endl;
    cout<<"By default, the results will be saved into a file named report.txt"<<endl;
    cout<<"----------------------Generic Help for ROSE tools--------------------------"<<endl;
  }

  if (CommandlineProcessing::isOptionWithParameter (argvList, report_option,"", report_filename,true))
  {
    cout<<"Using user specified file: "<<report_filename<<" for storing results."<<endl;
  }
//  else
//    cout<<"Using the default file:"<<report_filename<<" for storing results."<<endl;

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
      // Preorder is not friendly for transformation
      //exampleTraversal.traverseWithinFile(s_file, postorder);
      Rose_STL_Container<SgNode*> nodeList = NodeQuery::querySubTree(s_file,V_SgStatement);
      for (Rose_STL_Container<SgNode *>::reverse_iterator i = nodeList.rbegin(); i != nodeList.rend(); i++)
      {
        SgStatement *stmt= isSgStatement(*i);
        processStatements (stmt);
      }
    }
  }

  // Generate source code from AST and invoke your
  // desired backend compiler
  return backend(project);
}

