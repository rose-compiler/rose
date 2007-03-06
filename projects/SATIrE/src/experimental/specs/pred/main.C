#include "main.h"

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
   * the files specified on the command line. */
  SgProject* ast_root = frontend(argc, argv);
  
  /* Make sure everything is OK... */
  AstTests::runAllTests(ast_root);

  /* Construct a control-flow graph from the AST, make sure it has
   * the structrure expected by PAG, and run the analysis on it,
   * attributing the statements of the AST with analysis
   * information -- all in one handy function!
   * This returns a value of type DFI_STORE which you can use to
   * access analysis information if you want. */
  
  char* output=(char*)opt.getGdlFileName().c_str();
  DFI_STORE analysis_info = perform_pag_analysis(ANALYSIS)(ast_root,output,!opt.animationGeneration());

  /* The attribute_printer uses the functions
   * void* get_statement_pre_info(DFI_STORE store, SgStatement *stmt);
   * void* get_statement_post_info(DFI_STORE store, SgStatement *stmt);
   * to access analysis information. See file main-support for an example
   * of how to access the computed analysis information.
   * The output can be turned on by using the command line option --text_output */
  
  if(opt.analysisResultsTextOutput()) {
    PagDfiTextPrinter<DFI_STORE> p(analysis_info);
    p.traverseInputFiles(ast_root, preorder);
  }
  
  if(opt.analysisResultsSourceOutput()) {
    PagDfiCommentAnnotator<DFI_STORE> ca(analysis_info);
    ca.traverseInputFiles(ast_root, preorder);
    backend(ast_root);
  }

  // experimental: force dot output
  /*
  std::cout << "digraph G {\n";
  DfiDotGenerator<DFI_STORE> dotgen(analysis_info);
  dotgen.edgeInfoOff();
  dotgen.traverseInputFiles(ast_root, preorder);
  std::cout << "}\n";
  */

  /* Free all memory allocated by the PAG garbage collection */
  GC_finish();
  
  return 0;
}
