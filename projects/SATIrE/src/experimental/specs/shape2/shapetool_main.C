#include "main-support.h"
#include "main.h"

#include "ShapeAnalyzerOptions.h"
#include "ShapeCommandLineParser.h"
#include "PagAttributes.C"

#if 0
//  optional graph statistics collector
#include "GraphStats.h"
GraphStats gstats;
/* AliasPairsCounter 
 * counts may alias pairs using GraphStats interface */
int count_alias_pairs(SgStatement *stmt, std::string pos, std::string alias_type);
template <typename DFI_STORE_TYPE>
class AliasPairsCounter : public DfiCommentAnnotator<DFI_STORE_TYPE> {
private:
    ExpressionPairSet *pairs;
public:
    AliasPairsCounter(DFI_STORE_TYPE store, ExpressionPairSet *_pairs) : DfiCommentAnnotator<DFI_STORE>(store) {
        pairs = _pairs;
    }
protected:
    virtual void handleStmtDfi(SgStatement* stmt, std::string _unused1, std::string _unused2) {
	    gstats.addMayAliasCount(count_alias_pairs(stmt, "post", "may"));
	    gstats.addMustAliasCount(count_alias_pairs(stmt, "post", "must"));
	}
};
#endif

int main(int argc, char **argv)
{
  TimingPerformance *timer = new TimingPerformance("Complete run of SATIrE analysis:");

  /* parse the command line and extract analyzer options */
  opt = new ShapeAnalyzerOptions();
  ShapeCommandLineParser clp;
  clp.parse(opt,argc,argv);
  bool outputAll = opt->analysisWholeProgram();

  /* set the PAG options as specified on the command line */
  setPagOptions(*opt);
  
  /* Run the frontend to construct an abstract syntax tree from
   * the files specified on the command line (which has been processed
   * and commands only relevant to the analyzer have been removed), or
   * use the binary AST I/O mechanism to read a previously constructed
   * abstract syntax tree from a binary file. */
  SgProject* ast_root;
  if (!opt->inputBinaryAst()) {
    ast_root = frontend(opt->getCommandLineNum(), opt->getCommandLineCarray());
  } else {
    ast_root = AST_FILE_IO::readASTFromFile(opt->getInputBinaryAstFileName());
  }
  
  /* Make sure everything is OK... */
  if(opt->checkRoseAst()) {
    AstTests::runAllTests(ast_root);
  }

  /* Construct a control-flow graph from the AST, make sure it has
   * the structrure expected by PAG, and run the analysis on it,
   * attributing the statements of the AST with analysis
   * information. Use the StatementAttributeTraversal class for accessing
   * the analysis information on each statement */
  DFI_STORE analysis_info = perform_pag_analysis(ANALYSIS)(ast_root,opt);

  /* During construction of the ICFG, we sometimes modify the AST
   * although we shouldn't. This function corrects parent pointers. */
  satireResetParentPointers(ast_root, NULL);

  /* Handle command line option --textoutput */
  if(opt->outputText()) {
    TimingPerformance timer("Generate text output:");
    PagDfiTextPrinter<DFI_STORE> p(analysis_info);

    if (outputAll) p.traverse(ast_root, preorder);
    else p.traverseInputFiles(ast_root, preorder);    
  }
  
  /* Handle command line option --output-XXX 
   * The source code (i.e. the AST) is annotated with comments showing
   * the analysis results and by calling the backend, and an annotated C/C++
   * file is generated (is specified on command line) */
  if(opt->analysisAnnotation()) {
    TimingPerformance timer("Annotate source code with comments:");
    PagDfiCommentAnnotator<DFI_STORE> ca(analysis_info);

    if (outputAll) ca.traverse(ast_root, preorder);
    else ca.traverseInputFiles(ast_root, preorder);
  }

  /* check how many input files we have */
  if(ast_root->numberOfFiles()==1) {
    SgFile& file = ast_root->get_file(0);
    if(opt->outputSource() && opt->getOutputSourceFileName().size()>0) {
      TimingPerformance timer("Output (single) source file:");
      file.set_unparse_output_filename(opt->getOutputSourceFileName());
      unparseFile(&file,0,0);
    }
  }

  /* handle multiple input files */
  if(ast_root->numberOfFiles()>=1 && (opt->getOutputFilePrefix()).size()>0) {
    TimingPerformance timer("Output all source files:");
    /* Iterate over all input files (and its included files) */
    for (int i=0; i < ast_root->numberOfFiles(); ++i) {
      SgFile& file = ast_root->get_file(i);
      
      if(opt->outputSource()) {
        std::string filename=file.get_sourceFileNameWithoutPath();
        file.set_unparse_output_filename(opt->getOutputFilePrefix()+filename);
        unparseFile(&file,0,0);
      }
    }
  }

  /* Handle command line option --termoutput */
  if(opt->outputTerm()) {
    TimingPerformance timer("Output Prolog term:");
    TermPrinter<DFI_STORE> tp(analysis_info);
    
    if (outputAll) tp.traverse(ast_root);
    else tp.traverseInputFiles(ast_root);

    ofstream termfile;
    string filename = opt->getOutputTermFileName();
    termfile.open(filename.c_str());
    termfile << "% Termite term representation" << endl;
    termfile << tp.getTerm()->getRepresentation() << "." << endl;
    termfile.close();
  }

  /* output binary AST if requested */
  if (opt->outputBinaryAst()) {
    AST_FILE_IO::startUp(ast_root);
    AST_FILE_IO::writeASTToFile(opt->getOutputBinaryAstFileName());
  }

  /* Free all memory allocated by the PAG garbage collection */
  GC_finish();
  bool statistics = opt->statistics();
  delete opt;

#if 0
  if (opt->graphStatisticsFile() != NULL) {
    gstats.writeFile(opt->graphStatisticsFile());
  }

#endif
  delete timer;
  if (statistics) {
    TimingPerformance::generateReport();
  }

  return 0;
}

#if 0
ExpressionPairSet *get_statement_alias_pairs(SgStatement *stmt, std::string pos, std::string alias_type) {
	std::string label = "PAG " + alias_type + "-alias " + pos;
	AliasPairAttribute* attr = dynamic_cast<AliasPairAttribute *>(stmt->getAttribute(label));
	return attr->getPairs();
}

std::string format_alias_pairs(SgStatement *stmt, std::string pos, std::string alias_type) {
	ExpressionPairSet *pairs = get_statement_alias_pairs(stmt, pos, alias_type);
    if (pairs == NULL)
      return "<undefined dfi>";
	std::stringstream str;
	ExpressionPairSet::iterator i;
	std::pair<SgNode*,SgNode*> *pair;
	for (i=pairs->begin(); i!=pairs->end(); i++) {
		pair = *i;
		str << "(" << Ir::fragmentToString(pair->first) << "," << Ir::fragmentToString(pair->second) << "), ";
	}
	return str.str();
}

int count_alias_pairs(SgStatement *stmt, std::string pos, std::string alias_type) {
	ExpressionPairSet *pairs = get_statement_alias_pairs(stmt, pos, alias_type);
    if (pairs == NULL)
	  return 0;
	return pairs->size();
}
#endif



