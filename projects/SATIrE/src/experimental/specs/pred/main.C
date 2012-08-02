#include "main.h"

int main(int argc, char **argv)
{
  /* parse the command line and extract analyzer options */
  CommandLineParser clp;
  AnalyzerOptions opt=clp.parse(argc,argv);

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
    ast_root->unparse();
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
