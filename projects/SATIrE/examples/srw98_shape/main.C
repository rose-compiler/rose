/* -*- C++ -*- */

#include "main-support.h"
#include "main.h"

PAG_BOOL get_universal_attribute__option_kill_normalisation_temps()
{
	/* remove temporary variables introduced in the normalisation process */
	return true;
}

ShapeAnalyzerOptions *opt;

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

int main(int argc, char **argv)
{
  TimingPerformance *timer = new TimingPerformance("Complete run of SATIrE analysis:");

  /* parse the command line and extract analyzer options */
  opt = new ShapeAnalyzerOptions();
  ShapeCommandLineParser clp;
  clp.parse(opt,argc,argv);
  init_termite(argc, argv);
  bool outputAll = opt->analysisWholeProgram();

  /* set the PAG options as specified on the command line */
  setPagOptions(*opt);
  
  /* Run the frontend to construct an abstract syntax tree from
   * the files specified on the command line (which has been processed
   * and commands only relevant to the analyzer have been removed). */
  SgProject* ast_root = frontend(opt->getCommandLineNum(), opt->getCommandLineCarray());
  
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

  /* Extract all Pairs of Expressions from the Program so that they
   * can be compared for aliasing. */
  ExpressionCollector ec;
  ExpressionPairSet *pairs = ec.getExpressionPairs(ast_root);

  /* Add Alias Pairs to AST */
  AliasPairsAnnotator<DFI_STORE> annotator(analysis_info, pairs);
  annotator.traverseInputFiles(ast_root, preorder);

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

  if(opt->aliasTextOutput()) {
	TimingPerformance timer("Generate alias-pair text output:");
	AliasPairsTextPrinter<DFI_STORE> p(analysis_info, pairs);

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

  if(opt->aliasSourceOutput()) {
	TimingPerformance timer("Annotate source code with alias-pair comments:");
	AliasPairsCommentAnnotator<DFI_STORE> ca(analysis_info, pairs);

	if (outputAll) ca.traverse(ast_root, preorder);
	else ca.traverseInputFiles(ast_root, preorder);
  }

  if (opt->graphStatisticsFile() != NULL) {
	AliasPairsCounter<DFI_STORE> ca(analysis_info, pairs);

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

  /* Free all memory allocated by the PAG garbage collection */
  GC_finish();
  bool statistics = opt->statistics();
  delete opt;

  delete timer;
  if (statistics) {
	TimingPerformance::generateReport();
  }
  if (opt->graphStatisticsFile() != NULL) {
    gstats.writeFile(opt->graphStatisticsFile());
  }

  return 0;
}    

// -- custom visualisation --

static int edge = 0; // global variable for setting edges on/off

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







/* custom .gdl output functions */
int gdl_write_shapegraph_fp(FILE *fp, char *name, int n_graphs,  char *attrib, o_ShapeGraph sg) {
	int n_nodes = 0;

	o_NodeList	is_shared = o_extract_is_shared(sg);
	o_StackEdgeList stack_edges = o_extract_stackedges(sg);
	o_HeapEdgeList   heap_edges = o_extract_heapedges(sg);

	o_NodeList	    nodes = o_extract_heapnodes(sg);
	o_VarList        vars = o_vars_by_stackedges(o_extract_stackedge_set(sg));

	fprintf(fp, "    title: \"%s-sg%d\"\n", name, n_graphs);
	fprintf(fp, "    orientation: left_to_right\n");
	fprintf(fp, "    status: grey\n");
	fprintf(fp, "    display_edge_labels: yes\n");
	fprintf(fp, "    manhattan_edges: no\n");
	fprintf(fp, "    ignore_singles : no\n");
	fprintf(fp, "    xspace: 50\n");
	fprintf(fp, "    %s\n", attrib);

	// all heap nodes
	while (!o_NodeList_is_empty(nodes)) {
		o_Node node = o_NodeList_head(nodes);
		nodes = o_NodeList_tail(nodes);
		gstats.addHeapNode();
		n_nodes++;
		
		fprintf(fp, "    node: { /*heap node*/ \n");
		fprintf(fp, "      title: \"%s-sg%d", name, n_graphs);
		o_Node_print_fp(fp, node);
		fprintf(fp, "\"\n");
		fprintf(fp, "      label: \"");
		my_o_Node_print_fp(fp, node);
		fprintf(fp, "\"\n");
		if (o_NodeList_is_elem(is_shared, node))
		{
			fprintf(fp, "      shape: rhomb\n");
			if (!edge)
			{
	if (o_Node_is_empty(node)) 
	  fprintf(fp, "      color: darkmagenta\n");
	else
	  fprintf(fp, "      color: red\n");
			}
		}
		fprintf(fp, "    }\n\n");
	}

	// all var nodes
	while (!o_VarList_is_empty(vars)) {
		o_VariableId node = o_VarList_head(vars);
		gstats.addStackNode();

		vars = o_VarList_tail(vars);
		fprintf(fp, "    node: { /*var node*/ \n");
		fprintf(fp, "      title: \"%s-sg%d", name, n_graphs);
		o_VariableId_print_fp(fp, node);
		fprintf(fp, "\"\n      label: \"");
        o_VariableId_print_fp(fp, node);
		fprintf(fp, "\"\n");
		fprintf(fp, "      shape: ellipse\n");
		fprintf(fp, "      level: 0\n");
		fprintf(fp, "    }\n\n");
	}

	// all stack edges
	while (!o_StackEdgeList_is_empty(stack_edges)) {
		o_StackEdge	   edge = o_StackEdgeList_head(stack_edges);
		o_VariableId    var = o_StackEdge_select_1(edge);
		o_Node	  vars = o_StackEdge_select_2(edge);

		stack_edges = o_StackEdgeList_tail(stack_edges);
		fprintf(fp, "    edge: { /*stackedge*/\n");
		fprintf(fp, "      sourcename: \"%s-sg%d", name, n_graphs);
		o_VariableId_print_fp(fp, var);
		fprintf(fp, "\"\n");
		fprintf(fp, "      targetname: \"%s-sg%d", name, n_graphs);
		o_Node_print_fp(fp, vars);
		fprintf(fp, "\"\n");
		fprintf(fp, "    }\n\n");
	}

	// all heap edges
	while (!o_HeapEdgeList_is_empty(heap_edges)) {
		o_HeapEdge edge  = o_HeapEdgeList_head(heap_edges);
		o_Node node1     = o_HeapEdge_select_1(edge);
		o_VariableId var = o_HeapEdge_select_2(edge);
		o_Node node2     = o_HeapEdge_select_3(edge);

		heap_edges = o_HeapEdgeList_tail(heap_edges);
		fprintf(fp, "    edge: { /*heapedge*/\n");
		fprintf(fp, "      sourcename: \"%s-sg%d", name, n_graphs);
		o_Node_print_fp(fp, node1);
		fprintf(fp, "\"\n");
		fprintf(fp, "      targetname: \"%s-sg%d", name, n_graphs);
		o_Node_print_fp(fp, node2);
		fprintf(fp, "\"\n");
		fprintf(fp, "      label: \"");
        o_VariableId_print_fp(fp, var);
		fprintf(fp, "\"\n");
		fprintf(fp, "    }\n\n");
	}

	if (!n_nodes) {
		fprintf(fp, "  node: { /*no nodes*/ \n");
		fprintf(fp, "    title: \"%s-sg%d_dummy\"\n", name, n_graphs);
		fprintf(fp, "    label: \"\"\n");
		fprintf(fp, "    width : 0\n");
		fprintf(fp, "    height: 0\n");
		fprintf(fp, "  }\n");
	}
	
	// end graph
	fprintf(fp, "  }\n\n");
	return n_nodes;
}

void dfi_write_(FILE * fp, KFG g, char *name, char *attrib, o_dfi info,int id,int insnum,int ctx) {
	/* We have only one instruction per basic block ! */
	assert((id==-1 && insnum==-1) || insnum==0);
	if (o_dfi_istop(info) || o_dfi_isbottom(info))
	{
		fprintf(fp, "node: { /*single instruction*/\n");
		fprintf(fp, "  title: \"%s\"\n", name);
		fprintf(fp, "  label: \"%s\"\n", o_dfi_istop(info)?GDL_TOP:GDL_BOT);
		fprintf(fp, "}\n\n");
		return;
	}

	fprintf(fp, "graph: { /*carrier box*/\n");
	fprintf(fp, "  title: \"%s\"\n", name);
	fprintf(fp, "  label: \"\"\n");
	
	//fprintf(fp, "status: grey\n");
	if (opt->gdlFoldGraphs())
		fprintf(fp, "  status: folded\n");
	else
		fprintf(fp, "  status: boxed\n");
	
	fprintf(fp, "  orientation: left_to_right\n");
	fprintf(fp, "  color: white\n");
	fprintf(fp, "  display_edge_labels: yes\n");
	fprintf(fp, "  manhattan_edges: no\n");
	fprintf(fp, "  ignore_singles : no\n");
	fprintf(fp, "  xspace: 50\n");
	fprintf(fp, "  %s\n", attrib);

    int n_nodes = 0;
	if (!o_dfi_istop(info) && !o_dfi_isbottom(info)) {
		int n_graphs = 0;
		
		o_SrwNnhPair gpair = o_srw_extract_graphs(o_dfi_drop(info));
		
		if (opt->gdlShowSummaryGraph()) {
			gstats.addGraphs(1);
			fprintf(fp, "  graph: { /*summary graph*/\n");
			fprintf(fp, "    color: lightgrey\n");
			fprintf(fp, "    label: \"\"\n");
			n_graphs++;
			n_nodes += gdl_write_shapegraph_fp(fp, name, n_graphs, attrib, o_SrwNnhPair_select_1(gpair));
		}

		if (opt->gdlShowIndividualGraphs()) {
			o_ShapeGraphList graphs = o_SrwNnhPair_select_2(gpair);
			while (!o_ShapeGraphList_is_empty(graphs)) 
			{
				o_ShapeGraph sg = o_ShapeGraphList_head(graphs);
				graphs = o_ShapeGraphList_tail(graphs);
				
				fprintf(fp, "  graph: { /*shape graph*/\n");
				fprintf(fp, "    color: white\n");
				fprintf(fp, "    label: \"\"\n");
				n_graphs++;
				n_nodes += gdl_write_shapegraph_fp(fp, name, n_graphs, attrib, sg);
			}
		}
	}

	if (!n_nodes) {
		fprintf(fp, "  node: { /*no nodes*/ \n");
		fprintf(fp, "    title: \"%s_dummy\"\n", name);
		fprintf(fp, "    label: \"\"\n");
		fprintf(fp, "    width : 0\n");
		fprintf(fp, "    height: 0\n");
		fprintf(fp, "  }\n");
    }

	fprintf(fp, "}\n\n\n\n");
	return;
}

void my_o_Node_print_fp(FILE * fp, o_Node node) {
	_o_Node_cur cur;

	std::vector<std::string> varSet;
	o_Node_cur_reset(&cur,node);
	while(!o_Node_cur_is_empty(&cur)) {
        str var   = o_varid_str(o_Node_cur_get(&cur));
		o_Node_cur_next(&cur);
		//if (var!=0) fprintf(fp,"%s",var); // MS adapted to char*
		//else fprintf(fp,"NOVAR");
		//if(!o_Node_cur_is_empty(&cur))
		//fprintf(fp,",");
		if (var!=0)
		  varSet.push_back(var);
		else
		  varSet.push_back("NOVAR");
	}
	// we need to sort the set of variable names to ensure that the same string is generated for the same set
	sort(varSet.begin(),varSet.end());
	fprintf(fp,"{");
	for(std::vector<std::string>::iterator i=varSet.begin();i!=varSet.end(); i++) {
	  if(i!=varSet.begin())
	    fprintf(fp,",");
	  std::string varName=*i;
	  const char* c=varName.c_str();
	  fprintf(fp,c);

	}
	fprintf(fp,"}");
}

#include "satire_legacy.C"

// vim: ts=4 sts=2:
