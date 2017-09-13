/*
 * Automatic Parallelization using OpenMP 
 *
 * Input: sequential C/C++ code
 * Output: parallelized C/C++ code using OpenMP
 *
 * Algorithm:
 *   Read in semantics specification (formerly array abstraction) files 
 *   Collect all loops with canonical forms
 *     x. Conduct loop normalization
 *     x. Call dependence analysis from Qing's loop transformations
 *     x. Conduct liveness analysis and variable classification
 *     x. Judge if the loop is parallelizable
 *     x. Attach OmpAttribute if it is
 *     x. Insert OpenMP pragma accordingly
 *
 * By Chunhua Liao
 * Nov 3, 2008
 */
#include "rose.h"
#include "keep_going.h" // enable logging files which cannot be processed by AutoPar due to various reasons
// all kinds of analyses needed
#include "autoParSupport.h" 
#include <string> 
#include <Sawyer/CommandLine.h>
static const char* purpose = "This tool automatically inserts OpenMP directives into sequential codes.";
static const char* description =
     "This tool is an implementation of automatic parallelization using OpenMP. "
     "It can automatically insert OpenMP directives into input serial C/C++ codes. "
     "For input programs with existing OpenMP directives, the tool will double check "
     "the correctness when requested.";

using namespace std;
using namespace AutoParallelization;
using namespace SageInterface;

void findCandidateFunctionDefinitions (SgProject* project, std::vector<SgFunctionDefinition* >& candidateFuncDefs)
{
  ROSE_ASSERT (project != NULL);
  // For each source file in the project
  SgFilePtrList & ptr_list = project->get_fileList();
  for (SgFilePtrList::iterator iter = ptr_list.begin(); iter!=ptr_list.end();
      iter++)
  {
    SgFile* sageFile = (*iter);
    SgSourceFile * sfile = isSgSourceFile(sageFile);
    ROSE_ASSERT(sfile);
//    SgGlobal *root = sfile->get_globalScope();

    if (enable_debug)
      cout<<"Processing each function within the files "<< sfile->get_file_info()->get_filename() <<endl;
    //      cout<<"\t loop at:"<< cur_loop->get_file_info()->get_line() <<endl;

    // This is wrong, many functions in question are not top level declarations!!
    //SgDeclarationStatementPtrList& declList = root->get_declarations ();
    //VariantVector vv;
    Rose_STL_Container<SgNode*> defList = NodeQuery::querySubTree(sfile, V_SgFunctionDefinition); 
//    bool hasOpenMP= false; // flag to indicate if omp.h is needed in this file

    //For each function body in the scope
    //for (SgDeclarationStatementPtrList::iterator p = declList.begin(); p != declList.end(); ++p) 
    for (Rose_STL_Container<SgNode*>::iterator p = defList.begin(); p != defList.end(); ++p) 
    {
      SgFunctionDefinition *defn = isSgFunctionDefinition(*p);
      ROSE_ASSERT (defn != NULL);

      SgFunctionDeclaration *func = defn->get_declaration();
      ROSE_ASSERT (func != NULL);

      if (enable_debug)
        cout<<"\t considering function "<< func->get_name() << " at "<< func->get_file_info()->get_line()<<endl;
      //ignore functions in system headers, Can keep them to test robustness
      if (defn->get_file_info()->get_filename()!=sageFile->get_file_info()->get_filename())
      {
        if (enable_debug)
          cout<<"\t Skipped since the function's associated file name does not match current file being considered. Mostly from a header. "<<endl;
        continue;
      }
      candidateFuncDefs.push_back(defn);
    } // end for def list
  } // end for file list
}

// normalize all loops within candidate function definitions
void normalizeLoops (std::vector<SgFunctionDefinition* > candidateFuncDefs)
{
  for (std::vector<SgFunctionDefinition* >::iterator iter = candidateFuncDefs.begin(); iter != candidateFuncDefs.end(); iter++)
  {
    SgFunctionDefinition* funcDef = *iter; 
    ROSE_ASSERT (funcDef);
    // This has to happen before analyses are called.
    // For each loop 
    VariantVector vv (V_SgForStatement); 
    Rose_STL_Container<SgNode*> loops = NodeQuery::querySubTree(funcDef, vv); 

    if (enable_debug)
      cout<<"Normalize loops queried from memory pool ...."<<endl;

    // normalize C99 style for (int i= x, ...) to C89 style: int i;  (i=x, ...)
    // Liao, 10/22/2009. Thank Jeff Keasler for spotting this bug
    for (Rose_STL_Container<SgNode*>::iterator iter = loops.begin();
        iter!= loops.end(); iter++ )
    {
      SgForStatement* cur_loop = isSgForStatement(*iter);
      ROSE_ASSERT(cur_loop);

      if (enable_debug)
        cout<<"\t loop at:"<< cur_loop->get_file_info()->get_line() <<endl;
      // skip for (;;) , SgForStatement::get_test_expr() has a buggy assertion.
      SgStatement* test_stmt = cur_loop->get_test();
      if (test_stmt!=NULL && 
          isSgNullStatement(test_stmt))
      {
        if (enable_debug)
          cout<<"\t skipped due to empty loop header like for (;;)"<<endl;
        continue;
      }

      // skip system header
      if (insideSystemHeader (cur_loop) )
      {
        if (enable_debug)
          cout<<"\t skipped since the loop is inside a system header "<<endl;
        continue; 
      }
#if 0 // we now always normalize loops, then later undo some normalization 6/22/2016
      // SageInterface::normalizeForLoopInitDeclaration(cur_loop);
      if (keep_c99_loop_init) 
      {
        // 2/29/2016, disable for loop init declaration normalization
        // This is not used . No longer used.
        normalizeForLoopTest(cur_loop);
        normalizeForLoopIncrement(cur_loop);
        ensureBasicBlockAsBodyOfFor(cur_loop);
        constantFolding(cur_loop->get_test());
        constantFolding(cur_loop->get_increment());
      }
      else
#endif
        SageInterface::forLoopNormalization(cur_loop);
    } // end for all loops
  } // end for all function defs 
  
}

//! Initialize the switch group and its switches.
Sawyer::CommandLine::SwitchGroup commandLineSwitches() {
  using namespace Sawyer::CommandLine;


  // Default log files for keep_going option
  Rose::KeepGoing::report_filename__fail = "autoPar-failed-files.txt";
  Rose::KeepGoing::report_filename__pass = "autoPar-passed-files.txt";


  SwitchGroup switches("autoPar's switches");
  switches.doc("These switches control the autoPar tool. ");
  switches.name("rose:autopar"); 

  switches.insert(Switch("enable_debug")
      .intrinsicValue(true, AutoParallelization::enable_debug)
      .doc("Enable the debugging mode."));

  // Keep going option of autoPar
  switches.insert(Switch("keep_going")
      .intrinsicValue(true, AutoParallelization::keep_going)
      .doc("Allow auto parallelization to keep going if errors happen"));

  switches.insert(Switch("failure_report")
      .argument("string", anyParser(Rose::KeepGoing::report_filename__fail))
      .doc("Specify the report file for logging files autoPar cannot process"));

  switches.insert(Switch("success_report")
      .argument("string", anyParser(Rose::KeepGoing::report_filename__pass))
      .doc("Specify the report file for logging files autoPar can process"));

  switches.insert(Switch("enable_patch")
      .intrinsicValue(true, AutoParallelization::enable_patch)
      .doc("Enable generating patch files for auto parallelization"));

  switches.insert(Switch("no_aliasing")
      .intrinsicValue(true, AutoParallelization::no_aliasing)
      .doc("Assuming no pointer aliasing exists."));

  switches.insert(Switch("unique_indirect_index")
      .intrinsicValue(true, AutoParallelization::b_unique_indirect_index)
      .doc("Assuming all arrays used as indirect indices have unique elements (no overlapping)"));

  switches.insert(Switch("enable_diff")   
      .intrinsicValue(true, AutoParallelization::enable_diff)
      .doc("Compare user defined OpenMP pragmas to auto parallelization generated ones."));

  switches.insert(Switch("enable_distance")
      .intrinsicValue(true, AutoParallelization::enable_distance)
      .doc("Report the absolute dependence distance of a dependence relation preventing parallelization."));

  switches.insert(Switch("annot")
      .argument("string", anyParser(AutoParallelization::annot_filenames))
//      .shortPrefix("-") // this option allows short prefix
      .whichValue(SAVE_ALL)               // if switch appears more than once, save all values not just last
      .doc("Specify annotation file for semantics of abstractions"));

  switches.insert(Switch("dumpannot")
      .intrinsicValue(true, AutoParallelization::dump_annot_file)
      .doc("Dump annotation file content for debugging purposes."));

  return switches;
}

// New version of command line processing using Sawyer library
static std::vector<std::string> commandline_processing(std::vector< std::string > & argvList)
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

  // add back -annot file TODO: how about multiple appearances?
  for (size_t i=0; i<AutoParallelization::annot_filenames.size(); i++)
  {
    remainingArgs.push_back("-annot");
    remainingArgs.push_back(AutoParallelization::annot_filenames[i]);
  }

  if (AutoParallelization::keep_going)
    remainingArgs.push_back("-rose:keep_going");

// AFTER parse the command-line, you can do this:
 if (showRoseHelp)
    SgFile::usage(0);

 // work with the parser of the ArrayAbstraction module
 //Read in annotation files after -annot 
 CmdOptions::GetInstance()->SetOptions(remainingArgs);
 ArrayAnnotation* annot = ArrayAnnotation::get_inst();
 annot->register_annot();
 ReadAnnotation::get_inst()->read();
 if (AutoParallelization::dump_annot_file)  
   annot->Dump();

 //Strip off custom options and their values to enable backend compiler 
 CommandlineProcessing::removeArgsWithParameters(remainingArgs,"-annot");      

#if 0 // DEBUGGING [Robb P Matzke 2016-09-27]
  std::cerr <<"These are the arguments after parsing with Sawyer:\n";
  BOOST_FOREACH (const std::string &s, remainingArgs)
    std::cerr <<"    \"" <<s <<"\"\n";
#endif

  return remainingArgs;
}

int
main (int argc, char *argv[])
{
  vector<string> argvList(argv, argv+argc);
  //Processing debugging and annotation options
  //  autopar_command_processing(argvList);
  argvList = commandline_processing (argvList);
  // enable parsing user-defined pragma if enable_diff is true
  // -rose:openmp:parse_only
  if (enable_diff)
    argvList.push_back("-rose:openmp:parse_only");
  SgProject *project = frontend (argvList);
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

  // create a block to avoid jump crosses initialization of candidateFuncDefs etc.
  {                             
    std::vector<SgFunctionDefinition* > candidateFuncDefs; 
    findCandidateFunctionDefinitions (project, candidateFuncDefs);
    normalizeLoops (candidateFuncDefs);

    //Prepare liveness analysis etc.
    //Too much output for analysis debugging info.
    //initialize_analysis (project,enable_debug);   
    initialize_analysis (project, false);   

    // This is a bit redundant with findCandidateFunctionDefinitions ()
    // But we do need the per file control to decide if omp.h is needed for each file
    //
    // For each source file in the project
    SgFilePtrList & ptr_list = project->get_fileList();
    for (SgFilePtrList::iterator iter = ptr_list.begin(); iter!=ptr_list.end();
        iter++)
    {
      SgFile* sageFile = (*iter);
      SgSourceFile * sfile = isSgSourceFile(sageFile);
      ROSE_ASSERT(sfile);
      SgGlobal *root = sfile->get_globalScope();

      Rose_STL_Container<SgNode*> defList = NodeQuery::querySubTree(sfile, V_SgFunctionDefinition); 
      bool hasOpenMP= false; // flag to indicate if there is at least one loop is parallelized. also if omp.h is needed in this file

      //For each function body in the scope
      //for (SgDeclarationStatementPtrList::iterator p = declList.begin(); p != declList.end(); ++p) 
      for (Rose_STL_Container<SgNode*>::iterator p = defList.begin(); p != defList.end(); ++p) 
      {

        //      cout<<"\t loop at:"<< cur_loop->get_file_info()->get_line() <<endl;

        SgFunctionDefinition *defn = isSgFunctionDefinition(*p);
        ROSE_ASSERT (defn != NULL);

        SgFunctionDeclaration *func = defn->get_declaration();
        ROSE_ASSERT (func != NULL);

        //ignore functions in system headers, Can keep them to test robustness
        if (defn->get_file_info()->get_filename()!=sageFile->get_file_info()->get_filename())
        {
          continue;
        }

        SgBasicBlock *body = defn->get_body();  
        // For each loop 
        Rose_STL_Container<SgNode*> loops = NodeQuery::querySubTree(defn,V_SgForStatement); 
        if (loops.size()==0) 
        {
          if (enable_debug)
            cout<<"\t skipped since no for loops are found in this function"<<endl;
          continue;
        }

#if 0 // Moved to be executed before running liveness analysis.
        // normalize C99 style for (int i= x, ...) to C89 style: int i;  (i=x, ...)
        // Liao, 10/22/2009. Thank Jeff Keasler for spotting this bug
        for (Rose_STL_Container<SgNode*>::iterator iter = loops.begin();
            iter!= loops.end(); iter++ )
        {
          SgForStatement* cur_loop = isSgForStatement(*iter);
          ROSE_ASSERT(cur_loop);
          SageInterface::normalizeForLoopInitDeclaration(cur_loop);
        }
#endif
        // X. Replace operators with their equivalent counterparts defined 
        // in "inline" annotations
        AstInterfaceImpl faImpl_1(body);
        CPPAstInterface fa_body(&faImpl_1);
        OperatorInlineRewrite()( fa_body, AstNodePtrImpl(body));

        // Pass annotations to arrayInterface and use them to collect 
        // alias info. function info etc.  
        ArrayAnnotation* annot = ArrayAnnotation::get_inst(); 
        ArrayInterface array_interface(*annot);
        // alias Collect 
        // value collect
        array_interface.initialize(fa_body, AstNodePtrImpl(defn));
        // valueCollect
        array_interface.observe(fa_body);

        //FR(06/07/2011): aliasinfo was not set which caused segfault
        LoopTransformInterface::set_aliasInfo(&array_interface);

        for (Rose_STL_Container<SgNode*>::iterator iter = loops.begin(); 
            iter!= loops.end(); iter++ ) 
        {
          SgNode* current_loop = *iter;

          if (enable_debug)
          {
            SgForStatement * fl = isSgForStatement(current_loop);
            cout<<"\t\t Considering loop at "<< fl->get_file_info()->get_line()<<endl;
          }
          //X. Parallelize loop one by one
          // getLoopInvariant() will actually check if the loop has canonical forms 
          // which can be handled by dependence analysis
          SgInitializedName* invarname = getLoopInvariant(current_loop);
          if (invarname != NULL)
          {
            bool ret = ParallelizeOutermostLoop(current_loop, &array_interface, annot);
            if (ret) // if at least one loop is parallelized, we set hasOpenMP to be true for the entire file.
              hasOpenMP = true;  
          }
          else // cannot grab loop index from a non-conforming loop, skip parallelization
          {
            if (enable_debug)
              cout<<"Skipping a non-canonical loop at line:"<<current_loop->get_file_info()->get_line()<<"..."<<endl;
            // We should not reset it to false. The last loop may not be parallelizable. But a previous one may be.  
            //hasOpenMP = false;
          }
        }// end for loops
      } // end for-loop for declarations

      // insert omp.h if needed
      if (hasOpenMP && !enable_diff)
      {
        SageInterface::insertHeader("omp.h",PreprocessingInfo::after,false,root);
        if (enable_patch)
          generatePatchFile(sfile); 
      }
      // compare user-defined and compiler-generated OmpAttributes
      if (enable_diff)
        diffUserDefinedAndCompilerGeneratedOpenMP(sfile); 
    } //end for-loop of files

#if 1
    // undo loop normalization
    std::map <SgForStatement* , bool >::iterator iter = trans_records.forLoopInitNormalizationTable.begin();
    for (; iter!= trans_records.forLoopInitNormalizationTable.end(); iter ++) 
    {
      SgForStatement* for_loop = (*iter).first; 
      unnormalizeForLoopInitDeclaration (for_loop);
    }
#endif
    // Qing's loop normalization is not robust enough to pass all tests
    //AstTests::runAllTests(project);


    // clean up resources for analyses
    release_analysis();
  }

label_end: 
  // Report errors
  int status = backend (project);
  if (keep_going)
  {
    std::vector<std::string> orig_rose_cmdline(argv, argv+argc);
    Rose::KeepGoing::generate_reports (project, orig_rose_cmdline);
  }

  //project->unparse();
  //return backend (project);
  return status; 

}
