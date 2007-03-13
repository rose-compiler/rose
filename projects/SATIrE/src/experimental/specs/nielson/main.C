#include "main.h"
#include "main-vis.h"

int main(int argc, char **argv)
{
  /* parse the command line and extract analyzer options */
  CommandLineParser clp;
  AnalyzerOptions opt=clp.parse(argc,argv);

  /* use retfunc for combining information from local and return edge */
  global_retfunc = 1;

  /* set the PAG options as specified on the command line */
  setPagOptions(opt);
  
  /* Run the frontend to construct an abstract syntax tree from
   * the files specified on the command line (which has been processed
   * and commands only relevant to the analyzer have been removed). */
  SgProject* ast_root = frontend(opt.getCommandLineNum(), opt.getCommandLineCarray());
  
  /* Make sure everything is OK... */
  AstTests::runAllTests(ast_root);

  /* Construct a control-flow graph from the AST, make sure it has
   * the structrure expected by PAG, and run the analysis on it,
   * attributing the statements of the AST with analysis
   * information. Use the StatementAttributeTraversal class for accessing
   * the analysis information on each statement */
  char* outputfile=(char*)opt.getGdlFileName().c_str();
  DFI_STORE analysis_info = perform_pag_analysis(ANALYSIS)(ast_root,outputfile,!opt.animationGeneration());

  /* Handle command line option --textoutput */
  if(opt.analysisResultsTextOutput()) {
    PagDfiTextPrinter<DFI_STORE> p(analysis_info);
    p.traverseInputFiles(ast_root, preorder);
  }
  
  /* Handle command line option --sourceoutput 
   * The source code (i.e. the AST) is annotated with comments showing
   * the analysis results and by calling the backend an annotated C/C++
   * file is generated (named rose_<inputfilename>) */
  if(opt.analysisResultsSourceOutput()) {
    PagDfiCommentAnnotator<DFI_STORE> ca(analysis_info);
    ca.traverseInputFiles(ast_root, preorder);
    backend(ast_root);
  }

  //begin
  o_run_is_compatible_tests();
  
  AliasTraversal alias_printer(analysis_info);
  alias_printer.traverseInputFiles(ast_root, preorder);
  //end
  
  /* Free all memory allocated by the PAG garbage collection */
  GC_finish();
  
  return 0;
}
