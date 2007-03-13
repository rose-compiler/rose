#include "main.h"

#include <stdio.h>
//#include <str.h>
#include <assert.h>

#include "iface.h"
#include "iterate.h"
#include "mapping.h"
#include "shape-out/src/o_dfi.h"

void my_VarSet_print_fp(FILE * fp, o_VarSet node);

#define doit(analysis) xdoit(analysis)
#define xdoit(analysis) analysis##_doit

#define str(analysis) xstr(analysis)
#define xstr(analysis) #analysis

extern int debug_stat;
extern int verbose;

bool get_universal_attribute__kill_norm_temps()
{
  return true;
}

// declared in iterate.h
//extern "C" void *doit(ANALYSIS)(void *);

extern "C" void gdl_create(char *, int);

static int edge = 0; // Martin's global variable
// FIXME ?ms (wenn edge == 1 gibts beim laden einen syntaxfehler)

extern char* animation;

int main(int argc, char **argv)
{
  verbose    = 0; // prints PAG-info during analysis (see section 14.3.3.1 in User Manual)

  animation = NULL; //"shapeanim";
  SgProject *root = frontend(argc, argv);
  AstTests::runAllTests(root);
  std::cout << "collecting functions... ";
  ProcTraversal s;
  s.traverseInputFiles(root, preorder);
  std::cout << "done" << std::endl;

  std::cout << "generating cfg... ";
  CFGTraversal t(s.get_procedures());
  t.traverseInputFiles(root, preorder);
  std::cout << "done" << std::endl;

  std::cout << "testing cfg... " << std::endl;
  int test_result = kfg_testit(t.getCFG(), 0, "cfg_dump.gdl");
  if (test_result == 0)
  {
    std::cout << std::endl << "Warning: There are problems in the CFG." << std::endl;
    return 1;
  }
  else
  {
    std::cout << "no problems found" << std::endl;
  }

  std::cout << "performing analysis " str(ANALYSIS) "... " << std::endl;
  doit(ANALYSIS)(t.getCFG());
  std::cout << "done" << std::endl;
  std::cout << "generating visualization... " << std::flush;
  gdl_create(str(ANALYSIS) "_result.gdl", 0);
  std::cout << "done" << std::endl;

  return 0;
}

#ifdef DFI_WRITE

void dfi_write_(FILE * fp, KFG g, char *name, char *attrib, o_dfi info,int id,int insnum,int ctx)
{
  int show_summarygraph = 1; //shows the combined shape graph a la Sagiv/Reps/William (0: no summary graph)
  int show_shapegraphs  = 0; //shows the shape graphs (0: no shape graphs)
  int fold_shapegraphs  = 0; //fold the shapegraphs (0: display them all initially)

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
    o_ShapeGraph carrier = o_dfi_drop(info);
    o_ShapeGraphList graphs = o_emptylist();
    int n_graphs = 0;
    int n_nodes = 0;

    if (show_shapegraphs) 
    {
      graphs = o_ShapeGraphList_conc(o_srw_extract_shapeGraphs_t(carrier), graphs);
    }

    if (show_summarygraph)
    {
      graphs = o_ShapeGraphList_conc(o_srw_extract_summaryGraph_t(carrier), graphs);
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
        my_VarSet_print_fp(fp, node);
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

//print a VarSet to fp
void my_VarSet_print_fp(FILE * fp, o_VarSet node) {
  _o_VarSet_cur cur;

  fprintf(fp,"{");
  o_VarSet_cur_reset(&cur,node);

  while (!o_VarSet_cur_is_empty(&cur)) {
    char* var = str_to_charp(o_VarSet_cur_get(&cur)); // MS adapted int to char*
    o_VarSet_cur_next(&cur);

    if (var!=0)
      fprintf(fp,"%s",var); // MS adapted to char*
    else
      fprintf(fp,"NOVAR");

    if(!o_VarSet_cur_is_empty(&cur))
      fprintf(fp,",");
  }
  fprintf(fp,"}");
}
#endif
