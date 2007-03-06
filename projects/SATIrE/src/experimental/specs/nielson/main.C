#include "main.h"

// GRAPH OUTPUT
#include <stdio.h>
//#include <str.h>
#include <assert.h>

#include "iface.h"
#include "iterate.h"
#include "mapping.h"
#include "nielson-out/src/o_dfi.h"

#include <string>
#include <vector>
#include <algorithm>

bool get_universal_attribute__kill_norm_temps()
{
  return true;
}

void my_o_VarSet_print_fp(FILE * fp, o_VarSet node);

// declared in iterate.h
//extern "C" void *doit(ANALYSIS)(void *);

extern "C" void gdl_create(char *, int);
static int      edge = 0; // global variable for setting edges on/off

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

  /* Extract all Pairs of Expressions from the Program so that they
   * can be compared for aliasing. */
  ExpressionCollector ec;
  ExpressionPairVector *pairs = ec.getExpressionPairs(ast_root);

  /* Add Alias Pairs to AST */
  AliasPairsAnnotator<DFI_STORE> annotator(analysis_info, pairs);
  annotator.traverseInputFiles(ast_root, preorder);

  /* Handle command line option --textoutput */
  if(opt.analysisResultsTextOutput()) {
    //PagDfiTextPrinter<DFI_STORE> p(analysis_info);
    AliasPairsTextPrinter<DFI_STORE> p(analysis_info, pairs);
    p.traverseInputFiles(ast_root, preorder);
  }

  /* Handle command line option --sourceoutput 
   * The source code (i.e. the AST) is annotated with comments showing
   * the analysis results and by calling the backend an annotated C/C++
   * file is generated (named rose_<inputfilename>) */
  if(opt.analysisResultsSourceOutput()) {
    //PagDfiCommentAnnotator<DFI_STORE> ca(analysis_info);
    AliasPairsCommentAnnotator<DFI_STORE> ca(analysis_info, pairs);
    ca.traverseInputFiles(ast_root, preorder);
    ast_root->unparse();
  }

  /* Free all memory allocated by the PAG garbage collection */
  GC_finish();
  
  return 0;
}

#ifdef DFI_WRITE
void dfi_write_(FILE * fp, KFG g, char *name, char *attrib, o_dfi info,int id,int insnum,int ctx)
{
  int show_summarygraph = 0; //shows the combined shape graph a la Sagiv/Reps/William (0: no summary graph)
  int show_shapegraphs  = 1; //shows the shape graphs (0: no shape graphs)
  int fold_shapegraphs  = 1; //fold the shapegraphs (0: display them all initially)

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
  if (fold_shapegraphs)
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
  
	if (!o_dfi_istop(info) && !o_dfi_isbottom(info))
  {
    //the three lines marked with "**change" need to be 
    //modified when changing from srw to nielson graphs
    o_ShapeGraphSet carrier = o_dfi_drop(info); //**change
    o_ShapeGraphList graphs = o_emptylist();
    int n_graphs = 0;
    int n_nodes = 0;

    if (show_shapegraphs) 
    {
      graphs = o_ShapeGraphList_conc(o_nielson_extract_shapeGraphs(carrier), graphs); //**change
    }

    if (show_summarygraph)
    {
      graphs = o_ShapeGraphList_conc(o_nielson_extract_summaryGraph(carrier), graphs);  //**change
    }

    // all graphs
    while (!o_ShapeGraphList_is_empty(graphs)) 
    {
      o_ShapeGraph sg = o_ShapeGraphList_head(graphs);
      graphs = o_ShapeGraphList_tail(graphs);
      n_graphs++;

      o_NodeList        is_shared = o_extract_is_shared(sg);
      o_StackEdgeList stack_edges = o_extract_stackedges(sg);
      o_HeapEdgeList   heap_edges = o_extract_heapedges(sg);

      o_NodeList            nodes = o_extract_heapnodes(sg);
      o_StrList              vars = o_vars_by_stackedges(o_extract_stackedge_set(sg));

      if (show_summarygraph && n_graphs == 1)
      {
        fprintf(fp, "  graph: { /*summary graph*/\n");
        fprintf(fp, "    color: green\n");
      }
      else
      {
        fprintf(fp, "  graph: { /*shape graph*/\n");
        fprintf(fp, "    color: white\n");
      }
      fprintf(fp, "    title: \"%s-sg%d\"\n", name, n_graphs);
      fprintf(fp, "    label: \"\"\n");
      fprintf(fp, "    orientation: left_to_right\n");
      fprintf(fp, "    status: grey\n");
      fprintf(fp, "    display_edge_labels: yes\n");
      fprintf(fp, "    manhattan_edges: no\n");
      fprintf(fp, "    ignore_singles : no\n");
      fprintf(fp, "    xspace: 50\n");
      fprintf(fp, "    %s\n", attrib);

      // all heap nodes
      while (!o_NodeList_is_empty(nodes)) {
        o_VarSet          node = o_NodeList_head(nodes);

        nodes = o_NodeList_tail(nodes);
        n_nodes++;
        fprintf(fp, "    node: { /*heap node*/ \n");
        fprintf(fp, "      title: \"%s-sg%d", name, n_graphs);
        o_VarSet_print_fp(fp, node);
        fprintf(fp, "\"\n");
        fprintf(fp, "      label: \"");
        my_o_VarSet_print_fp(fp, node);
        fprintf(fp, "\"\n");
        if (o_NodeList_is_elem(is_shared, node))
        {
          fprintf(fp, "      shape: rhomb\n");
          if (!edge)
          {
            if (o_VarSet_is_empty(node)) 
              fprintf(fp, "      color: darkmagenta\n");
            else
              fprintf(fp, "      color: red\n");
          }
        }
        fprintf(fp, "    }\n\n");
      }

      // all var nodes
      while (!o_StrList_is_empty(vars)) {
        str            node = o_StrList_head(vars);

        vars = o_StrList_tail(vars);
        fprintf(fp, "    node: { /*var node*/ \n");
        fprintf(fp, "      title: \"%s-sg%d", name, n_graphs);
        str_print_fp(fp, node);
        fprintf(fp, "\"\n      label: \"");
        /* snum_print_fp(fp, node); */
                          //if (node < 0) 
        //fprintf(fp, "@_%s\"\n", getstr(-1*node));
        //else fprintf(fp, "%s\"\n", getstr(node));
        fprintf(fp, "%s\"\n", str_to_charp(node));
        fprintf(fp, "      shape: ellipse\n");
        fprintf(fp, "      level: 0\n");
        fprintf(fp, "    }\n\n");
      }

      // all stack edges
      while (!o_StackEdgeList_is_empty(stack_edges)) {
        o_StackEdge           edge = o_StackEdgeList_head(stack_edges);
        str             var = o_StackEdge_select_1(edge);
        o_VarSet          vars = o_StackEdge_select_2(edge);

        stack_edges = o_StackEdgeList_tail(stack_edges);
        fprintf(fp, "    edge: { /*stackedge*/\n");
        fprintf(fp, "      sourcename: \"%s-sg%d", name, n_graphs);
        str_print_fp(fp, var);
        fprintf(fp, "\"\n");
        fprintf(fp, "      targetname: \"%s-sg%d", name, n_graphs);
        o_VarSet_print_fp(fp, vars);
        fprintf(fp, "\"\n");
        fprintf(fp, "    }\n\n");
      }

      // all heap edges
      while (!o_HeapEdgeList_is_empty(heap_edges)) {
        o_HeapEdge edge = o_HeapEdgeList_head(heap_edges);
        o_VarSet  node1 = o_HeapEdge_select_1(edge);
        str         var = o_HeapEdge_select_2(edge);
        o_VarSet  node2 = o_HeapEdge_select_3(edge);

        heap_edges = o_HeapEdgeList_tail(heap_edges);
        fprintf(fp, "    edge: { /*heapedge*/\n");
        fprintf(fp, "      sourcename: \"%s-sg%d", name, n_graphs);
        o_VarSet_print_fp(fp, node1);
        fprintf(fp, "\"\n");
        fprintf(fp, "      targetname: \"%s-sg%d", name, n_graphs);
        o_VarSet_print_fp(fp, node2);
        fprintf(fp, "\"\n");
        fprintf(fp, "      label: \"");
        /* snum_print_fp(fp, var); */
        //	if (var < 0) 
        //fprintf(fp, "%s\"\n", getstr(-1*var));
        //else fprintf(fp, "%s\"\n", getstr(var));
        fprintf(fp, "%s\"\n", str_to_charp(var));
        fprintf(fp, "    }\n\n");
      }
      
      // end one graph
      fprintf(fp, "  }\n\n");
    }

    if (!n_nodes) {
      fprintf(fp, "  node: { /*no nodes*/ \n");
      fprintf(fp, "    title: \"%s-sg%d_dummy\"\n", name, n_graphs);
      fprintf(fp, "    label: \"\"\n");
      fprintf(fp, "    width : 0\n");
      fprintf(fp, "    height: 0\n");
      fprintf(fp, "  }\n");
    }
  }

	fprintf(fp, "}\n\n\n\n");
	return;
}

void my_o_VarSet_print_fp(FILE * fp, o_VarSet node) {
	_o_VarSet_cur cur;

	std::vector<std::string> varSet;
	o_VarSet_cur_reset(&cur,node);
	while(!o_VarSet_cur_is_empty(&cur)) {
	        char* var = str_to_charp(o_VarSet_cur_get(&cur)); // MS adapted int to char*
		o_VarSet_cur_next(&cur);
		//if (var!=0) fprintf(fp,"%s",var); // MS adapted to char*
		//else fprintf(fp,"NOVAR");
		//if(!o_VarSet_cur_is_empty(&cur))
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
#endif
