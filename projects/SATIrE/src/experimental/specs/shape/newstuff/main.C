#include <config.h>

#include "cfg/CFGTraversal.h"
#include "cfg/ProcTraversal.h"

#undef NDEBUG
#include "testcfg.h"

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

// declared in iterate.h
//extern "C" void *doit(ANALYSIS)(void *);

extern "C" void gdl_create(char *, int);

static int      edge = 0; // Martin's global variable
// FIXME ?ms (wenn edge == 1 gibts beim laden einen syntaxfehler)

extern char* animation;

int main(int argc, char **argv)
{
  debug_stat=0; // 1 or 2 for debugging, 2 means more detailed stats
  verbose=0; // prints PAG-info during analysis (see section 14.3.3.1 in User Manual)

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
  if (kfg_testit(t.getCFG(), 0, "cfg_dump.gdl") != 0)
  {
      std::cout << "done" << std::endl;
      std::cout << "performing analysis " str(ANALYSIS) "... ";
      doit(ANALYSIS)(t.getCFG());
      std::cout << "done" << std::endl;
      std::cout << "generating visualization... ";
      gdl_create(str(ANALYSIS) "_result.gdl", 0);
      std::cout << "done" << std::endl;
  }
  else
      std::cout << "oops, analysis not performed" << std::endl;

  return 0;
}

#ifdef DFI_WRITE
void dfi_write_(FILE * fp, KFG g, char *name, char *attrib, o_dfi info,int id,int insnum,int ctx) {
  int show_summary_graph = 1; //1 -> also show combined shape graph (in color)
  int fold_shapegraphs = 1;   //1 -> shape graphs are initially collapsed
  
  int n_nodes = 0;
	// id ist die KnotenID
	//printf("name:%s, attrib:%s\n",name,attrib);

	/* We have only one instruction per basic block! */
	assert((id==-1 && insnum==-1) || insnum==0);
	if (o_dfi_istop(info) || o_dfi_isbottom(info)) {
    /* FIXME ?ms */
		fprintf(fp,"node: {\n");
		fprintf(fp, "title: \"%s\"\n", name);
		fprintf(fp, "label: \"%s\"\n", o_dfi_istop(info)?GDL_TOP:GDL_BOT);
		fprintf(fp,"}\n\n");
		return;
	}

  fprintf(fp, "graph: {\n");
  fprintf(fp, "  title: \"%s\"\n", name);
  fprintf(fp, "  label: \"\"\n");
  
  //folded (for minimised)| boxed (for expanded view of shapegraphs)
  if (fold_shapegraphs)
    fprintf(fp, "  status: folded\n");
  else
    fprintf(fp, "  status: boxed\n");
  fprintf(fp, "  orientation: left_to_right\n");
  fprintf(fp, "  xbase: 20\n");
  fprintf(fp, "  ybase: 20\n");
  fprintf(fp, "  color: white\n");
  //fprintf(fp, "  bordercolor: green\n");
  //fprintf(fp, "  manhattan_edges: no\n");
  //fprintf(fp, "  ignore_singles: no\n");
  fprintf(fp, "  %s\n", attrib);

  int n_subgraphs;
  
	if (!o_dfi_istop(info) && !o_dfi_isbottom(info)) {
    o_dfii flow = o_dfi_drop(info);
    
    //o_dfiiList subgraphs = o_extract_shapegraphs_t(flow);  //zum testen
    o_dfiiList subgraphs = o_extract_shapegraphs_t(flow);
    n_subgraphs = 0;

    if (show_summary_graph) {
      subgraphs = o_dfiiList_conc(o_extract_summary_graph_t(flow), subgraphs); //zum testen
    }
    
    while (!o_dfiiList_is_empty(subgraphs)) {
      o_dfii          flow  = o_dfiiList_head(subgraphs);
      
      o_NodeList      share = o_is_as_list(flow);
      o_Var2NodeL     exter = o_ev_as_list(flow);
      o_Node2NodeL    inter = o_es_as_list(flow);
      
      o_NodeList      nodes = o_extract_nodes(flow);
      o_strl          vars  = o_ev_extract_vars(o_extract_EV(flow));

      subgraphs = o_dfiiList_tail(subgraphs);
      n_subgraphs++;
      
      //fprintf(fp, "//begin-%s\n", name);
      fprintf(fp, "graph: {\n");
      fprintf(fp, "  title: \"%s-sg%d\"\n", name, n_subgraphs);
      fprintf(fp, "  label: \"\"\n");
      fprintf(fp, "  orientation : left_to_right\n");
      if (show_summary_graph && n_subgraphs == 1)
        fprintf(fp, "  color       : green\n");
      else
        fprintf(fp, "  color       : white\n");
      fprintf(fp, "  status      : grey\n");
      fprintf(fp, "  display_edge_labels: yes\n");
      fprintf(fp, "  manhattan_edges: no\n");
      fprintf(fp, "  ignore_singles : no\n");
      fprintf(fp, "  xspace: 50\n");
      fprintf(fp, "  %s\n", attrib);

      //all nodes
      while (!o_NodeList_is_empty(nodes)) {
        o_VarSet node = o_NodeList_head(nodes);
        
        nodes = o_NodeList_tail(nodes);
        n_nodes++;
        
        fprintf(fp, "node: {\n");
        fprintf(fp, "  title: \"%s-sg%d", name, n_subgraphs);   o_VarSet_print_fp(fp, node);   fprintf(fp, "\"\n");
        fprintf(fp, "  label: \"");                            my_VarSet_print_fp(fp, node);   fprintf(fp, "\"\n");
        
        if (o_NodeList_is_elem(share, node)) { //if node is shared
          fprintf(fp, "  shape:rhomb\n");
          if (!edge) {
            if (o_VarSet_is_empty(node)) { //special summary node
              fprintf(fp, "  color:darkmagenta\n");
            }
            else { //any other node
              fprintf(fp, "  color:red\n");
            }
          }
        }
        fprintf(fp, "}\n\n");
      }

      //all vars
      while (!o_strl_is_empty(vars)) {
        str node = o_strl_head(vars);
        
        vars = o_strl_tail(vars);
        
        fprintf(fp, "node: {\n");
        fprintf(fp, "  title: \"%s-sg%d", name, n_subgraphs);   str_print_fp(fp, node);   fprintf(fp, "\"\n");
        fprintf(fp, "  label: \"%s\"\n", str_to_charp(node));
        fprintf(fp, "  shape:ellipse\n");
        fprintf(fp, "  level:0\n");
        fprintf(fp, "}\n\n");
      }

      //all var->node edges
      while (!o_Var2NodeL_is_empty(exter)) {
        o_Var2Node  edge = o_Var2NodeL_head(exter);
        str          var = o_Var2Node_select_1(edge);
        o_VarSet    node = o_Var2Node_select_2(edge);
        
        exter = o_Var2NodeL_tail(exter);
        
        fprintf(fp, "edge: {\n");
        fprintf(fp, "  sourcename: \"%s-sg%d", name, n_subgraphs);        str_print_fp(fp, var);    fprintf(fp, "\"\n");
        fprintf(fp, "  targetname: \"%s-sg%d", name, n_subgraphs);   o_VarSet_print_fp(fp, node);   fprintf(fp, "\"\n");
        fprintf(fp, "}\n\n");
      }

      //all node--sel-->node edges
      while (!o_Node2NodeL_is_empty(inter)) {
        o_Node2Node    edge = o_Node2NodeL_head(inter);
        o_VarSet      node1 = o_Node2Node_select_1(edge);
        str             sel = o_Node2Node_select_2(edge);
        o_VarSet      node2 = o_Node2Node_select_3(edge);

        inter = o_Node2NodeL_tail(inter);

        fprintf(fp, "edge: {\n");
        fprintf(fp, "  sourcename: \"%s-sg%d", name, n_subgraphs);   o_VarSet_print_fp(fp, node1);   fprintf(fp, "\"\n");
        fprintf(fp, "  targetname: \"%s-sg%d", name, n_subgraphs);   o_VarSet_print_fp(fp, node2);   fprintf(fp, "\"\n");
        fprintf(fp, "  label: \"%s\"\n", str_to_charp(sel));
        fprintf(fp, "}\n\n");
      }

      fprintf(fp, "}\n\n");
    }
  }
    
  if (n_nodes == 0) {
    fprintf(fp, "node:{\n");
    fprintf(fp, "  title: \"%s-sg%d_dummy\"\n", name, n_subgraphs);
    fprintf(fp, "  label: \"\"\n");
    fprintf(fp, "  width : 0\n");
    fprintf(fp, "  height: 0\n");
    fprintf(fp, "}\n");
  }

  fprintf(fp, "}\n");
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
