// Example ROSE Translator reads input program and implements a DSL embedded within C++
// to support the stencil computations, and required runtime support is developed separately.
#include "rose.h"
#include "ompAstConstruction.h"

#include "stencilAndStencilOperatorDetection.h"
#include "stencilEvaluation.h"

#include "shiftCalculusCompiler.h"

#include "dslCodeGeneration.h"

#include "VariableIdMapping.h"

#define DEBUG_USING_DOT_GRAPHS 1

using namespace SPRAY; 
using namespace SageInterface; 
using namespace SageBuilder; 

VariableIdMapping variableIdMapping;


int main( int argc, char * argv[] )
   {
  // If we want this translator to take specific options (beyond those defined 
  // by ROSE) then insert command line processing for new options here.

  // To better support the stencil specification that might benifit from constant 
  // folding, I have turned this ON is hte frontend.  By default it is OFF so that
  // we can preserve source code as much as possible (original expression trees).
  // The Stencil DSL can be made to work in eithr setting, but this make sure that
  // dimension dependent processing of the stencil coeficients will be evaluated 
  // to constants.  I will turn this off (and thus use a less blunt axe) when the new
  // constant expression evaluation in ROSE is fixed to support more general types
  // than integer expresion (should be done by JP later today).
  // bool frontendConstantFolding = true;
     bool frontendConstantFolding = false;

// Liao, support a flag to control if CUDA code should be generated
// ./shiftCalculusCompiler -rose:dslcompiler:cuda  -c input_file
    std::vector <std::string> argvList (argv, argv + argc);
    if (CommandlineProcessing::isOption (argvList,"-rose:dslcompiler:","cuda",true))
    {
      std::cout<<"Turning on CUDA code generation ..."<<std::endl;
      b_gen_cuda = true;
      // avoid invoking the built in lowering, just create AST
      //argvList.push_back("-rose:openmp:ast_only");
      //argvList.push_back("-rose:openmp:lowering");
      //OmpSupport::enable_accelerator = true;
    }
    else
      b_gen_cuda = false;
// Pei-Hung, enable loop collapsing
    if (CommandlineProcessing::isOption (argvList,"-rose:dslcompiler:","collapse",true))
    {
      std::cout<<"Turning on OpenMP loop collapsing ..."<<std::endl;
      b_enable_collapse = true;
//      argvList.push_back("-rose:openmp:lowering");
    }
    else
      b_enable_collapse = false;
// Pei-Hung, code generation to fulfill polyopt 
    if (CommandlineProcessing::isOption (argvList,"-rose:dslcompiler:","polyopt",true))
    {
      std::cout<<"Generating code for PolyOpt  ..."<<std::endl;
      b_enable_polyopt = true;
//      argvList.push_back("-rose:openmp:lowering");
    }
    else
      b_enable_polyopt = false;

// Pei-Hung, code generation to vectorization 
    if (CommandlineProcessing::isOption (argvList,"-rose:dslcompiler:","vectorization",true))
    {
      std::cout<<"Generating code for vectorization  ..."<<std::endl;
      b_gen_vectorization = true;
//      argvList.push_back("-rose:openmp:lowering");
    }
    else
      b_gen_vectorization = false;
// If MPI code generation is turned on
    if (CommandlineProcessing::isOption (argvList,"-rose:dslcompiler:","mpi",true))
    {
      std::cout<<"Turning on MPI code generation ..."<<std::endl;
      b_gen_mpi = true;
//      argvList.push_back("-rose:openmp:lowering");
    }
    else
      b_gen_mpi = false;

      
  // Generate the ROSE AST.
     //SgProject* project = frontend(argc,argv,frontendConstantFolding);
     SgProject* project = frontend(argvList,frontendConstantFolding);
     ROSE_ASSERT(project != NULL);

     try
        {
          variableIdMapping.computeVariableSymbolMapping(project);
        }
     catch(char* str)
        {
          cout << "*Exception raised: " << str << endl;
        } 
     catch(const char* str) 
        {
          cout << "Exception raised: " << str << endl;
        } 
     catch(string str)
        {
          cout << "Exception raised: " << str << endl;
        }

  // variableIdMapping.toStream(cout);

#if 1
     printf ("variableIdMapping.getVariableIdSet().size() = %zu \n",variableIdMapping.getVariableIdSet().size());
     ROSE_ASSERT(variableIdMapping.getVariableIdSet().size() > 0);
#endif

#if 0
     printf ("Exiting as a test after calling variableIdMapping.computeVariableSymbolMapping(project) \n");
     ROSE_ASSERT(false);
#endif

#if 0
     printf ("Calling constant folding \n");
     ConstantFolding::constantFoldingOptimization(project,false);

#if 0
     printf ("Exiting as a test after calling ConstantFolding::constantFoldingOptimization() \n");
     ROSE_ASSERT(false);
#endif
#endif

  // DQ (2/8/2015): Find the associated SgFile so we can restrict processing to the current file.
     ROSE_ASSERT(project->get_fileList().empty() == false);
     SgFile* firstFile = project->get_fileList()[0];
     ROSE_ASSERT(firstFile != NULL);

#if DEBUG_USING_DOT_GRAPHS
  // generateDOTforMultipleFile(*project);
  // generateDOT(*project,"_before_transformation");
     AstDOTGeneration astdotgen;
     astdotgen.generateWithinFile(firstFile,DOTGeneration<SgNode*>::TOPDOWNBOTTOMUP,"_before_transformation");
#endif
#if DEBUG_USING_DOT_GRAPHS && 1
     const int MAX_NUMBER_OF_IR_NODES_TO_GRAPH_FOR_WHOLE_GRAPH = 12000;
#endif
#if DEBUG_USING_DOT_GRAPHS && 1
  // Output an optional graph of the AST (the whole graph, of bounded complexity, when active)
     generateAstGraph(project,MAX_NUMBER_OF_IR_NODES_TO_GRAPH_FOR_WHOLE_GRAPH,"_before");
#endif

  // Build the inherited attribute
     Detection_InheritedAttribute inheritedAttribute;

  // Define the traversal
     DetectionTraversal shiftCalculus_DetectionTraversal;

#if 1
     printf ("Call the Detection traversal starting at the project (root) node of the AST \n");
#endif

  // Call the traversal starting at the project (root) node of the AST
  // Detection_SynthesizedAttribute result = shiftCalculus_DetectionTraversal.traverse(project,inheritedAttribute);
     Detection_SynthesizedAttribute result = shiftCalculus_DetectionTraversal.traverseWithinFile(firstFile,inheritedAttribute);
#if 0
     printf ("Stencil Operator was transformed: %s \n",result.get_stencilOperatorTransformed() ? "true" : "false");
#endif
     ROSE_ASSERT(result.get_stencilOperatorTransformed() == false);

#if 1
     printf ("DONE: Call the Detection traversal starting at the project (root) node of the AST \n");
#endif
#if 0
     shiftCalculus_DetectionTraversal.display();
#endif
#if DEBUG_USING_DOT_GRAPHS
  // generateDOTforMultipleFile(*project);
  // generateDOT(*project,"_before_transformation");
     AstDOTGeneration astdotgen_2;
     astdotgen_2.generateWithinFile(firstFile,DOTGeneration<SgNode*>::TOPDOWNBOTTOMUP,"_after_DSL_detection");
#endif
#if 0
     printf ("Exiting after the initial traversal to detect the stencil useage. \n");
     ROSE_ASSERT(false);
#endif

  // Build the inherited attribute
     StencilEvaluation_InheritedAttribute inheritedAttribute_stencilEval;

  // Define the traversal
  // StencilEvaluationTraversal shiftCalculus_StencilEvaluationTraversal(shiftCalculus_DetectionTraversal);
     StencilEvaluationTraversal shiftCalculus_StencilEvaluationTraversal;

#if 1
     printf ("Call the StencilEvaluation traversal starting at the project (root) node of the AST \n");
#endif

  // Call the traversal starting at the project (root) node of the AST
  // StencilEvaluation_SynthesizedAttribute result_stencilEval = shiftCalculus_StencilEvaluationTraversal.traverse(project,inheritedAttribute_stencilEval);
     StencilEvaluation_SynthesizedAttribute result_stencilEval = shiftCalculus_StencilEvaluationTraversal.traverseWithinFile(firstFile,inheritedAttribute_stencilEval);

#if 0
     printf ("Stencil Evaluation was transformed: %s \n",result_stencilEval.get_stencilOperatorTransformed() ? "true" : "false");
#endif
     ROSE_ASSERT(result_stencilEval.get_stencilOperatorTransformed() == false);

#if 1
     printf ("DONE: Call the StencilEvaluation traversal starting at the project (root) node of the AST \n");
#endif
#if 1
     shiftCalculus_StencilEvaluationTraversal.displayStencil("After evaluation of stencil");
#endif
#if DEBUG_USING_DOT_GRAPHS
  // generateDOTforMultipleFile(*project);
  // generateDOT(*project,"_before_transformation");
     AstDOTGeneration astdotgen_3;
     astdotgen_3.generateWithinFile(firstFile,DOTGeneration<SgNode*>::TOPDOWNBOTTOMUP,"_after_DSL_evaluation");
#endif

#if 1
     printf ("Call generateStencilCode to generate example code \n");
#endif

#if 0
     printf ("Exiting after the second traversal to evaluate the stencils. \n");
     ROSE_ASSERT(false);
#endif

  // Generate code from stencil data structure.
     bool generateLowlevelCode = true;
     generateStencilCode(shiftCalculus_StencilEvaluationTraversal,generateLowlevelCode);

#if 1
     printf ("DONE: Call generateStencilCode to generate example code \n");
#endif

   ROSE_ASSERT (project->get_fileList().size() ==1);
   SgFile * cur_file = project->get_fileList()[0];

   // Generate MPI specific code
   if (b_gen_mpi)
   { 
     //#include "mpi.h" 
     SageInterface::insertHeader (isSgSourceFile(cur_file), "libxomp_mpi.h", false);
     SageInterface::insertHeader (isSgSourceFile(cur_file), "mpi.h", false);
     SgFunctionDeclaration* main_decl = findMain(cur_file); 
     ROSE_ASSERT (main_decl != NULL);
     SgFunctionDefinition* main_def = main_decl->get_definition();
     ROSE_ASSERT (main_def != NULL);
     SgBasicBlock* func_body = main_def->get_body();
     ROSE_ASSERT (func_body != NULL);

     // Setup MPI
     SgStatement* decl_rank = buildStatementFromString("int _xomp_rank;", func_body); 
     prependStatement(decl_rank, func_body);

     SgStatement* decl_nprocs= buildStatementFromString("int _xomp_nprocs;", func_body); 
     prependStatement(decl_nprocs, func_body);

     // xomp_init_mpi (&argc, &argv, &_xomp_rank, &_xomp_nprocs);
     SgExprListExp * para_list = buildExprListExp (buildAddressOfOp (buildVarRefExp("argc", func_body)), 
                      buildAddressOfOp (buildVarRefExp("argv", func_body)),
                      buildAddressOfOp (buildVarRefExp("_xomp_rank", func_body)),
                      buildAddressOfOp (buildVarRefExp("_xomp_nprocs", func_body))
                      );
     SgExprStatement* mpi_init_stmt = buildFunctionCallStmt ("xomp_init_mpi", buildIntType(), para_list, func_body);
//     SgStatement* last_decl = findLastDeclarationStatement (func_body);
     insertStatementAfter (decl_rank, mpi_init_stmt);

   }

   // Further generate CUDA code if requested
   if (b_gen_cuda)
   {
     // We only process one single input file at a time

     OmpSupport::enable_accelerator = true;
     cur_file->set_openmp_lowering(true);
     cur_file->set_openmp(true);
     cur_file->set_openmp_parse_only(false);
     // process OpenMP directives, including omp target
     // This will translate inserted pragmas again
     OmpSupport::processOpenMP(isSgSourceFile(cur_file));

#if 0 // use rose:output instead to control this
     // rename output file to have .cu suffice
     // change .c suffix to .cu suffix
     std::string orig_name = cur_file->get_file_info()->get_filenameString();
     std::string file_suffix = StringUtility::fileNameSuffix(orig_name);
     // We only allow C file to be compatible with nvcc CUDA compiler
     //ROSE_ASSERT (CommandlineProcessing::isCFileNameSuffix(file_suffix));
     orig_name = StringUtility::stripPathFromFileName(orig_name);
     std::string naked_name = StringUtility::stripFileSuffixFromFileName(orig_name);
     cur_file->set_unparse_output_filename("rose_"+naked_name+".cu");
#endif
   }

#if 0
     printf ("Exiting after call to generateStencilCode() \n");
     ROSE_ASSERT(false);
#endif

  // AST consistency tests (optional for users, but this enforces more of our tests)
     AstTests::runAllTests(project);

#if DEBUG_USING_DOT_GRAPHS
     printf ("Write out the DOT file after the transformation \n");
  // generateDOTforMultipleFile(*project,"after_transformation");
     generateDOT(*project,"_after_transformation");
     printf ("DONE: Write out the DOT file after the transformation \n");
#endif
#if DEBUG_USING_DOT_GRAPHS && 0
  // Output an optional graph of the AST (the whole graph, of bounded complexity, when active)
  // const int MAX_NUMBER_OF_IR_NODES_TO_GRAPH_FOR_WHOLE_GRAPH = 10000;
     generateAstGraph(project,MAX_NUMBER_OF_IR_NODES_TO_GRAPH_FOR_WHOLE_GRAPH,"_after");
#endif

  // Regenerate the source code but skip the call the to the vendor compiler.
     return backend(project);
   }

